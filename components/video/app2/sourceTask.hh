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
// $Id: sourceTask.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __source_task_hh__
#define __source_task_hh__

#include <icf2/task.hh>

#include <Isabel_SDK/sourceDefinition.hh>

#include "lockedImage.hh"

enum sourceTaskMsg_e
{
    MSG_START= 0x9999,
    MSG_STOP,
    MSG_SET_SOURCE,
    MSG_SET_TBF
};

class startMsg_t: public tlMsg_t
{
public:
    double tbf;

    startMsg_t(double _tbf)
    : tlMsg_t(MSG_START), tbf(_tbf)
    {}
};

class setSourceMsg_t: public tlMsg_t
{
public:
    source_ref source;

    setSourceMsg_t(source_ref nSource)
    : tlMsg_t(MSG_SET_SOURCE), source(nSource)
    { }
};

class setTimeBetweenFramesMsg_t: public tlMsg_t
{
public:
    double tbf;

    setTimeBetweenFramesMsg_t(double _tbf)
    : tlMsg_t(MSG_SET_TBF), tbf(_tbf)
    {}
};

class sourceTask_t: public virtual threadedTask_t
{
private:
    io_ref           pipe;
    source_ref       source;
    lockedImage_ref  imgBuff;
    double           timeBetweenFrames;
    bool             running;

    virtual bool dispatchMsg(tlMsg_t *m);

    virtual void funeraryRites(void)
    {
        threadedTask_t::funeraryRites();
    }

    virtual ~sourceTask_t(void);

    void setSource(char *srcInfo, char *camera);
    void setGrabGeometry(unsigned w, unsigned h);
    void setTimeBetweenFrames(double timeBetweenFrames);

protected:
    virtual void __preInSchedRites(sched_t*)
    {
        tlRun();
    }

    virtual void heartBeat(void);

public:
    sourceTask_t(io_ref pipe, source_ref src, lockedImage_ref ib);

    virtual char const *className(void) const { return "sourceTask_t"; }

    friend class smartReference_t<sourceTask_t>;
};
typedef smartReference_t<sourceTask_t> sourceTask_ref;

#endif

