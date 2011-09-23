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
// $Id: output.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "output.h"
#include "sessionManager.h"
#include "socketPool.h"
#include "returnCode.h"

target_t::target_t(int PT,
                   inetAddr_t const &addr,
                   u16 ID,
                   u16 remotePort,
                   u16 localPort,
                   u16 localRTCPport
                  )
{
    this->PT = PT;
    this->ID = ID;
    this->natTarget = false;
    referenceCount = 0;

    // create sender
    char portStr[6];
    
    RTCPport = localRTCPport;
    RTPport = localPort;
    remoteRTPport = remotePort;

    sprintf(portStr, "%d", remotePort);
    inetAddr_t destAddr(addr.getStrValue(), portStr, SOCK_DGRAM);
    this->addr = destAddr;

    // create filters
    sender = new sender_t(destAddr, localPort);
    keepAlive = new keepAlive_t;
    traffic = new traffic_t;
    fec = new fecSender_t;
    keepAlive->setNext(sender);
    traffic->setNext(keepAlive);
    fec->setNext(traffic);
    APP->insertTask(keepAlive);
}

target_t::target_t(int PT, u16 ID, u16 localPort, u16 localRTCPport)
{
    this->natTarget = true;
    this->PT = PT;
    this->ID = ID;
    referenceCount = 0;

    RTCPport = localRTCPport;
    RTPport = localPort;
    remoteRTPport = -1;

    sender = NULL;
    keepAlive = NULL;
    traffic = NULL;
    fec = NULL;

    internalNatFEC = -1;
    internalNatAlive = -1;
    internalNatBW = -1;
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

void
target_t::setFec(unsigned int k)
{
    if (natTarget)
    {
        internalNatFEC = k;
    }

    fec->setK(k);
}

HRESULT
target_t::setBW(unsigned int BW)
{
    if (natTarget)
    {
        internalNatBW = BW;
        return S_OK;
    }
    return traffic->setBW(BW*1000); // BW in Kbps
}

HRESULT
target_t::setAlive(bool active)
{
    if (natTarget)
    {
        internalNatAlive = active ? 1 : 0;
        return S_OK;
    }
    return keepAlive->setAlive(active);
}


inetAddr_t
target_t::getIP(void)
{
    return addr;
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

int
target_t::getPT(void)
{
    return PT;
}

u16
target_t::getRTCPport(void)
{
    return RTCPport;
}

u16
target_t::getRTPport(void)
{
    return RTPport;
}

u16
target_t::getRemoteRTPport(void)
{
    return remoteRTPport;
}

HRESULT
target_t::deliver(RTPPacket_t *pkt)
{
    if (natTarget && remoteRTPport == -1)
    {
        return S_OK;
    }

    HRESULT hr = fec->deliver(pkt);
    errorCheck(hr, "target_t::deliver:");

    return hr;
}


HRESULT
target_t::assignIPandPort(inetAddr_t const &addr, u16 remotePort)
{
    if ( ! natTarget)
    {
        return S_OK;
    }

    remoteRTPport = remotePort;

    char portStr[128];
    sprintf(portStr, "%d", remotePort);
    inetAddr_t destAddr(addr.getStrValue(), portStr, SOCK_DGRAM);
    this->addr = destAddr;

    // create filters
    sender = new sender_t(destAddr, this->RTPport);
    keepAlive = new keepAlive_t;
    traffic = new traffic_t;
    fec = new fecSender_t;
    keepAlive->setNext(sender);
    traffic->setNext(keepAlive);
    fec->setNext(traffic);
    APP->insertTask(keepAlive);

    if (internalNatFEC != -1)
    {
        this->setFec(internalNatFEC);
    }
    if (internalNatAlive != -1)
    {
        this->setAlive(internalNatAlive==1?true:false);
    }
    if (internalNatBW != -1)
    {
        this->setBW(internalNatBW);
    }

    natTarget = false;

    return S_OK;
}
u16
target_t::getID(void)
{
    return ID;
}

