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
// $Id: channelMgr.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __channel_manager_hh__
#define __channel_manager_hh__

#include <map>

#include "channel.hh"

class channelMgr_t : public virtual simpleTask_t
{
private:
    sched_t *context;
    io_ref   sockSNMP;

private:
    std::map<u32, channel_t*> byIdentifier;

    channel_t *__getChannel   (u32 chId);
    void       __deleteChannel(channel_t *ch);

    void buryStats(u32 chId, const char *compID);

protected:
    virtual void heartBeat(void);

public:
    channelMgr_t(sched_t *ctx, const char *AgentPort);
    virtual ~channelMgr_t(void);

    bool newRecChannel(u32 chId, const char *compID);
    bool newNetChannel(u32 chId,
                       const char *rtpPort,
                       const char *rtcpPort,
                       const char *compID
                      );
    bool newPlayChannel(u32 chId, const char *compID);

    bool deleteChannel(u32 chId);
    void deleteAllChannels(void);

    channel_t * getChannel(u32 chId);

    char const *className(void) const { return "channelMgr_t"; }
};

//EXTERNAL channelMgr_t channelMgr;

#endif
