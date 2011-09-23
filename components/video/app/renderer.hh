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
// $Id: renderer.hh 21743 2010-12-17 10:44:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __renderer__hh__
#define __renderer__hh__

#include <vUtils/image.h>

#include <gwr/displayTask.h>

#include "stats.hh"

class renderer_t
{
private:
    window_ref win;
    u32        chId;
    bool       fontLoaded;

    stats_t *myStats;

    renderer_t(u32 nchId, stats_t *myS);
    virtual ~renderer_t(void);

    u32  getWinId(void);
    bool isMapped(void);

    void mapWindow  (void);
    void unmapWindow(void);
    void clear      (void);
    void paint      (image_t *img, bool followSize);
    void setGeometry(const char *gem);
    void setOverrideRedirect(bool ORFlag);
    void setTitle   (const char *title);
    void drawMsg    (const char *acro);

public:
    friend class winSink_t;
    friend class paintTask_t;
};

#endif
