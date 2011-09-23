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
// $Id: RTPDefinitions.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __RTP_definitions_hh__
#define __RTP_definitions_hh__

const double VIDEO_RTP_TSU   = 1.0/90000.0;
const double AUDIO_RTP_TSU   = 1.0/8000.0;
const double DEFAULT_RTP_TSU = 1.0/90000.0;

const double RTCP_MIN_TIME           = 5000000.0; // 5 seg (en useg.)
const double RTCP_SENDER_BW_FRACTION = 0.25;
const double RTCP_FRACTION           = 0.05;
const double RTCP_SIZE_GAIN          = 1.0/16.0;
const int    RTCP_SDES_INTERVAL      = 9;

const int    RTP_MAX_PACKET_SIZE     = 1400;
const int    RTP_BUFFER_SIZE         = 65536;
const int    RTP_VERSION             = 2;
const int    RTP_INVALID_PT          = 255;
const int    RTP_PAYLOAD_TYPES       = 128;
const int    RTP_MIN_SEQUENTIAL      = 2;
const int    RTP_MAX_DROPOUT         = 3000;
const int    RTP_MAX_MISORDER        = 100;
const int    RTP_SEQ_MOD             = 1 << 16;
const int    RTP_NTP_OFFSET          = +2195942400UL;  // segundos entre 1/6/1900 y 1/1/1970
const double RTP_TIMEOUTSEC          = 1800*1000000.0; // 30 minutos (en useg.)

const int    RTP_MAX_SEQ_NUM         = 0x10000; // 65536
const int    RTP_MAX_SEQ_DIFF        = 0x0FFF;  // rather arbitrary

#define TYPE_SDES                       9
#define TYPE_SDES_END                   0
#define TYPE_SDES_CNAME                 1
#define TYPE_SDES_NAME                  2
#define TYPE_SDES_EMAIL                 3
#define TYPE_SDES_PHONE                 4
#define TYPE_SDES_LOC                   5
#define TYPE_SDES_TOOL                  6
#define TYPE_SDES_NOTE                  7
#define TYPE_SDES_PRIV                  8

#define TYPE_RTCP_SR                    200
#define TYPE_RTCP_RR                    201
#define TYPE_RTCP_SDES                  202
#define TYPE_RTCP_BYE                   203

#endif

