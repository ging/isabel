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
// $Id: channel.hh 21743 2010-12-17 10:44:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __channel_hh__
#define __channel_hh__

#include "stats.hh"
#include "winSink.hh"

class channel_t
{
public:
    enum channelKind_e
    {
        PLAY_CHANNEL,
        REC_CHANNEL,
        NET_CHANNEL
    };

private:
    u32           chId;
    channelKind_e channelKind;

protected:
    sched_t   *context;
    winSink_t *winSink;
    stats_t   *myStats;

public:
    channel_t(sched_t *ctx, u32 nchId, channelKind_e k, const char *compId);

    virtual ~channel_t(void);

    //
    // general administrivia
    //
    u32           getId(void)    const { return chId;        }
    channelKind_e getKind (void) const { return channelKind; }

    char *getCompId(void) const { return myStats->getCompo(); }

    measures_ref getStatistics(void);

    //
    // presentation related methods
    //
    virtual void mapWindow(void);
    virtual void unmapWindow(void);
    void setWindowGeometry(const char *geom);
    void setWindowOverrideRedirect(bool ORFlag);
    void setWindowTitle(const char *a, const char *t);
    u32 getWinId(void);

    //
    // source related methods
    //
    void setAvatarImage(image_t *img);
    void setAvatarTimeout(unsigned lapse);
};

#endif
