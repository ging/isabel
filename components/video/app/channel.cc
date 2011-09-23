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
// $Id: channel.cc 21889 2011-01-18 17:30:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "channel.hh"

channel_t::channel_t(sched_t *ctx,
                     u32 nchId,
                     channelKind_e k,
                     const char *compId
                    )
: chId(nchId),
  channelKind(k),
  context(ctx)
{
    myStats= new stats_t(chId, compId);

    winSink= new winSink_t(ctx, chId, myStats, 1);
}

channel_t::~channel_t(void)
{
    delete winSink;

    delete myStats;
}

measures_ref
channel_t::getStatistics(void)
{
    measures_ref mr= myStats->getStatistics();

    myStats->reset();

    return mr;
}

void
channel_t::mapWindow(void)
{
    winSink->mapWindow();
}

void
channel_t::unmapWindow(void)
{
    winSink->unmapWindow();
}

void
channel_t::setWindowGeometry(const char *geom)
{
    winSink->setWindowGeometry(geom);
}

void
channel_t::setWindowOverrideRedirect(bool ORFlag)
{
    winSink->setWindowOverrideRedirect(ORFlag);
}

void
channel_t::setWindowTitle(const char *a, const char *t)
{
    winSink->setWindowTitle(a, t);
}

u32
channel_t::getWinId(void)
{
    return winSink->getWinId();
}

void
channel_t::setAvatarImage(image_t *img)
{
    winSink->setAvatarImage(img);
}

void
channel_t::setAvatarTimeout(unsigned lapse)
{
    winSink->setAvatarTimeout(lapse);
}

