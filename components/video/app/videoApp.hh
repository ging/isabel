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
// $Id: videoApp.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __video_app_hh__
#define __video_app_hh__

#include <icf2/stdTask.hh>

#include "channelMgr.hh"
#include "videoRTPSession.hh"

class videoApp_t: public application_t
{
private:
    streamSocket_t *serverSock;

    bool srcV4L;
    bool srcWin;
    bool srcShm;

public:
    displayTask_ref dpy;
    channelMgr_t *channelMgr;

    videoApp_t(int &argc, argv_t &argv);

    virtual ~videoApp_t(void);

    void doStats(bool doIt);
    void updateSources(void);

    char const *className(void) const { return "videoApp_t"; }
};

extern videoApp_t *theApp;

#endif
