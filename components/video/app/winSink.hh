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
// $Id: winSink.hh 21890 2011-01-18 17:31:24Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __win_sink_hh__
#define __win_sink_hh__

#include <icf2/ql.hh>
#include <icf2/sched.hh>

#include <vUtils/image.h>
#include <vUtils/imgTranslator.h>

#include "renderer.hh"

#include "stats.hh"

#define MAX_ARRIVAL_DIFF 10000000.0
#define MAX_DISORDER 10
#define MAX_BUFFER_DIFF 3000000.0

class paintTask_t: public virtual simpleTask_t, public virtual lockedItem_t
{
public:
    paintTask_t(u32 nchId, renderer_t *rr, int maxTTL);

    virtual ~paintTask_t(void);

    void setFollowSize(bool nfollowSize);
    void setAcronym(const char *nacro);
    void setAvatarImage(image_t *img);
    void setAvatarTimeout(unsigned lapse);
    void unmapWindow (void);

    void paintImage (image_t *img);

    virtual void timeOut (const alarm_ref &alarm);
    virtual void heartBeat(void);

private:
    u32 chId;

    renderer_t *theRenderer;

    ql_t<image_t*>  imageList;
    image_t        *avatarImage;

    bool imagesPainted;
    u32  lastTS;
    int  disorderFrames;

    alarm_ref imageTimeout;

    struct timeval timeToPaint;
    struct timeval lastPaintTime;

    int ttl; // to check no available images
    int maxTTL;

    char *acro, *title;

    bool followSize;

    void paintAvatar(void);

public:

    virtual char const *className(void) const { return "paintTask_t"; }
};

class winSink_t
{
private:
    sched_t *context;

    u32 chId;

    imgTranslator_t *imgTranslator;

    u32 lastImgFmt;

    paintTask_t *paintTask;

    renderer_t *theRenderer;

    stats_t *myStats;

public:
    winSink_t(sched_t *ctx, u32 nchId, stats_t *myS, int maxTTL);

    virtual ~winSink_t(void);

    void mapWindow  (void);
    void unmapWindow(void);
    bool isMapped   (void);

    void setWindowGeometry (const char *geom);
    void setWindowOverrideRedirect (bool ORFlag);
    void setWindowTitle    (const char *acronym, const char *title);

    u32  getWinId(void);

    void setAvatarImage    (image_t *img);
    void setAvatarTimeout  (unsigned lapse);

    void setFollowSize   (bool nfollowSize);
    void paintFrame      (image_t *img);
};

#endif
