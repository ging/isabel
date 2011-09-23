/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: sessionManager.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include <aCodecs/codecs.h>

#include <rtp/RTPUtils.hh>

#include "sessionManager.h"
#include "controlProtocol.h"
#include "grid.h"
#include "grid2.h"
#include "mixer.h"
#include "switch.h"
#include "switchSSRC.h"
#include "audioTrans.h"
#include "videoTrans.h"

// definir en el gestor de sesiones
audioInfo_t::audioInfo_t(void)
{
    audioMode = AUDIO_SWITCH_MODE;
    PT = -1;
};

audioInfo_t::~audioInfo_t(void)
{
}

videoInfo_t::videoInfo_t(void)
{
    videoMode = VIDEO_SWITCH_MODE;
    rateMode  = CBR;
    gridMode  = GRID_AUTO;
    BW        = -1; //unlimited
    Q         = 100;
    FR        = 30;
    Height    = 0;
    Width     = 0;
    PT        = 0;
}

videoInfo_t::~videoInfo_t(void)
{
}

sessionManager_t::participantInfo_t::participantInfo_t(u16 ID,
                                                       inetAddr_t const &addr,
                                                       inetAddr_t *groupAddr,
                                                       ql_t<u16> localPortList
                                                      )
{
    this->inetAddr = addr;
    this->groupAddr = groupAddr;
    this->ID = ID;
    this->localPortList = localPortList;
    natParticipant = false;
    groupJoined = false;
}

sessionManager_t::participantInfo_t::participantInfo_t(u16 ID,
                                                       ql_t<u16> localPortList
                                                      )
{
    this->ID = ID;
    this->localPortList = localPortList;
    natParticipant = true;
    completedNatParticipant = false;
    numNatPorts = localPortList.len();
}

sessionManager_t::participantInfo_t::~participantInfo_t(void)
{
}

sessionManager_t::sessionInfo_t::sessionInfo_t(u16 id)
{
    this->ID= id;
}

sessionManager_t::sessionInfo_t::~sessionInfo_t(void)
{
    // delete targets
    while (targetArray.size())
    {
        target_t *tgt= targetArray.elementAt(0);
        targetArray.remove(0);
        delete tgt;
    }


    // delete flowProcessors
    while (flowProcessorList.len())
    {
        flowProcessor_t * flowProcessor = flowProcessorList.head();
        delete flowProcessor;
        delete flowProcessorList.head();
        flowProcessorList.behead();
    }

    // delete participants
    while (participantInfoArray.size())
    {
        participantInfo_t *pInfo= participantInfoArray.elementAt(0);
        participantInfoArray.remove(0);
        delete pInfo;
    }
}

sessionManager_t::sessionManager_t(int &argc, argv_t &argv)
: application_t(argc, argv,1204)
{
	APP= this;

    optionDefList_t opt;
    appParamList_t *parList;

    streamSocket_t *serverSock= NULL;

    enum myOptions
    {
       cport,
       notify,
       h
       // write options here
       // ...
    };

    // Available options
    opt
       << new optionDef_t("@cport", cport , "Opens a Control Socket in <port>")
       << new optionDef_t("@notify", notify , "Notify file")
       << new optionDef_t("h", h , "MCU Help");

    parList = getOpt(opt, argc, argv);

    for ( ; parList->len(); parList->behead())
    {
        switch(parList->head()->parId)
        {
        case cport:
            {
                inetAddr_t ctrlAddr("0.0.0.0", parList->head()->parValue , serviceAddr_t::STREAM);
                serverSock= new streamSocket_t(ctrlAddr);
                if (serverSock->listen(ctrlAddr) < 0)
                {
                    NOTIFY("Problems when calling listen() "
                           "for control socket, bailing out"
                           );
                    abort();
                }

                *this<< new tcpServer_t<mcu_Interface_t>(*serverSock);
            }
            break;

        case notify:
            char *file;
            file = strdup(parList->head()->parValue);
            setNotifyFile(file);
            break;

        case h:
        default:
            for (optionDefList_t::iterator_t i = opt.begin(); i != opt.end(); i++)
            {
                optionDef_t * op= static_cast<optionDef_t*>(i);
                if (op->optDesc)
                    fprintf(stderr, "    %-10s %-5s\t-- %s\n",
                    op->optName,
                    op->optHasArg?"value":"",
                    op->optDesc);
                else
                    fprintf(stderr, "    %-10s %-5s\t",
                    op->optName,
                    op->optHasArg?"":"value");
            }
            exit(0);
        }
    }
    // create DEMUX
    demux = new demux_t();

    // create RTCPDemux
    RTCPDemux = new RTCPDemux_t();

    // create statGatherer
    statGatherer = new statGatherer_t();
}


sessionManager_t::~sessionManager_t(void)
{
    // delete sessions
    while (sessionArray.size())
    {
        sessionInfo_t *session= sessionArray.elementAt(0);
        sessionArray.remove(0);
        delete session;
    }

    if (demux)
    {
        delete demux;
    }
    if (RTCPDemux)
    {
        delete RTCPDemux;
    }
    if (statGatherer)
    {
        delete statGatherer;
    }
}

// private methods

HRESULT
sessionManager_t::getSession(u16 ID_session, sessionInfo_t **session)
{
    for (int i = 0; i < sessionArray.size(); i++)
    {
        sessionInfo_t *puntero = sessionArray.elementAt(i);
        if (ID_session == puntero->ID)
        {
            if (session)
            {
                *session = puntero;
            }
            return S_OK;
        }
    }
    return E_SESSION_NOT_EXISTS;
}

HRESULT
sessionManager_t::getParticipantByID(sessionInfo_t *session,
                                     u16 ID_part,
                                     participantInfo_t **participant
                                    )
{
    for (int i= 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *puntero= session->participantInfoArray.elementAt(i);
        if (ID_part == puntero->ID)
        {
            if (participant)
            {
                *participant = puntero;
            }
            return S_OK;
        }
    }
    return E_PART_NOT_EXISTS;
}

HRESULT
sessionManager_t::getParticipantByIPPort(sessionInfo_t *session,
                                         inetAddr_t const &addr,
                                         ql_t<u16> portList,
                                         participantInfo_t **participant
                                        )
{
    for (int i = 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *puntero= session->participantInfoArray.elementAt(i);
        if (puntero->natParticipant)
        {
            for (ql_t<u16>::iterator_t j = portList.begin();j != portList.end();j++)
            {
                u16 puertoUno = j;
                for (ql_t<u16>::iterator_t k = puntero->localPortList.begin();k != puntero->localPortList.end();k++)
                {
                    u16 puertoDos = k;
                    if (puertoUno == puertoDos)
                    {
                        if (participant)
                        {
                            *participant = puntero;
                        }
                        return S_OK;
                    }
                }
            }
        }
        else if (addr.equalIPAddr(puntero->inetAddr))
        {
            for (ql_t<u16>::iterator_t j = portList.begin();j != portList.end();j++)
            {
                u16 puertoUno = j;
                for (ql_t<u16>::iterator_t k = puntero->localPortList.begin();k != puntero->localPortList.end();k++)
                {
                    u16 puertoDos = k;
                    if (puertoUno == puertoDos)
                    {
                        if (participant)
                        {
                            *participant = puntero;
                        }
                        return S_OK;
                    }
                }
            }
        }
    }

    return E_PART_NOT_EXISTS;
}

HRESULT
sessionManager_t::getSessionByIPPort(inetAddr_t const &addr,
                                     u16 port,
                                     int *ID
                                    )
{
    for (int i = 0; i < sessionArray.size(); i++)
    {
        sessionInfo_t *session = sessionArray.elementAt(i);
        for (int j = 0; j < session->participantInfoArray.size(); j++)
        {
            participantInfo_t *puntero=
                session->participantInfoArray.elementAt(j);
            participantInfo_t *part = NULL;
            if (addr.equalIPAddr(puntero->inetAddr))
            {
                part = puntero;
                for (ql_t<u16>::iterator_t k = part->localPortList.begin();
                     k != part->localPortList.end();
                     k++
                    )
                {
                    u16 participantPort = (u16)k;
                    if (port == participantPort)
                    {
                        *ID = session->ID;
                        return S_OK;
                    }
                }
            }
        }
    }
    return E_SESSION_NOT_EXISTS;
}

HRESULT
sessionManager_t::getSessionByNatPort(u16 port, int *ID)
{
    for (int i = 0; i < sessionArray.size(); i++)
    {
        sessionInfo_t *session = sessionArray.elementAt(i);
        for (int j = 0; j < session->participantInfoArray.size(); j++)
        {
            participantInfo_t *puntero=
                session->participantInfoArray.elementAt(j);
            participantInfo_t *part = NULL;
            if (puntero->natParticipant)
            {
                part = puntero;
                for (ql_t<u16>::iterator_t k = part->localPortList.begin();
                     k != part->localPortList.end();
                     k++
                    )
                {
                    u16 participantPort = (u16)k;
                    if (port == participantPort)
                    {
                        *ID = session->ID;
                        return S_OK;
                    }
                }
            }
        }
    }
    return E_SESSION_NOT_EXISTS;
}


// session Commands
HRESULT
sessionManager_t::createSession(u16 ID_session)
{
    __CONTEXT("sessionManager_t::createSession");
    if (SUCCEEDED(getSession(ID_session)))
    {
       return E_SESSION_EXISTS;
    }

    // create flowProcessors
    switcher_t *switcher = new switcher_t;
    switcherSSRC_t *switcherSSRC = new switcherSSRC_t;
    mixer_t *mixer = new mixer_t;
    grid_t *grid = new grid_t;
    grid2_t *grid2 = new grid2_t;
    audioTrans_t *audioTrans = new audioTrans_t;
    videoTrans_t *videoTrans = new videoTrans_t;

    // create session
    sessionInfo_t *session = new sessionInfo_t(ID_session);

    session->flowProcessorList.insert(switcher);
    session->flowProcessorList.insert(switcherSSRC);
    session->flowProcessorList.insert(mixer);
    session->flowProcessorList.insert(grid);
    session->flowProcessorList.insert(grid2);
    session->flowProcessorList.insert(audioTrans);
    session->flowProcessorList.insert(videoTrans);

    sessionArray.add(session);

    HRESULT hr = statGatherer->createSession(ID_session);
    errorCheck(hr, "sessionManager_t");
    return hr;
}

HRESULT
sessionManager_t::removeSession(u16 ID_session)
{
    // test if session exists
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // delete flowProcessors
    while (session->flowProcessorList.len())
    {
        delete session->flowProcessorList.head();
        session->flowProcessorList.behead();
    }

    // delete targets & participants
    while (session->targetArray.size())
    {
        target_t *target = session->targetArray.elementAt(0);
        session->targetArray.remove(0);
        delete target;
    }

    while (session->participantInfoArray.size())
    {
        participantInfo_t *participant =
            session->participantInfoArray.elementAt(0);

        // delete flow from demux
        flow_t inFlow;
        inFlow.ID = participant->ID;
        inFlow.portList = participant->localPortList;
        inFlow.PT = -1;
        demux->deleteFlow(inFlow, participant->inetAddr);

        // delete participant
        session->participantInfoArray.remove(0);
        delete participant;
    }

    HRESULT hr = statGatherer->removeSession(ID_session);
    errorCheck(hr, "sessionManager_t");

    // remove session from sessionArray
    for (int j = 0; j < sessionArray.size(); j++)
    {
        if (sessionArray.elementAt(j)->ID == session->ID)
        {
            delete session;
            sessionArray.remove(j);
            break;
        }
    }
    return hr;
}

// participants
HRESULT
sessionManager_t::newParticipant(u16 ID_session,
                                 inetAddr_t const &addr,
                                 inetAddr_t *groupAddr,
                                 ql_t<u16> portList,
                                 u16 *ID
                                )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant already exists
    for (int i = 0; i < sessionArray.size(); i++)
    {
        sessionInfo_t *puntero = sessionArray.elementAt(i);
        if (SUCCEEDED(getParticipantByIPPort(puntero, addr, portList)))
        {
            return E_PART_EXISTS;
        }
    }

    // generate an ID
    u16 auxID = 0;
    for (int i = 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *puntero = session->participantInfoArray.elementAt(i);
        if (auxID == puntero->ID)
        {
            auxID++;
            //if (auxID == SSM_ID)
            //    auxID++;
            i = 0; // starts again
        }
    }

    // create participant
    participantInfo_t *participant =
        new participantInfo_t(auxID, addr, groupAddr, portList);

    session->participantInfoArray.add(participant);

    // save ID
    *ID = auxID;

    return S_OK;
}


HRESULT
sessionManager_t::newNatParticipant(u16 ID_session,
                                    ql_t<u16> portList,
                                    u16 *ID
                                   )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if ports are available for nat participants
    for (int i = 0; i < sessionArray.size(); i++)
    {
        sessionInfo_t *puntero = sessionArray.elementAt(i);
        for (int i = 0; i < session->participantInfoArray.size(); i++)
        {
            participantInfo_t *puntero=
                session->participantInfoArray.elementAt(i);
            for (ql_t<u16>::iterator_t j = portList.begin();
                 j != portList.end();
                 j++
                )
            {
                for (ql_t<u16>::iterator_t k = puntero->localPortList.begin();
                     k != puntero->localPortList.end();
                     k++
                    )
                {
                    if (j == k)
                    {
                        return E_NAT_PORT;
                    }
                }
            }
        }
    }

    // generate an ID
    u16 auxID = 0;
    for (int i= 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *puntero = session->participantInfoArray.elementAt(i);
        if (auxID == puntero->ID)
        {
            auxID++;
            i = 0; // starts again
        }
    }

    // create participant
    participantInfo_t *participant = new participantInfo_t(auxID, portList);

    session->participantInfoArray.add(participant);

    // save ID
    *ID = auxID;

    return S_OK;
}


HRESULT
sessionManager_t::removeParticipant(u16 ID_session, u16 ID_part)
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *participant = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // delete targets the participant belongs
    int j = 0;
    for (j = 0; j < session->targetArray.size(); j++)
    {
        target_t *target = session->targetArray.elementAt(j);
        if (target->getID() == participant->ID)
        {
            //-------------------------
            // delete every references
            // to target in the flowProcessors
            //-------------------------

            for (ql_t<flowProcessor_t *>::iterator_t k =
                     session->flowProcessorList.begin();
                 k != session->flowProcessorList.end();
                 k++
                )
            {
                flowProcessor_t *puntero = k;
                puntero->deleteTarget(target);
            }

            session->targetArray.remove(j);
            delete target;
            j--;
        }
    }

    // delete participant
    for (j = 0; j < session->participantInfoArray.size(); j++)
    {
        participantInfo_t *part = session->participantInfoArray.elementAt(j);
        if (ID_part == part->ID)
        {
            flow_t inFlow;
            inFlow.ID = part->ID;
            inFlow.portList = part->localPortList;
            inFlow.PT = -1;
            demux->deleteFlow(inFlow, part->inetAddr);
            delete part;
            session->participantInfoArray.remove(j);
            break;
        }
    }

    return S_OK;
}

HRESULT
sessionManager_t::configureParticipant(u16 ID_session,
                                       u16 ID_part,
                                       bool alive,
                                       int PT,
                                       u8  FEC,
                                       u32 BW
                                      )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *participant = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // find selected target by PT
    for (int j = 0; j < session->targetArray.size(); j++)
    {
        target_t *target = session->targetArray.elementAt(j);

        if (target->getID() == participant->ID && target->getPT() == PT)
        {
            // configure it
            target->setFec(FEC);
            HRESULT hr = target->setAlive(alive);
            errorCheck(hr, "sessionManager_t");
            hr = target->setBW(BW);
            errorCheck(hr, "sessionManager_t");
            return S_OK;
        }
    }

    return E_TARGET_NOT_EXISTS;
}

HRESULT
sessionManager_t::getParticipants(u16 ID_session, char **participants)
{
    // get session
    sessionInfo_t *session = NULL;
    *participants[0] = 0;

    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // get session's participants
    char tmpParticipants[1024];
    memset(tmpParticipants,0,1024);

    for (int i= 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *participant =
            session->participantInfoArray.elementAt(i);

        char str[3];
        sprintf(str, "%d,", participant->ID);
        strcat(tmpParticipants, str);
    }
    if (strlen(tmpParticipants) > 0)
    {
        tmpParticipants[strlen(tmpParticipants)-1]= 0; // delete last ','
        strcpy(*participants, tmpParticipants);
    }
    return S_OK;
}




HRESULT
sessionManager_t::bindRtp(u16 ID_session,
                          u16 ID_part,
                          int PT,
                          u16 remoteRTPport,
                          u16 localRTPport,
                          u16 localRTCPport
                         )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *participant = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    inetAddr_t *destAddr;
    char destPort[128];
    sprintf(destPort,"%d",remoteRTPport);
    if (participant->groupAddr != NULL)
    {
        destAddr = new inetAddr_t(participant->groupAddr->getStrValue(),destPort,SOCK_DGRAM);
    }
    else{
        destAddr = new inetAddr_t(participant->inetAddr.getStrValue(),destPort,SOCK_DGRAM);
    }

    // test if target already exists

    target_t *target = NULL;

    for (int j = 0; j < session->targetArray.size(); j++)
    {
            target_t *t = session->targetArray.elementAt(j);
            if (participant->ID == t->getID()&&
                t->getRemoteRTPport() == remoteRTPport && t->getPT() == PT
               )
            {
                t->incRef();
                return S_OK;
            }
        }

    if (participant->natParticipant)
    {
        target = new target_t(PT,
                              ID_part,
                              localRTPport,
                              localRTCPport
                             );
        target->incRef();
        session->targetArray.add(target);

    }
    else
    {
        if (participant->groupAddr != NULL)
        {
            for (int i = 0; i < session->targetArray.size(); i++)
            {
                target_t *t = session->targetArray.elementAt(i);
                if (destAddr->equalIPAddr(target->getIP()) &&
                    t->getRemoteRTPport() == remoteRTPport && t->getPT() == PT
                   )
                {
                    target = t;
                }
            }

            if (target == NULL)
            {
                target = new target_t(PT,
                                      *destAddr,
                                      ID_part,
                                      remoteRTPport,
                                      localRTPport,
                                      localRTCPport
                                     );
            }
        }
        else
        {
            target = new target_t(PT,
                                  *destAddr,
                                  ID_part,
                                  remoteRTPport,
                                  localRTPport,
                                  localRTCPport
                                 );
            target->incRef();
            session->targetArray.add(target);
        }
    }

    // create target and insert in targetList;
    if ( ! participant->natParticipant)
    {
        participant->remotePortArray.add(remoteRTPport);
    }

    //---------------------------------------------------
    // create RTCP INPUT FLOW
    //---------------------------------------------------
    if ( ! participant->natParticipant && localRTCPport > 0)
    {
        RTCPDemux->addReceiver(participant->inetAddr, PT, localRTCPport);
    }
    return S_OK;
}

HRESULT
sessionManager_t::unbindRtp(u16 ID_session, u16 ID_part, int PT)
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *participant = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if target exists
    for (int j = 0; j < session->targetArray.size(); j++)
    {
        target_t *target = session->targetArray.elementAt(j);
        if (participant->ID == target->getID() && target->getPT() == PT)
        {
            if (participant->natParticipant)
            {
                if (target->decRef() == 0)
                {
                    for (int k= 0; k < sessionArray.size(); k++)
                    {
                        sessionInfo_t *auxSession = sessionArray.elementAt(k);
                        for (ql_t<flowProcessor_t *>::iterator_t l = auxSession->flowProcessorList.begin(); l != auxSession->flowProcessorList.end(); l++)
                        {
                            flowProcessor_t *processor = l;
                            processor->deleteTarget(target);
                        }
                    }

                    RTCPDemux->deleteReceiver(participant->inetAddr,target->getRTCPport());
                    session->targetArray.remove(j);
                    delete target;
                    j--;
                }
            }
            else
            {
                for (int i = 0; i < participant->remotePortArray.size(); i++)
                {
                    if (target->getRemoteRTPport() == participant->remotePortArray.elementAt(i))
                    {
                        if (target->decRef() == 0)
                        {
                            // Look for any references in flowProcessors
                            // if any, delete them.
                            for (int k = 0; k < sessionArray.size(); k++)
                            {
                                sessionInfo_t *auxSession =
                                    sessionArray.elementAt(k);

                                for (ql_t<flowProcessor_t *>::iterator_t l = auxSession->flowProcessorList.begin(); l != auxSession->flowProcessorList.end(); l++)
                                {
                                    flowProcessor_t *processor = l;
                                    processor->deleteTarget(target);
                                }
                            }

                            //--------------------------------------------
                            // Delete REFERENCE TO RTCP INPUT FLOW
                            //--------------------------------------------
                            RTCPDemux->deleteReceiver(participant->inetAddr,target->getRTCPport());
                            session->targetArray.remove(j);
                            delete target;
                            j--;
                        }
                        participant->remotePortArray.remove(i);
                        return S_OK;
                    }
                }
            }
        }
    }
    return E_TARGET_NOT_EXISTS;
}

HRESULT
sessionManager_t::bindRtcp(u16 ID_session,
                           u16 ID_part,
                           u16 localRTPport,
                           u16 remoteRTCPport,
                           u16 localRTCPport
                          )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *participant = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    //----------------------------------------------------------------
    // Create RTCPtarget_t to send RTCP flow generated by localRTPport
    //----------------------------------------------------------------
    HRESULT hr = statGatherer->bindRtcp(ID_session,
                                        participant->inetAddr,
                                        remoteRTCPport,
                                        localRTCPport
                                       );
    return hr;
}

HRESULT
sessionManager_t::unbindRtcp(u16 ID_session,
                             u16 ID_part,
                             u16 localRTPport,
                             u16 remoteRTCPport
                            )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *participant = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    //----------------------------
    // Delete RTCPtarget_t
    //----------------------------
    //HRESULT hr = statGatherer->unbindRTCP(...)

    return S_OK;
}

// codec
HRESULT
sessionManager_t::getCodecs(char *codecs)
{
    int numCodecs;
    char tmpCodecs[1024];
    memset(tmpCodecs, 0, 1024);

    // Get audio codecs
    int  audioPT[128];
    memset(audioPT, 0, 128);
    numCodecs = GetCodecPTList(audioPT, 128);
    for (u16 i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", audioPT[i]);
        strcat(tmpCodecs, str);
    }
    // Get video PTs
    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalCoders];
    numCodecs = vGetCodecFmtList(videoFmts, totalCoders);
    for (u16 i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", getPTByFmt(videoFmts[i]));
        strcat(tmpCodecs, str);
    }
    strcpy(codecs, tmpCodecs);

    delete [] videoFmts;

    return S_OK;
}

// codec
HRESULT
sessionManager_t::getVideoCodecs(char *codecs)
{
    int numCodecs;
    char tmpCodecs[1024];
    memset(tmpCodecs, 0, 1024);

    // Get video PTs
    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalCoders];
    memset(videoFmts, 0, 128);
    numCodecs = vGetCodecFmtList(videoFmts, totalCoders);
    for (u16 i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", getPTByFmt(videoFmts[i]));
        strcat(tmpCodecs, str);
    }
    strcpy(codecs, tmpCodecs);

    delete []videoFmts;

    return S_OK;
}

// codec
HRESULT
sessionManager_t::getAudioCodecs(char *codecs)
{
    int numCodecs;
    char tmpCodecs[1024];
    memset(tmpCodecs, 0, 1024);

     // Get audio codecs
    int  audioPT[128];
    memset(audioPT, 0, 128);
    numCodecs = GetCodecPTList(audioPT, 128);
    for (u16 i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", audioPT[i]);
        strcat(tmpCodecs, str);
    }
    strcpy(codecs, tmpCodecs);

    return S_OK;
}

// flows
HRESULT
sessionManager_t::receive(u16 ID_session,
                          u16 ID_part_rx,
                          u16 ID_part_tx,
                          int PT
                         )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t *part_rx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_rx, &part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t *part_tx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_tx, &part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // find part_rx target
    target_t *target = NULL;
    for (int i = 0; i < session->targetArray.size(); i++)
    {
        target_t *auxTarget = session->targetArray.elementAt(i);
        if (auxTarget->getID() == part_rx->ID &&
            auxTarget->getPT() == PT)
        {
            target = auxTarget;
            break;
        }
    }

    if ( ! target)
    {
        return E_TARGET_NOT_EXISTS;
    }

    // use switcher
    switcher_t *switcher = NULL;
    for (ql_t<flowProcessor_t *>::iterator_t i =
             session->flowProcessorList.begin();
         i != session->flowProcessorList.end();
         i++
        )
    {
        flowProcessor_t *flowProcessor = i;
        if (flowProcessor->getFlowProcessorType() == FLOW_SWITCH)
        {
             switcher = static_cast<switcher_t *>(flowProcessor);
             break;
        }
    }

    if ( ! switcher)
    {
        return E_SWITCHER_NOT_EXISTS;
    }

    // create inFlow
    flow_t inFlow;
    inFlow.ID   = part_tx->ID;
    inFlow.PT   = PT;
    inFlow.portList = part_tx->localPortList;

    demux->putFlow(inFlow,
                   session->flowProcessorList,
                   part_rx->natParticipant,
                   part_tx->inetAddr
                  );

    /*vector_t<u16> aux = session->flowMatrix[part_tx->ID];
    bool found = false;
    for (int iter = 0; iter < aux.size(); iter++)
    {
        u16 id = aux.elementAt(iter);
        if (id == part_rx->ID)
            found = true;
    }

    if ( ! found )
    {
        aux.add(part_rx->ID);
    }

    if (part_tx->groupAddr != NULL && !part_tx->groupJoined)
    {
        demux->joinSSMGroup(part_tx->inetAddr, *part_tx->groupAddr);
        part_tx->groupJoined = true;
    }*/

    // addSwitch
    HRESULT hr = switcher->setFlow(inFlow, target);
    errorCheck(hr, "sessionManager_t");
    return hr;
}

HRESULT
sessionManager_t::discard(u16 ID_session,
                          u16 ID_part_rx,
                          u16 ID_part_tx,
                          int PT
                         )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t *part_rx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_rx, &part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t *part_tx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_tx, &part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // find part_rx target
    target_t *target = NULL;
    for (int i = 0; i < session->targetArray.size(); i++)
    {
        target_t *auxTarget = session->targetArray.elementAt(i);
        if (auxTarget->getID() == part_rx->ID)
        {
            target = auxTarget;
            break;
        }
    }

    if ( ! target)
    {
        return E_TARGET_NOT_EXISTS;
    }

    // use switcher
    switcher_t *switcher = NULL;
    for (ql_t<flowProcessor_t *>::iterator_t i =
             session->flowProcessorList.begin();
         i != session->flowProcessorList.end();
         i++
        )
    {
        flowProcessor_t *flowProcessor = i;
        if (flowProcessor->getFlowProcessorType() == FLOW_SWITCH)
        {
             switcher = static_cast<switcher_t *>(flowProcessor);
             break;
        }
    }

    if ( ! switcher)
    {
        return E_SWITCHER_NOT_EXISTS;
    }

    // delete inFlow from demux
    flow_t inFlow;
    inFlow.ID   = part_tx->ID;
    inFlow.PT   = PT;
    inFlow.portList = part_tx->localPortList;

    // demux->deleteFlow(inFlow);

    // del Switch
    HRESULT hr = switcher->unsetFlow(inFlow, target);
    errorCheck(hr, "sessionManager_t");

    /*vector_t<u16> aux = session->flowMatrix[part_tx->ID];
    for (int i = 0; i < aux.size(); i++)
    {
        if (aux.elementAt(i) == part_rx->ID)
        {
            aux.remove(i);
            break;
        }
    }

    aux = session->flowVideoMatrix[part_tx->ID];
    bool found = false;
    for (int iter = 0; iter < aux.size(); iter++)
    {
        u16 id = aux.elementAt(iter);
        if (id == part_rx->ID)
            found = true;
    }
    if ( ! found)
    {
        aux = session->flowAudioMatrix[part_tx->ID];
        for (int iter = 0; iter < aux.size(); iter++)
        {
            u16 id = aux.elementAt(iter);
            if (id == part_rx->ID)
                found = true;
        }
        if ( ! found && part_tx->groupJoined)
        {
            demux->leaveSSMGroup(part_tx->inetAddr, *part_tx->groupAddr);
            part_tx->groupJoined = false;
        }
    }*/

    return hr;
}

HRESULT
sessionManager_t::receiveVideoMode(u16 ID_session,
                                   u16 ID_part,
                                   videoMode_e mode,
                                   int  PT,
                                   u32  BW,
                                   u8   FR,
                                   u8   Q,
                                   u16  Width,
                                   u16  Height,
                                   u32  SSRC,
                                   gridMode_e gridMode
                                  )
{
    videoInfo_t info;

    info.BW = BW;
    info.FR = FR;
    info.PT = PT;
    info.Q  = Q;
    info.Height = Height;
    info.Width  = Width;
    info.SSRC   = SSRC;
    info.videoMode = mode;
    info.gridMode = gridMode;
    if (BW)
    {
        info.rateMode = CBR;
    }
    else
    {
        info.rateMode = VBR;
    }

    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *part = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &part)))
    {
        return E_PART_NOT_EXISTS;
    }

    part->videoInfo = info;
    return S_OK;
}


HRESULT
sessionManager_t::receiveVideo(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx,
                               int  PT,
                               u32  BW,
                               u8   FR,
                               u8   Q,
                               u16  Width,
                               u16  Height,
                               u32  SSRC
                              )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t *part_rx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_rx, &part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t *part_tx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_tx, &part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every video codec set flowProcessor
    // find part_rx target
    HRESULT hr = S_OK;
    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalCoders];
    int numCodecs = vGetCodecFmtList(videoFmts, totalCoders);
    for (u16 i = 0; i < numCodecs; i++)
    {
        u8 _PT = getPTByFmt(videoFmts[i]);
        target_t *target = NULL;

        for (int j = 0; j < session->targetArray.size(); j++)
        {
            target_t *auxTarget = session->targetArray.elementAt(j);
            if (auxTarget->getID() == part_rx->ID)
            {
                if (part_rx->videoInfo.PT > -1)  // if must transcode or grid
                {
                    if (auxTarget->getPT() == part_rx->videoInfo.PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
                else
                { // else (switch)
                    if (auxTarget->getPT() == _PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
            }
        }

        if ( ! target)
        {
            NOTIFY("sessionManager_t::receiveVideo: no target for PT = %d\n",_PT);
            continue;
        }

        // create inFlow
        flow_t inFlow;
        inFlow.ID   = part_tx->ID;
        inFlow.PT   = _PT;
        inFlow.portList = part_tx->localPortList;

        // create flow at demux
        demux->putFlow(inFlow,
                       session->flowProcessorList,
                       part_tx->natParticipant,
                       part_tx->inetAddr
                      );

        flowProcessor_t *flowProcessor = NULL;
        int flowType = -1;

        switch (part_rx->videoInfo.videoMode)
        {
        case VIDEO_TRANS_MODE:
            flowType = FLOW_VIDEO_TRANS;
            break;
        case VIDEO_SWITCH_MODE:
            flowType = FLOW_SWITCH;
            break;
        case VIDEO_SSRC_CHANGE_MODE:
            flowType = FLOW_SWITCH_SSRC;
            break;
        case VIDEO_GRID_MODE: // if must mix
            flowType = FLOW_GRID;
            break;
        case VIDEO_GRID2_MODE: // if must mix
            flowType = FLOW_GRID2;
            break;
        default:
            NOTIFY("sessionManager_t::receive_video: "
                   "participant with unknown video mode [%d].Aborting...\n",
                   part_rx->videoInfo.videoMode
                  );
            abort();
        }

        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor = k;
            if (flowProcessor->getFlowProcessorType() == flowType)
            {
                break;
            }
        }
        if (flowProcessor)
        {
            switch (flowType)
            {
            case FLOW_GRID:
            case FLOW_GRID2:
                flowProcessor->setFlow(inFlow,
                                        target,
                                        target->getPT(),
                                        part_rx->videoInfo.BW,
                                        part_rx->videoInfo.Width,
                                        part_rx->videoInfo.Height,
                                        part_rx->videoInfo.FR,
                                        part_rx->videoInfo.Q,
                                        part_rx->videoInfo.SSRC,
                                        part_rx->videoInfo.gridMode
                                       );
                break;
            case FLOW_VIDEO_TRANS:
                if (PT     == -1) PT     = part_rx->videoInfo.PT;
                if (BW     ==  0) BW     = part_rx->videoInfo.BW;
                if (Width  ==  0) Width  = part_rx->videoInfo.Width;
                if (Height ==  0) Height = part_rx->videoInfo.Height;
                if (FR     ==  0) FR     = part_rx->videoInfo.FR;
                if (Q      ==  0) Q      = part_rx->videoInfo.Q;
                if (SSRC   ==  0) SSRC   = part_rx->videoInfo.SSRC;
                flowProcessor->setFlow(inFlow,
                                       target,
                                       PT,
                                       BW,
                                       Width,
                                       Height,
                                       FR,
                                       Q,
                                       SSRC
                                      );
                break;
            case FLOW_SWITCH:
                flowProcessor->setFlow(inFlow, target);
                break;
            case FLOW_SWITCH_SSRC:
                if (SSRC   ==  0) SSRC   = part_rx->videoInfo.SSRC;
                flowProcessor->setFlow(inFlow,
                                       target,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       SSRC                                       
                                      );
                break;
            default:
                NOTIFY("receiveVideo :: unknown flowtype. Aborting...\n");
                abort();
            }
        }
        else
        {
            NOTIFY("sessionManager_t :: receiveVideo :: no flowProcessor\n"
                   "Something is going bad, the bug is near...\n"
                   "Aborting...\n"
                  );
            abort();
        }
    } // end for
    delete [] videoFmts;

    // Me uno al grupo SSM si es participante de SSM Multicast
    if (part_tx->groupAddr != NULL && !part_tx->groupJoined)
    {
        demux->joinSSMGroup(part_tx->inetAddr, *part_tx->groupAddr);
        part_tx->groupJoined = true;
    }
/*
    vector_t<u16> aux = session->flowVideoMatrix[part_tx->ID];
    bool found = false;
    for (int iter = 0; iter < aux.size(); iter++)
    {
        u16 id = aux.elementAt(iter);
        if (id == part_rx->ID)
            found = true;
    }
    if ( ! found )
    {
        aux.add(part_rx->ID);
    }
*/
    return hr;
}

HRESULT
sessionManager_t::discardVideo(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx
                              )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t *part_rx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_rx, &part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t *part_tx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_tx, &part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every video codec set flowProcessor
    // find part_rx target
    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalCoders];
    int numCodecs = vGetCodecFmtList(videoFmts, totalCoders);
    for (u16 i = 0; i < numCodecs; i++)
    {
        u8 _PT = getPTByFmt(videoFmts[i]);
        target_t *target = NULL;

        for (int j = 0; j < session->targetArray.size(); j++)
        {
            target_t *auxTarget = session->targetArray.elementAt(j);
            if (auxTarget->getID() == part_rx->ID)
            {
                if (part_rx->videoInfo.PT > 0) // if must transcode or grid
                {
                    if (auxTarget->getPT() == part_rx->videoInfo.PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
                else
                { // else (switch)
                    if (auxTarget->getPT() == _PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
            }
        }

        if ( ! target)
        {
            continue;
        }

        // create inFlow
        flow_t inFlow;
        inFlow.ID   = part_tx->ID;
        inFlow.PT   = _PT; // target->getPT();
        inFlow.portList = part_tx->localPortList;

        // demux->deleteFlow(inFlow);

        // now we must discard audio input flow
        // from correct processor at every flowProcessor
        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor_t *processor = k;
            processor->unsetFlow(inFlow, target);
        } // end switch
    } // end for

    delete [] videoFmts;

    /*vector_t<u16> aux = session->flowVideoMatrix[part_tx->ID];
    for (int i = 0; i < aux.size(); i++)
    {
        if (aux.elementAt(i) == part_rx->ID)
        {
            aux.remove(i);
            break;
        }
    }

    aux = session->flowMatrix[part_tx->ID];
    bool found = false;
    for (int iter = 0; iter < aux.size(); iter++)
    {
        u16 id = aux.elementAt(iter);
        if (id == part_rx->ID)
            found = true;
    }
    if ( ! found)
    {
        aux = session->flowAudioMatrix[part_tx->ID];
        for (int iter = 0; iter < aux.size(); iter++)
        {
            u16 id = aux.elementAt(iter);
            if (id == part_rx->ID)
                found = true;
        }
        if ( ! found && part_tx->groupJoined)
        {
            demux->leaveSSMGroup(part_tx->inetAddr, *part_tx->groupAddr);
            part_tx->groupJoined = false;
        }
    }*/

    return S_OK;
}

HRESULT
sessionManager_t::receiveAudioMode(u16 ID_session,
                                   u16 ID_part,
                                   audioMode_e mode,
                                   int PT,
                                   int SL,
                                   int SSRC
                                  )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *part = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &part)))
    {
        return E_PART_NOT_EXISTS;
    }

    // If we were mixing and continue mixing
    // then only change output PT of mixer

    part->audioInfo.audioMode = mode;
    part->audioInfo.PT        = PT;
    part->audioInfo.SSRC      = SSRC;

    if (mode == AUDIO_MIXER_MODE &&
        part->audioInfo.audioMode == AUDIO_MIXER_MODE
       )
    {
        //---------------------------------
        // PT change of mixer
        //---------------------------------
        mixer_t *mixer = NULL;
        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor_t *flowProcessor = k;
            if (flowProcessor->getFlowProcessorType() == FLOW_MIX)
            {
                mixer = static_cast<mixer_t *>(flowProcessor);
                break;
            }
        }

        if ( ! mixer)
        {
            return E_MIXER_NOT_EXISTS;
        }

        // change PT
        HRESULT hr;
        hr = mixer->setPT(PT, part->ID, session->ID, part->audioInfo.PT);
        errorCheck(hr, "sessionManager_t");
        hr = mixer->setSilenceLevel(SL);
        errorCheck(hr, "sessionManager_t");
    }

    // If we were transcoding and continue transcoding
    // then only change output PT of transcoder

    if (mode == AUDIO_SWITCH_MODE &&
        part->audioInfo.audioMode == AUDIO_SWITCH_MODE &&
        PT >= 0 &&
        part->audioInfo.PT >=0
       )
    {
        //---------------------------------
        // PT change of transcoder
        //---------------------------------
        audioTrans_t *audioTrans = NULL;
        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor_t *flowProcessor = k;
            if (flowProcessor->getFlowProcessorType() == FLOW_AUDIO_TRANS)
            {
                audioTrans = static_cast<audioTrans_t *>(flowProcessor);
                break;
            }
        }

        if ( ! audioTrans)
        {
            return E_TRANSCODER_NOT_EXISTS;
        }

        // change PT
        HRESULT hr;
        hr = audioTrans->setPT(PT,
                               part->ID,
                               &part->localPortList,
                               part->audioInfo.PT
                              );
        audioTrans->setSSRC(SSRC, part->ID, &part->localPortList);
        errorCheck(hr, "sessionManager_t");
        hr = audioTrans->setSilenceLevel(SL);
        errorCheck(hr, "sessionManager_t");
    }

    return S_OK;
}

HRESULT
sessionManager_t::receiveAudio(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx,
                               int SSRC
                              )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session ,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t *part_rx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_rx, &part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t *part_tx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_tx, &part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every audio codec set flowProcessor
    // find part_rx target
    HRESULT hr = S_OK;
    for (u16 i = 0; GetCodecFactory()[i].name != NULL; i++)
    {
        int PT = (u8)GetCodecFactory()[i].pt;
        target_t *target = NULL;

        for (int j = 0; j < session->targetArray.size(); j++)
        {
            target_t *auxTarget = session->targetArray.elementAt(j);
            if (auxTarget->getID() == part_rx->ID)
            {
                if (part_rx->audioInfo.PT > 0) // if must transcode or grid
                {
                    if (auxTarget->getPT() == part_rx->audioInfo.PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
                else
                { // else (switch)
                    if (auxTarget->getPT() == PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
            }
        }

        if ( ! target)
        {
            NOTIFY("sessionManager_t::receiveAudio: no target for PT = %d\n",PT);
            continue;
        }

        // create inFlow
        flow_t inFlow;
        inFlow.ID   = part_tx->ID;
        inFlow.PT   = PT;
        inFlow.portList = part_tx->localPortList;

        // create flow at demux
        demux->putFlow(inFlow,
                       session->flowProcessorList,
                       part_tx->natParticipant,
                       part_tx->inetAddr
                      );

        flowProcessor_t *flowProcessor = NULL;
        int flowType = -1;

        switch (part_rx->audioInfo.audioMode)
        {
        case AUDIO_SWITCH_MODE:
            if (part_rx->audioInfo.PT >= 0) // if must transcode
            {
                flowType = FLOW_AUDIO_TRANS;
            }
            else
            { // else must switch
                flowType = FLOW_SWITCH;
            }
            break;
        case AUDIO_MIXER_MODE: // if must mix
            flowType = FLOW_MIX;
            break;
        case AUDIO_SWITCH_SSRC_MODE:
            flowType = FLOW_SWITCH_SSRC;
            break;
        default:
            NOTIFY("receive_audio:: participant with unknown audio mode [%d]."
                   "Aborting...\n",
                   part_rx->audioInfo.audioMode
                  );
            abort();
        } // end switch

        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor = k;
            if (flowProcessor->getFlowProcessorType() == flowType)
            {
                break;
            }
        }
        if (flowProcessor)
        {
            if (SSRC == 0)
            {
                SSRC = part_rx->audioInfo.SSRC;
            }
            flowProcessor->setFlow(inFlow,target,target->getPT(),0,0,0,0,0,SSRC);
        }
        else
        {
            NOTIFY("sessionManager_t :: receiveAudio :: no flowProcessor\n"
                   "Something is going bad, the bug is near...\n"
                   "Aborting...\n"
                  );
            abort();
        }
    } // end for

    // Me uno al grupo SSM si es participante de SSM Multicast
    if (part_tx->groupAddr != NULL && !part_tx->groupJoined)
    {
        demux->joinSSMGroup(part_tx->inetAddr, *part_tx->groupAddr);
        part_tx->groupJoined = true;
    }

    /*vector_t<u16> aux = session->flowAudioMatrix[part_tx->ID];
    bool found = false;
    for (int iter = 0; iter < aux.size(); iter++)
    {
        u16 id = aux.elementAt(iter);
        if (id == part_rx->ID)
            found = true;
    }
    if ( ! found )
    {
        aux.add(part_rx->ID);
    }
    */
    return hr;
}

HRESULT
sessionManager_t::discardAudio(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx
                              )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t *part_rx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_rx, &part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t *part_tx = NULL;
    if (FAILED(getParticipantByID(session, ID_part_tx, &part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every audio codec set flowProcessor
    for (u16 i = 0; GetCodecFactory()[i].name != NULL; i++)
    {
        int PT = (u8)GetCodecFactory()[i].pt;
        target_t *target = NULL;

        for (int j = 0; j < session->targetArray.size(); j++)
        {
            target_t *auxTarget = session->targetArray.elementAt(j);
            if (auxTarget->getID() == part_rx->ID)
            {
                if (part_rx->audioInfo.PT > 0)  // if must transcode or grid
                {
                    if (auxTarget->getPT() == part_rx->audioInfo.PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
                else
                { // else (switch)
                    if (auxTarget->getPT() == PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
            }
        }
        if ( ! target)
        {
            continue;
        }

        // create inFlow
        flow_t inFlow;
        inFlow.ID   = part_tx->ID;
        inFlow.PT   = PT;
        inFlow.portList = part_tx->localPortList;

        //demux->deleteFlow(inFlow);

        // now we must discard audio input flow
        // from correct processor at every flowProcessor
        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor_t *processor = k;
            processor->unsetFlow(inFlow, target);
        } // end switch
    } // end for
/*
    vector_t<u16> aux = session->flowAudioMatrix[part_tx->ID];
    for (int i = 0; i < aux.size(); i++)
    {
        if (aux.elementAt(i) == part_rx->ID)
        {
            aux.remove(i);
            break;
        }
    }

    aux = session->flowVideoMatrix[part_tx->ID];
    bool found = false;
    for (int iter = 0; iter < aux.size(); iter++)
    {
        u16 id = aux.elementAt(iter);
        if (id == part_rx->ID)
            found = true;
    }
    if ( ! found)
    {
        aux = session->flowMatrix[part_tx->ID];
        for (int iter = 0; iter < aux.size(); iter++)
        {
            u16 id = aux.elementAt(iter);
            if (id == part_rx->ID)
                found = true;
        }
        if ( ! found && part_tx->groupJoined)
        {
            demux->leaveSSMGroup(part_tx->inetAddr, *part_tx->groupAddr);
            part_tx->groupJoined = false;
        }
    }
*/
    return S_OK;
}

// DTE methods
HRESULT
sessionManager_t::getAudioLosses(u16 ID_session, u16 ID_part, double *losses)
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *part = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &part)))
    {
        return E_PART_NOT_EXISTS;
    }

    *losses = RTCPDemux->getLosses(AUDIO_FLOW, part->inetAddr);

    return S_OK;
}


HRESULT
sessionManager_t::getVideoLosses(u16 ID_session, u16 ID_part, double *losses)
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *part = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &part)))
    {
        return E_PART_NOT_EXISTS;
    }

    *losses = RTCPDemux->getLosses(VIDEO_FLOW, part->inetAddr);

    return S_OK;
}

HRESULT
sessionManager_t::getLosses(u16 ID_session, u16 ID_part, double *losses)
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *part = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &part)))
    {
        return E_PART_NOT_EXISTS;
    }

    *losses = RTCPDemux->getLosses(UNKNOWN_FLOW, part->inetAddr);

    return S_OK;
}

HRESULT
sessionManager_t::configAudioPart(u16 ID_session,
                                  u16 ID_part,
                                  u8  oldPT,
                                  u8  newPT
                                 )
{
    // get session
    sessionInfo_t *session = NULL;
    if (FAILED(getSession(ID_session, &session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t *part = NULL;
    if (FAILED(getParticipantByID(session, ID_part, &part)))
    {
        return E_PART_NOT_EXISTS;
    }

    switch (part->audioInfo.audioMode)
    {
    case AUDIO_SWITCH_SSRC_MODE:
        return S_OK;

    case AUDIO_MIXER_MODE:
        {
            // if we are mixing, then change output PT
            mixer_t *mixer = NULL;
            for (ql_t<flowProcessor_t *>::iterator_t k =
                     session->flowProcessorList.begin();
                 k != session->flowProcessorList.end();
                 k++
                )
            {
                flowProcessor_t *flowProcessor = k;
                if (flowProcessor->getFlowProcessorType() == FLOW_MIX)
                {
                    mixer = static_cast<mixer_t *>(flowProcessor);
                    break;
                }
            }

            if ( ! mixer)
            {
                    return E_MIXER_NOT_EXISTS;
            }

            // change PT
            HRESULT hr = mixer->setPT(newPT,
                                      part->ID,
                                      session->ID,
                                      oldPT
                                     );
            errorCheck(hr, "sessionManager_t");
            break;
        } // case AUDIO_MIXER_MODE end

    case AUDIO_SWITCH_MODE:
        {
            // if we are transcoding, then change every output PT
            audioTrans_t *audioTrans = NULL;
            for (ql_t<flowProcessor_t *>::iterator_t k =
                     session->flowProcessorList.begin();
                 k != session->flowProcessorList.end();
                 k++
                )
            {
                 flowProcessor_t *flowProcessor = k;
                 if (flowProcessor->getFlowProcessorType() == FLOW_AUDIO_TRANS)
                 {
                     audioTrans = static_cast<audioTrans_t *>(flowProcessor);
                     break;
                 }
            }

            if ( ! audioTrans)
            {
                 return E_TRANSCODER_NOT_EXISTS;
            }

            if (part->audioInfo.PT >= 0) // if we're transcoding
            {
                 // chage oldPT-->newPT
                 HRESULT hr = audioTrans->setPT(newPT,
                                                part->ID,
                                                &part->localPortList,
                                                oldPT
                                               );
                 errorCheck(hr, "sessionManager_t");
            }
            else
            {    // if we are only switching
                 // For now:: configureAudio only works changing outgoing
                 //           PT at mixer or transcoder, not used with
                 //           switcher!
                 return E_ERROR;
            }

            break;
        } // case AUDIO_SWITCH_MODE end
    } // switch mode end

    return E_NOT_IMPLEMENTED;
}

HRESULT
sessionManager_t::configVideoCBRPart(u16 ID_session,
                                     u16 ID_part,
                                     u8  oldPT,
                                     u8  newPT,
                                     u32 BW
                                    )
{
    return E_NOT_IMPLEMENTED;
}

HRESULT
sessionManager_t::configVideoVBRPart(u16 ID_session,
                                     u16 ID_part,
                                     u8  oldPT,
                                     u8  newPT,
                                     double FR,
                                     u8  Q,
                                     u16 Width,
                                     u16 Height
                                    )
{
    return E_NOT_IMPLEMENTED;
}

