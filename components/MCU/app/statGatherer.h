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
// $Id: statGatherer.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_STAT_GATHERER_H_
#define _MCU_STAT_GATHERER_H_

#include <map>

#include <icf2/ql.hh>
#include <icf2/vector.hh>
#include <icf2/notify.hh>
#include <icf2/sched.hh>
#include <icf2/sockIO.hh>

#include <rtp/RTPPacket.hh>

#include "socketPool.h"

struct RXStats_t
{
    u32 BWTotal;
    u32 BWData;
    u32 BWFEC;
    u32 jitter;
    u8  fractionLost;
};


class RXData_t
{
public:

    u16 lostPktsInInterval, receivedPktsInInterval;
    u16 pktNo, lastSequenceNo, SNcounter;
    u32 dataLengthRX, totalLengthRX, recoveredRX, duplicatedPkts,
        lostPkts, recoveredPkts, receivedPkts, unorderedPkts;

    timeval initial, final;
    bool restartInterval;

    inetAddr_t  IPAddr;
    u32         orgSSRC;
    RXStats_t   stats;

    RXData_t(void);
    ~RXData_t(void);
};


#define MAX_SESSIONS 0xffff
#define MAX_USERS_PER_SESSION 0xffff
#define PAYLOAD_NUMBER 0x80


class IPPayloads_t
{
public:
    RXData_t *PayloadList[PAYLOAD_NUMBER];
    IPPayloads_t(void);
    ~IPPayloads_t(void);
};

class IPList_t
{
public:
    IPPayloads_t *IPList[MAX_USERS_PER_SESSION];
    IPList_t(void);
    ~IPList_t(void);
};

class rtcpSender_t: public simpleTask_t
{
private:
    struct RTCPReport
    {
        RTCPHeader_t   header;
        SSRCHeader_t   ssrcHeader;
        ReportBlock_t  report;
    };

    dgramSocket_t *socket;
    u32 SSRC;
    inetAddr_t *addr;

    virtual void heartBeat(void);


    HRESULT buildRtcpPkt (void);
    HRESULT flush(RTCPReport const& pkt);

public:

    u16 session;
    RXData_t *data;

    rtcpSender_t(inetAddr_t destAddr,
                 u16        localRTCPport,
                 u32        period,
                 RXData_t  *stats,
                 u16        ID_session
                );
    ~rtcpSender_t(void);
};

class statGatherer_t
{
private:

    // Dictionary to link SessionID and IP addresses on it
    std::map<u16, IPList_t*> SessionList;
    vector_t<rtcpSender_t*> rtcpClientsArray;

    // Needed to calculate client position into the hash table
    u16 getPosition(inetAddr_t const &addr, u16 ID_session, int PT);

public:

    statGatherer_t(void);
    virtual ~statGatherer_t(void);

    // From Demux
    HRESULT setRXData(RTPPacket_t *pkt, inetAddr_t const &addr, u16 ID_session);

    // When new event is created in the MCU
    HRESULT createSession(u16 ID_session);
    HRESULT removeSession(u16 ID_session);

    HRESULT bindRtcp(u16 ID_session,
                     inetAddr_t const &addr,
                     u16 remoteRTCPport,
                     u16 localRTCPport
                    );

    // If there is any recovered data...
    HRESULT setRCVData(RTPPacket_t *pkt,
                       inetAddr_t const &addrOrig,
                       u16 ID_session
                      );
};

#endif

