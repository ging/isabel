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
// $Id: RTPReceiver.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_receiver_hh__
#define __rtp_receiver_hh__

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPacketProcessor.hh>
#include <rtp/RTPPacket.hh>
#include <rtp/RTPSession.hh>

#include <icf2/smartReference.hh>
#include <icf2/ql.hh>
#include <icf2/task.hh>



class RTPSession_t;


/**
* This class is used to receive and process RTP packets
* by means of the corresponding socket.
* This processing consists of:
* - Checking if the RTP header is correct.
* - If the packet is correct, RTPReceiver_t will form internal library
*   a message with the information carried in RTP packet and
*   will send it to the RTP session.
*/
class  RTPReceiver_t: public simpleTask_t
{
public:

    /**
    * RTPReceiver_t constructor.
    * @param rtpSocket: socket by means of RTP packets will be received.
    * @param ses: RTP Session which internal library messages will be sent to.
    */
    RTPReceiver_t (dgramSocket_t *rtpSocket, RTPSession_t *ses);

    /**
    * RTPReceiver_t destructor.
    */
    ~RTPReceiver_t (void);

    /**
    * called when an I/O event happens, in this case, a RTCP compound
    * packet reception.
    */
    void IOReady (io_ref &io);

    virtual const char *className(void) const { return "RTPReceiver_t"; }

    friend class smartReference_t<RTPReceiver_t>;

private:
    RTPSession_t *session;
    RTPPacketProcessor_t *packetProcessor;
    u8 *buffer;
};

typedef smartReference_t<RTPReceiver_t> RTPReceiver_ref;

#endif

