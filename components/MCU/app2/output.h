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
// $Id: output.h 6519 2005-04-25 14:42:33Z sirvent $
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
    unsigned int    PT;
    sockaddr_storage        IP;
    u16             referenceCount;

    // filter modules
    sender_t    *sender;
    keepAlive_t *keepAlive;
    fecSender_t *fec;
    traffic_t   *traffic;

    u16 RTCPport;

public:

    target_t(unsigned int PT,
             sockaddr_storage addr,
             u16 remotePort,
             u16 localPort = 0,
             u16 localRTCPport = 0
            );

    virtual ~target_t(void);

    u16     incRef(void);
    u16     decRef(void);

    HRESULT setFec(unsigned int k); // k= 0 OFF
    HRESULT setBW(unsigned int BW); // BW= 0 OFF
    HRESULT setAlive(bool active);

    sockaddr_storage getIP(void);
    u8 getPT(void);
    u16 getRTCPport(void);

    HRESULT deliver(RTPPacket_t *pkt);
};

#endif

