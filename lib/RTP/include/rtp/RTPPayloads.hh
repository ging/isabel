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
// $Id: RTPPayloads.hh 22120 2011-03-17 16:36:28Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __RTP_payloads_hh__
#define __RTP_payloads_hh__

#include <icf2/general.h>

// payloads, defined in RFC 3551

const unsigned short GSM_8_PT    =   3;
const unsigned short GSM_16_PT   = 107;
const unsigned short GSM_32_PT   = 115;

const unsigned short AMR_8_PT    = 116;
const unsigned short AMR_16_PT   = 117;
const unsigned short AMR_32_PT   = 121;

const unsigned short NONE_8_PT   = 100;
const unsigned short NONE_16_PT  = 102;
const unsigned short NONE_22_PT  = 108;
const unsigned short NONE_44_PT  =  11;
const unsigned short NONE_48_PT  = 119;

const unsigned short PCMA_8_PT   =   8; // ITU-T G.711 PCM  a-Law 64kbit/s
const unsigned short PCMU_8_PT   =   0; // ITU-T G.711 PCM mu-Law 64kbit/s
const unsigned short PCMU_16_PT  = 103;
const unsigned short PCMU_22_PT  = 109;
const unsigned short PCMU_44_PT  = 112;

const unsigned short G722_8_PT   =   9;
const unsigned short G722_16_PT  = 104;

const unsigned short G726_8_PT   = 101;
const unsigned short G726_16_PT  = 105;

const unsigned short MP3_8_PT    =  14;
const unsigned short MP3_16_PT   =  15;
const unsigned short MP3_32_PT   =  16;
const unsigned short MP3_44_PT   =  17;
const unsigned short MP3_48_PT   =  18;

const unsigned short SPEEX_8_PT  = 123;
const unsigned short SPEEX_16_PT = 124;

const unsigned short VUMETER_PT = 22;

const unsigned short CELB_PT  =  25;
const unsigned short MJPEG_PT =  26;
const unsigned short MPEG1_PT =  32; // MPV in RFC 3551
const unsigned short MPEG2_PT =  33; // MP2T in RFC 3551
const unsigned short H263_PT  =  34;
const unsigned short H263P_PT =  42;
const unsigned short H264_PT  =  98;
const unsigned short MPEG4_PT =  96;
const unsigned short XVID_PT  = 118;

// alive payload type
const unsigned short ALIVE_PT= 120; // to keep alive NAT bindings

enum flowKind_e
{
    VIDEO_FLOW,
    AUDIO_FLOW,
    VUMETER_FLOW,
    ALIVE_FLOW,
    UNKNOWN_FLOW
};

u32 getFmtByPT(u8 PT);
u8  getPTByFmt(u32 fmt);

int getRateByPT(u8 PT);        // audio rate
int getMSPerPacketByPT(u8 PT); // milliseconds each packet
int getBWByPT(u8 PT);          // bandwidth required for pt
flowKind_e getFlowByPT(u8 PT);

double getTimestampUnitByPT(u8 PT);

#endif
