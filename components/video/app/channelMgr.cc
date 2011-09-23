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
// $Id: channelMgr.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "channelMgr.hh"
#include "recchannel.hh"
#include "netchannel.hh"
#include "playchannel.hh"

channelMgr_t::channelMgr_t(sched_t *ctx, const char *AgentPort)
: simpleTask_t(), context(ctx)
{
    debugMsg(dbg_App_Paranoic, "channelMgr_t", "constructor invoked");

    inetAddr_t statsAddr1("localhost", NULL, serviceAddr_t::DGRAM);
    inetAddr_t statsAddr2("localhost", AgentPort, serviceAddr_t::DGRAM);

    sockSNMP = new dgramSocket_t(statsAddr1, statsAddr2);
}


channelMgr_t::~channelMgr_t(void)
{
    debugMsg(dbg_App_Paranoic, "~channelMgr_t", "destructor invoked");
}


channel_t*
channelMgr_t::__getChannel(u32 chId)
{
    std::map<u32,channel_t*>::iterator iter= byIdentifier.find(chId);

    if (iter == byIdentifier.end()) // not found
    {
        return NULL;
    }

    return iter->second;
}


void
channelMgr_t::__deleteChannel(channel_t *ch)
{
    byIdentifier.erase(ch->getId());

    buryStats(ch->getId(), ch->getCompId());

    delete ch;
}


bool
channelMgr_t::newRecChannel(u32 chId, const char *compID)
{
    if (__getChannel(chId))
    {
        return false;
    }

    channel_t *ch= new recChannel_t(context, chId, compID);

    byIdentifier[chId]= ch;

    return true;
}


bool
channelMgr_t::newNetChannel(u32         chId,
                            const char *rtpPort,
                            const char *rtcpPort,
                            const char *compID
                           )
{
    if (__getChannel(chId))
    {
        return false;
    }

    if ( ! rtpPort || ! rtcpPort )
    {
        NOTIFY ("channeMgrl_t::extChannel_t: Impossible create this channel. "
                "External RTP port or external RTCP Port not defined\n"
               );
        return false;
    }

    channel_t *ch= new netChannel_t(context, chId, rtpPort, rtcpPort, compID);

    byIdentifier[chId]= ch;

    return true;
}


bool
channelMgr_t::newPlayChannel(u32 chId, const char *compID)
{
    if (__getChannel(chId))
    {
        return false;
    }

    channel_t *ch= new playChannel_t(context, chId, compID);

    byIdentifier[chId]= ch;

    return true;
}


bool
channelMgr_t::deleteChannel(u32 chId)
{
    channel_t *ch= __getChannel(chId);

    if (ch == NULL)
    {
        return false;
    }

    __deleteChannel(ch);

    return true;
}


channel_t*
channelMgr_t::getChannel(u32 chId)
{
    return __getChannel(chId);
}


void
channelMgr_t::deleteAllChannels(void)
{
    std::map<u32,channel_t*>::iterator iter;

    for (iter= byIdentifier.begin(); iter != byIdentifier.end(); ++iter)
    {
        __deleteChannel(iter->second);
    }
}


void
channelMgr_t::buryStats(u32 chId, const char *compID)
{
    stats_ctrl_packet_t info(compID, chId);

    sockSNMP->write(info.serialize(), info.PDUSize());
}

void
channelMgr_t::heartBeat(void)
{
    std::map<u32, channel_t*>::iterator iter;

    for (iter= byIdentifier.begin(); iter != byIdentifier.end(); ++iter)
    {
        channel_t *cr= iter->second;

        // Cojo lo que tengo que publicar
        measures_ref vm= cr->getStatistics();

        stats_data_packet_t info(vm);

        sockSNMP->write(info.serialize(), info.PDUSize());
    }
}


