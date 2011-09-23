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
// $Id: RTCPReceiver.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTCPReceiver.hh>

RTCPReceiver_t::RTCPReceiver_t (dgramSocket_t *rtcpSocket, RTPSession_t *ses)
: simpleTask_t (rtcpSocket)
{
    debugMsg(dbg_App_Verbose, "RTCPReceiver_t", "Creating RTCPReceiver_t"); 
    session = ses;

    buffer = (u8*) malloc (RTP_BUFFER_SIZE);
    if ( ! buffer)
    {
        NOTIFY ("RTCPReceiver_t::RTCPReceiver_t:: No enough memory for buffer\n");
    }
    packetProcessor = new RTPPacketProcessor_t; 
}


RTCPReceiver_t::~RTCPReceiver_t (void)
{
    delete packetProcessor;
    free (buffer);
    debugMsg(dbg_App_Verbose, "~RTCPReceiver_t", "Deleting RTCPReceiver_t");
}


void 
RTCPReceiver_t::IOReady (io_ref &io)
{
    int n = io -> read (buffer, RTP_BUFFER_SIZE); 
    debugMsg(dbg_App_Verbose, "IOReady_t", "Receiving Packet Size: %d", n);
    bool status = packetProcessor -> checkRTCP (buffer, n);
    if (status)
    {
        int lengthProcessed = 0;
        if (lengthProcessed < n)
        {
            RTCPHeader_t *rtcpHeader= (RTCPHeader_t*)(buffer + lengthProcessed);
            if  ((rtcpHeader -> packettype == TYPE_RTCP_SR) || 
                 (rtcpHeader -> packettype == TYPE_RTCP_RR) )
            {
                RTCPReport_t *report = packetProcessor -> getReport (buffer, lengthProcessed);
                if (report != NULL)
                {
                    session -> receiveReport (report);
                }
            }
        }
        if (lengthProcessed < n)
        {
            RTCPHeader_t *rtcpHeader= (RTCPHeader_t*)(buffer + lengthProcessed);
            if (rtcpHeader -> packettype == TYPE_RTCP_SDES)
            {
                ql_t<RTCPSDES_t *> *sdes = packetProcessor -> getSDES (buffer, lengthProcessed);
                if (sdes != NULL)
                {
                    for (ql_t<RTCPSDES_t *>::iterator_t i = sdes -> begin(); i != sdes -> end(); i++)
                    {
                        RTCPSDES_t *aux = static_cast<RTCPSDES_t *> (i);
                        session -> receiveSDES (aux);
                    }
                }
            }
        }
        if (lengthProcessed < n)
        {
            RTCPHeader_t *rtcpHeader= (RTCPHeader_t*)(buffer + lengthProcessed);
            if (rtcpHeader -> packettype == TYPE_RTCP_BYE)
            {
                RTCPBYE_t *bye = packetProcessor -> getBYE (buffer, lengthProcessed);
                if (bye != NULL)
                {
                    session -> receiveBYE (bye);
                }
            }
        }
    }else{
        NOTIFY("RTCPReceiver_t::IOReady:: Problems with the RTCP header, "
               "skipping packet\n");
    }
}
