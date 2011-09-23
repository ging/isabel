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
// $Id: dtX11.cc 10867 2007-09-19 17:20:23Z gabriel $
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

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/dictionary.hh>

#include <gwr/displayTask.h>

#include "dtX11.hh"
#include "bufferTranslations.hh"

#define __SDK_ERR_BUF_LEN 1024

// X11 stuff
bool XError;
int
HandleXError(Display *dpy, XErrorEvent *xerror)
{
    char errorString[1024];
    XError= True;

    XGetErrorText(dpy, xerror->error_code, errorString, 1024);
    NOTIFY("SDK::HandleXError: X Error: %s\n", errorString);
    NOTIFY("SDK::HandleXError: code:  [%d]\n", xerror->request_code);

    return 0;
}

// tengo que arreglar el codigo de 24

//
// constants -- to be documented
//
static unsigned const MAX_COLORS= 96;
static unsigned const NUM_COLS= 256;

//
// utility classes -- to be documented
//
static const char *visualNames[]=
{
    "StaticGray",
    "GrayScale",
    "StaticColor",
    "PseudoColor",
    "TrueColor",
    "DirectColor"
};
static int visualColorAlloc[]=
{
    AllocNone,  // "StaticGray",
    AllocAll,   // "GrayScale",
    AllocNone,  // "StaticColor",
    AllocAll,   // "PseudoColor",
    AllocNone,  // "TrueColor",
    AllocAll    // "DirectColor"
};

class dpyX11Visual_t: public virtual collectible_t
{
    Visual        *visualStruct;
    VisualID       visualIdentifier;
    int            visualScreen;         // must be zero
    unsigned       visualDepth;
    int            visualClass;
    unsigned long  visualRMask;
    unsigned long  visualGMask;
    unsigned long  visualBMask;
    int            visualCmapSize;
    int            visualBPRGB;

    friend class dtX11_t;
    friend class dtX11_t::windowX11_t;
    friend class smartReference_t<dpyX11Visual_t>;
};
typedef smartReference_t<dpyX11Visual_t> dpyX11Visual_ref;

template class       smartReference_t<dpyX11Visual_t>  ;
template class ql_t< smartReference_t<dpyX11Visual_t> >;




//
// Private parts -- to be documented
//
class dpyX11TaskInfo_t
{
    Display *dpy;

    ql_t<dpyX11Visual_ref> dpyVisuals;

    Window rootWinID;
    int dpyScreen;

    unsigned      dpyRsvCols;
    unsigned long dpyRsvPixels[MAX_COLORS];

    dtX11_t::windowX11_t *rootWindow;

    dictionary_t<Window, windowX11_ref> windowsByIdentifier;

    dpyX11TaskInfo_t(void)
    {
    };

    ~dpyX11TaskInfo_t(void)
    {
        dpy= NULL;

        while (dpyVisuals.len() > 0)
        {
            dpyVisuals.behead();
        }

        rootWindow= NULL;

        ql_t<Window> *keys= windowsByIdentifier.getKeys();
        while (keys->len() > 0)
        {
            windowsByIdentifier.remove(keys->head());
            keys->behead();
        }
        delete keys;
    }

    friend class dtX11_t;
    friend class dtX11_t::windowX11_t;
};

class winX11Info_t
{
private:
    dtX11_t *winDisplayTask;

    unsigned winWidth;
    unsigned winHeight;
    unsigned winXpos;
    unsigned winYpos;

    dpyX11Visual_ref winVisual;

    Window winID;

    Colormap winCmap;

    u32 sexChange;
    int bits_per_pixel;

    Pixmap bgPixmap;

    GC theGC;

    bool mapped;

    friend class dtX11_t;
    friend class dtX11_t::windowX11_t;

public:
    winX11Info_t(dtX11_t    *d,
                 unsigned   w,
                 unsigned   h,
                 unsigned   x,
                 unsigned   y
                )
    : winDisplayTask(d),
      winWidth(w),
      winHeight(h),
      winXpos(x),
      winYpos(y),
      bgPixmap(None),
      mapped(false)
    {
    }

    ~winX11Info_t(void)
    {
    }
};



//
// utility macros -- to be documented
//
#define VISUALS_OF_DISPLAY(d)   \
(                               \
    d->                         \
        dpyInfo->               \
        dpyVisuals              \
)
#define VISUALS_OF_THIS_DISPLAY VISUALS_OF_DISPLAY(this)



#define DPY_OF_WIN(w)     \
(                         \
    w->                   \
        winInfo->         \
        winDisplayTask->  \
        dpyInfo->         \
        dpy               \
)
#define DPY_OF_THIS_WIN (DPY_OF_WIN(this))



#define SCREEN_OF_WIN(w)    \
(                           \
    w->                     \
        winInfo->           \
        winDisplayTask->    \
        dpyInfo->           \
        dpyScreen           \
)
#define SCREEN_OF_THIS_WIN  (SCREEN_OF_WIN(this))




//
// Display -- to be documented
//
dtX11_t::dtX11_t(const char *options)
{
    assert ((options == NULL) && "dtX11_t: no options allowed\n");
#if 1
    if ( ! XInitThreads())
    {
        NOTIFY("dtX11_t::dtX11_t: XInitThreads failed\n");
    }
    else
    {
        NOTIFY("dtX11_t::dtX11_t: XInitThread succeded!\n");
    }
#endif

    dpyInfo= new dpyX11TaskInfo_t;
    dpyInfo->dpy= XOpenDisplay(NULL);


    if ( ! dpyInfo->dpy )
    {
        NOTIFY("dtX11_t::dtX11_t: cannot open display %s, "
               "bailing out\n",
               getenv("DISPLAY")
              );

        exit(1);
    }

    //
    // find out visual stuff
    //
    {
        int         nVinfo_return;
        XVisualInfo vinfo_template;

        XVisualInfo *returnedVisuals=
            XGetVisualInfo(dpyInfo->dpy,
                           0,
                           &vinfo_template,
                           &nVinfo_return
                          );

        Visual *dv= DefaultVisual(dpyInfo->dpy, DefaultScreen(dpyInfo->dpy));

        ql_t<dpyX11Visual_ref> aux;

        for (int i= 0; i < nVinfo_return; i++)
        {
            dpyX11Visual_t *v= new dpyX11Visual_t;

            v->visualStruct    = returnedVisuals[i].visual;
            v->visualIdentifier= returnedVisuals[i].visualid;
            v->visualScreen    = returnedVisuals[i].screen;
            v->visualDepth     = returnedVisuals[i].depth;
            v->visualClass     = returnedVisuals[i].c_class;
            v->visualRMask     = returnedVisuals[i].red_mask;
            v->visualGMask     = returnedVisuals[i].green_mask;
            v->visualBMask     = returnedVisuals[i].blue_mask;
            v->visualCmapSize  = returnedVisuals[i].colormap_size;
            v->visualBPRGB     = returnedVisuals[i].bits_per_rgb;

            if (v->visualStruct == dv)
                dpyInfo->dpyVisuals.insert(v);
            else
                aux.insert(v);
        }

        dpyInfo->dpyVisuals.insert(aux);

        XFree(returnedVisuals);
    }


    //
    // investigate root window
    //
    dpyInfo->rootWinID= DefaultRootWindow(dpyInfo->dpy);
    dpyInfo->dpyScreen= DefaultScreen(dpyInfo->dpy);


    if (dpyInfo->dpyVisuals.head()->visualClass == PseudoColor)
    {
//        NOTIFY("getting colors for default visual\n");

        int ncolors= MAX_COLORS;
        unsigned long plane_masks[32];

        while (1)
        {
            if (XAllocColorCells(dpyInfo->dpy,
                                DefaultColormap(
                                    dpyInfo->dpy,
                                    dpyInfo->dpyScreen
                                ),
                                False,
                                plane_masks,
                                0 /* nplanes */,
                                dpyInfo->dpyRsvPixels,
                                ncolors
                            ))
                break;

            ncolors--;

            if (ncolors == 0)
            {
//                NOTIFY ("ncolors 0, using instaled cmap \n");
                break;
            }
        }

        NOTIFY("dtX11_t::dtX11_t: got %d colors\n", ncolors);
        dpyInfo->dpyRsvCols= ncolors;
    }


    dpyInfo->rootWindow= new dtX11_t::windowX11_t(this);

    add_IO(new io_t( XConnectionNumber(dpyInfo->dpy)));
}

dtX11_t::~dtX11_t(void)
{
    Display *d= dpyInfo->dpy;
    delete dpyInfo;
    XCloseDisplay(d);
}


void
dtX11_t::getDefaultVisualCombination(unsigned *visualDepth, int *visualClass)
{
    if (visualDepth) *visualDepth= dpyInfo->dpyVisuals.head()->visualDepth;
    if (visualClass) *visualClass= dpyInfo->dpyVisuals.head()->visualClass;
    debugMsg(dbg_App_Verbose,
             "getDefaultVisualCombination",
             "default visual= [%s x %d]",
             visualClass?visualNames[*visualClass]:"Unknown Class",
             *visualDepth
            );
}


bool
dtX11_t::supportedVisualCombination(unsigned visualDepth, int visualClass)
{
    switch (visualClass)
    {
    case DirectColor:
        NOTIFY("dtX11_t::supportedVisualCombination: "
               "DirectColor not supported by SDK, "
               "returning 'false' to application\n"
              );
        return false;
    case GrayScale:
        NOTIFY("dtX11_t::supportedVisualCombination: "
               "GrayScale not supported by SDK, "
               "returning 'false' to application\n"
              );
        return false;
    };

    for (ql_t<dpyX11Visual_ref>::iterator_t i= VISUALS_OF_THIS_DISPLAY.begin();
         i != VISUALS_OF_THIS_DISPLAY.end();
         i++
        )
    {
        dpyX11Visual_ref v= static_cast<dpyX11Visual_ref>(i);
        if (v->visualDepth == visualDepth)
            if (v->visualClass == visualClass)
                return true;
    }

    return false;
}


displayTask_t::window_t *
dtX11_t::rootWindow(void)
{
    return dpyInfo->rootWindow;
}


Display*
dtX11_t::getDisplay(void)
{
    return dpyInfo->dpy;
}


window_ref
dtX11_t::createWindow(const char *title,
                      const char *geom,
                      window_ref  parent,
                      unsigned    visualDepth,
                      int         visualClass,
                      unsigned    defaultWidth,
                      unsigned    defaultHeight
                     )
{
NOTIFY("dtX11_t::createWindow: parent %s valid\n", 
parent.isValid() ? "IS" : "IS NOT");
    return new dtX11_t::windowX11_t(this,
                                    title,
                                    geom,
                                    parent,
                                    visualDepth,
                                    visualClass,
                                    defaultWidth,
                                    defaultHeight
                                   );
}

bool
dtX11_t::destroyWindow(smartReference_t<displayTask_t::window_t> win)
{
    window_t    *wp  = win;
    windowX11_t *wxp = dynamic_cast<windowX11_t *>(wp);
    
    if ( ! dpyInfo->windowsByIdentifier.lookUp(wxp->getWinId()) )
    {
        return false;
    }

    __unregisterWindow(wxp);

    return win->release(); // deactivate window
}


void
dtX11_t::IOReady(io_ref &io)
{
    debugMsg(dbg_App_Verbose, "IOReady", "checking for events");

    checkXevents();
}


void
dtX11_t::checkXevents(void)
{
    XEvent xev;

    while (XPending(dpyInfo->dpy))
    {
        debugMsg(dbg_App_Verbose, "checkXevents", "got an event");

        XNextEvent(dpyInfo->dpy, &xev);

        switch (xev.type)
        {
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
                XConfigureEvent *xConfEv= (XConfigureEvent *)&xev;

                windowX11_ref eventWindow=
                    dpyInfo->
                    windowsByIdentifier.lookUp(xConfEv->window);

                if (eventWindow.isValid())
                {
                    eventWindow->handleReconfigureEvent(xConfEv);
                }
            }
            break;
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

                windowX11_ref eventWindow=
                    dpyInfo->
                    windowsByIdentifier.lookUp(xMapEv->window);

                if (eventWindow.isValid())
                {
                    eventWindow->handleMapEvent();
                }
            }
            break;
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

                windowX11_ref eventWindow=
                    dpyInfo->
                    windowsByIdentifier.lookUp(xMapEv->window);

                if (eventWindow.isValid())
                {
                    eventWindow->handleUnmapEvent();
                }
            }
            break;
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
                windowX11_ref eventWindow=
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

                windowX11_ref eventWindow=
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

                windowX11_ref eventWindow=
                    dpyInfo->
                        windowsByIdentifier.lookUp(xDesWinEv->window);

                if (eventWindow.isValid())
                {
                    NOTIFY("checkXevents :: window was registered\n");
                    __unregisterWindow(eventWindow);
                }
                else
                {
                    NOTIFY("checkXevents :: window was NOT registered\n");
                }
            } break;

        default:
            NOTIFY("checkXevents:: unforeseen received event (%d)\n",
                   xev.type
                  );
        }
    }
}

void
dtX11_t::__registerWindow(windowX11_ref w)
{
    dpyInfo->windowsByIdentifier.insert( w->getWinId(), w);
}

void
dtX11_t::__unregisterWindow(windowX11_ref w)
{
    dpyInfo->windowsByIdentifier.remove(w->getWinId());
}


//
// window -- to be documented
// general windowX11_t contructor, for any kind of window
// (except root window, created by the displayTask)
//
dtX11_t::windowX11_t::windowX11_t(dtX11_t     *d,
                                  const char  *title,
                                  const char  *geomStr,
                                  window_ref   theparent,
                                  unsigned     visualDepth,
                                  int          visualClass,
                                  unsigned     defaultWidth,
                                  unsigned     defaultHeight
                                 )
: fgcolor(d->dpyInfo->dpy, "#FFFFFF"),
  bgcolor(d->dpyInfo->dpy, "#000000"),
  fontInfo(NULL),
  pBufferSize(0),
  pBuffer(NULL),
  ximage(NULL),
  useMITShm(true)
{
    assert ((defaultWidth > 0) && (defaultHeight > 0) && "bad window geom\n");

    winInfo= new winX11Info_t(d,
                              defaultWidth,
                              defaultHeight,
                              0, 0
                             );

    assert(d->supportedVisualCombination(visualDepth, visualClass));

    //
    // lets start by finding the correct visual
    //
    dpyX11Visual_ref visual= NULL;
    ql_t<dpyX11Visual_ref>::iterator_t i;
    for (i= winInfo->winDisplayTask->dpyInfo->dpyVisuals.begin();
         i != winInfo->winDisplayTask->dpyInfo->dpyVisuals.end();
         i++
        )
    {
        if (static_cast<dpyX11Visual_ref>(i)->visualDepth == visualDepth)
        if (static_cast<dpyX11Visual_ref>(i)->visualClass == visualClass)
        {
            visual= static_cast<dpyX11Visual_ref>(i);
                break;
        }
    }
    assert(visual.isValid());

    winInfo->winVisual= visual;

    // parent window, root by default
    Window pWin= theparent.isValid()
                     ? theparent->getWinId()
                     : DefaultRootWindow(DPY_OF_THIS_WIN);

    //
    // now, lets calculate the window attributes
    //

    XSetWindowAttributes xsa;
    unsigned             xsaFlags= 0;

    // back color
    xsa.background_pixel= BlackPixel(DPY_OF_THIS_WIN, SCREEN_OF_THIS_WIN);
    xsaFlags|= CWBackPixel;

    xsa.border_pixel= 0;
    xsaFlags|= CWBorderPixel;

    // event selection
    xsa.event_mask= StructureNotifyMask /* | ExposureMask */;
    xsaFlags|= CWEventMask;

    winInfo->winCmap=
        XCreateColormap(DPY_OF_THIS_WIN,
                        winInfo->winDisplayTask->dpyInfo->rootWinID,
                        visual->visualStruct,
                        visualColorAlloc[visual->visualClass]
                       );

    xsa.colormap= winInfo->winCmap;
    xsaFlags|= CWColormap;


    //
    // finally lets create the window
    //
    winInfo->winID= XCreateWindow(DPY_OF_THIS_WIN,
                                  pWin,
                                  winInfo->winXpos, winInfo->winYpos,
                                  winInfo->winWidth, winInfo->winHeight,
                                  0,
                                  visual->visualDepth,
                                  InputOutput,
                                  visual->visualStruct,
                                  xsaFlags,
                                  &xsa
                                 );

    setTitle(title);

    //
    // set position
    //
    setGeometry(geomStr);

    //
    // create graphic context
    //
    XGCValues gcvalues;

    gcvalues.line_width= 1;
    gcvalues.cap_style = CapRound;
    gcvalues.join_style= JoinRound;

    unsigned long gc_mask= GCLineWidth | GCCapStyle | GCJoinStyle;


    winInfo->theGC= XCreateGC(DPY_OF_THIS_WIN,
                              winInfo->winID,
                              gc_mask,
                              &gcvalues
                             );

    //
    // bits_per_pixel, how the hell is easy to guess them ?
    //
    XImage *img= XCreateImage(
                DPY_OF_THIS_WIN,
                winInfo->winVisual->visualStruct,
                winInfo->winVisual->visualDepth,
                ZPixmap,
                0,
                NULL,
                320, 240,
                8,  // should be 32
                0
              );

    winInfo->bits_per_pixel= img->bits_per_pixel;
    XDestroyImage(img);

    // determining sex of this machine
#ifdef __BUILD_FOR_LITTLE_ENDIAN
    winInfo->sexChange= 0;
#else
    winInfo->sexChange= 1;
#endif
    winInfo->sexChange^= XImageByteOrder(DPY_OF_THIS_WIN);
    NOTIFY("dtX11_t::windowX11_t: <win=0x%x> sexChange=%d\n",
           winInfo->winID,
           winInfo->sexChange
          );

    //
    // image buffer initialization
    //
    pBufferSize= 1024*768*4;
    pBuffer= (u8*)malloc(pBufferSize);
    if ( ! pBuffer)
    {
        NOTIFY("dtX11_t::windowX11_t: run out of memory, bailing out\n");
        abort();
    }

    //
    // gamma correction tables initializaction (may not be needed)
    //
    setGamma(1.0, 1.0, 1.0);

    //
    // Shared Memory Initialization
    //
    ximage= CreateXImage(winInfo->winWidth, winInfo->winHeight);

    XFlush(DPY_OF_THIS_WIN);//sleep(2);

    d->__registerWindow(this);
}

//
// private windowX11_t contructor, for the rootWindow only
//
dtX11_t::windowX11_t::windowX11_t(dtX11_t *d)
: fgcolor(d->dpyInfo->dpy, "#FFFFFF"),
  bgcolor(d->dpyInfo->dpy, "#000000"),
  fontInfo(NULL),
  pBufferSize(0),
  pBuffer(NULL),
  ximage(NULL),
  useMITShm(true)
{
    winInfo= new winX11Info_t(d,
                              DisplayWidth(
                                  d->dpyInfo->dpy,
                                  d->dpyInfo->dpyScreen
                              ),
                              DisplayHeight(
                                  d->dpyInfo->dpy,
                                  d->dpyInfo->dpyScreen
                              ),
                              0, 0
                             );

    winInfo->winVisual= d->dpyInfo->dpyVisuals.head();
    winInfo->winID = d->dpyInfo->rootWinID;

    winInfo->winCmap=
        DefaultColormap(d->dpyInfo->dpy,
                        DefaultScreen(d->dpyInfo->dpy)
                       );

#if 1
    winInfo->theGC= DefaultGC(d->dpyInfo->dpy,
                              d->dpyInfo->dpyScreen
                             );
#else
    XGCValues gcvalues;
    winInfo->theGC= XCreateGC(d->dpyInfo->dpy,
                              winInfo->winID,
                              0,
                              &gcvalues
                             );
#endif

    //
    // bits_per_pixel, how the hell is easy to guess them ?
    //
    XImage *img= XCreateImage(d->dpyInfo->dpy,
                              winInfo->winVisual->visualStruct,
                              winInfo->winVisual->visualDepth,
                              ZPixmap,
                              0,
                              NULL,
                              320, 240,
                              8,  // should be 32
                              0
                             );

    winInfo->bits_per_pixel= img->bits_per_pixel;
    XDestroyImage(img);

    // determining sex of this machine
#ifdef __BUILD_FOR_LITTLE_ENDIAN
    winInfo->sexChange= 0;
#else
    winInfo->sexChange= 1;
#endif
    winInfo->sexChange^= XImageByteOrder(d->dpyInfo->dpy);
    NOTIFY("dtX11_t::windowX11_t: <rootWin> sexChange=%d\n",
           winInfo->sexChange
          );


    setTitle("RootWindow");

    //
    // gamma correction tables initializaction (may not be needed)
    //
    setGamma(1.0, 1.0, 1.0);
}

dtX11_t::windowX11_t::~windowX11_t(void)
{
    if (this == winInfo->winDisplayTask->rootWindow())
    {
        NOTIFY("dtX11_t::windowX11_t::~windowX11_t: "
               "rootWindow does not free Graphics Context\n"
              );
    }
    else
    {
        XFreeGC(DPY_OF_THIS_WIN, winInfo->theGC);
    }

    XDestroyWindow(DPY_OF_THIS_WIN, winInfo->winID);

    //
    // Shared Memory Releasing
    //
    if (ximage != NULL)
    {
        DestroyXImage(ximage);
        ximage= NULL;
    }

    if (fontInfo)
    {
        XFreeFont(DPY_OF_THIS_WIN, fontInfo);
    }

    XSync(DPY_OF_THIS_WIN, true);

    if ( pBuffer )
    {
        free(pBuffer);
    }

    delete winInfo;
}

//
// the GUI window has been destroy, release data
//
bool
dtX11_t::windowX11_t::release(void)
{
    return true;
}

void
dtX11_t::windowX11_t::resize(unsigned w, unsigned h)
{
    if (this == winInfo->winDisplayTask->rootWindow())
        return;

    winInfo->winWidth = w;
    winInfo->winHeight= h;

    XResizeWindow(DPY_OF_THIS_WIN,
                  winInfo->winID,
                  winInfo->winWidth,
                  winInfo->winHeight
                 );

#if 0
    XSizeHints hints;

    hints.flags= PSize|PMinSize|PMaxSize;
    hints.width                          =
        hints.min_width= hints.max_width =
        winInfo->winWidth     ;
    hints.height= hints.min_height       =
        hints.max_height                 =
        winInfo->winHeight    ;
    XSetWMNormalHints(
        DPY_OF_THIS_WIN,
        winInfo->winID,
        &hints
    );
#endif

    XFlush(DPY_OF_THIS_WIN);
}

void
dtX11_t::windowX11_t::reposition(int x, int y)
{
    winInfo->winXpos= x;
    winInfo->winYpos= y;

    XMoveWindow(DPY_OF_THIS_WIN,
                winInfo->winID,
                winInfo->winXpos,
                winInfo->winYpos
               );

#if 0
    XSizeHints sizehints;

    sizehints.flags=
        USPosition | USSize  |
        PPosition  | PSize   ;
        sizehints.x           = winInfo->winXpos;
        sizehints.y           = winInfo->winYpos;

    XSetWMNormalHints(
        DPY_OF_THIS_WIN,
        winInfo->winID,
        &sizehints
    );
#endif

    XFlush(DPY_OF_THIS_WIN);
}

void
dtX11_t::windowX11_t::setTitle(const char *title)
{
    if ( title && (strlen(title) > 0) )
    {
        XTextProperty winTitle;

        winTitle.value   = (unsigned char *)title;
        winTitle.encoding= XA_STRING;
        winTitle.format  = 8;
        winTitle.nitems  = strlen(title)+1;

        XSetTextProperty(DPY_OF_THIS_WIN,
                         winInfo->winID,
                         &winTitle,
                         XA_WM_NAME
                        );
    }
}

void
dtX11_t::windowX11_t::setGeometry(const char *geomStr)
{
    bool doReposition= false;
    bool doResize    = false;

    debugMsg(dbg_App_Verbose, "setGeometry", "requested <%s>", geomStr);

    if ( ! geomStr || (strlen(geomStr) == 0) )
    {
        return;
    }

    unsigned width = winInfo->winWidth; // initial value, in case X|Y Negative
    unsigned height= winInfo->winHeight;
    int      xPos;
    int      yPos;
    int res= XParseGeometry(geomStr, &xPos, &yPos, &width, &height);

    if (res & XNegative)
        xPos= DisplayWidth(DPY_OF_THIS_WIN, DefaultScreen(DPY_OF_THIS_WIN))+xPos-width;
    if (res & YNegative)
        yPos= DisplayHeight(DPY_OF_THIS_WIN, DefaultScreen(DPY_OF_THIS_WIN))+yPos-height-32;

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
        sizehints.x         = winInfo->winXpos;
        sizehints.y         = winInfo->winYpos;
        sizehints.width     = winInfo->winWidth;
        sizehints.height    = winInfo->winHeight;
        sizehints.min_width = winInfo->winWidth;
        sizehints.min_height= winInfo->winHeight;
        sizehints.max_width = winInfo->winWidth;
        sizehints.max_height= winInfo->winHeight;

        XSetWMNormalHints(DPY_OF_THIS_WIN,
                          winInfo->winID,
                          &sizehints
                         );
    }
#endif
}

unsigned
dtX11_t::windowX11_t::getWidth(void) const
{
    return winInfo->winWidth;
}

unsigned
dtX11_t::windowX11_t::getHeight(void) const
{
    return winInfo->winHeight;
}

void
dtX11_t::windowX11_t::map(void)
{
    XMapWindow(DPY_OF_THIS_WIN, winInfo->winID);

    XMoveResizeWindow(DPY_OF_THIS_WIN,
                      winInfo->winID,
                      winInfo->winXpos,
                      winInfo->winYpos,
                      winInfo->winWidth,
                      winInfo->winHeight
                     );

    XSync(DPY_OF_THIS_WIN, False);

    winInfo->mapped= true;
}

void
dtX11_t::windowX11_t::unmap(void)
{
    XUnmapWindow(DPY_OF_THIS_WIN, winInfo->winID);

    XSync(DPY_OF_THIS_WIN, False);

    winInfo->mapped= false;
}

void
dtX11_t::windowX11_t::clear(void)
{
    XClearWindow(DPY_OF_THIS_WIN, winInfo->winID);
}

static
void
getColormap(Display *dpy, Colormap cmap, clutColor_t *clt)
{
    unsigned cmap_size= NUM_COLS;
    XColor   defs[NUM_COLS];


    /* grab the current cmap and lay it down */
    for (unsigned i= 0; i < cmap_size; i++)
    {
        defs[i].pixel = i;
        defs[i].flags = DoRed | DoGreen | DoBlue;
    }

    XQueryColors(dpy, cmap, defs, cmap_size);

    for (unsigned i= 0; i < cmap_size; i++)
    {
        clt[defs[i].pixel].r = (defs[i].red  >> 8) & 0xFF;
        clt[defs[i].pixel].g = (defs[i].green>> 8) & 0xFF;
        clt[defs[i].pixel].b = (defs[i].blue >> 8) & 0xFF;
    }
}


//
// paint methods
//
void
dtX11_t::windowX11_t::putRGB24(const u8 *data,
                               unsigned width,
                               unsigned height,
                               float zoomx,
                               float zoomy
                              )
{
    transRGB24AndPaint(data, width, height, zoomx, zoomy, false);
}

void
dtX11_t::windowX11_t::putRGB24inBG(const u8 *data,
                                   unsigned width,
                                   unsigned height,
                                   float zoomx,
                                   float zoomy
                                  )
{
    transRGB24AndPaint(data, width, height, zoomx, zoomy, true);
}


void
dtX11_t::windowX11_t::setGamma(float rGamma, float gGamma, float bGamma)
{
    switch (winInfo->winVisual->visualClass)
    {
    case StaticGray:
        break;

    case StaticColor:
    case TrueColor:
        createLookupTablesFromRGBMask(winInfo->winVisual->visualRMask,
                                      winInfo->winVisual->visualGMask,
                                      winInfo->winVisual->visualBMask,
                                      redLookup,
                                      greenLookup,
                                      blueLookup,
                                      rGamma,
                                      gGamma,
                                      bGamma
                                     );

        break;

    case PseudoColor:
        {
        u32 rMask= 0xE0;
        u32 gMask= 0x1C;
        u32 bMask= 0x03;

        createLookupTablesFromRGBMask(rMask,
                                      gMask,
                                      bMask,
                                      redLookup,
                                      greenLookup,
                                      blueLookup,
                                      rGamma,
                                      gGamma,
                                      bGamma
                                     );
        createPaletteFromRGBMask(rMask,
                                 gMask,
                                 bMask,
                                 pal
                                );

        }
        break;
    }
}


void
dtX11_t::windowX11_t::setOverrideRedirect(bool ORflag)
{
    XSetWindowAttributes swa;
    swa.override_redirect= ORflag;
    
    XChangeWindowAttributes(DPY_OF_THIS_WIN,
                            winInfo->winID,
                            CWOverrideRedirect, 
                            &swa
                           );
}


u32
dtX11_t::windowX11_t::getWinId(void) const
{
    return (u32)winInfo->winID;
}

int
dtX11_t::windowX11_t::getXPos(void) const
{
    return winInfo->winXpos;
}

int
dtX11_t::windowX11_t::getYPos(void) const
{
    return winInfo->winYpos;
}

void
dtX11_t::windowX11_t::handleMapEvent(void)
{
    //NOTIFY("windowX11_t::handleMapEvent: Ohhh! I'm visible!!\n");

    winInfo->mapped = true;
}

void
dtX11_t::windowX11_t::handleUnmapEvent(void)
{
    //NOTIFY("windowX11_t::handleUnmapEvent: Ohhh! I'm invisible!!\n");

    winInfo->mapped = false;
}

void
dtX11_t::windowX11_t::handleExposeEvent(XExposeEvent *xExpEv)
{
    NOTIFY("windowX11_t::handleExposeEvent: Ohhh! I'm being painted!!\n");
    NOTIFY("\tExposing area %dx%d+%d+%d (count=%d)\n",
           xExpEv->width,
           xExpEv->height,
           xExpEv->x,
           xExpEv->y,
           xExpEv->count
          );

    XCopyArea(DPY_OF_THIS_WIN,
              winInfo->bgPixmap,
              winInfo->winID,
              winInfo->theGC,
              xExpEv->x,
              xExpEv->y,
              xExpEv->width,
              xExpEv->height,
              xExpEv->x,
              xExpEv->y
             );
    XFlush(DPY_OF_THIS_WIN);
}

void
dtX11_t::windowX11_t::handleNoExposeEvent(XExposeEvent *xExpEv)
{
    NOTIFY("windowX11_t::handleNoExposeEvent: Ohhh! I'm being hidden!!\n");
    NOTIFY("\tHiding area %dx%d+%d+%d (count=%d)\n",
           xExpEv->width,
           xExpEv->height,
           xExpEv->x,
           xExpEv->y,
           xExpEv->count
          );
}

void
dtX11_t::windowX11_t::handleReconfigureEvent(XConfigureEvent *xConfEv)
{
    //NOTIFY("windowX11_t::handleReconfigureEvent: Ohhh! I'm being manipulated!!\n");

    winInfo->winWidth = xConfEv->width;
    winInfo->winHeight= xConfEv->height;

// the following causes a misfunction, should investigate what's happening
//    winInfo->winXpos= xConfEv->x;
//    winInfo->winYpos= xConfEv->y;
}


bool
dtX11_t::windowX11_t::isMapped(void) const
{
    return winInfo->mapped;
}

dpyX11Visual_ref
dtX11_t::windowX11_t::getWinVisual()
{
    return winInfo->winVisual;
}


//
// Shared Memory Managment and Functions
//

XImage*
dtX11_t::windowX11_t::CreateXImage(unsigned width, unsigned height)
{
    debugMsg(dbg_App_Verbose,
             "CreateXImage",
             "requested <%dx%d> (bpp=%d) for 0x%x",
             width, height,
             winInfo->bits_per_pixel,
             winInfo->winID
            );

    assert ((width > 0) && (height > 0));

    XImage *retImage= NULL;

    char __sdk_strerr[__SDK_ERR_BUF_LEN];

    int (*lastHandler)(Display *, XErrorEvent*);

    if ( ! useMITShm )   // shortcut
    {
        goto NO_MITSHM;
    }

    XError= false;
    lastHandler= XSetErrorHandler(HandleXError);

    if ( ! XShmQueryExtension(DPY_OF_THIS_WIN))
    {
        debugMsg(dbg_App_Verbose,
                 "CreateXImage",
                 "MIT_SHM extension NOT found"
                 "reversion to grab display\n"
                );

        XSetErrorHandler(lastHandler);

        NOTIFY("\tVoy a NO_MISHM razon 1\n");
        goto NO_MITSHM;
    }

    debugMsg(dbg_App_Verbose,
             "CreateXImage",
             "MIT_SHM extension found"
            );

    retImage= XShmCreateImage(DPY_OF_THIS_WIN,
                              winInfo->winVisual->visualStruct,
                              winInfo->winVisual->visualDepth,
                              ZPixmap,
                              NULL,
                              &shminfo,
                              width,
                              height
                             );

    if ( ! retImage)
    {
        NOTIFY("CreateXImage:: "
               "XShmCreateImage failed, reversing to grabdisplay\n"
              );

        XSetErrorHandler(lastHandler);

        NOTIFY("\tVoy a NO_MISHM razon 2\n");
        goto NO_MITSHM;
    }

    debugMsg(dbg_App_Verbose,
             "CreateXImage",
             "parameters: bpl=%d, bpp= %d, width=%d, height=%d\n",
             retImage->bytes_per_line,
             retImage->bits_per_pixel,
             retImage->width,
             retImage->height
            );

    shminfo.shmid = shmget(IPC_PRIVATE,
                           retImage->bytes_per_line * retImage->height,
                           IPC_CREAT | 0600
                          );

    if (shminfo.shmid < 0)
    {
        strerror_r(errno, __sdk_strerr, __SDK_ERR_BUF_LEN);
        NOTIFY("dtX11_t::windowX11_t::CreateXImage: "
               "Fail to create shared memory, reversing to grabdisplay"
               "reason: %s\n",
               __sdk_strerr
              );
        XDestroyImage(retImage);
        retImage= NULL;

        XSetErrorHandler(lastHandler);

        NOTIFY("\tVoy a NO_MISHM razon 3\n");
        goto NO_MITSHM;
    }

    shminfo.shmaddr = (char *)shmat(shminfo.shmid, 0, 0);

    if (shminfo.shmaddr == ((char *)-1))
    {
        strerror_r(errno, __sdk_strerr, __SDK_ERR_BUF_LEN);
        NOTIFY("dtX11_t::windowX11_t::CreateXImage: "
               "Fail to attach shared memory, reversing to grabdisplay"
               "reason: %s\n",
               __sdk_strerr
              );
        XDestroyImage(retImage);
        retImage= NULL;
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);

        XSetErrorHandler(lastHandler);

        NOTIFY("\tVoy a NO_MISHM razon 4\n");
        goto NO_MITSHM;
    }

    retImage->data= shminfo.shmaddr;
    shminfo.readOnly= False;

    if ( ! XShmAttach(DPY_OF_THIS_WIN, &shminfo))
    {
        debugMsg(dbg_App_Verbose,
                 "CreateXImage",
                 "XShmAttach failed, "
                 "reversion to grab display\n"
                );
        XDestroyImage(retImage);
        retImage= NULL;
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);

        XSetErrorHandler(lastHandler);

        NOTIFY("\tVoy a NO_MISHM razon 5\n");
        goto NO_MITSHM;
    }

    XSync(DPY_OF_THIS_WIN, False);

    debugMsg(dbg_App_Verbose, "CreateXImage", "XShmCreateImage done!!");

    if (XError)
    {
        NOTIFY("CreateXImage: got an XError, "
               "let's free ShmPixmap and shared memory\n"
              );

        XDestroyImage (retImage);
        retImage= NULL;
        shmdt (shminfo.shmaddr);
        shmctl (shminfo.shmid, IPC_RMID, 0);

        NOTIFY("\tVoy a NO_MISHM razon 6\n");
        goto NO_MITSHM;
    }

    XSetErrorHandler(lastHandler);

    NOTIFY("dtX11_t::windowX11_t::CreateXImage: <win=0x%x> "
           "using SHM id=%d\n",
           winInfo->winID,
           shminfo.shmid
          );

    return retImage;

NO_MITSHM:

    char *ximage_data;

    useMITShm= false;

    debugMsg(dbg_App_Verbose,
             "CreateXImage",
             "Not using MIT Shm"
             "reversion to grab display"
            );

    ximage_data= (char*)malloc(winInfo->bits_per_pixel * width * height);

    if (ximage_data == NULL)
    {
        NOTIFY("display_t::windowX11_t::CreateXImage: run out of memory!!\n");
        exit(1);
    }

    retImage= XCreateImage(DPY_OF_THIS_WIN,
                         winInfo->winVisual->visualStruct,
                         winInfo->winVisual->visualDepth,
                         ZPixmap,
                         0,
                         ximage_data,
                         width, height,
                         8,  // should be 32
                         0
                        );

    if (retImage == NULL)
    {
        NOTIFY("display_t::windowX11_t::CreateXImage: cannot create ximage!\n");
        exit(1);
    }

    memset(retImage->data, 0, retImage->bytes_per_line * retImage->height);

    NOTIFY("dtX11_t::windowX11_t::CreateXImage: <win=0x%x> NOT using SHM\n",
           winInfo->winID
          );

    return retImage;
}


void
dtX11_t::windowX11_t::DestroyXImage (XImage *theximage)
{
    if ( useMITShm )
    {
        XShmDetach (DPY_OF_THIS_WIN, &shminfo);
        XDestroyImage (theximage);
        shmdt (shminfo.shmaddr);
        shmctl (shminfo.shmid, IPC_RMID, 0);
    }
    else
    {
        XDestroyImage (theximage);
    }
}

#define MIN(a,b) ((a)<(b)?(a):(b))

#define COL_DIST_RGB2(x,y)  \
  ( \
  (int(pal[x].r) - int(hwPal[y].r)) * (int(pal[x].r) - int(hwPal[y].r)) \
+ (int(pal[x].g) - int(hwPal[y].g)) * (int(pal[x].g) - int(hwPal[y].g)) \
+ (int(pal[x].b) - int(hwPal[y].b)) * (int(pal[x].b) - int(hwPal[y].b)) \
  )

void
dtX11_t::windowX11_t::transRGB24AndPaint(const u8 *data,
                                     unsigned width,
                                     unsigned height,
                                     float zoomx,
                                     float zoomy,
                                     bool  background
                                    )
{
    //
    // letstart by zooming image
    //
    float actualZoomX= (zoomx<0
        ?(float)winInfo->winWidth/(float)width
        :zoomx
      );
    float actualZoomY= (zoomy<0
        ?(float)winInfo->winHeight/(float)height
        :zoomy
      );

    unsigned zoomedWidth = int(width*actualZoomX);
    unsigned zoomedHeight= int(height*actualZoomY);

    unsigned croppedZoomedWidth = MIN(
        zoomedWidth ,
        winInfo->winWidth
      );
    unsigned croppedZoomedHeight= MIN(
        zoomedHeight,
        winInfo->winHeight
      );

    croppedZoomedWidth= (croppedZoomedWidth + 4) >> 3;
    croppedZoomedWidth= croppedZoomedWidth << 3;

    unsigned croppedZoomedArea  = croppedZoomedWidth*croppedZoomedHeight;

    if (croppedZoomedArea * 4 > pBufferSize)
    {
        // realloc pBuffer
        pBufferSize= croppedZoomedArea * 4;
        free(pBuffer);
        pBuffer= (u8*)malloc(pBufferSize);
        if ( ! pBuffer)
        {
            NOTIFY("transRGB24AndPaint: run out of memory, bailing out\n");
            abort();
        }
    }

    switch (winInfo->winVisual->visualClass)
    {
    case StaticGray:
        stdc_translate_rgb24_to_luma8_and_zoom(pBuffer,
                                               data,
                                               width,
                                               height,
                                               actualZoomX,
                                               actualZoomY,
                                               croppedZoomedWidth,
                                               croppedZoomedHeight
                                              );

        break;

    case StaticColor:
    case TrueColor:
        switch (winInfo->winVisual->visualDepth)
        {
        case 8:
            stdc_translate_rgb24_to_xxx8_and_zoom(pBuffer,
                                                  data,
                                                  width,
                                                  height,
                                                  redLookup,
                                                  greenLookup,
                                                  blueLookup,
                                                  actualZoomX,
                                                  actualZoomY,
                                                  croppedZoomedWidth,
                                                  croppedZoomedHeight
                                                 );
            break;
        case 12:
        case 15:
        case 16:
            stdc_translate_rgb24_to_rgb16_and_zoom(pBuffer,
                                                   data,
                                                   width,
                                                   height,
                                                   redLookup,
                                                   greenLookup,
                                                   blueLookup,
                                                   actualZoomX,
                                                   actualZoomY,
                                                   croppedZoomedWidth,
                                                   croppedZoomedHeight
                                                  );

            if (winInfo->sexChange)
            {
                u8 *wk= reinterpret_cast<u8*>(pBuffer);
                for (unsigned i= 0; i < croppedZoomedArea; i++)
                {
                    u8 tmp   = wk[2*i+0];
                    wk[2*i+0]= wk[2*i+1];
                    wk[2*i+1]= tmp;
                }
            }

            break;

        case 24:
        case 32:
            if (winInfo->bits_per_pixel == 24)
            {
                stdc_translate_rgb24_to_rgb24_and_zoom(pBuffer,
                                                       data,
                                                       width,
                                                       height,
                                                       redLookup,
                                                       greenLookup,
                                                       blueLookup,
                                                       actualZoomX,
                                                       actualZoomY,
                                                       croppedZoomedWidth,
                                                       croppedZoomedHeight
                                                      );
            }
            else
            {
                stdc_translate_rgb24_to_rgb32_and_zoom(pBuffer,
                                                       data,
                                                       width,
                                                       height,
                                                       redLookup,
                                                       greenLookup,
                                                       blueLookup,
                                                       actualZoomX,
                                                       actualZoomY,
                                                       croppedZoomedWidth,
                                                       croppedZoomedHeight
                                                      );
            }

            //
            // esta mal, hay que mirar bits per pixel
            //
            if (winInfo->sexChange)
            {
                u8 *wk= reinterpret_cast<u8*>(pBuffer);
                for (unsigned i= 0; i < croppedZoomedArea; i++)
                {
                    u8 tmp0  = wk[4*i+0];
                    u8 tmp1  = wk[4*i+1];
                    wk[4*i+0]= wk[4*i+3];
                    wk[4*i+1]= wk[4*i+2];
                    wk[4*i+3]= tmp0;
                    wk[4*i+2]= tmp1;
                }
            }

            break;
        }
        break;

    case PseudoColor:
        {
            stdc_translate_rgb24_to_xxx8_and_zoom(pBuffer,
                                                  data,
                                                  width,
                                                  height,
                                                  redLookup,
                                                  greenLookup,
                                                  blueLookup,
                                                  actualZoomX,
                                                  actualZoomY,
                                                  croppedZoomedWidth,
                                                  croppedZoomedHeight
                                                 );

            u32 convTable[256];

            int used[NUM_COLS];
            for (unsigned i= 0; i < NUM_COLS; i++)
                used[i]= 0;

//            u8 *src= pBuffer;
//
//            for (unsigned jj= 0; jj < NUM_COLS; jj+=3)
//            {
//                used[jj]= 1;
//            }
            memset(used, 1, sizeof(used));
//            for (unsigned y= 0; y < croppedZoomedWidth ; y++)
//            {
//                for (unsigned x= 0; x < croppedZoomedHeight; x++)
//                {
//                    used[*src ]= 1;
//                    src++;
//                }
//            }

            clutColor_t hwPal[NUM_COLS];
            unsigned c= 0;
            unsigned rsvCols=
                winInfo->winDisplayTask->dpyInfo->dpyRsvCols;

            if ( ! rsvCols)
            {
                getColormap(DPY_OF_THIS_WIN,
                            winInfo->winCmap,
                            hwPal
                           );
            }

            XColor defs[NUM_COLS];

            for (unsigned i= 0; i< NUM_COLS; i++)
            {
                if (used[(587*i)%256]> 0)
                {
                    if (c < rsvCols)   /* hay sitio */
                    {
                        defs[c].red   = 0x101*pal[(587*i)%256].r;
                        defs[c].green = 0x101*pal[(587*i)%256].g;
                        defs[c].blue  = 0x101*pal[(587*i)%256].b;

#define DISPERSION 1 /*587*/

                        convTable[(587*i)%256]= winInfo->
                                                winDisplayTask->
                                                dpyInfo->
                                                dpyRsvPixels[c];

                        defs[c].flags = DoRed | DoGreen | DoBlue;
                        defs[c].pixel = winInfo->
                                        winDisplayTask->
                                        dpyInfo->
                                        dpyRsvPixels[c];

                        c++;

                        if (c == rsvCols)
                        {
                            XStoreColors(DPY_OF_THIS_WIN,
                                         winInfo->winCmap,
                                         defs,
                                         rsvCols
                                        );
                            getColormap(DPY_OF_THIS_WIN,
                                        winInfo->winCmap,
                                        hwPal
                                       );
                        }
                    }
                    else
                    {
                        /* no hay sitio busca cercano */
                        int      distance=0x7fffffff;
                        unsigned nearest= 0;;

                        for (unsigned j= 0; j < NUM_COLS; j++)
                        {
                            int curr_distance= COL_DIST_RGB2((587*i)%256, j);

                            if (curr_distance< distance)
                            {
                                distance= curr_distance;
                                nearest= j;
                            }

                        }
                        convTable[(587*i)%256]= nearest;
                    }
                }
            }

            if (c < rsvCols)
            {
                XStoreColors(DPY_OF_THIS_WIN,
                             winInfo->winCmap,
                             defs,
                             c
                            );
            }

            u8 *bb= reinterpret_cast<u8*>(pBuffer);
            for (unsigned i= 0; i < croppedZoomedArea; i++)
            {
                bb[i]= convTable[bb[i]];
            }

        }
        break;
    }

    //
    // lets paint
    //
    if (background)
    {
        paintInBG(pBuffer, croppedZoomedWidth, croppedZoomedHeight);
    }
    else
    {
        paint(pBuffer, croppedZoomedWidth, croppedZoomedHeight);
    }
}

void
dtX11_t::windowX11_t::paint(void *workBuffer, unsigned width, unsigned height)
{
    debugMsg(dbg_App_Verbose,
             "paint",
             "with the following parameters:\n"
              "window <%dx%d+%d+%d> \tdepth: %d\n"
              "image <%dx%d> bpp=%d\n",
             winInfo->winWidth,
             winInfo->winHeight,
             winInfo->winXpos,
             winInfo->winYpos,
             winInfo->winVisual->visualDepth,
             width,
             height,
             winInfo->bits_per_pixel
            );

    if (    (ximage == NULL)
         || (width  != ximage->width)
         || (height != ximage->height)
       )
    {
        debugMsg(dbg_App_Verbose,
                 "paint",
                 "size does not match or ximage is NULL, creating new XImage"
                 "<w=%d, h=%d, depth=%d>",
                 winInfo->winWidth,
                 winInfo->winHeight,
                 winInfo->winVisual->visualDepth
                );

        if (ximage != NULL)
        {
            DestroyXImage(ximage);
            ximage= NULL;
        }
        ximage= CreateXImage(width, height);
    }

    if (ximage == NULL)
    {
        NOTIFY("dtX11_t::windowX11_t::paint: ximage is NULL! skipping\n");
        return ;
    }

    int size= ximage->bytes_per_line * height;

    //NOTIFY("5 copying %p -> %p, %d bytes \n", workBuffer, ximage->data, size);

    memcpy(ximage->data, workBuffer, size);

    if ( useMITShm )
    {
        XShmPutImage(DPY_OF_THIS_WIN,
                     winInfo->winID,
                     winInfo->theGC,
                     ximage,
                     0, 0,
                     0, 0,
                     width, height,
                     false
                    );
    }
    else
    {
        XPutImage(DPY_OF_THIS_WIN,
                  winInfo->winID,
                  winInfo->theGC,
                  ximage,
                  0, 0,
                  0, 0,
                  width, height
                 );
    }

    XFlush(DPY_OF_THIS_WIN);
}

void
dtX11_t::windowX11_t::paintInBG(void     *workBuffer,
                                unsigned  croppedZoomedWidth,
                                unsigned  croppedZoomedHeight
                               )
{
    debugMsg(dbg_App_Verbose,
             "paintInBG",
             "with the following parameters:\n"
              "window <%dx%d+%d+%d> \tdepth: %d\n"
              "image <%dx%d>\n",
             winInfo->winWidth,
             winInfo->winHeight,
             winInfo->winXpos,
             winInfo->winYpos,
             winInfo->winVisual->visualDepth,
             croppedZoomedWidth,
             croppedZoomedHeight
            );

    XImage *img= XCreateImage(DPY_OF_THIS_WIN,
                              winInfo->winVisual->visualStruct,
                              winInfo->winVisual->visualDepth,
                              ZPixmap,
                              0,
                              NULL,
                              croppedZoomedWidth, croppedZoomedHeight,
                              8,  // should be 32
                              0
                             );

    if (img == NULL)
    {
        NOTIFY("windowX11_t::paintInBG: cannot create XImage!!\n");
        exit(1);
    }

    char *ximage_data;
    int size= img->bytes_per_line * croppedZoomedHeight;

    ximage_data= (char*)malloc(size);

    if (ximage_data == NULL)
    {
        NOTIFY("display_t::windowX11_t::CreateXImage: run out of memory!!\n");
        exit(1);
    }

    img->data= ximage_data;

    //NOTIFY("5 copying %p -> %p, %d bytes \n", workBuffer, img->data, size);

    memcpy(img->data, workBuffer, size);

    Pixmap pm= XCreatePixmap(DPY_OF_THIS_WIN,
                             winInfo->winID,
                             winInfo->winWidth,
                             winInfo->winHeight,
                             winInfo->winVisual->visualDepth
                            );
    if (   (winInfo->winWidth  > croppedZoomedWidth)
        || (winInfo->winHeight > croppedZoomedHeight)
       )
    {
        XFillRectangle(DPY_OF_THIS_WIN,
                       pm,
                       winInfo->theGC,
                       0, 0,
                       winInfo->winWidth,
                       winInfo->winHeight
                      );
    }
    XPutImage(DPY_OF_THIS_WIN,
              pm,
              winInfo->theGC,
              img,
              0, 0,
              0, 0,
              croppedZoomedWidth, croppedZoomedHeight
             );
    XSetWindowBackgroundPixmap(DPY_OF_THIS_WIN,
                               winInfo->winID,
                               pm
                              );
    XClearWindow(DPY_OF_THIS_WIN, winInfo->winID);

    if (winInfo->bgPixmap != None)
    {
        XFreePixmap(DPY_OF_THIS_WIN, winInfo->bgPixmap);
    }
    winInfo->bgPixmap= pm;

    XFlush(DPY_OF_THIS_WIN);

    //
    // collect garbage
    //
    XDestroyImage(img);
}


bool
dtX11_t::windowX11_t::setFgColor(const char *color)
{
    bool res = fgcolor.setColor(color);

    if (res)
    {
        XSetForeground(DPY_OF_THIS_WIN, winInfo->theGC, fgcolor.getPixel());
    }

    return res;
}


bool
dtX11_t::windowX11_t::setFgColor(u8 r, u8 g, u8 b)
{
    NOTIFY("setFgcolor: TBI\n");
    abort();

    return false;
}


bool
dtX11_t::windowX11_t::loadFont(const char *fontname, float size, int style)
{
    XGCValues gcvalues;

    // freeing old font, if loaded
    if (fontInfo)
    {
        XFreeFont(DPY_OF_THIS_WIN, fontInfo);
        fontInfo= NULL;
    }

    // loading font
    if ((fontInfo= XLoadQueryFont(DPY_OF_THIS_WIN, fontname)) == NULL)
    {
        NOTIFY("dtX11_t::windowX11_t::loadFont: cannot open %s font", fontname);
        return false;
    }

    gcvalues.font= fontInfo->fid;

    XChangeGC(DPY_OF_THIS_WIN, winInfo->theGC, GCFont, &gcvalues);

    return true;
}

bool
dtX11_t::windowX11_t::setBgColor(const char *color)
{
    bool res= bgcolor.setColor(color);

    if (res)
    {
        XSetForeground(DPY_OF_THIS_WIN, winInfo->theGC, bgcolor.getPixel());
    }

    return res;
}


bool
dtX11_t::windowX11_t::setBgColor(u8 r, u8 g, u8 b)
{
    NOTIFY("setBgcolor: TBI\n");
    abort();

    return false;
}


void
dtX11_t::windowX11_t::drawLineInBG(int x1, int y1, int x2, int y2, int lwidth)
{
    XGCValues gcvalues;

    debugMsg(dbg_App_Verbose,
             "drawLineInBG",
             " dpy=%p, win=0x%x, GC=%p, (%d,%d,)->(%d,%d), lwidth=%d\n",
             DPY_OF_THIS_WIN,
             winInfo->winID,
             winInfo->theGC,
             x1, y1,
             x2, y2,
             lwidth
            );

    if (lwidth <= 0)
    {
        NOTIFY("drawLineInBG: invalid line width %d\n", lwidth);
        return;
    }

    gcvalues.line_width= lwidth;

    XChangeGC(DPY_OF_THIS_WIN,
              winInfo->theGC,
              GCLineWidth,
              &gcvalues
             );

    XDrawLine(DPY_OF_THIS_WIN,
              winInfo->winID,
              winInfo->theGC,
              x1, y1,
              x2, y2
             );
    if (winInfo->bgPixmap != None)
    {
        XDrawLine(DPY_OF_THIS_WIN,
                  winInfo->bgPixmap,
                  winInfo->theGC,
                  x1, y1,
                  x2, y2
                 );
    }

    XFlush(DPY_OF_THIS_WIN);
}

void
dtX11_t::windowX11_t::drawString(int x, int y, const char *msg, align_e a)
{
    debugMsg(dbg_App_Verbose,
             "drawString",
             " dpy=%p, win=0x%x, GC=%p, (%d,%d,%s) s=%s\n",
             DPY_OF_THIS_WIN,
             winInfo->winID,
             winInfo->theGC,
             x, y,
             a == RIGHT ? "RIGHT" : (a == LEFT ? "LEFT" : "CENTER"),
             msg
            );

    if ( ! fontInfo)
    {
        NOTIFY("dtX11_t::windowX11_t::drawString: no font available\n");
        return;
    }

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

    XDrawString(DPY_OF_THIS_WIN,
                winInfo->winID,
                winInfo->theGC,
                x, y,
                msg,
                lenMsg
               );

    XFlush(DPY_OF_THIS_WIN);
}

