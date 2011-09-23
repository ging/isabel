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
// $Id: RTPPacketProcessor.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __RTP_packetprocessor_hh__
#define __RTP_packetprocessor_hh__

#include <icf2/sockIO.hh>
#include <icf2/item.hh>
#include <icf2/dictionary.hh>
#include <icf2/ql.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPacket.hh>


/**
* This class is used to:
* - Check if the RTP header and the RTCP headers of
*   RTP/RTCP compound packets are correct.
* - Form internal library messages corresponding to the
*   RTCP individual packets.
*/
class  RTPPacketProcessor_t: public item_t
{
public:

    /**
    * RTPPacketProcessor_t constructor.
    */
    RTPPacketProcessor_t (void);

    /**
    * RTPPacketProcessor_t destructor.
    */
    ~RTPPacketProcessor_t (void);

    /**
    * Checks if the RTP header of each packet is correct or not.
    * @param packet: buffer obtained from the reception of a RTP packet.
    * @param length: RTP packet length.
    * @param realLength: A RTP packet can have padding. The method returns
    *                    the real length in this parameter.
    * @returns TRUE if the RTP header is correct and FALSE if not.
    */
    bool checkRTP (u8 *packet, int length, int &realLength);

    /**
    * Checks if each RTCP individual packet' header within a
    * RTCP compound packet is correct.
    * @param packet: buffer obtained from the reception of a
    *                RTCP compound packet.
    * @param length: RTCP compound packet length.
    * returns TRUE if each RTCP header is correct, FALSE if not.
    */
    bool checkRTCP (u8 *packet, int length);

    /**
    * Returns a message corresponding to a RTCP SR and/or RR packet received.
    * @param packet: buffer obtained from the reception of a RTCP SR
    *                and/or RR packet.
    * @param length: RTCP SR and/or RR packet length.
    * @returns A RTPMsgRTCPReport_t message.
    */
    RTCPReport_t *getReport (u8 *packet, int &length);

    /**
    * Returns a list of RTPMsgRTCPSDES_t messages corresponding to
    * a RTCP SDES packet received.
    * @param packet: buffer obtained from the reception of a RTCP SDES packet.
    * @param length: RTCP SDES packet length.
    * @returns A list of RTPMsgRTCPSDES_t.
    */
    ql_t<RTCPSDES_t *> *getSDES (u8 *packet, int &length);

    /**
    * Returns a RTPMsgRTCPBYE_t message corresponding to a RTCP BYE
    * packet received.
    * @param packet: buffer obtained from the reception of a RTCP BYE packet.
    * @param length: RTCP BYE length.
    * @returns A RTPMsgRTCPBYE_t message.
    */
    RTCPBYE_t *getBYE (u8 *packet, int &length);

    virtual const char *className(void) const { return "RTPPacketProcessor_t"; }
};


#endif

