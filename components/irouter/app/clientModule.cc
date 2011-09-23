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
// $Id: clientModule.cc 22221 2011-04-06 13:31:50Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>

#ifdef __BUILD_FOR_LINUX
#include <sys/errno.h>
#endif

#include <rtp/RTPDefinitions.hh>

#include "clientModule.hh"
#include "output.hh"
#include "flow.hh"
#include "transApp.hh"
#include "measures.hh"

#define APP ((transApp_t *)(this->get_owner()))

clientModule_t::clientModule_t(flow_t *flow, const char *lPort)
{
    flowId= flow->flowId;

    _clientId= (char *)malloc(64);
    sprintf(_clientId, "client%08x", itemId);

    inetAddr_t localAddr(NULL, lPort, serviceAddr_t::DGRAM);

    // socket server listens "0.0.0.0"
    my_dgramSock = new dgramSocket_t(localAddr);

    my_dgramSock->setSockMaxBuf();

    debugMsg(dbg_App_Paranoic,
             "clientModule_t",
             "Nuevo cliente medio=%d handle=%d",
             flowId,
             my_dgramSock->sysHandle()
            );

    add_IO(my_dgramSock);
}

clientModule_t::clientModule_t(flow_t *flow,
                               const char *lPort,
                               const char *rPort
                              )
{
//alive config
    set_period(ALIVE_PERIOD);
    watchDog = false;
//

    flowId= flow->flowId;

    _clientId= (char *)malloc(64);
    sprintf(_clientId, "client%08x", itemId);

    const char *loopback;
    if (getenv("SIP_IPV6"))
        loopback = "::1";
    else
        loopback = "127.0.0.1";

#ifdef __BUILD_FOR_LINUX
    if ( (flowId == audioId) || (flowId == vumeterId))
    {
        inetAddr_t localAddr(NULL, lPort, serviceAddr_t::DGRAM);
        inetAddr_t userAddr(loopback, rPort, serviceAddr_t::DGRAM);

        my_dgramSock= new dgramSocket_t(localAddr, userAddr); //receiver
    } else {
#endif

        inetAddr_t localAddr(NULL, lPort, serviceAddr_t::DGRAM);
        inetAddr_t userAddr(loopback, rPort, serviceAddr_t::DGRAM);

#ifdef WIN32
        inetAddr_t localAddr2(loopback, "6666", serviceAddr_t::DGRAM);
        my_dgramSock2= new dgramSocket_t(localAddr2,userAddr); //sender for Windows
#endif

        my_dgramSock= new dgramSocket_t(localAddr, userAddr); //receiver

#ifdef __BUILD_FOR_LINUX
    }
#endif

    my_dgramSock->setSockMaxBuf();

#ifdef WIN32
    my_dgramSock2->setSockMaxBuf();
#endif

    debugMsg(dbg_App_Paranoic,
             "clientModule_t",
             "New client flow=%d handle=%d",
             flowId,
             my_dgramSock->sysHandle()
            );

    add_IO(my_dgramSock); // insert socket server in sched
}


clientModule_t::~clientModule_t(void)
{
    debugMsg(dbg_App_Paranoic, "~clientModule_t",
             "Destroying clientModule_t\n"
            );

    if (_clientId)
    {
        free(_clientId);
        _clientId=NULL;
    }

    debugMsg(dbg_App_Paranoic, "~clientModule_t", "Destroyed clientModule_t\n");
}

void
clientModule_t::heartBeat(void)
{
    //If nobody sets watchdog
    //send alive packet;

    if ( ! watchDog)
    {
        RTPHeader_t rtpHeader;
        memset(&rtpHeader, 0, sizeof(RTPHeader_t));

#ifdef WIN32
        u16 SQ  = (u16)rand();
        u32 TS  = (u32)rand();
        u32 SSRC= (u32)rand();
#endif
#ifdef __BUILD_FOR_LINUX
        u16 SQ  = (u16)random();
        u32 TS  = (u32)random();
        u32 SSRC= (u32)random();
#endif

        rtpHeader.version     = RTP_VERSION;
        rtpHeader.padding     = 0;
        rtpHeader.extension   = 0;
        rtpHeader.cc          = 0;
        rtpHeader.marker      = 1;
        rtpHeader.payloadtype = ALIVE_PT;
        rtpHeader.seqnum      = htons(SQ);
        rtpHeader.timestamp   = htonl(TS);
        rtpHeader.ssrc        = htonl(SSRC);

        RTPPacket_t *RTPPacket= new RTPPacket_t((u8*)&rtpHeader, sizeof(RTPHeader_t));

        sharedPkt_t *shPkt = new sharedPkt_t;

        memcpy(shPkt->sharedBuf->data,
               RTPPacket->getPacket(),
               RTPPacket->getTotalLength()
              );

        shPkt->sharedBuf->len= RTPPacket->getTotalLength();
        shPkt->flowId= flowId;
        //NOTIFY("clientModule_t [%d] :: sending alive packet\n", flowId);

        APP->centralSwitch.clientData(shPkt);

        delete shPkt;
        delete RTPPacket;
    }
    else
    {
        watchDog = false;
    }
}

void
clientModule_t::IOReady(io_ref &io)
{
    debugMsg(dbg_App_Verbose, "IOReady", "Read data from client");

    sharedPkt_t *pkt= new sharedPkt_t;

    pkt->flowId= flowId;

    if ((pkt->flowId == audioId) && APP->audioMixer2->isActive())
    {
        struct timeval time;

        gettimeofday(&time, NULL);

        pkt->time.tv_sec  = time.tv_sec;
        pkt->time.tv_usec = time.tv_usec;
        pkt->pktInLink    = defaultLocalLink;
    }

    assert(io.isValid() && "Vaya una porqueria de socket\n");
    dgramSocket_t *dg= static_cast<dgramSocket_t *>(static_cast<io_t*>(io));
    pkt->sharedBuf->len= dg->read(pkt->sharedBuf->data, MAX_PKT_SIZE);

    if (pkt->sharedBuf->len<= 0)
    {
        if (pkt->sharedBuf->len == 0)
        {
            NOTIFY("clientModule_t::IOReady:: read data length=0"
                   " to=%s flow type=%d\n",
                   dg->clientAddr->getStrValue(),
                   flowId
                  );
        }
        else
        {
            NOTIFY("clientModule_t::IOReady socket error"
                   " to=%s flow type=%d errno=[%s]\n",
                   dg->clientAddr->getStrValue(),
                   flowId,
                   strerror(errno)
                  );
        }

        delete pkt;

        return;
    }

    debugMsg(dbg_App_Verbose, "IOReady", "%s read packet", _clientId);

    debugMsg(dbg_App_Verbose,
             "IOReady",
             "%s sending packet to central switch, flow= %d",
             _clientId,
             pkt->flowId
            );

    RTPHeader_t *header = (RTPHeader_t*)pkt->sharedBuf->data;
    measures->savePkt("SRC",
                      ntohl(header->ssrc),
                      header->payloadtype,
                      pkt->sharedBuf->len,
                      ntohl(header->timestamp),
                      ntohs(header->seqnum)
                     );


    if ((pkt->flowId == audioId) && APP->audioMixer2->isActive())
    {
        APP->centralSwitch.audioToMix(pkt);
    }
    else
    {
        APP->centralSwitch.clientData(pkt);
    }

    // feed watchDog
    watchDog = true;

    delete pkt;
}


void
clientModule_t::deliver(void)
{
}

void
clientModule_t::deliver(const void *b, int n)
{
    debugMsg(dbg_App_Verbose, "deliver", "Putting packet at clients home\n");
    debugMsg(dbg_App_Verbose, "deliver", "*******************************\n");
    debugMsg(dbg_App_Verbose, "deliver", "HDL=%d\n", my_dgramSock->sysHandle());
    debugMsg(dbg_App_Verbose, "deliver", "*******************************\n");

#if 0
    {
        RTPHeader_t *rtpH= (RTPHeader_t*)b;
        NOTIFY ("Sending to client: SSRC= %u, PT=%d SeqNum: %u TS: %u\n",
                ntohl(rtpH->ssrc),
                rtpH->payloadtype,
                ntohs(rtpH->seqnum),
                ntohl(rtpH->timestamp)
               );
    }
#endif

#ifdef __BUILD_FOR_LINUX
    int written= my_dgramSock->write(b, n);
#endif
#ifdef WIN32
    int written= my_dgramSock2->write(b, n);
#endif
    debugMsg(dbg_App_Verbose,
             "deliver",
             "write :: b at 0x%x :: n=%d, written=%d",
             &b,
             n,
             written
            );
    if (written != n)
    {
        NOTIFY("clientModule_t::deliver: flowId=%d len=%d write=%d"
               " to=%s:%d errno=[%s]\n",
               flowId, n, written,
               my_dgramSock->clientAddr->getStrValue(),
               my_dgramSock->clientAddr->getPort(),
               strerror(errno)
              );
    }
}



//
// clientBinder_t stuff
//
clientBinder_t::clientBinder_t(): clientDict(MAX_CLIENTS)
{
}

clientBinder_t::~clientBinder_t(void)
{
   debugMsg(dbg_App_Paranoic, "~clientBinder", "Destroying clientBinder_t\n");
}



clientModule_t *
clientBinder_t::newClient(flow_t *flow, const char *lPort, const char *rPort)
{
    clientModule_t *theClient= rPort ?
        new clientModule_t(flow, lPort, rPort) :
        new clientModule_t(flow, lPort);

    clientDict.insert(theClient->clientId(), theClient);

    return theClient;
}



bool
clientBinder_t::deleteClient(const char *name)
{
    clientModule_t *theClient= clientDict.lookUp(name);

    if ( ! theClient)
    {
        return false;
    }

#if 0
    clientDict.delete(name);
    delete theClient;
#else
    NOTIFY("clientBinder_t::deleteClient:: client "
           "deletion not fully implemented\n"
          );
#endif

    return true;
}


clientModule_t *
clientBinder_t::lookUp(const char *name)
{
    return clientDict.lookUp(name);
}

