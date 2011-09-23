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
// $Id: videoRTPSession.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "videoRTPSession.hh"
#include "playchannel.hh"

videoRTPSession_ref rtpSession;

videoRTPSession_t::videoRTPSession_t(sched_t      *ctx,
                                     channelMgr_t *chMgr,
                                     const char   *rtpHost,
                                     const char   *rtpPort,
                                     const char   *rtcpHost,
                                     const char   *rtcpPort,
                                     double        tsUnit
                                    )
: RTPSession_t (ctx, rtpHost, rtpPort, rtcpHost, rtcpPort, tsUnit)
{
    channelMgr = chMgr;
}


videoRTPSession_t::~videoRTPSession_t(void)
{
    debugMsg(dbg_App_Paranoic, "~videoRTPSession_t", "destructor invoked");
}


void
videoRTPSession_t::receivedData (RTPPacket_t *packet)
{
    //NOTIFY("videoRTPSession_t::receivedData 1 SSRC=%d\n", packet->getSSRC());
    //NOTIFY ("Recibiendo paquete TS: %u SQ: %d -- Perdidos: %s\n",
    //        packet->getTimestamp(),
    //        packet -> getSequenceNumber(),
    //        rtpSession->getPacketsLost(packet->getSSRC())
    //       );

    //
    // get channel to enqueue data
    //
    channel_t *currChannel= channelMgr->getChannel(packet->getSSRC());

    if ( currChannel == NULL)
    {
        if (getenv("DYNAMIC"))
        {
            char chname[256];
            sprintf (chname, "%u", packet -> getSSRC());

            char title[256], acronym[256];
            strcpy (title, "Network video: Channel Identifier ");
            strcat (title, chname);
            title[255] = 0;
            strcpy (acronym, "NV:");
            strcat (acronym, chname);
            acronym[9] = 0;

            if ( ! channelMgr->newPlayChannel(packet->getSSRC(), "UNK"))
            {
                NOTIFY("videoRTPSession_t::receivedData "
                       "could not create play channel SSRC=%d\n",
                       packet->getSSRC()
                      );
                return;
            }
            channel_t *cr= channelMgr->getChannel(packet->getSSRC());
            playChannel_t *pc= dynamic_cast<playChannel_t *>(cr);
            pc->setWindowTitle(acronym, title);
            pc->mapWindow();
        }
        else
        {
            return;
        }
    }

    if (currChannel -> getKind() != channel_t::PLAY_CHANNEL)
    {
        if ( ! getenv("PIGGY"))
        {
            return;
        }

        currChannel = channelMgr->getChannel((packet->getSSRC()) + 1);

        if (currChannel == NULL)
        {
            return;
        }
        if (currChannel -> getKind()!= channel_t::PLAY_CHANNEL)
        {
            return;
        }
    }

    channel_t     *cht= currChannel;
    playChannel_t *pl = dynamic_cast<playChannel_t*>(cht);

    //NOTIFY ("videoRTPSession::receivedData: "
    //        "Channel: %d -- LastFragment: %d -- TS: %u -- Seq. number: %u\n",
    //        packet -> getSSRC(),
    //        packet -> getMark(),
    //        packet -> getTimestamp(),
    //        packet -> getSequenceNumber()
    //       );

    pl->addPkt(packet);
}


void
videoRTPSession_t::eliminateReceiver (u32 chId, const char *reason)
{
    channel_t *currChannel = channelMgr->getChannel(chId);

    if (   (currChannel != NULL)
        && (currChannel -> getKind() == channel_t::PLAY_CHANNEL)
       )
    {
        channelMgr->deleteChannel(chId);
        NOTIFY ("videoRTPSession_t::eliminateReceiver: "
                "Channel identifier: %d. Reason: %s\n",
                 chId, reason
               );
    }
}


