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
// $Id: videoRTPSession.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __video_rtp_session_hh__
#define __video_rtp_session_hh__

#include <rtp/RTPSession.hh>

#include "channelMgr.hh"

class videoRTPSession_t: public RTPSession_t
{
private:
    channelMgr_t *channelMgr;

public:
    videoRTPSession_t (sched_t      *ctx,
                       channelMgr_t *chMgr,
                       const char   *rtpHost,
                       const char   *rtpPort,
                       const char   *rtcpHost,
                       const char   *rtcpPort,
                       double        tsUnit
                      );
    virtual ~videoRTPSession_t(void);

    virtual void receivedData (RTPPacket_t *packet);
    virtual void eliminateReceiver (u32 chId, const char *reason);

    virtual const char *className(void) const { return "videoRTPSession_t"; }

    friend class smartReference_t<videoRTPSession_t>;
};
typedef smartReference_t<videoRTPSession_t> videoRTPSession_ref;

extern videoRTPSession_ref rtpSession;

#endif

