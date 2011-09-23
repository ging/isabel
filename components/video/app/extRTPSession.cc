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
// $Id: extRTPSession.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "extRTPSession.hh"
#include "netchannel.hh"

extRTPSession_ref extrtpSession;

extRTPSession_t::extRTPSession_t(sched_t      *theSched,
                                 netChannel_t *myCh,
                                 const char   *rtpHost,
                                 const char   *rtpPort,
                                 const char   *rtcpHost,
                                 const char   *rtcpPort,
                                 double        tsUnit
                                )
: RTPSession_t (theSched, rtpHost, rtpPort, rtcpHost, rtcpPort, tsUnit),
  lastSeqNumber(0)
{
    myChannel= myCh;
}

void
extRTPSession_t::receivedData (RTPPacket_t *packet)
{
#if 0
    NOTIFY ("extRTPSession::receivedData: Channel: %d -- Size: %d -  LastFragment: %d -- TS: %u -- Seq. number: %u\n",
             packet -> getSSRC(),
             packet -> getDataLength(),
             packet -> getMark(),
             packet -> getTimestamp(),
             packet -> getSequenceNumber()
           );

    if ( (packet -> getSequenceNumber()) != (lastSeqNumber+1) ){
         NOTIFY("extRTPSession::receivedData: FALTAN PAQUETES RTP: "
                "last Seq. number: %u -- Seq. number: %u\n",
                lastSeqNumber,
                packet -> getSequenceNumber()
               );
    }
    lastSeqNumber = packet -> getSequenceNumber();

#endif

    //NOTIFY("extRTPSession::receivedData: "
    //       "Channel: %d -- Size: %d -  "
    //       "LastFragment: %d -- TS: %u -- Seq. number: %u\n",
    //       packet -> getSSRC(),
    //       packet -> getDataLength(),
    //       packet -> getMark(),
    //       packet -> getTimestamp(),
    //       packet -> getSequenceNumber()
    //      );

    myChannel->addPkt(packet);
}


void
extRTPSession_t::eliminateReceiver (u32 chId, const char *reason)
{
    NOTIFY("extRTPSession_t::eliminateReceiver: "
           "Cannot delete channel, sorry\n"
          );
}


