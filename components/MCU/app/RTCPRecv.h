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
// $Id: RTCPRecv.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_RTCP_RECV_H_
#define _MCU_RTCP_RECV_H_

#include <icf2/vector.hh>
#include <icf2/stdTask.hh>

#include "general.h"

#include <rtp/RTPPacket.hh>

#define MAX_USERS 0xffff

// losses types
#define AUDIO_LOSSES 0x01
#define VIDEO_LOSSES 0x02
#define TOTAL_LOSSES 0x03

// save all report info
class addrReport_t
{
private:

    inetAddr_t             addr; //user IP
    int                    PT;
    ql_t<ReportBlock_t>    rrList;

public:

    addrReport_t(void);
    ~addrReport_t(void);

    friend class RTCPRecv_t;
};



//RTCP receiver: Receives
//               RTCP stats of the
//               sended RTP flow
class RTCPRecv_t: public simpleTask_t
{
private:
    u8 data[MAX_PKT_LEN];

    addrReport_t  *addrReportArr[MAX_USERS]; //RTCP report list by addr

    u16 port;
    dgramSocket_t *socket;

    // receive RTCP pkt
    virtual void IOReady(io_ref &io);

    // hash table
    u16 getPosition(inetAddr_t const &addr);

public:
    RTCPRecv_t(u16);
    ~RTCPRecv_t(void);

    double getLosses(int type, inetAddr_t const &addr);
    u16    getPort(void);

    // ADD/DELETE RTCP receiver
    HRESULT addReceiver(inetAddr_t const &addr, int PT);
    HRESULT deleteReceiver(inetAddr_t const &addr);
};

class RTCPDemux_t
{
private:

    vector_t<RTCPRecv_t *> RTCPRecvArray;

public:

    RTCPDemux_t(void);
    ~RTCPDemux_t(void);

    // ADD/DELETE RTCP receiver
    HRESULT addReceiver(inetAddr_t const &addr, int PT, u16 localRTCPport);
    HRESULT addReceiver(int PT, u16 localRTCPport);
    HRESULT deleteReceiver(inetAddr_t const &addr, u16 localRTCPport);

    // get Report (fraction lost field)
    // type can be:
    //      -- AUDIO_LOSSES
    //      -- VIDEO_LOSSES
    //      -- TOTAL_LOSSES
    double getLosses(int type, inetAddr_t const &addr);
};

#endif

