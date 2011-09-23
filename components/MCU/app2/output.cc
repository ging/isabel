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
// $Id: output.cc 6569 2005-05-04 10:05:16Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>

#ifdef __BUILD_FOR_LINUX
#include <sys/types.h> //to include inet_ntop()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#endif

#include "output.h"
#include "sessionManager.h"
#include "socketPool.h"
#include "returnCode.h"

target_t::target_t(unsigned int PT,
                   sockaddr_storage addr,
                   u16 remotePort,
                   u16 localPort,
                   u16 localRTCPport
                  )
{
    this->PT = PT;
    this->IP = addr;
    referenceCount = 0;

    // create sender
    char portStr[6];
    char IP[128];

    RTCPport = localRTCPport;

    switch (addr.ss_family)
    {
    case AF_INET:
        {
            sockaddr_in * IP4 = (sockaddr_in *)(&addr);
            inet_ntop(IP4->sin_family,(u8 *)&IP4->sin_addr.s_addr,IP,128);
            break;
        }
    case AF_INET6:
        {
            sockaddr_in6 * IP6 = (sockaddr_in6 *)(&addr);
            inet_ntop(IP6->sin6_family,(u8 *)&(IP6->sin6_addr),IP,128);
            break;
        }
    default:
        NOTIFY("target_t::target_t::Unknown family [%d]\n",addr.ss_family);
        abort();
    }
    sprintf(portStr,"%d",remotePort);
    inetAddr_t inetAddr(IP,portStr,SOCK_DGRAM);

    // create filters
    sender = new sender_t(inetAddr,localPort);
    keepAlive = new keepAlive_t;
    traffic = new traffic_t;
    fec = new fecSender_t;
    keepAlive->setNext(sender);
    traffic->setNext(keepAlive);
    fec->setNext(traffic);
    APP->insertTask(keepAlive);
}

target_t::~target_t(void)
{
    //----------------------------------
    // deleteTarget when reference = 0;
    // every call to unbind_rtp reference count --
    // NOTE : Remember to delete every reference
    //        to this target in every flowProcessor!
    //----------------------------------

    // delete filters
    if (sender)
    {
        delete sender;
    }
    if (keepAlive)
    {
        APP->removeTask(keepAlive);
    }
    if (traffic)
    {
        delete traffic;
    }
    if (fec)
    {
        delete fec;
    }
}

HRESULT
target_t::setFec(unsigned int k)
{
    return fec->setK(k);
}

HRESULT
target_t::setBW(unsigned int BW)
{
    return traffic->setBW(BW*1000); // BW in Kbps
}

HRESULT
target_t::setAlive(bool active)
{
    return keepAlive->setAlive(active);
}


sockaddr_storage
target_t::getIP(void)
{
    return IP;
}

u16
target_t::incRef(void)
{
    referenceCount++;

    return referenceCount;
}

u16
target_t::decRef(void)
{
    referenceCount--;

    return referenceCount;
}

u8
target_t::getPT(void)
{
    return PT;
}

u16
target_t::getRTCPport(void)
{
    return RTCPport;
}

HRESULT
target_t::deliver(RTPPacket_t *pkt)
{
    HRESULT hr = fec->deliver(pkt);
    errorCheck(hr, "target_t::deliver");

    return hr;
}

