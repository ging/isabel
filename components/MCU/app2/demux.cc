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
// $Id: demux.cc 7138 2005-07-15 17:02:44Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPPayloads.hh>

#include "sessionManager.h"
#include "general.h"
#include "demux.h"
#include "socketPool.h"
#include "returnCode.h"

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
    memset(&addr, 0, sizeof(sockaddr_storage));
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
    memset(addrList,0,MAX_USERS*sizeof(addrDesc_t*));
    socket = socketPool->get(port);
    add_IO(socket);
}

portListener_t::~portListener_t(void)
{
    for (u16 i=0; i< MAX_USERS; i++)
    {
        if (addrList[i])
        {
            delete addrList[i];
            addrList[i] = NULL;
        }
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

    if (n < 0)
    {
        /*
        NOTIFY("portListener_t (%d) :: socket->read() returns %d\n",
                socket->localPort,
                n);
                */
        return;
    }

    sockaddr_storage *IP =
        (sockaddr_storage *)&socket->temporaryClientAddr->saddr.ssaddr;
    // get addrDesc
    addrDesc_t *addrDesc = addrList[getPosition(IP)];
    
    if ( ! addrDesc)
    {
        //No encuentro, mirar si es un NAT
        return;
    }

    // create RTPPacket with data
    RTPPacket_t *pkt = new RTPPacket_t(data, n);
#if 0
	char IPstr[128];
    sockaddr_in  *IP4 = NULL;
    sockaddr_in6 *IP6 = NULL;
    switch (IP->ss_family)
    {
    case AF_INET:
        IP4 = (sockaddr_in *)(IP);
        inet_ntop(IP4->sin_family,(u8 *)&(IP4->sin_addr),IPstr,128);
        break;
    case AF_INET6:
        IP6 = (sockaddr_in6 *)(IP);
        inet_ntop(IP6->sin6_family,(u8 *)&(IP6->sin6_addr),IPstr,128);
        break;
    }
    NOTIFY("Recibo datos de %s, PT = %d, port = %d\n",IPstr,pkt->getPayloadType(),IP4->sin_port);
#endif

   

   
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
        RTPFecHeader_t *RTPFecHeader = (RTPFecHeader_t *)pkt->getPacket();
        PT = RTPFecHeader->secondWord>>24;
    }

    // get flowDescriptor

    flowDesc_t *flowDesc = NULL;
    for (int i = 0; i < addrDesc->flowDescArray.size(); i++)
    {
        flowDesc_t *aux = addrDesc->flowDescArray.elementAt(i);
        if (aux->PT == PT)
        {
            flowDesc = aux;
            break;
        }
    }

    if ( ! flowDesc)
    {
        //NOTIFY("demux_t :: receiving PT = %d, can't find flowDesc\n",PT);
        delete pkt;
        return;
    }

    int ID_session = -1;
    HRESULT hr = APP->getSessionByIPPort(*IP, getPort(), &ID_session);
    if (FAILED(hr))
    {
        // if failed , we're receiving pkts from unregistered host IP

        delete pkt;
        return;
    }

    // create inFlow struct
    flow_t inFlow;
    switch(IP->ss_family)
    {
    case AF_INET:
        memcpy(&inFlow.IP,IP,sizeof(sockaddr_in));
        break;
    case AF_INET6:
        memcpy(&inFlow.IP,IP,sizeof(sockaddr_in6));
        break;
    default:
        delete pkt;
        return;
    }

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
            flowProcessor_t *flowProcessor = i;
            flowProcessor->deliver(fecPkt, inFlow);
        }
        // refresh statGatherer
        if (ID_session >= 0)
        {
            APP->statGatherer->setRCVData(fecPkt, *IP, ID_session);
        }
    }

    for (ql_t<flowProcessor_t *>::iterator_t i =
         flowDesc->flowProcessorList.begin();
         i != flowDesc->flowProcessorList.end();
         i++
        )
    {
        flowProcessor_t *flowProcessor = i;
        flowProcessor->deliver(pkt, inFlow);
    }

    // refresh statGatherer
    if (ID_session>=0)
    {
        APP->statGatherer->setRXData(pkt, inFlow.IP, ID_session);
    }

    delete pkt;
}

u16
portListener_t::getPosition(sockaddr_storage * IP)
{
    u16 position = getIndex(*IP);
    // check duplicates
    while (addrList[position])
    {
        if (addrEquals(*IP,addrList[position]->addr))
        {
            break;
        }
        position = (position + 1)%MAX_USERS;
    }

    return position;
}


// put flow : assigns flow processor for an IP/PT pair
// substitutes old flow processor if exists
HRESULT
portListener_t::putFlow(flow_t inFlow,
                        ql_t<flowProcessor_t *> flowProcessorList
                       )
{
    u16 pos = getPosition(&inFlow.IP);
    addrDesc_t * addrDesc = addrList[pos];

    if (!addrDesc)
    {
        addrDesc = new addrDesc_t();
        addrDesc->addr = inFlow.IP;
        addrDesc->ID   = inFlow.ID;
        addrList[pos]  = addrDesc;
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
portListener_t::deleteFlow(flow_t inFlow)
{
    u16 pos = getPosition(&inFlow.IP);
    addrDesc_t * addrDesc = addrList[pos];

    if ( ! addrDesc)
    {
        return E_IP;
    }

    // check if PT exists
    for (int i = 0; i < addrDesc->flowDescArray.size(); i++)
    {
        flowDesc_t *flowDesc= addrDesc->flowDescArray.elementAt(i);
        if ((flowDesc->PT == inFlow.PT || inFlow.PT == -1) &&
            addrDesc->ID == inFlow.ID
           )
        {
            addrDesc->flowDescArray.remove(i);
            delete flowDesc;
            i--;
        }
    }

    // if not flows delete addrDesc
    if ( ! addrDesc->flowDescArray.size())
    {
        delete addrDesc;
        addrList[pos] = NULL;
        for (u16 k = 0; k<MAX_USERS; k++)
        {
            int pos1 = (pos + k)%MAX_USERS;
            int pos2 = (pos1 + 1)%MAX_USERS;
            if (addrList[pos2])
            {
                if (getPosition(&addrList[pos2]->addr)==pos1)
                {
                    addrList[pos1] = addrList[pos2];
                    addrList[pos2] = NULL;
                }
            }else{
                break;
            }
        }
    }

    return S_OK;
}

// true if addrList is empty.
bool
portListener_t::isEmpty(void)
{
    for (u16 i = 0; i<MAX_USERS; i++)
    {
        if (addrList[i])
        {
            return false;
        }
    }

    return true;
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
                 ql_t<flowProcessor_t *> flowProcessorList
                )
{
    // check if port Listener exists in every port
    for (ql_t<u16>::iterator_t i = inFlow.portList.begin();
         i != inFlow.portList.end();
         i++
        )
    {

        portListener_t *portListener = NULL;
        u16 port = (u16)i;
        NOTIFY("demux_t::putFlow: Test port %d\n",port);
        
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
            NOTIFY("demux_t::putFlow: Create %d port listener\n",port);
            portListener = new portListener_t(port);
            portListenerArray.add(portListener);
            APP->insertTask(portListener);
        }

        // now create inFlow
        HRESULT hr = portListener->putFlow(inFlow, flowProcessorList);

        errorCheck(hr, "demux_t");
    }
    return S_OK;
}


HRESULT
demux_t::deleteFlow(flow_t inFlow)
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
            // now create inFlow
            HRESULT hr = portListener->deleteFlow(inFlow);
            errorCheck(hr, "demux_t");

            if (portListener->isEmpty())
            {
                APP->removeTask(portListener);
                // task_ref count == 0
                // delete portListener
                portListenerArray.remove(j);
            }
        }
    }
    return S_OK;
}

