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
// $Id: netchannel.hh 20770 2010-07-07 13:19:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __netchannel_hh__
#define __netchannel_hh__

#include <vCodecs/codecs.h>

#include <icf2/sched.hh>

#include <rtp/defragmenter.hh>

#include <vUtils/rtp2image.h>

#include "channel.hh"
#include "netSink.hh"
#include "extRTPSession.hh"

class netChannel_t: public channel_t
{
private:
    extRTPSession_ref  extRTPSession;

    netSink_t   *netSink;

    rtp2image_t *rtp2image;

    bool     noFlowYet;
    u32      SSRC;
    long     numFrames;
    unsigned eachFrame;

    bool sending; // true is video flow is being sent

public:
    netChannel_t(sched_t    *ctx,
                 u32         chId,
                 const char *rtpPort,
                 const char *rtcpPort,
                 const char *compId
                );

    virtual ~netChannel_t(void);

    // source related parameters
    bool           setGrabGeometry(const char *geom);
    void           setSendFrames(int n);

    bool           setSource(const char *srcInfo, const char *camera);
    const char    *getSourceInfo(void);
    const char    *getCamera(void);

    // line related parameters
    void           sendFlow(bool doIt);
    void           setChannelBandwidth(double bw);
    void           setLineFmt(u32 newFmt);

    void           setCodecQuality(int quality);
    void           setH263Mode(const char *factor);
    void           setMpegPBetweenI(int pbeti);

    // reception methods
    void addPkt(RTPPacket_t *pkt);
};

#endif
