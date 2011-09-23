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
// $Id: demux.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPUtils.hh>

#include "sessionManager.h"
#include "general.h"
#include "demux.h"
#include "socketPool.h"
#include "returnCode.h"

#ifdef __BUILD_FOR_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

using namespace std;

flowDesc_t::flowDesc_t(u8 nPT, u8 fecPT)
{
    PT= nPT;
    RTPFecReceiver= new RTPFecReceiver_t(fecPT);
}

flowDesc_t::~flowDesc_t(void)
{
    PT= 0;
    delete RTPFecReceiver;
}

addrDesc_t::addrDesc_t(void)
{
}

addrDesc_t::~addrDesc_t(void)
{
    while(flowDescArray.size())
    {
        flowDesc_t *flowDesc= flowDescArray.elementAt(0);
        flowDescArray.remove(0);
        delete flowDesc;
    }
}

portListener_t::portListener_t(unsigned int port)
{
    natPort = false;
    tempAddrDesc = NULL;
    addrList.clear();
    socket = socketPool->get(port);
    add_IO(socket);
}

portListener_t::portListener_t(unsigned int port,bool isForNat)
{
    natPort = isForNat;
    tempAddrDesc = NULL;
    addrList.clear();
    socket = socketPool->get(port);
    add_IO(socket);
}

portListener_t::~portListener_t(void)
{
    if (natPort)
    {
        delete tempAddrDesc;
        tempAddrDesc = NULL;
    }
    else
    {
        for (map<inetAddr_t,addrDesc_t*>::iterator iter = addrList.begin(); 
            iter != addrList.end(); ++iter)
        {
            if (iter->second)
            {
                delete iter->second;                
            }
        }
        addrList.clear();
    }
    del_IO(socket);
}

// Input data
void
portListener_t::IOReady(io_ref &io)
{
    io_t *ioxx = static_cast<io_t *>(io);
    dgramSocket_t *socket = static_cast<dgramSocket_t *>(ioxx);

    int n = socket->read(data, MAX_PKT_LEN);

    if (n<0)
    {
        //NOTIFY("portListener_t::IOReady: (%d) read() returns %d\n",
        //        socket->localPort,
        //        n
        //       );

        return;
    }

    // get addrDesc
    if (natPort)
    {
        sessionManager_t::sessionInfo_t *session = NULL;
        sessionManager_t::participantInfo_t *participant = NULL;
        APP->getSessionByNatPort(getPort(), &sessionID);
        APP->getSession(sessionID, &session);
        APP->getParticipantByID(session, tempAddrDesc->ID, &participant);

        addrDesc_t *addrDesc = new addrDesc_t;
        addrDesc->addr = *socket->temporaryClientAddr;
        addrDesc->ID   = tempAddrDesc->ID;
        addrDesc->flowDescArray = tempAddrDesc->flowDescArray;
        addrList[*socket->temporaryClientAddr] = addrDesc;
        tempAddrDesc = NULL;

        participant->inetAddr = *socket->temporaryClientAddr;
        u16 remotePort = socket->temporaryClientAddr->getPort();
        participant->remotePortArray.add(remotePort);

        for (int i = 0; i < session->targetArray.size(); i++)
        {
            target_t *target = session->targetArray.elementAt(i);
            if (target->natTarget && target->getRTPport() == socket->localPort)
            {
                target->assignIPandPort(*socket->temporaryClientAddr,remotePort);
                break;
            }
        }

        natPort = false;
        participant->numNatPorts --;
        if (participant->numNatPorts == 0)
            participant->natParticipant = false;
    }

    inetAddr_t sourceAddr(*socket->temporaryClientAddr);
    sourceAddr.saddr.ssin.sin_port = 0;
    addrDesc_t * addrDesc = addrList[sourceAddr];

    if ( ! addrDesc)
    {
        return;
    }

    // create RTPPacket with data
    RTPPacket_t *pkt = new RTPPacket_t(data, n);

    // check if it's an alive pkt
    if (pkt->getPayloadType() == ALIVE_PT)
    {
        delete pkt;
        return;
    }

    // get PT
    int PT = pkt->getPayloadType();
    if (PT == FEC_PT)
    {
        RTPFecHeader_t *RTPFecHeader = (RTPFecHeader_t*)pkt->getPacket();
        PT = RTPFecHeader->secondWord>>24;
    }

    // get flowDescriptor

    flowDesc_t *flowDesc = NULL;
    for (int i = 0; i < addrDesc->flowDescArray.size(); i++)
    {
        flowDesc_t *aux = addrDesc->flowDescArray.elementAt(i);
        if (aux->PT == PT || aux->PT == -1)
        {
            flowDesc = aux;
            break;
        }
    }

    if ( ! flowDesc)
    {
        NOTIFY("demux_t :: receiving PT = %d, can't find flowDesc\n",PT);
        delete pkt;
        return;
    }

    int ID_session = -1;
    HRESULT hr =
        APP->getSessionByIPPort(*socket->temporaryClientAddr,getPort(),
        &ID_session
        );
    if (FAILED(hr))
    {
        // if failed , we're receiving pkts from unregistered host IP

        delete pkt;
        return;
    }

    // create inFlow struct
    flow_t inFlow;
    inFlow.ID = addrDesc->ID;
    inFlow.PT = PT;
    inFlow.portList.insert(socket->localPort);

    // check FEC
    RTPPacket_t *fecPkt = flowDesc->RTPFecReceiver->recvPacket(pkt);
    if (fecPkt)
    {
        for (ql_t<flowProcessor_t *>::iterator_t i =
            flowDesc->flowProcessorList.begin();
            i != flowDesc->flowProcessorList.end();
        i++
            )
        {
            flowProcessor_t * flowProcessor = i;
            flowProcessor->deliver(fecPkt,inFlow);
        }
        // refresh statGatherer
        if (ID_session >= 0)
        {
            APP->statGatherer->setRCVData(fecPkt,
                *socket->temporaryClientAddr,
                ID_session
                );
        }
    }

    for (ql_t<flowProcessor_t *>::iterator_t i =
        flowDesc->flowProcessorList.begin();
        i != flowDesc->flowProcessorList.end();
    i++
        )
    {
        flowProcessor_t *flowProcessor = i;
        flowProcessor->deliver(pkt,inFlow);
    }

    // refresh statGatherer
    if (ID_session>=0)
    {
        APP->statGatherer->setRXData(pkt,*socket->temporaryClientAddr,ID_session);
    }
    delete pkt;
}

// put flow : asigns flow processor for an IP/PT pair
// substitutes old flow processor if exists
HRESULT
portListener_t::putFlow(flow_t inFlow,
                        ql_t<flowProcessor_t *> flowProcessorList,
                        inetAddr_t const &addr
                        )
{
    addrDesc_t * addrDesc;
    if ( ! natPort)
    {
        addrDesc = addrList[addr];
        if ( ! addrDesc)
        {
            addrDesc = new addrDesc_t();
            addrDesc->addr = addr;
            addrDesc->ID   = inFlow.ID;
            addrList[addr] = addrDesc;
        }
    }
    else
    {
        if ( ! tempAddrDesc)
        {
            tempAddrDesc = new addrDesc_t();
            tempAddrDesc->ID= inFlow.ID;
        }
        addrDesc = tempAddrDesc;
    }

    // check if PT exists
    flowDesc_t *flowDesc = NULL;
    for (int i = 0; i < addrDesc->flowDescArray.size(); i++)
    {
        flowDesc_t *aux = addrDesc->flowDescArray.elementAt(i);
        if (aux->PT == inFlow.PT)
        {
            flowDesc = aux;
            break;
        }
    }
    if ( ! flowDesc)
    {
        flowDesc = new flowDesc_t(inFlow.PT, FEC_PT);
        addrDesc->flowDescArray.add(flowDesc);

        flowDesc->flowProcessorList = flowProcessorList;
    }

    return S_OK;
}

HRESULT
portListener_t::deleteFlow(flow_t inFlow, inetAddr_t const &addr)
{
    addrDesc_t *addrDesc;
    if (!natPort)
    {
        addrDesc = addrList[addr];
    }else{
        addrDesc = tempAddrDesc;
    }

    if (!addrDesc)
    {
        return E_IP;
    }

    // check if PT exists
    for (int i = 0; i < addrDesc->flowDescArray.size(); i++)
    {
        flowDesc_t *flowDesc= addrDesc->flowDescArray.elementAt(i);
        if ((flowDesc->PT == inFlow.PT || inFlow.PT==-1) &&
            addrDesc->ID == inFlow.ID
            )
        {
            addrDesc->flowDescArray.remove(i);
            delete flowDesc;
            i--;
        }
    }

    // if not flows delete addrDesc
    if (!addrDesc->flowDescArray.size())
    {
        if (!natPort)
        {
            addrList[addr] = NULL;
        }
        else
        {
            natPort = false;
        }
        delete addrDesc;
        addrDesc = NULL;
        tempAddrDesc = NULL;
     }
    return S_OK;
}

HRESULT
portListener_t::joinSSMGroup(inetAddr_t const &addr,
                             inetAddr_t const &groupAddr
                             )
{
    printf("JoinSSM!!!");
    this->socket->joinSSM(addr, groupAddr);
    return S_OK;
}

HRESULT
portListener_t::leaveSSMGroup(inetAddr_t const &addr,
                              inetAddr_t const &groupAddr
                              )
{
    this->socket->leaveSSM(addr, groupAddr);

    return S_OK;
}

// true if addrList is empty.
bool
portListener_t::isEmpty(void)
{
    if (natPort)
    {
        return true;
    }
    return addrList.empty();
}

unsigned int
portListener_t::getPort(void)
{
    return socket->localPort;
}

demux_t::demux_t(void)
{
}

demux_t::~demux_t(void)
{
    while (portListenerArray.size())
    {
        APP->removeTask(portListenerArray.elementAt(0));
        portListenerArray.remove(0);
    }
}

// flow Works
HRESULT
demux_t::putFlow(flow_t inFlow,
                 ql_t<flowProcessor_t *> flowProcessorList,
                 bool flowNAT,
                 inetAddr_t const &addr
                 )
{
    // check if port Listener exists in every port
    for (ql_t<u16>::iterator_t i = inFlow.portList.begin();
        i != inFlow.portList.end();
        i++
        )
    {
        portListener_t * portListener = NULL;
        u16 port = (u16)i;
        for (int j = 0; j < portListenerArray.size(); j++)
        {
            portListener_t *aux = portListenerArray.elementAt(j);
            if (aux->getPort() == port)
            {
                portListener = aux;
                break;
            }
        }
        // create portListener if not exists
        if ( ! portListener)
        {
            portListener = new portListener_t(port, flowNAT);
            portListenerArray.add(portListener);
            APP->insertTask(portListener);
        }

        // now create inFlow
        HRESULT hr = portListener->putFlow(inFlow,
            flowProcessorList,
            addr
            );

        errorCheck(hr, "demux_t");
    }
    return S_OK;
}


HRESULT
demux_t::deleteFlow(flow_t inFlow, inetAddr_t const &addr)
{
    // check if port Listener exists in every port
    for (ql_t<u16>::iterator_t i = inFlow.portList.begin();
        i != inFlow.portList.end();
        i++
        )
    {
        portListener_t *portListener = NULL;
        u16 port = (u16)i;
        u16 j = 0;
        for (; j < portListenerArray.size(); j++)
        {
            portListener_t *aux= portListenerArray.elementAt(j);
            if (aux->getPort() == port)
            {
                portListener = aux; // found!
                break;
            }
        }

        if (portListener)
        {
            // now delete inFlow
            HRESULT hr = portListener->deleteFlow(inFlow, addr);
            errorCheck(hr, "demux_t");

            if (portListener->isEmpty())
            {
                APP->removeTask(portListener);
                //task_ref count == 0
                //delete portListener
                portListenerArray.remove(j);
            }
        }
    }
    return S_OK;
}

HRESULT
demux_t::joinSSMGroup(inetAddr_t const &addr, inetAddr_t const &groupAddr)
{
    assert(portListenerArray.size() > 0 && "joining SSM without binding");

    portListener_t *portListener = portListenerArray.elementAt(0);
    portListener->joinSSMGroup(addr, groupAddr);

    return S_OK;
}

HRESULT
demux_t::leaveSSMGroup(inetAddr_t const &addr, inetAddr_t const &groupAddr)
{
    portListener_t *portListener = portListenerArray.elementAt(0);
    portListener->leaveSSMGroup(addr, groupAddr);

    return S_OK;
}
