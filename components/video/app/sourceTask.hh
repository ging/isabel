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
// $Id: sourceTask.hh 21929 2011-01-25 16:59:33Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __source_task_hh__
#define __source_task_hh__

#include <icf2/task.hh>
#include <icf2/lockSupport.hh>

#include <Isabel_SDK/sourceDefinition.hh>

#include "channel.hh"
#include "lockedImage.hh"

class sourceTask_t: public threadedTask_t, public lockedItem_t
{
private:
    io_ref           pipe;
    source_ref       source;
    lockedImage_ref  imgBuff;

    u32 prev_period; // remember previous grab rate if source gives errors
    int numWarnings; // failed images from source, if > 0, stops grabbing

    virtual ~sourceTask_t(void);

protected:
    void __preInSchedRites(sched_t*)
    {
        tlRun();
    }

    virtual void timeOut(const alarm_ref &ar);
    virtual void heartBeat(void);

public:
    sourceTask_t(io_ref pipe, lockedImage_ref ib);

    virtual u32 set_period(u32 p);

    void stopAndRelease(void);
    void setSource(source_ref newSource);

    char const *className(void) const { return "sourceTask_t"; }

    friend class smartReference_t<sourceTask_t>;
};
typedef smartReference_t<sourceTask_t> sourceTask_ref;

#endif

