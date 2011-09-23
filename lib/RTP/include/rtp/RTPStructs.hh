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
// $Id: RTPStructs.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_structs_hh__
#define __rtp_structs_hh__

#include <icf2/general.h>
#include <icf2/ql.hh>

struct RTPHeader_t
{
    u32 cc:4;
    u32 extension:1;
    u32 padding:1;
    u32 version:2;
    u32 payloadtype:7;
    u32 marker:1;
    u32 seqnum:16;
    u32 timestamp;
    u32 ssrc;
};


struct ExtensionHeader_t
{
    u32 id:16;
    u32 length:16;
};


struct RTCPHeader_t
{
    u32 blockcount:5;
    u32 padding:1;
    u32 version:2;
    u32 packettype:8;
    u32 length:16;
};


struct SDESItem_t
{
    u8 identifier;
    u8 length;
    char *info;
};


struct SSRCHeader_t
{
    u32 ssrc;
};


struct SenderInfo_t
{
    u32 NTPmsw;
    u32 NTPlsw;
    u32 rtptimestamp;
    u32 senderpacketcount;
    u32 senderoctetcount;
};


struct ReportBlock_t
{
    u32 ssrc;
    u8 fractionlost;
    u8 packetslost[3];
    u32 exthsnr; // extended highest sequence number received
    u32 jitter;
    u32 lsr; // last SR timestamp
    u32 dlsr; // delay since last SR
};


struct RTCPReport_t
{
    RTCPHeader_t *rtcpHeader;
    SSRCHeader_t *ssrcHeader;
    SenderInfo_t *senderInfo;
    ql_t<ReportBlock_t *> *rrList;
};


struct RTCPSDES_t
{
    SSRCHeader_t *ssrcHeader;
    ql_t<SDESItem_t *> *sdesList;
};


struct RTCPBYE_t
{
    RTCPHeader_t *rtcpHeader;
    ql_t<u32> *csrcList;
    char *byeReason;
};


#endif

