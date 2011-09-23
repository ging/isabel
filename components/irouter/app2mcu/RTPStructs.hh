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
// $Id: RTPStructs.hh 6191 2005-02-10 16:55:40Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#ifndef __rtp_structs_hh__
#define __rtp_structs_hh__

#include <icf2/general.h>
#include <icf2/ql.hh>

struct _Dll_ RTPHeader{
        u8 cc:4;
	u8 extension:1;
	u8 padding:1;
	u8 version:2;
	u8 payloadtype:7;
        u8 marker:1;
        u16 seqnum: 16;
        u32 timestamp;
        u32 ssrc;
};


struct _Dll_ ExtensionHeader{
	u16 id;
	u16 length;
};


struct _Dll_ RTCPHeader{
        u8 blockcount:5;
	u8 padding:1;
	u8 version:2;
        u8 packettype;
        u16 length: 16;
};


struct _Dll_ SDESItem{
        u8 identifier;
        u8 length;
        char *info;
};


struct _Dll_ SSRCHeader{
        u32 ssrc;
};


struct _Dll_ SenderInfo{
        u32 NTPmsw;
        u32 NTPlsw;
        u32 rtptimestamp;
        u32 senderpacketcount;
        u32 senderoctetcount;
};


struct _Dll_ ReportBlock{
        u32 ssrc;
        u8 fractionlost;
        u8 packetslost[3];
        u32 exthsnr; // extended highest sequence number received
        u32 jitter;
        u32 lsr; // last SR timestamp
        u32 dlsr; // delay since last SR
};


struct _Dll_ RTCPReport_t{
        RTCPHeader *rtcpHeader;
        SSRCHeader *ssrcHeader;
        SenderInfo *senderInfo;
        ql_t<ReportBlock *> *rrList;
};


struct _Dll_ RTCPSDES_t{
        SSRCHeader *ssrcHeader;
        ql_t<SDESItem *> *sdesList;
};


struct _Dll_ RTCPBYE_t{
        RTCPHeader *rtcpHeader;
        ql_t<u32> *csrcList;
        char *byeReason;
};


#endif


