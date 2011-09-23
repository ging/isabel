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
// $Id: srcMgr.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __recchannel_hh__
#define __recchannel_hh__

#include <icf2/sched.hh>

#include <Isabel_SDK/sourceDefinition.hh>

#include "lockedImage.hh"
#include "shmSink.hh"

class imageWorker_t: public simpleTask_t
{
private:
    lockedImage_ref  imgBuff;
    shmSink_ref      shmSink;

protected:
    virtual void IOReady(io_ref &io);

public:
    imageWorker_t(io_ref io, lockedImage_ref ib, shmSink_ref ssr);

    virtual ~imageWorker_t(void);
};


class srcMgr_t: public virtual item_t, public virtual collectible_t
{
private:
    sched_t          *context;
    threadedTask_ref  sourceTask;
    source_ref        theSource; // shared with sourceTask, created here
    int               notificationPipe[2];
    shmSink_ref       shmSink;
    imageWorker_t    *imgWorker;

public:
    srcMgr_t(sched_t *ctx, int shmId, int shmSize);

    virtual ~srcMgr_t(void);

    // source related parameters
    void           setGrabSize(unsigned w, unsigned h);
    void           setFrameRate(double fr);
    void           setVideoSaturation(int value);
    int            getVideoSaturation(void);
    void           setVideoBrightness(int value);
    int            getVideoBrightness(void);
    void           setVideoHue(int value);
    int            getVideoHue(void);
    void           setVideoContrast(int value);
    int            getVideoContrast(void);
    std::string    getVideoStandardList();
    bool           setVideoStandard(const char *norm);
    std::string    getVideoStandard(void);

    void           setSource(char *srcInfo, char *camera);
    const char    *getSourceInfo(void);
    const char    *getCamera(void);

    // administrivia related parameters
    virtual const char *className(void) const { return "srcMgr_t"; }
};

#endif
