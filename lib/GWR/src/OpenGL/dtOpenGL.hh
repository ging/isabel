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
// $Id: dtOpenGL.hh 10868 2007-09-19 17:20:49Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr__dtOpenGL__hh__
#define __gwr__dtOpenGL__hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/task.hh>

#include <gwr/gwr.h>
#include <gwr/displayTask.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "../X11/dtX11.hh"

#include <time.h>

#define MAX_TEXTURE_ID 1024

typedef struct
{
    Display *dpy;
    int screen;
    Window win;
    GLXContext ctx;
    unsigned int width, height;
} GLWindow_t;


class dtOpenGL_t: public virtual dtX11_t
{
public:

    class windowOpenGL_t: public virtual dtX11_t::windowX11_t {
    private:

//        dtX11_t *myDisplayTask;

        char          *myTitle;
        unsigned       myWidth;
        unsigned       myHeight;
        unsigned       myXpos;
        unsigned       myYpos;
        bool           mappedStatus;




        smartReference_t<dtOpenGL_t> dtParent;
        
        GLuint texId;

        windowOpenGL_t(dtOpenGL_t *d);

        windowOpenGL_t(dtOpenGL_t *d,
                       const char *title,
                       const char *geom,
                       window_ref  parent,
                       unsigned    visualDepth,
                       int         visualClass,
                       unsigned    defaultWidth,
                       unsigned    defaultHeight
                      );

    public:
        virtual ~windowOpenGL_t(void);

        virtual bool release(void);

        virtual u32      getWinId(void) const;

        virtual void     setTitle(const char *s);
        virtual void     setGeometry(const char *g);

        virtual unsigned getWidth(void) const;
        virtual unsigned getHeight(void) const;
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

        virtual void drawString(int x, int y, const char *msg, align_e a);

        virtual void setGamma(float rGamma, float gGamma, float bGamma);
        virtual void setOverrideRedirect(bool ORflag);

        friend class dtOpenGL_t;
        friend class smartReference_t<windowOpenGL_t>;

        virtual const char *className(void) const { return "windowOpenGL_t"; };
    };

private:
    unsigned long lastDraw;
    unsigned long usLimit;
    float rotation;
    GLWindow_t mainWindow;
    
    GLuint texture[MAX_TEXTURE_ID];
    GLuint surface;
    
    window_ref realWindow;
    unsigned windowCount;
    
private:
    void initializeGL(void);
    void drawGLScene(void);
    void resizeGLScene(unsigned w, unsigned h);

public:
    dtOpenGL_t(const char *dpyName= NULL);

    window_ref
        createWindow(const char *title,
                     const char *geom,
                     window_ref  parent,
                     unsigned    visualDepth,
                     int         visualClass,
                     unsigned    defaultWidth,
                     unsigned    defaultHeight
                    );

    bool destroyWindow(window_ref win);

    void             setLastDraw(unsigned long time);
    unsigned long    getLastDraw(void);
    void             setUsLimit(unsigned fps);
    void             setNoLimitation(void);
    unsigned long    getUsLimit(void);
    window_ref       getRealWindow();
    
    GLuint           buildTexture();
    
    void             refreshTexture(GLuint id, const u8* data, unsigned width, unsigned height);

protected:
    virtual ~dtOpenGL_t(void);

public:
    virtual char const *className(void) const { return "dtOpenGL_t"; };


    friend class dtOpenGL_t::window_t;
    friend class smartReference_t<dtOpenGL_t>;

};

typedef smartReference_t<dtOpenGL_t          > dtOpenGL_ref;
typedef smartReference_t<dtOpenGL_t::windowOpenGL_t> windowOpenGL_ref;

#endif
