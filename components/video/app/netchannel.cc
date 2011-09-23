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
// $Id: netchannel.cc 20791 2010-07-08 14:25:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>
#include <icf2/ql.hh>

#include "channelMgr.hh"
#include "netchannel.hh"
#include "videoApp.hh"

////////////////////////////////////////////////////
//               SOURCE CHANNEL                   //
////////////////////////////////////////////////////

const int MAX_NET_BUF_FRAMES= 4;

netChannel_t::netChannel_t(sched_t    *ctx,
                           u32         chId,
                           const char *rtpPort,
                           const char *rtcpPort,
                           const char *compId
                          )
: channel_t(ctx, chId, channel_t::NET_CHANNEL, compId),
  noFlowYet(true),
  SSRC(0),
  numFrames(0),
  eachFrame(2),
  sending(false)
{
    netSink= new netSink_t(chId, INITIAL_BW, myStats);

    rtp2image= new rtp2image_t(MAX_NET_BUF_FRAMES);

    myStats->setDesiredBW(INITIAL_BW);
    myStats->setDesiredFR((1/0.1));

    rtpSession->assignSender(chId);

    // source of this channel: a RTP session
    extRTPSession= new extRTPSession_t(ctx,
                                       this,
                                       NULL,
                                       rtpPort,
                                       NULL,
                                       rtcpPort,
                                       VIDEO_RTP_TSU
                                      );
    NOTIFY("netChannel_t::netChannel_t: port: %s\n", rtpPort);
    extRTPSession->deliverAnyPacket(true);
}

netChannel_t::~netChannel_t(void)
{
    delete netSink;

    delete rtp2image;

    rtpSession->deleteSender(this->getId(), "Channel deleted");

    if (SSRC != 0)
    {
        extRTPSession->deleteFlow(SSRC);
    }
    extRTPSession->killSockets();
    extRTPSession = NULL;
}

bool
netChannel_t::setGrabGeometry(const char *geom)
{
    NOTIFY("netChannel_t::setGrabGeometry: trying to set to %s\n", geom);

    return false;
}


void
netChannel_t::setSendFrames(int n)
{
    assert (n > 0);

    eachFrame= n;
}


bool
netChannel_t::setSource(const char *srcInfo, const char *camera)
{
#if 0
    NOTIFY("netChannel_t::setSource(%s,%s)\n", srcInfo, camera);

    theSource= NULL;
    sourceTask->stopAndRelease();

    if (strcmp(srcInfo, "NONE") == 0)
    {
        return true;
    }

    source_ref s;
    sourceFactoryInfoList_ref sfl= getSourceFactoryInfoList();

    for (ql_t<sourceFactoryInfo_ref>::iterator_t i= sfl->begin();
         i != sfl->end();
         i++
        )
    {
        sourceFactoryInfo_ref sfi= i;
        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        if (strcmp(srcInfo, sd->getName()) == 0)
        {
            s= sf->createSource(sd, camera);

            if (s.isValid())
                break;
        }
    }
    if ( ! s.isValid())
    {
        NOTIFY("netChannel_t::setSource: Grabber NOT found\n");
        return false;
    }

    theSource= s;
    theSource->setGrabFormat(I420P_FORMAT);

    sourceTask->tlPostMsg(new setSourceMsg_t(source));
    sourceTask->tlPostMsg(new startMsg_t(1/FR));

    return true;
#else
    return false;
#endif
}


const char*
netChannel_t::getSourceInfo(void)
{
#if 0
    if ( ! source.isValid())
    {
        NOTIFY("netChannel_t::getSourceInfo cannot get source info "
               "[invalid source]\n"
              );
        return NULL;
    }

    return source->getDescriptor()->getName();
#else
    return "yo-que-se";
#endif
}


const char*
netChannel_t::getCamera(void)
{
#if 0
    if ( ! source.isValid())
    {
        NOTIFY("netChannel_t::getSourceInfo cannot get camera name "
               "[invalid source]\n"
              );
        return NULL;
    }

    return source->getPropertyValue("InputPort");
#else
    return "RTP source";
#endif
}


void
netChannel_t::sendFlow(bool doIt)
{
    sending= doIt;
}

void
netChannel_t::setChannelBandwidth (double bw)
{
    netSink->setBandwidth((int)bw); // ojo, sólo para compilar
}


void
netChannel_t::setLineFmt(u32 newFmt)
{
    netSink->setLineFmt(newFmt);
}


void
netChannel_t::setCodecQuality(int quality)
{
    netSink->setCodecQuality(quality);
}


void
netChannel_t::setH263Mode(const char *mode)
{
    netSink->setCodecMode(mode);
}


void
netChannel_t::setMpegPBetweenI(int pbeti)
{
    netSink->setCodecPBetI(pbeti);
}


void
netChannel_t::addPkt(RTPPacket_t *pkt)
{
    //NOTIFY ("netChannel_t::addPkt: "
    //        "Channel: %d -- TS: %u -- Seq. number: %u -- LastFragment: %d\n",
    //        pkt->getSSRC(),
    //        pkt->getTimestamp(),
    //        pkt->getSequenceNumber(),
    //        pkt->getMark()
    //       );

    myStats->accountRecvBytes(pkt->getTotalLength());

    u32 ssrc= pkt->getSSRC();

    if ( (noFlowYet) || (SSRC != ssrc) )
    {
        extRTPSession->deleteFlow(SSRC);
        SSRC = ssrc;
        extRTPSession->newFlow(ssrc);
        //extRTPSession->deliverAnyPacket(false);
        noFlowYet= false;
    }

    image_t *newImg= rtp2image->addPkt(pkt);

    if (newImg == NULL)
    {
        // not image yet
        return;
    }

    myStats->accountEnsembledFrame();
    myStats->setCodecInUse(vGetFormatNameById(newImg->getFormat()));
    myStats->setImageSize(newImg->getWidth(), newImg->getHeight());

    if ((numFrames % eachFrame) == 0)
    {
        if (sending)
        {
            image_t *sendImg= new image_t(newImg->getBuff(),
                                          newImg->getNumBytes(),
                                          newImg->getFormat(),
                                          newImg->getWidth(),
                                          newImg->getHeight(),
                                          newImg->getTimestamp()
                                         );
            netSink->sendFrame(sendImg);
        }
        numFrames= 0;
    }
    numFrames++;

    winSink->paintFrame(newImg);
}

