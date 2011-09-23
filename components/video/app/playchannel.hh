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
// $Id: playchannel.hh 20770 2010-07-07 13:19:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __playchannel_hh__
#define __playchannel_hh__

#include <icf2/sched.hh>

#include <vUtils/rtp2image.h>

#include "channel.hh"

#define MAX_IMG 6
#define MAX_PKT 200

class playChannel_t: public channel_t
{
private:
    rtp2image_t *rtp2image;

public:
    playChannel_t(sched_t *ctx, u32 chId, const char *compId);

    virtual ~playChannel_t(void);

    // line related parameters
    void setFollowSize(bool);

    void addPkt(RTPPacket_t *pkt);

    // administrivia related parameters
    virtual const char *className(void) const { return "playChannel_t"; }
};

#endif
