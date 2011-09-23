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
// $Id: sessionManager.cc 7506 2005-10-20 13:29:33Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <aCodecs/codecs.h>

#include <rtp/RTPPayloads.hh>

#include "sessionManager.h"
#include "controlProtocol.h"
#include "grid.h"
#include "grid2.h"
#include "mixer.h"
#include "switch.h"
#include "audioTrans.h"
#include "videoTrans.h"

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
    BW        = -1; // unlimited
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
                                                       sockaddr_storage const& addr,
                                                       ql_t<u16> portList
                                                       )
{
    this->ID = ID;
    this->portList = portList;
    memcpy(&IP,&addr,sizeof(sockaddr_storage));
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
: application_t(argc, argv, 1024)
{
    APP= this;
    
    noSL = 0;

    optionDefList_t opt;
    appParamList_t *parList;

    streamSocket_t *serverSock= NULL;

    enum myOptions
    {
       cport,
       notify,
       nosl,
       h
       // write options here
       // ...
    };

    // Available options
    opt
       << new optionDef_t("@cport", cport , "Opens a Control Socket in <port>")
       << new optionDef_t("@notify", notify , "Notify file")
       << new optionDef_t("nosl", nosl , "No silence detection")
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

        case nosl:
            NOTIFY("Silence Level deactivated\n");
            noSL = 1;
            break;
        case h:
        default:
            for (optionDefList_t::iterator_t i = opt.begin();
                 i != opt.end();
                 i++
                )
            {
                optionDef_t *op= static_cast<optionDef_t*>(i);
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
    // delete targets
    while (targetArray.size())
    {
        target_t *tgt= targetArray.elementAt(0);
        targetArray.remove(0);
        delete tgt;
    }

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
sessionManager_t::getSession(u16 ID_session, sessionInfo_t ** session)
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
sessionManager_t::getParticipantByID(sessionInfo_t * session,
                                     u16 ID_part,
                                     participantInfo_t ** participant
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
                return S_OK;
            }
        }
    }
    return E_PART_NOT_EXISTS;
}

HRESULT
sessionManager_t::getParticipantByIPPort(sessionInfo_t *session,
                                         sockaddr_storage const& IP,
                                         ql_t<u16> portList,
                                         participantInfo_t ** participant
                                        )
{
    for (int i = 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *puntero= session->participantInfoArray.elementAt(i);
        if (addrEquals(IP, puntero->IP))
        {
            for (ql_t<u16>::iterator_t j = portList.begin();
                 j != portList.end();
                 j++
                )
            {
                u16 puertoUno = j;
                for (ql_t<u16>::iterator_t k = puntero->portList.begin();
                     k != puntero->portList.end();
                     k++
                    )
                {
                    u16 puertoDos = k;
                    if (puertoUno == puertoDos)
                    {
                        if (participant)
                        {
                            *participant = puntero;
                            return S_OK;
                        }
                    }
                }
            }
        }
    }

    return E_PART_NOT_EXISTS;
}

HRESULT
sessionManager_t::getSessionByIPPort(sockaddr_storage const& IP,
                                     u16 port,
                                     int *ID
                                    )
{
    for (int i = 0; i < sessionArray.size(); i++)
    {
        sessionInfo_t *session = sessionArray.elementAt(i);
        for (int j = 0; j < session->participantInfoArray.size(); j++)
        {
            participantInfo_t *part=
                session->participantInfoArray.elementAt(j);

            if (addrEquals(IP, part->IP))
            {
                for (ql_t<u16>::iterator_t k = part->portList.begin();
                     k != part->portList.end();
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
    switcher_t * switcher = new switcher_t;
    mixer_t * mixer = new mixer_t;
    grid_t * grid = new grid_t;
    grid2_t * grid2 = new grid2_t;
    audioTrans_t * audioTrans = new audioTrans_t;
    videoTrans_t * videoTrans = new videoTrans_t;

    // create session
    sessionInfo_t * session = new sessionInfo_t(ID_session);

    session->flowProcessorList.insert(switcher);
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
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // delete flowProcessors
    while(session->flowProcessorList.len())
    {
        delete session->flowProcessorList.head();
        session->flowProcessorList.behead();
    }

    // delete targets & participants
    while (targetArray.size())
    {
        target_t *target = targetArray.elementAt(0);
        targetArray.remove(0);
        delete target;
    }

    while (session->participantInfoArray.size())
    {
        participantInfo_t *participant =
            session->participantInfoArray.elementAt(0);

        // delete flow from demux
        flow_t inFlow;
        inFlow.IP = participant->IP;
        inFlow.ID = participant->ID;
        inFlow.portList = participant->portList;
        inFlow.PT = -1;
        demux->deleteFlow(inFlow);

        // delete participant
        delete participant;
        session->participantInfoArray.remove(0);
    }

    HRESULT hr = statGatherer->removeSession(ID_session);
    errorCheck(hr, "sessionManager_t");

    // remove session from sessionList
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
                                 sockaddr_storage const& ip,
                                 ql_t<u16> portList,
                                 u16 *ID
                                )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant already exists
    if (SUCCEEDED(getParticipantByIPPort(session,ip,portList)))
    {
        return E_PART_EXISTS;
    }

    // generate an ID
    u16 auxID = 0;
    for (int i = 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *part= session->participantInfoArray.elementAt(i);
        if (auxID == part->ID)
        {
            auxID++;
            i = 0; // starts again
        }
    }

    // create participant
    participantInfo_t * participant = new participantInfo_t(auxID,ip,portList);

    session->participantInfoArray.add(participant);

    // save ID
    *ID = auxID;

    return S_OK;
}

HRESULT
sessionManager_t::removeParticipant(u16 ID_session, u16 ID_part)
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * participant = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // delete targets the participant belongs
    int j = 0;
    for (j = 0; j < targetArray.size(); j++)
    {
        target_t *target = targetArray.elementAt(j);
        if (addrEquals(participant->IP, target->getIP()))
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
                flowProcessor_t *flowProc = k;
                flowProc->deleteTarget(target);
            }

            targetArray.remove(j);
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
            inFlow.IP = part->IP;
            inFlow.ID = part->ID;
            inFlow.portList = part->portList;
            inFlow.PT = -1;

            demux->deleteFlow(inFlow);
            session->participantInfoArray.remove(j);
    
            // unset all flows in every flowprocessor whose input is the
            // participant that is going to be deleted.
            // delete targets the participant belongs
            for (j = 0; j < targetArray.size(); j++)
            {
                target_t *target = targetArray.elementAt(j);
                for (ql_t<flowProcessor_t *>::iterator_t iter = session->flowProcessorList.begin();
                     iter != session->flowProcessorList.end(); iter++)
                {
                    flowProcessor_t *flowProcessor = iter;
                    flowProcessor->unsetFlow(inFlow,target);    
                }
            }
    
            delete part;
            break;
        }
    }

    return S_OK;
}

HRESULT
sessionManager_t::configureParticipant(u16 ID_session,
                                       u16 ID_part,
                                       bool alive,
                                       u8  PT,
                                       u8  FEC,
                                       u32 BW
                                      )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * participant = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // find selected target by PT
    for (int j = 0; j < targetArray.size(); j++)
    {
        target_t *target = targetArray.elementAt(j);

        if (addrEquals(participant->IP, target->getIP()) &&
            target->getPT() == PT
           )
        {
            // configure it
            HRESULT hr = target->setFec(FEC);
            errorCheck(hr, "sessionManager_t");
            hr = target->setAlive(alive);
            errorCheck(hr, "sessionManager_t");
            hr = target->setBW(BW);
            errorCheck(hr, "sessionManager_t");
            return S_OK;
        }
    }

    return E_TARGET_NOT_EXISTS;
}

HRESULT
sessionManager_t::getParticipants(u16 ID_session, char ** participants)
{
    // get session
    sessionInfo_t * session = NULL;
    *participants[0] = 0;

    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }
    // get session's participants
    //
    char tmpParticipants[1024];
    memset(tmpParticipants,0,1024);

    for (int i= 0; i < session->participantInfoArray.size(); i++)
    {
        participantInfo_t *participant =
            session->participantInfoArray.elementAt(i);

        char str[30];
        sprintf(str, "%d,", participant->ID);
        strcat(tmpParticipants,str);
    }
    if (strlen(tmpParticipants)>0)
    {
        tmpParticipants[strlen(tmpParticipants)-1]=0; // delete last ','
        strcpy(*participants,tmpParticipants);
    }
    return S_OK;
}




HRESULT
sessionManager_t::bindRtp(u16 ID_session,
                          u16 ID_part,
                          u8  PT,
                          u16 remoteRTPport,
                          u16 localRTPport,
                          u16 localRTCPport
                         )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * participant = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if target already exists
    for (int j = 0; j < targetArray.size(); j++)
    {
        target_t *t = targetArray.elementAt(j);
        if (addrEquals(participant->IP, t->getIP()) &&
            t->getPT() == PT
           )
        {
            t->incRef();
            return S_OK;
        }
    }

    // create target and insert in targetList;
    target_t *target = new target_t(PT,
                                    participant->IP,
                                    remoteRTPport,
                                    localRTPport,
                                    localRTCPport
                                   );
    target->incRef();
    targetArray.add(target);


    //---------------------------------------------------
    // create RTCP INPUT FLOW
    //---------------------------------------------------
    if (localRTCPport > 0)
    {
        RTCPDemux->addReceiver(participant->IP, PT, localRTCPport);
    }

    return S_OK;
}

HRESULT
sessionManager_t::unbindRtp(u16 ID_session, u16 ID_part, u8  PT)
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * participant = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if target exists
    for (int j = 0; j < targetArray.size(); j++)
    {
        target_t *target = targetArray.elementAt(j);

        if (addrEquals(participant->IP, target->getIP()) &&
            target->getPT() == PT
           )
        {
            if (target->decRef() == 0)
            {
                // Look for any references in flowProcessors
                // if any, delete them.
                for (int k= 0; k < sessionArray.size(); k++)
                {
                    sessionInfo_t *auxSession = sessionArray.elementAt(k);

                    for (u16 i = 0; i<auxSession->flowProcessorList.len(); i++)
                    for (ql_t<flowProcessor_t *>::iterator_t iter =
                            auxSession->flowProcessorList.begin();
                         iter != auxSession->flowProcessorList.end();
                         iter++
                        )
                    {
                        flowProcessor_t *processor = iter;
                        processor->deleteTarget(target);
                    }
                }

                //--------------------------------------------
                // Delete REFERENCE TO RTCP INPUT FLOW
                //--------------------------------------------
                RTCPDemux->deleteReceiver(participant->IP,target->getRTCPport());
                targetArray.remove(j);
                delete target;
                j--;
            }
            return S_OK;
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
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * participant = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    sockaddr_storage * IP =(sockaddr_storage *)&participant->IP;
    sockaddr_in  * IP4 = NULL;
    sockaddr_in6 * IP6 = NULL;

    switch (IP->ss_family)
    {
    case AF_INET:
        {
            IP4 = (sockaddr_in *)(IP);
            // port field = 0
            IP4->sin_port = localRTPport;
            break;
        }
    case AF_INET6:
        {
            IP6 = (sockaddr_in6 *)(IP);
            IP6->sin6_port = localRTPport;
            break;
        }
    default:
        NOTIFY("portListener_t::IOReady::Unknown family [%d]\n",IP->ss_family);
        abort();
    }

    //----------------------------------------------------------------
    // Create RTCPtarget_t to send RTCP flow generated by localRTPport
    //----------------------------------------------------------------
    HRESULT hr = statGatherer->bindRtcp(ID_session,
                                        *IP,
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
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * participant = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&participant)))
    {
        return E_PART_NOT_EXISTS;
    }

    //----------------------------
    // Delete RTCPtarget_t
    //----------------------------
    // HRESULT hr = statGatherer->unbindRTCP(...)

    return S_OK;
}

// codec
HRESULT
sessionManager_t::getCodecs(char *codecs)
{
    int numTotal, numCodecs;
    char tmpCodecs[1024];
    memset(tmpCodecs,0,1024);

    // Get audio PTs
    numTotal= aGetNumRegisteredCoders();
    int *audioFmts= new int[numTotal] ;
    numCodecs = aGetCodecFmtList(audioFmts, numTotal);
    for (int i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", getPTByFmt(audioFmts[i]));
        strcat(tmpCodecs, str);
    }

    // Get video PTs
    numTotal= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[numTotal];
    numCodecs = vGetCodecFmtList(videoFmts, numTotal);
    for (int i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", getPTByFmt(videoFmts[i]));
        strcat(tmpCodecs, str);
    }
    strcpy(codecs, tmpCodecs);

    delete [] audioFmts;
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
    int totalFormats= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalFormats];
    numCodecs = vGetCodecFmtList(videoFmts, totalFormats);
    for (int i = 0; i < numCodecs; i++)
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
    int totalFormats= aGetNumRegisteredCoders();
    int *audioFmts= new int[totalFormats];
    numCodecs = aGetCodecFmtList(audioFmts, totalFormats);
    for (int i = 0; i < numCodecs; i++)
    {
        char str[5];
        sprintf(str, "%d,", getPTByFmt(audioFmts[i]));
        strcat(tmpCodecs, str);
    }
    strcpy(codecs, tmpCodecs);

    delete []audioFmts;

    return S_OK;
}

// flows
HRESULT
sessionManager_t::receive(u16 ID_session,
                          u16 ID_part_rx,
                          u16 ID_part_tx,
                          u8  PT,
                          u32 SSRC
                         )
{    
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t * part_rx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_rx,&part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t * part_tx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_tx,&part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // find part_rx target
    target_t *target = NULL;
    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *auxTarget = targetArray.elementAt(i);
        if (addrEquals(part_rx->IP, auxTarget->getIP()) &&
            auxTarget->getPT() == PT
           )
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
    switcher_t * switcher = NULL;
    for (ql_t<flowProcessor_t *>::iterator_t iter =
             session->flowProcessorList.begin();
         iter != session->flowProcessorList.end();
         iter++
        )
    {
        flowProcessor_t *flowProcessor = iter;
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
    inFlow.IP   = part_tx->IP;
    inFlow.PT   = PT;
    inFlow.portList = part_tx->portList;


    demux->putFlow(inFlow,session->flowProcessorList);

    // addSwitch
    HRESULT hr = switcher->setFlow(inFlow,target,0,0,0,0,0,0,SSRC);
    errorCheck(hr, "sessionManager_t");

    return hr;
}

HRESULT
sessionManager_t::discard(u16 ID_session,
                          u16 ID_part_rx,
                          u16 ID_part_tx,
                          u8  PT
                         )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t * part_rx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_rx,&part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t * part_tx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_tx,&part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // find part_rx target
    target_t *target = NULL;
    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *auxTarget = targetArray.elementAt(i);
        if (addrEquals(part_rx->IP, auxTarget->getIP()) &&
            auxTarget->getPT() == PT
           )
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
    switcher_t * switcher = NULL;
    for (ql_t<flowProcessor_t *>::iterator_t iter =
             session->flowProcessorList.begin();
         iter != session->flowProcessorList.end();
         iter++
        )
    {
        flowProcessor_t *flowProcessor = iter;

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
    inFlow.IP   = part_tx->IP;
    inFlow.PT   = PT;
    inFlow.portList = part_tx->portList;

    // demux->deleteFlow(inFlow);

    // del Switch
    HRESULT hr = switcher->unsetFlow(inFlow,target);
    errorCheck(hr, "sessionManager_t");

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
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * part = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&part)))
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
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t * part_rx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_rx,&part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t * part_tx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_tx,&part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every video codec set flowProcessor
    // find part_rx target
    HRESULT hr = S_OK;
    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalCoders];
    int numCodecs = vGetCodecFmtList(videoFmts, totalCoders);
    for (int i = 0; i < numCodecs; i++)
    {
        u8 _PT = getPTByFmt(videoFmts[i]);
        target_t *target = NULL;

        for (int j = 0; j < targetArray.size(); j++)
        {
            target_t *auxTarget = targetArray.elementAt(j);

            if (addrEquals(part_rx->IP, auxTarget->getIP()))
            {
                if (part_rx->videoInfo.PT > -1) // if must transcode or grid
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
            NOTIFY("sessionManager_t :: receiveVideo :: Part ID = %d no target for PT = %d\n",part_tx->ID, _PT);
            continue;
        }

        // create inFlow
        flow_t inFlow;
        inFlow.ID   = part_tx->ID;
        inFlow.IP   = part_tx->IP;
        inFlow.PT   = _PT;
        inFlow.portList = part_tx->portList;

        // create flow at demux
        demux->putFlow(inFlow,session->flowProcessorList);

        int flowType = -1;

        switch (part_rx->videoInfo.videoMode)
        {
        case VIDEO_TRANS_MODE:
            flowType = FLOW_VIDEO_TRANS;
            break;
        case VIDEO_SWITCH_MODE:
            flowType = FLOW_SWITCH;
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

        flowProcessor_t *flowProcessor = NULL;
        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor_t *aux= k;
            if (aux->getFlowProcessorType() == flowType)
            {
                flowProcessor = aux;
                break;
            }
        }

        if ( ! flowProcessor)
        {
            NOTIFY("sessionManager_t :: receiveVideo :: no flowProcessor\n"
                   "Something is going bad, the bug is near...\n"
                   "Aborting...\n"
                  );
            abort();
        }

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
            flowProcessor->setFlow(inFlow,
                                   target,
                                   PT,
                                   BW?BW:part_rx->videoInfo.BW,
                                   Width,
                                   Height,
                                   FR,
                                   Q,
                                   SSRC
                                  );
                break;
        case FLOW_SWITCH:
            flowProcessor->setFlow(inFlow, target, 0, 0, 0, 0, 0, 0, SSRC);
            break;
        default:
            {
                NOTIFY("receiveVideo :: unknown flowtype. Aborting...\n");
                abort();
            }
        }
    }
    delete [] videoFmts;

    return hr;
}

HRESULT
sessionManager_t::discardVideo(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx
                              )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }
    // test if participant RX exists
    participantInfo_t * part_rx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_rx,&part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }
    // test if participant TX exists
    participantInfo_t * part_tx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_tx,&part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every video codec set flowProcessor
    // find part_rx target
    int totalCoders= vGetNumRegisteredCoders();
    u32 *videoFmts= new u32[totalCoders];
    int numCodecs = vGetCodecFmtList(videoFmts, totalCoders);
    for (int i = 0; i < numCodecs; i++)
    {
        u8 _PT = getPTByFmt(videoFmts[i]);
        target_t * target = NULL;

        for (int j = 0; j < targetArray.size(); j++)
        {
            target_t *auxTarget = targetArray.elementAt(j);

            if (addrEquals(part_rx->IP, auxTarget->getIP()))
            {
                if (part_rx->videoInfo.PT > -1) // if must transcode or grid
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
        inFlow.IP   = part_tx->IP;
        inFlow.PT   = target->getPT();
        inFlow.portList = part_tx->portList;

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
            processor->unsetFlow(inFlow,target);
        }
    }

    delete [] videoFmts;

    return S_OK;
}

HRESULT
sessionManager_t::receiveAudioMode(u16 ID_session,
                                   u16 ID_part,
                                   audioMode_e mode,
                                   int PT,
                                   int SL
                                  )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * part = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&part)))
    {
        return E_PART_NOT_EXISTS;
    }


    // If we were mixing and continue mixing
    // then only change output PT of mixer

    if (mode == AUDIO_MIXER_MODE &&
        part->audioInfo.audioMode == AUDIO_MIXER_MODE
       )
    {
        //---------------------------------
        // PT change of mixer
        //---------------------------------
        mixer_t * mixer = NULL;
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
        hr = mixer->setPT(PT,part->IP,part->audioInfo.PT);
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
        audioTrans_t * audioTrans = NULL;
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
        hr = audioTrans->setPT(PT,part->IP,part->audioInfo.PT);
        errorCheck(hr, "sessionManager_t");
        hr = audioTrans->setSilenceLevel(SL);
        errorCheck(hr, "sessionManager_t");
    }

    part->audioInfo.audioMode = mode;
    part->audioInfo.PT        = PT;

    return S_OK;
}

HRESULT
sessionManager_t::receiveAudio(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx,
                               u32 SSRC
                              )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t * part_rx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_rx,&part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t * part_tx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_tx,&part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every audio codec set flowProcessor
    // find part_rx target
    HRESULT hr = S_OK;

    // Get audio formats list
    int totalCodecs= aGetNumRegisteredCoders();
    int *audioFmts= new int[totalCodecs];
    int numCodecs = aGetCodecFmtList(audioFmts, totalCodecs);
    for (int i = 0; i < numCodecs; i++)
    {
        u8 PT = getPTByFmt(audioFmts[i]);
        target_t *target = NULL;

        for (int j = 0; j < targetArray.size(); j++)
        {
            target_t * auxTarget = targetArray.elementAt(j);
            if (addrEquals(part_rx->IP, auxTarget->getIP()))
            {
                if (part_rx->audioInfo.PT > -1) // if must transcode or mix
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
        } // end for

        if ( ! target)
        {
            NOTIFY("sessionManager_t :: receiveAudio :: no target for PT = %d\n",PT);
            continue;
        }

        // create inFlow
        flow_t inFlow;
        inFlow.ID   = part_tx->ID;
        inFlow.IP   = part_tx->IP;
        inFlow.PT   = PT;
        inFlow.portList = part_tx->portList;

        // create flow at demux
        demux->putFlow(inFlow,session->flowProcessorList);

        int flowType = -1;

        switch (part_rx->audioInfo.audioMode)
        {
        case AUDIO_SWITCH_MODE:
            if (part_rx->audioInfo.PT >= 0) // if must transcode
            {
                flowType = FLOW_AUDIO_TRANS;
            }else{ // if must switch
                flowType = FLOW_SWITCH;
            }
            break;
        case AUDIO_MIXER_MODE: // if must mix
            flowType = FLOW_MIX;
            break;
        default:
            NOTIFY("receive_audio:: participant with unknown audio mode [%d]."
                   "Aborting...\n",
                   part_rx->audioInfo.audioMode
                  );
            abort();
        } // end switch

        flowProcessor_t *flowProcessor = NULL;
        for (ql_t<flowProcessor_t *>::iterator_t k =
                 session->flowProcessorList.begin();
             k != session->flowProcessorList.end();
             k++
            )
        {
            flowProcessor_t *aux= k;
            if (aux->getFlowProcessorType() == flowType)
            {
                flowProcessor = aux;
                break;
            }
        }
        if ( ! flowProcessor)
        {
            NOTIFY("sessionManager_t :: receiveAudio :: no flowProcessor\n"
                   "Something is going bad, the bug is near...\n"
                   "Aborting...\n");
            abort();
        }

        flowProcessor->setFlow(inFlow,target,target->getPT(),0,0,0,0,0,SSRC);
    }
    delete [] audioFmts;

    return hr;
}

HRESULT
sessionManager_t::discardAudio(u16 ID_session,
                               u16 ID_part_rx,
                               u16 ID_part_tx
                              )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant RX exists
    participantInfo_t * part_rx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_rx,&part_rx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // test if participant TX exists
    participantInfo_t * part_tx = NULL;
    if (FAILED(getParticipantByID(session,ID_part_tx,&part_tx)))
    {
        return E_PART_NOT_EXISTS;
    }

    // for every audio codec set flowProcessor
    // Get audio formats list
    int totalCodecs= aGetNumRegisteredCoders();
    int *audioFmts= new int[totalCodecs];
    int numCodecs = aGetCodecFmtList(audioFmts, totalCodecs);
    for (int i = 0; i < numCodecs; i++)
    {
        u8 PT = getPTByFmt(audioFmts[i]);
        target_t *target = NULL;

        for (int j = 0; j < targetArray.size(); j++)
        {
            target_t *auxTarget = targetArray.elementAt(j);
            if (addrEquals(part_rx->IP, auxTarget->getIP()))
            {
                if (part_rx->audioInfo.PT > -1) // if must transcode
                {
                    if (auxTarget->getPT() == part_rx->audioInfo.PT)
                    {
                        target = auxTarget;
                        break;
                    }
                }
                else
                {
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
        inFlow.IP   = part_tx->IP;
        inFlow.PT   = PT;
        inFlow.portList = part_tx->portList;

        // demux->deleteFlow(inFlow);

        // now we must discard audio input flow
        // from correct processor at every flowProcessor
        for (ql_t<flowProcessor_t *>::iterator_t iter =
                 session->flowProcessorList.begin();
             iter != session->flowProcessorList.end();
             iter++
            )
        {
            flowProcessor_t *processor = iter;
            processor->unsetFlow(inFlow, target);
        }
    }
    delete []audioFmts;

    return S_OK;
}

// DTE methods
HRESULT
sessionManager_t::getAudioLosses(u16 ID_session,
                                 u16 ID_part,
                                 double * losses
                                )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * part = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&part)))
    {
        return E_PART_NOT_EXISTS;
    }

    *losses = RTCPDemux->getLosses(AUDIO_FLOW,part->IP);

    return S_OK;
}


HRESULT
sessionManager_t::getVideoLosses(u16 ID_session,
                                 u16 ID_part,
                                 double * losses
                                )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * part = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&part)))
    {
        return E_PART_NOT_EXISTS;
    }

    *losses = RTCPDemux->getLosses(VIDEO_FLOW,part->IP);

    return S_OK;
}

HRESULT
sessionManager_t::getLosses(u16 ID_session,
                            u16 ID_part,
                            double * losses
                           )
{
    // get session
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * part = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&part)))
    {
        return E_PART_NOT_EXISTS;
    }

    *losses = RTCPDemux->getLosses(UNKNOWN_FLOW,part->IP);

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
    sessionInfo_t * session = NULL;
    if (FAILED(getSession(ID_session,&session)))
    {
        return E_SESSION_NOT_EXISTS;
    }

    // test if participant exists
    participantInfo_t * part = NULL;
    if (FAILED(getParticipantByID(session,ID_part,&part)))
    {
        return E_PART_NOT_EXISTS;
    }

    switch (part->audioInfo.audioMode)
    {
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
            HRESULT hr= mixer->setPT(newPT, part->IP, oldPT);

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
                HRESULT hr = audioTrans->setPT(newPT,part->IP,oldPT);
                errorCheck(hr, "sessionManager_t");
            }
            else
            {   // if we are only switching
                // For now:: configureAudio only works changing outgoing
                //           PT at mixer or transcoder, not used with
                //           switcher!
                return E_ERROR;
            }

            break;
        } // case AUDIO_SWITCH_MODE end
    }

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

