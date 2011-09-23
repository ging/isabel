/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: output.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_TARGET_H_
#define _MCU_TARGET_H_

#include <icf2/sched.hh>

#include "sender.h"
#include "traffic.h"
#include "fec.h"
#include "keepAlive.h"

class target_t
{
private:
    int        PT;
    inetAddr_t addr;
    u16        ID;
    u16        referenceCount;

    // filter modules
    sender_t    *sender;
    keepAlive_t *keepAlive;
    fecSender_t *fec;
    traffic_t   *traffic;

    u16 RTCPport;
    int RTPport;
    int remoteRTPport;

public:
    // params for NAT supporting
    bool natTarget;
    u16 localPort;
    int internalNatFEC;
    int internalNatAlive;
    int internalNatBW;

    target_t(int PT,
             inetAddr_t const &addr,
             u16 ID,
             u16 remotePort,
             u16 localPort = 0,
             u16 localRTCPport = 0
            );

    target_t(int PT,
             u16 ID,
             u16 localPort = 0,
             u16 localRTCPport = 0
            );

    virtual ~target_t(void);

    u16     incRef(void);
    u16     decRef(void);

    void    setFec(unsigned int k); // k= 0 OFF
    HRESULT setBW(unsigned int BW); // BW= 0 OFF
    HRESULT setAlive(bool active);

    inetAddr_t getIP(void);
    int getPT(void);
    u16 getRTCPport(void);
    u16 getRTPport(void);
    u16 getRemoteRTPport(void);
    u16 getID(void);

    HRESULT deliver(RTPPacket_t *pkt);
    HRESULT assignIPandPort(inetAddr_t const &addr, u16 remotePort);
};

#endif

