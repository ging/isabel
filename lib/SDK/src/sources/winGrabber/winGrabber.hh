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
// $Id: winGrabber.hh 1900 2002-06-04 18:27:18Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __win_grabber_hh__
#define __win_grabber_hh__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include <icf2/general.h>
#include <icf2/item.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

#include "grabImage.hh"

class windowGrabber_t: public source_t, public item_t
{
public:
    // this struct contains values useful for convertImage routine
    struct cached_t
    {
        bool isLsbMachine, flipBytes;
        u32  rmask, gmask, bmask;
        int  rshift, gshift, bshift, r8shift, g8shift, b8shift;
    };

private:

    sourceDescriptor_ref  theSourceDescriptor;

    int                screen;
    Window             winId;
    XWindowAttributes  winInfo; // cached
    Display           *dpy;
    char              *dpyName;
    struct cached_t    cached;

    int getXColors(XColor **colors);

    //
    // Shared Memory Managment
    //
    XShmSegmentInfo  shminfo;
    XImage          *ShmImage;
    int              ShmSize;

    void InitShmResources(void);
    void FreeShmResources(void);

    XImage *capturaVentana(Drawable,
                           int, int, int, int,
                           unsigned long, int
                          );

    void DestroyXImage(XImage *);

    //int           rootDump(void);
    void          beep(void);

public:

    windowGrabber_t(sourceDescriptor_ref sdr);
    windowGrabber_t(sourceDescriptor_ref sdr, const char *dpyName);

    ~windowGrabber_t(void);

    //
    // Added to the source_t
    //
    bool selectWindowByClicking(void);
    bool selectWindowByWinId(Window newWinId);

    //
    // Imposed by source_t
    //
    image_t *getImage(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);
    bool   setGrabSize(unsigned width, unsigned height);
    void   getGrabSize(unsigned *width, unsigned *height);
    bool   setGrabFormat(u32 fcc);
    u32    getGrabFormat(void);

    sourceDescriptor_ref getDescriptor(void) const;

    char const *className(void) const { return "windowGrabber_t"; }
};

#endif

