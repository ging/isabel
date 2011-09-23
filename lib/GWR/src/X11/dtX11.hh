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
// $Id: dtX11.hh 10651 2007-08-24 16:11:55Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr__dtX11__hh__
#define __gwr__dtX11__hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/task.hh>

#include <gwr/gwr.h>
#include <gwr/displayTask.h>

#define HAVE_MITSHM

#ifdef HAVE_MITSHM

#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#endif


#include "color.hh"

class winX11Info_t;
class dpyX11TaskInfo_t;
class dpyX11Visual_t;
                       
class dtX11_t: public virtual displayTask_t
{
private:
    virtual void __preInSchedRites (sched_t *) { tlRun(); };

public:
    class windowX11_t: public virtual displayTask_t::window_t
    {
    private:
        winX11Info_t *winInfo;

        Color_t fgcolor, bgcolor;
        XFontStruct *fontInfo;

    protected:
        windowX11_t(dtX11_t *d);

        windowX11_t(dtX11_t    *d,
                    const char *title,
                    const char *geom,
                    window_ref  parent,
                    unsigned    visualDepth,
                    int         visualClass,
                    unsigned    defaultWidth,
                    unsigned    defaultHeight
                   );

    public:

        virtual ~windowX11_t(void);

        virtual bool     release(void); // window does not longer exist

        virtual u32      getWinId(void) const;

        virtual void     setTitle(const char *s);
        virtual void     setGeometry(const char *);

        virtual unsigned getWidth(void) const ;
        virtual unsigned getHeight(void) const ;
        virtual int      getXPos(void) const;
        virtual int      getYPos(void) const;

        virtual bool     isMapped(void) const;

        virtual smartReference_t<dpyX11Visual_t> getWinVisual(); 

        virtual void map(void);
        virtual void unmap(void);
        virtual void clear(void);
        virtual void resize(unsigned w, unsigned h);
        virtual void reposition(int x, int y);

        virtual void putRGB24(const u8 *data,
                              unsigned  width,
                              unsigned  height,
                              float     zoomx,
                              float     zoomy
                             );

        virtual void putRGB24inBG(const u8 *data,
                                  unsigned  width,
                                  unsigned  height,
                                  float     zoomx,
                                  float     zoomy
                                 );

        virtual bool loadFont(const char *fontname, float size, int style);
        virtual bool setBgColor(const char *color);
        virtual bool setBgColor(u8 r, u8 g, u8 b);
        virtual bool setFgColor(const char *color);
        virtual bool setFgColor(u8 r, u8 g, u8 b);

        virtual void drawLineInBG(int x1, int y1, int x2, int y2, int lw);
        virtual void drawString(int x, int y, const char *msg, align_e a);

        virtual void setGamma(float rGamma, float gGamma, float bGamma);
        virtual void setOverrideRedirect(bool ORflag);

    protected:
        virtual void handleMapEvent(void);
        virtual void handleUnmapEvent(void);
        virtual void handleExposeEvent(XExposeEvent *xExpEv);
        virtual void handleNoExposeEvent(XExposeEvent *xExpEv);
        virtual void handleReconfigureEvent(XConfigureEvent *xConfEv);

        friend class dtX11_t;
        friend class smartReference_t<windowX11_t>;

    private:

        void transRGB24AndPaint(const u8 *data,
                                unsigned  width,
                                unsigned  height,
                                float     zoomx,
                                float     zoomy,
                                bool      background= false
                               );

        void paint(void *workBuffer,
                   unsigned croppedZoomedWidth,
                   unsigned croppedZoomedHeight
                  );
        void paintInBG(void *workBuffer,
                       unsigned croppedZoomedWidth,
                       unsigned croppedZoomedHeight
                      );

        //
        // buffer for painting images
        //
        int pBufferSize;
        u8 *pBuffer;

        //
        // gamma correction tables
        //
        u32 redLookup[256];
        u32 greenLookup[256];
        u32 blueLookup[256];

        clutColor_t pal[256];

        //
        // Shared Memory managment
        //
        XShmSegmentInfo  shminfo;
        XImage          *ximage;
        bool             useMITShm;

        XImage *CreateXImage(unsigned width, unsigned height);
        void    DestroyXImage(XImage *ximage);

    public:
        virtual const char *className(void) const { return "windowX11_t"; };
    };


protected:
    dpyX11TaskInfo_t *dpyInfo;

private:
    void __registerWindow(smartReference_t<dtX11_t::windowX11_t> wr);
    void __unregisterWindow(smartReference_t<dtX11_t::windowX11_t> wr);

public:
    dtX11_t(const char *options= NULL);

protected:
    virtual ~dtX11_t(void);


public:
    void getDefaultVisualCombination(unsigned *visualDepth, int *visualClass);

    bool supportedVisualCombination(unsigned visualDepth, int visualClass);


    Display  *getDisplay(void);
    window_t *rootWindow(void);

    window_ref createWindow(const char *title,
                            const char *geom,
                            window_ref  parent,
                            unsigned    visualDepth,
                            int         visualClass,
                            unsigned    defaultWidth,
                            unsigned    defaultHeight
                           );

    bool destroyWindow(smartReference_t<displayTask_t::window_t> win);

protected:
    virtual void IOReady(io_ref &);
    virtual void checkXevents(void);

public:
    virtual char const *className(void) const { return "dtX11_t"; };


    friend class dtX11_t::windowX11_t;
    friend class smartReference_t<dtX11_t>;

};

typedef smartReference_t<dtX11_t          > dtX11_ref;
typedef smartReference_t<dtX11_t::windowX11_t> windowX11_ref;

#endif
