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
// $Id: RTCPReceiver.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_rtcp_receiver_hh__
#define __rtp_rtcp_receiver_hh__

#include <icf2/smartReference.hh>
#include <icf2/task.hh>
#include <icf2/ql.hh>
#include <icf2/sockIO.hh>

#include <rtp/RTPSession.hh>
#include <rtp/RTPStructs.hh>
#include <rtp/RTPPacketProcessor.hh>

class RTPSession_t;

/**
* This class is used to receive and process RTCP compound packets
* by means of the corresponding socket.
* This processing consists of:
* - Checking if the RTCP headers are correct.
* - If single packets are correct, RTCPReceiver_t will form internal
*   library messages with the information carried in RTCP packets and
*   will send them to the RTP session.
*/
class RTCPReceiver_t: public simpleTask_t
{
private:
    RTPPacketProcessor_t* packetProcessor;
    RTPSession_t *session;
    u8 *buffer;

public:
    /**
    * RTCPReceiver_t constructor.
    * @param rtcpSocket: This is the socket by means of RTCP compound
    * packets will be received.
    * @param ses: RTP Session which internal library messages will be sent to.
    */
    RTCPReceiver_t (dgramSocket_t *rtcpSocket, RTPSession_t *ses);

    /**
    * RTCPReceiver_t destructor.
    */
    ~RTCPReceiver_t (void);

    /**
    * This method is used to be called when an I/O event happens,
    * in this case, a RTCP compound packet arrives.
    */
    void IOReady (io_ref &io);

    virtual const char *className(void) const { return "RTCPReceiver_t"; }

    friend class smartReference_t<RTCPReceiver_t>;
};
typedef smartReference_t<RTCPReceiver_t> RTCPReceiver_ref;


#endif

