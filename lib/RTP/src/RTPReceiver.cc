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
// $Id: RTPReceiver.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPReceiver.hh>

RTPReceiver_t::RTPReceiver_t (dgramSocket_t *rtpSocket, RTPSession_t *ses)
:simpleTask_t (rtpSocket)
{
    debugMsg(dbg_App_Verbose, "RTPReceiver_t", "Creating RTPReceiver_t");
    session = ses;
    buffer = (unsigned char*)malloc (RTP_BUFFER_SIZE);
    if (!buffer)
    {
        NOTIFY ("RTPReceiver_t::RTPReceiver_t:: No enough memory for buffer\n");
    }
    packetProcessor = new RTPPacketProcessor_t;
}

RTPReceiver_t::~RTPReceiver_t (void)
{
    delete packetProcessor;
    free (buffer);
    debugMsg(dbg_App_Verbose, "~RTPReceiver_t", "Deleting RTPReceiver_t");
}

void
RTPReceiver_t::IOReady (io_ref &io)
{
    int n = io -> read (buffer, RTP_BUFFER_SIZE);

    debugMsg(dbg_App_Normal, "IOReady_t", "Receiving Packet Size: %d", n);

    int realLength;
    bool status = packetProcessor->checkRTP (buffer, n, realLength);
    if (status && realLength > 0)
    {
        RTPPacket_t *pkt= new RTPPacket_t (buffer, realLength);
        session->receivePacket (pkt);
        delete pkt;
    }
    else
    {
        NOTIFY ("RTPReceiver_t::IOReady:: Problems with the RTP header, "
                "skipping packet\n"
               );
    }
}
