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
// $Id: playchannel.cc 20956 2010-07-26 14:07:02Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <rtp/RTPPayloads.hh>

#include "channelMgr.hh"
#include "videoApp.hh"
#include "videoRTPSession.hh"
#include "playchannel.hh"

const int MAX_BUF_FRAMES= 4;

playChannel_t::playChannel_t(sched_t *ctx, u32 chId, const char *compId)
: channel_t(ctx, chId, channel_t::PLAY_CHANNEL, compId)
{
    winSink->setAvatarTimeout(5);

    rtp2image= new rtp2image_t(MAX_BUF_FRAMES);

    rtpSession->newFlow(chId);
}

playChannel_t::~playChannel_t(void)
{
    delete rtp2image;

    rtpSession->deleteFlow(getId());
}

void
playChannel_t::addPkt(RTPPacket_t *pkt)
{
    //NOTIFY ("playChannel_t::addPkt: "
    //        "Channel: %d -- TS: %u -- Seq. number: %u -- LastFragment: %d\n",
    //        pkt->getSSRC(),
    //        pkt->getTimestamp(),
    //        pkt->getSequenceNumber(),
    //        pkt->getMark()
    //       );

    myStats->accountRecvBytes(pkt->getTotalLength());
    u32 fmt= getFmtByPT(pkt->getPayloadType());
    myStats->setCodecInUse(vGetFormatNameById(fmt));

    image_t *newImg= rtp2image->addPkt(pkt);

    if (newImg == NULL)
    {
        // not image yet
        return;
    }

    myStats->accountEnsembledFrame();
    myStats->setImageSize(newImg->getWidth(), newImg->getHeight());

    winSink->paintFrame(newImg);
}


void
playChannel_t::setFollowSize(bool followFlag)
{
    winSink->setFollowSize(followFlag);
}

