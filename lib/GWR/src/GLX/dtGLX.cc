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

#include <assert.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>

#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xcms.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/dictionary.hh>

#include <gwr/displayTask.h>

#include "dtGLX.hh"

static int snglBuf[] = {
    GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[] = {
    GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

//
//  parts -- to be documented
//
class dpyGLXTaskInfo_t
{
    Display     *dpy;
    XVisualInfo *vi;

    GLboolean doubleBuffer;

    dictionary_t<Window, winGLX_ref> windowsByIdentifier;

    friend class dtGLX_t;
    friend class dtGLX_t::winGLX_t;
};

//
// utility macros -- to be documented
//
#define DPY_OF_WIN(w)       \
(                           \
    w->                     \
        winDisplayTask->    \
        dpyInfo->           \
        dpy                 \
)
#define DPY_OF_THIS_WIN (DPY_OF_WIN(this))



#define SCREEN_OF_WIN(w)    \
(                           \
    w->                     \
        winDisplayTask->    \
        dpyInfo->           \
        dpyScreen           \
)
#define SCREEN_OF_THIS_WIN  (SCREEN_OF_WIN(this))




//
// Display -- to be documented
//
dtGLX_t::dtGLX_t(const char *options)
{
    assert ((options == NULL) && "dtGLX_t: no options allowed\n");
#if 0
    if ( ! XInitThreads())
    {
        NOTIFY("dtGLX_t::dtGLX_t: XInitThreads failed\n");
    }
    else
    {
        NOTIFY("dtGLX_t::dtGLX_t: XInitThread succeded!\n");
    }
#endif

    dpyInfo= new dpyGLXTaskInfo_t;
    dpyInfo->dpy= XOpenDisplay(NULL);


    if ( ! dpyInfo->dpy)
    {
        NOTIFY("dtGLX_t::dtGLX_t: cannot open display %s, "
               "bailing out\n",
               getenv("DISPLAY")
              );

        exit(1);
    }

    int dummy;
    if ( ! glXQueryExtension(dpyInfo->dpy, &dummy, &dummy))
    {
        XCloseDisplay(dpyInfo->dpy);
        NOTIFY("dtGLX_t::dtGLX_t: no OpenGL GLX extension found, bailing out\n");
        throw ("NO GLX extension");
    }

    //
    // find out visual stuff suitable for OpenGL RGB
    //
    dpyInfo->doubleBuffer= GL_TRUE;
    dpyInfo->vi= glXChooseVisual(dpyInfo->dpy, DefaultScreen(dpyInfo->dpy), dblBuf);
    if (dpyInfo->vi == NULL)
    {
        dpyInfo->doubleBuffer= GL_FALSE;
        dpyInfo->vi= glXChooseVisual(dpyInfo->dpy, DefaultScreen(dpyInfo->dpy), snglBuf);

        if (dpyInfo->vi == NULL)
        {
            NOTIFY("dtGLX_t::dtGLX_t: no RGB visual with depth buffer\n");
            exit(1);
        }
    }

    if (dpyInfo->vi->c_class != TrueColor)
    {
        NOTIFY("dtGLX_t::dtGLX_t: only TrueColor, sorry\n");
        exit(1);
    }

    NOTIFY("Visual created, depth= %d, class= %d\n",  dpyInfo->vi->depth, dpyInfo->vi->c_class);

    add_IO(new io_t( XConnectionNumber(dpyInfo->dpy)));
}

dtGLX_t::~dtGLX_t(void)
{
    Display *d= dpyInfo->dpy;

    delete dpyInfo;

    XCloseDisplay(d);
}


void
dtGLX_t::getDefaultVisualCombination(unsigned *visualDepth, int *visualClass)
{
    if (visualDepth) *visualDepth= dpyInfo->vi->depth;
    if (visualClass) *visualClass= dpyInfo->vi->c_class;
}


bool
dtGLX_t::supportedVisualCombination(unsigned visualDepth, int visualClass)
{
    switch (visualClass)
    {
    case DirectColor:
        NOTIFY("dtGLX_t::supportedVisualCombination: "
               "DirectColor not supported by SDK, "
               "returning 'false' to application\n"
              );
        return false;
    case GrayScale:
        NOTIFY("dtGLX_t::supportedVisualCombination: "
               "GrayScale not supported by SDK, "
               "returning 'false' to application\n"
              );
        return false;
    };

    return ( (dpyInfo->vi->depth   == visualDepth) &&
             (dpyInfo->vi->c_class == visualClass)
           );
}

displayTask_t::window_t *
dtGLX_t::rootWindow(void)
{
    return NULL;
}

window_ref
dtGLX_t::createWindow(const char *title,
                      const char *geom,
                      window_ref  parent,
                      unsigned    visualDepth,
                      int         visualClass,
                      unsigned    iWidth,
                      unsigned    iHeight
                     )
{
NOTIFY("dtGLX_t::createWindow: parent %s valid\n", 
parent.isValid() ? "IS" : "IS NOT");
    return new dtGLX_t::winGLX_t(this,
                                 title,
                                 geom,
                                 parent,
                                 visualDepth,
                                 visualClass,
                                 iWidth,
                                 iHeight
                                );
}

bool
dtGLX_t::destroyWindow(smartReference_t<displayTask_t::window_t> win)
{
    window_t *wp  = win;
    winGLX_t *wxp = dynamic_cast<winGLX_t *>(wp);
    
    if ( ! dpyInfo->windowsByIdentifier.lookUp(wxp->getWinId()) )
    {
        return false;
    }

    unregisterWindow(wxp);

    return win->release(); // deactivate window
}


void
dtGLX_t::IOReady(io_ref &io)
{
    debugMsg(dbg_App_Verbose, "IOReady", "checking for events");

    checkXevents();
}


void
dtGLX_t::checkXevents(void)
{
    XEvent xev;

    while (XPending(dpyInfo->dpy))
    {

        debugMsg(dbg_App_Verbose, "checkXevents", "got an event");

        XNextEvent(dpyInfo->dpy, &xev);

        switch (xev.type) {
            case CirculateNotify:
                //
                // Ignore circulate notifies, just put a debug msg
                //
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "circulate== %d",
                         xev.type
                        );
                break;
            case ConfigureNotify:
                //
                // window has been moved or resized
                //
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "configure== %d",
                         xev.type
                        );
                {
                    XConfigureEvent *xConfEv   = (XConfigureEvent *)&xev;

                    winGLX_ref eventWindow=
                        dpyInfo->
                        windowsByIdentifier.lookUp(xConfEv->window);

                    if (eventWindow.isValid())
                    {
                        eventWindow->handleReconfigureEvent(xConfEv);
                    }

                } break;
            case GravityNotify:
                //
                // Ignore gravity notifies, just put a debug msg
                //
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "gravity== %d",
                         xev.type
                        );
                break;
            case MapNotify:
                //
                // window has been mapped
                //
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "map== %d",
                         xev.type
                        );
                {
                    XMapEvent *xMapEv= (XMapEvent *)&xev;

                    winGLX_ref eventWindow=
                        dpyInfo->
                        windowsByIdentifier.lookUp(xMapEv->window);

                    if (eventWindow.isValid())
                    {
                        eventWindow->handleMapEvent();
                    }
                } break;
            case UnmapNotify:
                //
                // window has been unmapped
                //
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "unmap== %d",
                         xev.type
                        );
                {
                    XMapEvent *xMapEv= (XMapEvent *)&xev;

                    winGLX_ref eventWindow=
                        dpyInfo->
                        windowsByIdentifier.lookUp(xMapEv->window);

                    if (eventWindow.isValid())
                    {
                        eventWindow->handleUnmapEvent();
                    }
                } break;
            case ReparentNotify:
                //
                // Ignore reparent notifies, just put a debug msg
                //
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "reparent== %d",
                         xev.type
                        );
                break;

            case Expose:
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "expose== %d",
                         xev.type
                        );
                {
                    XExposeEvent *xExpEv= (XExposeEvent*)&xev;
                    winGLX_ref eventWindow=
                        dpyInfo->
                            windowsByIdentifier.lookUp(xExpEv->window);
                    if (eventWindow.isValid())
                    {
                        eventWindow->handleExposeEvent(xExpEv);
                    }
                } break;

            case NoExpose:
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "no expose== %d",
                         xev.type
                        );
                {
                    XExposeEvent *xExpEv= (XExposeEvent*)&xev;
                    winGLX_ref eventWindow=
                        dpyInfo->
                            windowsByIdentifier.lookUp(xExpEv->window);
                    if (eventWindow.isValid())
                    {
                        eventWindow->handleNoExposeEvent(xExpEv);
                    }
                } break;

            case DestroyNotify:
                debugMsg(dbg_App_Verbose,
                         "checkXevents",
                         "destroy notify== %d",
                         xev.type
                        );
                {
                    XDestroyWindowEvent *xDesWinEv= (XDestroyWindowEvent*)&xev;

                    NOTIFY("checkXevents :: destroy notify received (%d) "
                           "for (0x%x)\n",
                           xDesWinEv->type, xDesWinEv->window
                          );

                    winGLX_ref eventWindow=
                        dpyInfo->
                            windowsByIdentifier.lookUp(xDesWinEv->window);
                    if (eventWindow.isValid())
                    {
                        NOTIFY("checkXevents :: window was registered\n");
                        unregisterWindow(eventWindow);
                    }
                    else
                    {
                        NOTIFY("checkXevents :: window was NOT registered\n");
                    }
                } break;

            default:
                NOTIFY("checkXevents :: unforeseen received event (%d)\n",
                       xev.type
                      );
            break;
        }
    }
}

void
dtGLX_t::registerWindow(winGLX_ref w)
{
    dpyInfo->windowsByIdentifier.insert(w->getWinId(), w);
}

void
dtGLX_t::unregisterWindow(winGLX_ref w)
{
    dpyInfo->windowsByIdentifier.remove(w->getWinId());
}


//
// window -- to be documented
// general winGLX_t contructor, for any kind of window
// (except root window, created by the displayTask)
//
dtGLX_t::winGLX_t::winGLX_t(dtGLX_t     *d,
                            const char  *title,
                            const char  *geomStr,
                            window_ref   theparent,
                            unsigned     visualDepth,
                            int          visualClass,
                            unsigned     iWidth,
                            unsigned     iHeight
                           )
: winDisplayTask(d),
  fontInfo(NULL),
  winWidth(iWidth),
  winHeight(iHeight),
  winXpos(0),
  winYpos(0),
  mapped(false)
{
    assert ((iWidth > 0) && (iHeight > 0) && "bad window geom\n");

    assert(d->supportedVisualCombination(visualDepth, visualClass));

    // parent window, root by default
    Window pWin= theparent.isValid()
                     ? theparent->getWinId()
                     : DefaultRootWindow(DPY_OF_THIS_WIN);

    //
    // now, lets calculate the window attributes
    //

    XSetWindowAttributes swa;
    unsigned             swaFlags= 0;

    // back color
    //swa.background_pixel= BlackPixel(DPY_OF_THIS_WIN, SCREEN_OF_THIS_WIN);
    //swaFlags|= CWBackPixel;

    swa.border_pixel= 0;
    swaFlags|= CWBorderPixel;

    // event selection
    swa.event_mask= StructureNotifyMask /* | ExposureMask */;
    swaFlags|= CWEventMask;

#if 1
    Colormap cmap=
        XCreateColormap(DPY_OF_THIS_WIN,
                        DefaultRootWindow(DPY_OF_THIS_WIN),
                        winDisplayTask->dpyInfo->vi->visual,
                        AllocNone
                       );
    swa.colormap= cmap;
#else
    swa.colormap= DefaultColormap(DPY_OF_THIS_WIN,
                                  DefaultScreen(DPY_OF_THIS_WIN)
                                 );
#endif
    swaFlags|= CWColormap;

    //
    // finally lets create the window
    //
    winID= XCreateWindow(DPY_OF_THIS_WIN,
                         pWin,
                         winXpos, winYpos,
                         winWidth, winHeight,
                         0,
                         winDisplayTask->dpyInfo->vi->depth,
                         InputOutput,
                         winDisplayTask->dpyInfo->vi->visual,
                         swaFlags,
                         &swa
                        );

    //XFlush(DPY_OF_THIS_WIN);//sleep(2);
    XSync(DPY_OF_THIS_WIN, true);//sleep(2);

    d->registerWindow(this);

    // create an OpenGL rendering context
    cx= glXCreateContext(DPY_OF_THIS_WIN, winDisplayTask->dpyInfo->vi, None, GL_TRUE);

    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    reshape(winWidth, winHeight);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (winDisplayTask->dpyInfo->doubleBuffer)
    {
        glXSwapBuffers(DPY_OF_THIS_WIN, winID);
    }
    else
    {
        glFlush();
    }

    setTitle(title);

    //
    // set position
    //
    setGeometry(geomStr);

    //
    // gamma correction tables initializaction (may not be needed)
    //
    setGamma(1.0, 1.0, 1.0);
}

dtGLX_t::winGLX_t::~winGLX_t(void)
{
    XDestroyWindow(DPY_OF_THIS_WIN, winID);

    if (fontInfo)
    {
        XFreeFont(DPY_OF_THIS_WIN, fontInfo);
    }

    XSync(DPY_OF_THIS_WIN, true);
}

//
// the GUI window has been destroy, release data
//
bool
dtGLX_t::winGLX_t::release(void)
{
    return true;
}

void
dtGLX_t::winGLX_t::reshape(unsigned w, unsigned h)
{
    GLdouble size;
    GLdouble aspect;

    winWidth= w;
    winHeight= h;

    glViewport(0, 0, winWidth, winHeight);

   /* We are going to do some 2-D orthographic drawing. */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   size = (GLdouble)((w >= h) ? w : h) / 2.0;
   if (w <= h) {
      aspect = (GLdouble)h/(GLdouble)w;
      glOrtho(-size, size, -size*aspect, size*aspect, -100000.0, 100000.0);
   }
   else {
      aspect = (GLdouble)w/(GLdouble)h;
      glOrtho(-size*aspect, size*aspect, -size, size, -100000.0, 100000.0);
   }

   /* Make the world and window coordinates coincide so that 1.0 in */
   /* model space equals one pixel in window space.                 */
   glScaled(aspect, aspect, 1.0);

   /* Now determine where to draw things. */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glFlush();
}

void
dtGLX_t::winGLX_t::resize(unsigned w, unsigned h)
{
    XResizeWindow(DPY_OF_THIS_WIN,
                  winID,
                  w,
                  h
                 );

    XFlush(DPY_OF_THIS_WIN);

#if 0
    XSizeHints hints;

    hints.flags= PSize|PMinSize|PMaxSize;
    hints.width                          =
        hints.min_width= hints.max_width =
        winWidth     ;
    hints.height= hints.min_height       =
        hints.max_height                 =
        winHeight    ;
    XSetWMNormalHints(DPY_OF_THIS_WIN, winID, &hints);
#endif

    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    reshape(w, h);
}

void
dtGLX_t::winGLX_t::reposition(int x, int y)
{
    winXpos= x;
    winYpos= y;

    XMoveWindow(DPY_OF_THIS_WIN,
                winID,
                winXpos,
                winYpos
               );

#if 0
    XSizeHints sizehints;

    sizehints.flags=
        USPosition | USSize  |
        PPosition  | PSize   ;
        sizehints.x           = winXpos;
        sizehints.y           = winYpos;

    XSetWMNormalHints(DPY_OF_THIS_WIN, winID, &sizehints);
#endif

    XFlush(DPY_OF_THIS_WIN);
}

void
dtGLX_t::winGLX_t::setTitle(const char *title)
{
    if (title != NULL && (strlen(title) > 0))
    {
#if 0
        XTextProperty winTitle;

        winTitle.value   = (unsigned char *)title;
        winTitle.encoding= XA_STRING;
        winTitle.format  = 8;
        winTitle.nitems  = strlen(title)+1;

        XSetTextProperty(DPY_OF_THIS_WIN,
                         winID,
                         &winTitle,
                         XA_WM_NAME
                        );
#else
        XSetStandardProperties(DPY_OF_THIS_WIN,
                               winID,
                               title,
                               title,
                               None,
                               0,
                               NULL,
                               NULL
                              );
#endif
    }
}

void
dtGLX_t::winGLX_t::setGeometry(const char *geomStr)
{
    bool doReposition= false;
    bool doResize    = false;

    debugMsg(dbg_App_Verbose, "setGeometry", "requested <%s>", geomStr);

    if ( ! geomStr || (strlen(geomStr) == 0) )
    {
        return;
    }

    unsigned width = winWidth; // initial value, in case X|Y Negative
    unsigned height= winHeight;
    int      xPos;
    int      yPos;
    int res= XParseGeometry(geomStr, &xPos, &yPos, &width, &height);

    if (res & XNegative)
        xPos= DisplayWidth(DPY_OF_THIS_WIN, DefaultScreen(DPY_OF_THIS_WIN))+xPos-width;
    if (res & YNegative)
        xPos= DisplayHeight(DPY_OF_THIS_WIN, DefaultScreen(DPY_OF_THIS_WIN))+yPos-height-32;

    if (res & (XValue    |     YValue)) doReposition= true;
    if (res & (WidthValue|HeightValue)) doResize    = true;

    if (doResize)
        resize(width, height);
    if (doReposition)
        reposition(xPos, yPos);

#if 0
    if (doResize || doReposition)
    {
        XSizeHints sizehints;

        sizehints.flags=
            USPosition | USSize  |
            PPosition  | PSize   |
            PMinSize   | PMaxSize;
        sizehints.x         = __winPrivateInfo->__winXpos;
        sizehints.y         = __winPrivateInfo->__winYpos;
        sizehints.width     = __winPrivateInfo->__winWidth;
        sizehints.height    = __winPrivateInfo->__winHeight;
        sizehints.min_width = __winPrivateInfo->__winWidth;
        sizehints.min_height= __winPrivateInfo->__winHeight;
        sizehints.max_width = __winPrivateInfo->__winWidth;
        sizehints.max_height= __winPrivateInfo->__winHeight;

        XSetWMNormalHints(DPY_OF_THIS_WIN,
                          __winPrivateInfo->__winIdentifier,
                          &sizehints
                         );
    }
#endif
}

unsigned
dtGLX_t::winGLX_t::getWidth(void) const
{
    return winWidth;
}

unsigned
dtGLX_t::winGLX_t::getHeight(void) const
{
    return winHeight;
}

void
dtGLX_t::winGLX_t::map(void)
{
    XMapWindow(DPY_OF_THIS_WIN, winID);

    XMoveResizeWindow(DPY_OF_THIS_WIN,
                      winID,
                      winXpos,
                      winYpos,
                      winWidth,
                      winHeight
                     );

    XSync(DPY_OF_THIS_WIN, False);

    mapped= true;
}

void
dtGLX_t::winGLX_t::unmap(void)
{
    XUnmapWindow(DPY_OF_THIS_WIN, winID);

    XSync(DPY_OF_THIS_WIN, False);

    mapped= false;
}

void
dtGLX_t::winGLX_t::clear(void)
{
    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    glClear(GL_COLOR_BUFFER_BIT);

    if (winDisplayTask->dpyInfo->doubleBuffer)
    {
        glXSwapBuffers(DPY_OF_THIS_WIN, winID);
    }
    else
    {
        glFlush();
    }
}

#define MIN(a,b) ((a)<(b)?(a):(b))

//
// paint methods
//
void
dtGLX_t::winGLX_t::putRGB24(const u8 *data,
                            unsigned width,
                            unsigned height,
                            float zoomx,
                            float zoomy
                           )
{
    //
    // letstart by zooming image
    //
    float actualZoomX= (zoomx < 0
                        ? (float)winWidth / (float)width
                        : zoomx
                       );
    float actualZoomY= (zoomy < 0
                        ? (float)winHeight / (float)height
                        : zoomy
                       );

    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }


    glRasterPos2i(- (winWidth / 2) + 1, winHeight / 2 - 1);
    glPixelZoom(actualZoomX, -1.0*actualZoomY);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
 
    if (winDisplayTask->dpyInfo->doubleBuffer)
    {
        glXSwapBuffers(DPY_OF_THIS_WIN, winID);
    }
    else
    {
        glFlush();
    }
}

void
dtGLX_t::winGLX_t::putRGB24inBG(const u8 *data,
                                unsigned width,
                                unsigned height,
                                float zoomx,
                                float zoomy
                               )
{
}


void
dtGLX_t::winGLX_t::setGamma(float rGamma, float gGamma, float bGamma)
{
}


void
dtGLX_t::winGLX_t::setOverrideRedirect(bool ORflag)
{
    XSetWindowAttributes swa;
    swa.override_redirect= ORflag;
    
    XChangeWindowAttributes(DPY_OF_THIS_WIN,
                            winID,
                            CWOverrideRedirect, 
                            &swa
                           );
}


u32
dtGLX_t::winGLX_t::getWinId(void) const
{
    return (u32)winID;
}

int
dtGLX_t::winGLX_t::getXPos(void) const
{
    return winXpos;
}

int
dtGLX_t::winGLX_t::getYPos(void) const
{
    return winYpos;
}

void
dtGLX_t::winGLX_t::handleMapEvent(void)
{
    //NOTIFY("winGLX_t::handleMapEvent: Ohhh! I'm visible!!\n");

    clear(); // to avoid transparent window

    mapped = true;
}

void
dtGLX_t::winGLX_t::handleUnmapEvent(void)
{
    //NOTIFY("winGLX_t::handleUnmapEvent: Ohhh! I'm invisible!!\n");

    mapped = false;
}

void
dtGLX_t::winGLX_t::handleExposeEvent(XExposeEvent *xExpEv)
{
    NOTIFY("winGLX_t::handleExposeEvent: Ohhh! I'm being painted!!\n");
    NOTIFY("\tExposing area %dx%d+%d+%d (count=%d)\n",
           xExpEv->width,
           xExpEv->height,
           xExpEv->x,
           xExpEv->y,
           xExpEv->count
          );
}

void
dtGLX_t::winGLX_t::handleNoExposeEvent(XExposeEvent *xExpEv)
{
    NOTIFY("winGLX_t::handleNoExposeEvent: Ohhh! I'm being hidden!!\n");
    NOTIFY("\tHiding area %dx%d+%d+%d (count=%d)\n",
           xExpEv->width,
           xExpEv->height,
           xExpEv->x,
           xExpEv->y,
           xExpEv->count
          );
}

void
dtGLX_t::winGLX_t::handleReconfigureEvent(XConfigureEvent *xConfEv)
{
    //NOTIFY("winGLX_t::handleReconfigureEvent: Ohhh! I'm being manipulated!!\n");

    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    reshape(xConfEv->width, xConfEv->height);

// the following causes a misfunction, should investigate what's happening
//    winXpos= xConfEv->x;
//    winYpos= xConfEv->y;
}


bool
dtGLX_t::winGLX_t::isMapped(void) const
{
    return mapped;
}


bool
dtGLX_t::winGLX_t::setFgColor(const char *color)
{
    XColor theColor;

    theColor.flags= DoRed | DoGreen | DoBlue;

    XcmsColor exactColor, screenColor;

    if ( XcmsLookupColor(DPY_OF_THIS_WIN,
                         DefaultColormap(DPY_OF_THIS_WIN,
                                         DefaultScreen(DPY_OF_THIS_WIN)
                                        ),
                         color,
                         &exactColor,
                         &screenColor,
                         XcmsRGBiFormat
                        ) == XcmsFailure
       )
    {
        NOTIFY("winGLX_t::setFgColor: "
               "unknown color or bad color format: %s\n",
               color
              );
        return false;
    }

    if (screenColor.format != XcmsRGBiFormat)
    {
        // do I really need to check that
        NOTIFY("setFgColor: unexpected color specification\n");
        return false;
    }

    XcmsFloat r, g, b;
    r= screenColor.spec.RGBi.red;
    g= screenColor.spec.RGBi.green;
    b= screenColor.spec.RGBi.blue;

    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    glColor4f(r, g, b, 0.0);

    return true;
}


bool
dtGLX_t::winGLX_t::setFgColor(u8 r, u8 g, u8 b)
{
    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    glColor4f(r, g, b, 0.0);

    return false;
}


bool
dtGLX_t::winGLX_t::loadFont(const char *fontname, float size, int style)
{
    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    fontBase= glGenLists(256);
    if ( ! glIsList(fontBase) )
    {
        NOTIFY("loadFont: glGenList failed, could not load font\n");
        return false;
    }

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)

    if (fontInfo) // free old font
    {
        XFreeFont(DPY_OF_THIS_WIN, fontInfo);
        fontInfo = NULL;
    }

    // loading font
    if ((fontInfo= XLoadQueryFont(DPY_OF_THIS_WIN, fontname)) == NULL)
    {
        NOTIFY("dtX11_t::windowX11_t::loadFont: cannot open %s font", fontname);
        return false;
    }

    unsigned long fid= fontInfo->fid;
    unsigned first = fontInfo->min_char_or_byte2;
    unsigned last  = fontInfo->max_char_or_byte2;
    glXUseXFont(fid, first, last-first+1, fontBase);
#endif

    return true;
}

bool
dtGLX_t::winGLX_t::setBgColor(const char *color)
{
    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);

    return false;
}


bool
dtGLX_t::winGLX_t::setBgColor(u8 r, u8 g, u8 b)
{
    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
    }

    glClearColor(r, g, b, 0.0);

    return false;
}


void
dtGLX_t::winGLX_t::drawLineInBG(int x1, int y1, int x2, int y2, int lwidth)
{
}

void
dtGLX_t::winGLX_t::drawString(int x, int y, const char *msg, align_e a)
{
    debugMsg(dbg_App_Verbose,
             "drawString",
             " dpy=%p, win=0x%x, (%d,%d,%s) s=%s\n",
             DPY_OF_THIS_WIN,
             winID,
             x, y,
             a == RIGHT ? "RIGHT" : (a == LEFT ? "LEFT" : "CENTER"),
             msg
            );

#if 0
NOTIFY("drawString dpy=%p, win=0x%x[%dx%d], (%d,%d,%s) s=%s\n",
             DPY_OF_THIS_WIN,
             winID,
             winWidth, winHeight,
             x, y,
             a == RIGHT ? "RIGHT" : (a == LEFT ? "LEFT" : "CENTER"),
             msg
            );
#endif

    int lenMsg, widthMsg;

    if (msg == NULL || msg[0] == '\0')
    {
        NOTIFY("drawString: no message to draw\n");
        return;
    }

    lenMsg= strlen(msg);
    widthMsg= XTextWidth(fontInfo, msg, lenMsg);

    switch (a)
    {
    case CENTER:
        x -= widthMsg/2;
        break;
    case LEFT:
        // do nothing
        break;
    case RIGHT:
        x -= widthMsg;
        break;
    default:  
        NOTIFY("dtX11_t::windowX11_t::drawText", "unknown alignment (%d)", a);
        return;
    }

    if ( ! glXMakeContextCurrent(DPY_OF_THIS_WIN, winID, winID, cx) )
    {
        NOTIFY("glXMakeContextCurrent failed!\n");
        return;
    }

#if 0
   // position test, do not delete until becoming an OpenGL master
   glColor4f(1.0, 0.0, 0.0, 0.0);
   glBegin(GL_LINES); {
      glVertex2f(0.0, -10.0);
      glVertex2f(0.0, 10.0);
   } glEnd();
   glColor4f(0.0, 1.0, 0.0, 0.0);
   glBegin(GL_LINES); {
      glVertex2f(-10.0, 0.0);
      glVertex2f(10.0, 0.0);
   } glEnd();
   glColor4f(0.0, 1.0, 0.0, 0.0);
   glBegin(GL_LINES); {
      glVertex2f(-10.0,  10.0);
      glVertex2f( 10.0, -10.0);
      //glVertex2f(-winWidth/2.0, winHeight / 2);
      //glVertex2f( winWidth/2.0,  -winHeight / 2);
   } glEnd();
#endif

    if ( ! glIsList(fontBase))
    {
        NOTIFY("drawString: font glyphs were not created\n");
        return;
    }

    glRasterPos2i(x - winWidth / 2, winHeight / 2 - y);
 
    glPushAttrib(GL_LIST_BIT);

    glListBase(fontBase);
    glCallLists(lenMsg, GL_UNSIGNED_BYTE, msg);

    glPopAttrib();

    if (winDisplayTask->dpyInfo->doubleBuffer)
    {
        glXSwapBuffers(DPY_OF_THIS_WIN, winID);
    }
    else
    {
        glFlush();
    }
}

