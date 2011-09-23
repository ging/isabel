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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr__dtGLX__hh__
#define __gwr__dtGLX__hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/task.hh>

#include <gwr/gwr.h>
#include <gwr/displayTask.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/glx.h>
#include <GL/gl.h>


class dpyGLXTaskInfo_t;

class dtGLX_t: public virtual displayTask_t
{
public:
    class winGLX_t: public virtual displayTask_t::window_t
    {
    private:
        dtGLX_t  *winDisplayTask;

        Window  winID;

        GLXContext cx;

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        XFontStruct *fontInfo;
        GLuint fontBase;
#endif

        unsigned  winWidth;
        unsigned  winHeight;
        unsigned  winXpos;
        unsigned  winYpos;

        bool mapped;

        void reshape(unsigned w, unsigned h);

    protected:
        winGLX_t(dtGLX_t    *d,
                 const char *title,
                 const char *geom,
                 window_ref  parent,
                 unsigned    visualDepth,
                 int         visualClass,
                 unsigned    iWidth,
                 unsigned    iHeight
                );

    public:

        virtual ~winGLX_t(void);

        virtual bool     release(void); // window does not longer exist

        virtual u32      getWinId(void) const;

        virtual void     setTitle(const char *s);
        virtual void     setGeometry(const char *);

        virtual unsigned getWidth(void) const ;
        virtual unsigned getHeight(void) const ;
        virtual int      getXPos(void) const;
        virtual int      getYPos(void) const;

        virtual bool     isMapped(void) const;

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

        friend class dtGLX_t;
        friend class smartReference_t<winGLX_t>;

    public:
        virtual const char *className(void) const { return "winGLX_t"; };
    };


protected:
    dpyGLXTaskInfo_t *dpyInfo;

private:
    void registerWindow(smartReference_t<dtGLX_t::winGLX_t> wr);
    void unregisterWindow(smartReference_t<dtGLX_t::winGLX_t> wr);

public:
    dtGLX_t(const char *options= NULL);

protected:
    virtual ~dtGLX_t(void);


public:
    void getDefaultVisualCombination(unsigned *visualDepth, int *visualClass);

    bool supportedVisualCombination(unsigned visualDepth, int visualClass);

    window_t *rootWindow(void);

    window_ref createWindow(const char *title,
                            const char *geom,
                            window_ref  parent,
                            unsigned    visualDepth,
                            int         visualClass,
                            unsigned    iWidth,
                            unsigned    iHeight
                           );

    bool destroyWindow(smartReference_t<displayTask_t::window_t> win);

protected:
    virtual void IOReady(io_ref &);
    virtual void checkXevents(void);

public:
    virtual char const *className(void) const { return "dtGLX_t"; };


    friend class dtGLX_t::winGLX_t;
    friend class smartReference_t<dtGLX_t>;

};

typedef smartReference_t<dtGLX_t          > dtGLX_ref;
typedef smartReference_t<dtGLX_t::winGLX_t> winGLX_ref;

#endif
