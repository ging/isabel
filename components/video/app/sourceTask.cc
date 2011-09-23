
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
// $Id: sourceTask.cc 21973 2011-01-26 16:38:33Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "sourceTask.hh"

sourceTask_t::sourceTask_t(io_ref io, lockedImage_ref ib)
: pipe(io),
  source(NULL),
  imgBuff(ib),
  prev_period(0),
  numWarnings(0)
{
}

sourceTask_t::~sourceTask_t(void)
{
}

void
sourceTask_t::stopAndRelease(void)
{
    locker_t the_lock= lock();

    set_period(0);
    source= NULL;
    numWarnings= 0;
}

void
sourceTask_t::setSource(source_ref newSource)
{
    locker_t the_lock= lock();

    source= newSource;
    numWarnings= 0;
}

u32
sourceTask_t::set_period(u32 p)
{
    // I need to overload, as recChannel_t may change this
    // and previous rate is required

    prev_period= threadedTask_t::set_period(p);

    // instead of cancelling the alarm, set numWarnings to zero
    // timeOut method takes it into account
    numWarnings= 0;

    return prev_period;
}

void
sourceTask_t::timeOut(const alarm_ref &ar)
{
    NOTIFY("sourceTask_t::timeOut: alarm, trying to recover source\n");

    if ( ! source.isValid() )
    {
        NOTIFY("sourceTask_t::timeOut: source is not valid anymore\n");
        return;
    }

    if (get_period() > 0)
    {
        // the source was restarted by channel, nothing to do here
        NOTIFY("sourceTask_t::timeOut: prev_period > 0, alarm not needed\n");
        return;
    }

    source->startGrabbing();
    numWarnings= 0;

    threadedTask_t::set_period(prev_period);
    NOTIFY("imageWorker_t::timeOut: setting period %d\n", prev_period);
}

void
sourceTask_t::heartBeat(void)
{
    locker_t the_lock= lock();

    debugMsg(dbg_App_Paranoic, "sourceTask_t::heartBeat", "");

    if ( ! source.isValid() )
    {
        return;
    }

    image_t *img= NULL;
    img= source->getImage();

    if (img == NULL)
    {
        NOTIFY("sourceTask_t::heartBeat: invalid image, discarding\n");
        numWarnings++;
        if (numWarnings > 2)
        {
            NOTIFY("imageWorker_t::heartBeat: ENOUGH!, stopping source");
            NOTIFY_ND(" (will try to restart source in five seconds)\n");
            prev_period= set_period(0); // remember to restart at same rate
            source->stopGrabbing();
            this->set_alarm(5000000, NULL);
        }
        return;
    }
    numWarnings= 0;

    imgBuff->putImage(img);
    pipe->write("GO", 2);
}

