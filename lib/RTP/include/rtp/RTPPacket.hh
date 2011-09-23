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
// $Id: RTPPacket.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __RTP_packet_hh__
#define __RTP_packet_hh__

#include <icf2/general.h>
#include <icf2/ql.hh>

#include <rtp/RTPStructs.hh>

/**
* This class represents a received RTP packet.
*/
class RTPPacket_t
{
private:
    int totalLength;
    u8 *packet;

    RTPHeader_t *rtpHeader;
    int rtpFixedHeaderLength;
    int rtpHeaderLength;
    ql_t<u32> list;
    u8 *data;
    int dataLength;

    u8 *extensionData;
    u16 extLength;
    u16 extensionId;

public:

    RTPPacket_t (u8 *packet, int length);
    RTPPacket_t (const RTPPacket_t &packet);

    ~RTPPacket_t (void);

    u8        *getPacket (void);
    u8        *getData (void);
    int        getDataLength (void);
    int        getTotalLength (void);
    u32        getSSRC (void);
    u16        getSequenceNumber (void);
    bool       getMark (void);
    u8         getPayloadType (void);
    u32        getTimestamp (void);
    ql_t<u32> *getCSRCList (void);
    u16        getExtensionLength (void);
    u16        getTotalExtensionLength (void);
    u8        *getExtensionData (void);
    u16        getExtensionId (void);
    bool       getExt (void);
    u8         getCC (void);
    bool       getPadding (void);

    void setDataLength(u16 len);
    void setSSRC(u32 SSRC);
};


#endif

