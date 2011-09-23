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
// $Id: sourceTask.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>
#include <icf2/ql.hh>

#include "shmSrcApp.hh"
#include "sourceTask.hh"
#include "srcMgr.hh"

sourceTask_t::sourceTask_t(io_ref io, source_ref src, lockedImage_ref ib)
: pipe(io),
  source(src),
  imgBuff(ib),
  timeBetweenFrames(1.0/25.0),
  running(false)
{
}

sourceTask_t::~sourceTask_t(void)
{
}

bool
sourceTask_t::dispatchMsg(tlMsg_t *m)
{
    debugMsg(dbg_App_Paranoic,
             "sourceTask_t::dispatchMsg",
             "Dispatching msg [0x%08x]\n",
             m->msgKind
            );

    switch(m->msgKind)
    {
    case MSG_START:
        {
            startMsg_t *sm= static_cast<startMsg_t*>(m);

            setTimeBetweenFrames(sm->tbf);
            running= true;
        }
        return true;
    case MSG_STOP:
        {
            this->set_period(0);
            running= false;
        }
        return true;
    case MSG_SET_SOURCE:
        {
            setSourceMsg_t *ssm= static_cast<setSourceMsg_t*>(m);

            source= ssm->source;
        }
        return true;
    case MSG_SET_TBF:
        {
            setTimeBetweenFramesMsg_t *stbfm=
                static_cast<setTimeBetweenFramesMsg_t*>(m);

            setTimeBetweenFrames(stbfm->tbf);
        }
        return true;
    default:
        return threadedTask_t::dispatchMsg(m);
    }
}

void
sourceTask_t::setTimeBetweenFrames(double tbf)
{
    if ( ! source.isValid() )
    {
        NOTIFY("sourceTask_t::setTimeBetweenFrames: invalid source\n");
        return;
    }

    double fps= 1/tbf;

    source->setFrameRate(fps);

    double cfps= source->getFrameRate();

    if (cfps == 0.0)
    {
        // source is unable to deliver frames, so...
        NOTIFY("sourceTask_t::setTimeBetweenFrames: "
               "source is not ready, fps= 0\n"
              );
        set_period(0);
        return;
    }
    if (cfps > 0)
    {
        // source imposes the frame rate, so...
        tbf= 1 / cfps;
    }
    else
    {   // < 0, any rate may apply
        tbf = 1 / fps;
    }

    unsigned newPeriod= (u32)(tbf*1000000);
    this->set_period(newPeriod);

    NOTIFY("Asynchronous source: period=%u, TBF=%f (%f fps)\n",
           newPeriod,
           tbf,
           1.0 / tbf
          );
}

void
sourceTask_t::heartBeat(void)
{
    debugMsg(dbg_App_Paranoic, "sourceTask_t::heartBeat", "");

    if ( ! running)
    {
        return;
    }

    if ( ! source.isValid() )
    {
        return;
    }

    image_t *img= source->getImage();

    if (img == NULL)
    {
        NOTIFY("sourceTask_t::heartBeat: "
               "invalid image from source, discarding\n"
              );
        return;
    }

    imgBuff->putImage(img);
    pipe->write("OK", 2);
}

