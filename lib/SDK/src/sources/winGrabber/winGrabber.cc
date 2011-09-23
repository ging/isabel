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
// $Id: winGrabber.cc 10216 2007-05-25 14:38:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/time.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>

#include <icf2/notify.hh>

#ifdef X_NOT_STDC_ENV
 extern int errno;
#endif

#include "xvgrab.hh"
#include "winGrabber.hh"

// X11 stuff
bool XError;
bool XIOError;
int HandleXIOError(Display *dpy);
int HandleXError(Display *dpy, XErrorEvent *xerror);

int  format        = ZPixmap;
int Image_Size (XImage *image);

windowGrabber_t::windowGrabber_t(sourceDescriptor_ref sdr,
                                 const char *newDpyName
                                )
: theSourceDescriptor(sdr),
  winId(None),
  ShmImage(NULL),
  ShmSize(0)
{
    if ( ! newDpyName || ! newDpyName[0])
    {
        NOTIFY("windowGrabber_t::windowGrabber_t: dpy name NULL or empty\n");
        NOTIFY("windowGrabber_t::windowGrabber_t: bailing out\n");
        exit(1);
    }
    dpyName= strdup(newDpyName);
    if ((dpy= XOpenDisplay(dpyName)) == NULL)
    {
        NOTIFY("windowGrabber_t::windowGrabber_t: XOpenDisplay failed.\n");
        exit(1); // Que deberiamos hacer?
    }
    screen= DefaultScreen(dpy);

    (void)XSetErrorHandler(HandleXError);
    (void)XSetIOErrorHandler(HandleXIOError);

    selectWindowByWinId(DefaultRootWindow(dpy));
}

windowGrabber_t::windowGrabber_t(sourceDescriptor_ref sdr)
: theSourceDescriptor(sdr),
  winId(None),
  ShmImage(NULL),
  ShmSize(0)
{
    dpyName = strdup(getenv("DISPLAY"));
    if ( ! dpyName || ! dpyName[0])
    {
        NOTIFY("windowGrabber_t::windowGrabber_t: "
               "getenv(DISPLAY) NULL or empty\n"
              );
        NOTIFY("windowGrabber_t::windowGrabber_t: bailing out\n");
        exit(1);
    }
    if ((dpy= XOpenDisplay(dpyName)) == NULL)
    {
       NOTIFY("windowGrabber_t::windowGrabber_t: XOpenDisplay failed.\n");
       exit(1); // Que deberiamos hacer?
    }
    screen= DefaultScreen(dpy);

    (void)XSetErrorHandler(HandleXError);
    (void)XSetIOErrorHandler(HandleXIOError);

    selectWindowByWinId(DefaultRootWindow(dpy));
}

windowGrabber_t::~windowGrabber_t(void)
{
    FreeShmResources();
    XCloseDisplay(dpy);
    free(dpyName);
}

image_t*
windowGrabber_t::getImage(void)
{
    int       xPos, yPos;
    XImage   *image= NULL;
    XColor   *colors;
    Window    dummywin;
    int       ncolors= 0;

    int      absx, absy, x, y;
    int      dwidth, dheight;
    int      width, height;

    int      bw;
    unsigned  buffer_size;

    //--- Get the real parameters of the window being dumped. ---

    if (winId == None)
    {
        NOTIFY("windowGrabber_t::captureWindow: winId == None\n");
        return NULL;
    }

#if 1
    if ( ! XGetWindowAttributes(dpy, winId, &winInfo))
    {
        NOTIFY("windowGrabber_t::captureWindow:"
               " cannot get window attributes of 0x%lx\n",
               winId
              );
        return NULL;
    }
#endif

    if (winInfo.map_state == IsUnmapped)
    {
        NOTIFY("windowGrabber_t::captureWindow: winId isUnmmapped\n");
        return NULL;
    }
    if (winInfo.map_state == IsUnviewable)
    {
        NOTIFY("windowGrabber_t::captureWindow: winId isUnviewable\n");
        return NULL;
    }

    //--- Handle any frame window ---
    if ( ! XTranslateCoordinates(dpy,
                                 winId,
                                 RootWindow(dpy, screen),
                                 0, 0,
                                 &absx, &absy,
                                 &dummywin
                                )
       )
    {
        NOTIFY("windowGrabber_t::captureWindow: "
               "windowDump:  unable to translate window coordinates (%d,%d)\n",
               absx, absy
              );

        return NULL;
    }

    winInfo.x= absx;
    winInfo.y= absy;
    width    = winInfo.width;
    height   = winInfo.height;
    dwidth   = DisplayWidth (dpy, screen);
    dheight  = DisplayHeight(dpy, screen);
    bw       = 0;

    if ( (absx+(int)width <= 0) || (absy+(int)height <= 0) )
    {
        NOTIFY("windowGrabber_t::captureWindow: invalid coordinates\n");
        return NULL;
    }

    //---- Clip to window ---
    if(absx < 0) width  += absx, absx= 0;
    if(absy < 0) height += absy, absy= 0;
    if(absx + width  > dwidth ) width = dwidth  - absx;
    if(absy + height > dheight) height= dheight - absy;

    //--- Snarf the pixmap with XGetImage ---

    x= absx - winInfo.x;
    y= absy - winInfo.y;

    xPos= absx;
    yPos= absy;

    image= capturaVentana(winId,
                          x, y,
                          width, height,
                          AllPlanes,
                          format
                         );

    if ( ! image || ! image->data)
    {
        NOTIFY("windowGrabber_t::captureWindow: "
               "unable to get image at %dx%d+%d+%d\n",
                width, height,
                x, y
               );
        return NULL;
    }

    //--- Determine the pixmap size. ---
    buffer_size= Image_Size(image);

    ncolors    = getXColors(&colors);

    XFlush(dpy);

    grabImage_t *grabImage= (grabImage_t *)malloc(sizeof(grabImage_t));

#if 1
    if ( ! convertImage(image,
                        colors,
                        ncolors,
                        &winInfo,
                        grabImage,
                        cached
                       )
       )
    {
        free(grabImage);
        grabImage= NULL;
    }
#else
    /* build the 'global' grabPic stuff */

    size_t picSize= (size_t)image->bytes_per_line * image->height;
    grabImage->grabPic= (u8*)malloc(picSize);
    memcpy(grabImage->grabPic, image->data, picSize);
    grabImage->gbits= 16;
    grabImage->gWIDE= image->width;
    grabImage->gHIGH= image->height;

#endif

    DestroyXImage(image);
    free(colors);

    if (grabImage == NULL)
    {
        NOTIFY("windowGrabber_t::captureWindow: grabImage is NULL\n");
        return NULL; 
    }

    // get time
    struct timeval currentTime;
    gettimeofday (&currentTime, NULL);
    u32 timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

    image_t *img= new image_t(grabImage->grabPic,
                              3 * grabImage->gWIDE * grabImage->gHIGH,
                              RGB24_FORMAT,
                              grabImage->gWIDE,
                              grabImage->gHIGH,
                              timestamp
                            );

    free(grabImage->grabPic);
    free(grabImage);

    return img;
}

int
winlowbitnum(unsigned long ul)
{
    // returns position of lowest set bit in 'ul' as an integer (0-31),
    // or -1 if none

    int i;
    for (i= 0; ((ul&1) == 0) && i < 32;  i++, ul >>= 1);
    if (i == 32) i = -1;
    return i;
}

//
// trick to guess LSB or MSB machine
//
union swapun
{
    u32 l;
    u16 s;
    u8  b[sizeof(u32)];
};

//
// Routine to select a window by its Window Identifier
// this routine cache values, so any other method to select
// windows should call this before returning
//
bool
windowGrabber_t::selectWindowByWinId(Window newWinId)
{
    if (newWinId == None)
    {
        NOTIFY("windowGrabber_t::selectWindowByWinId: "
               "cannot select \"None\" window\n"
              );
        return false;
    }

    if ( ! XGetWindowAttributes(dpy, newWinId, &winInfo))
    {
        NOTIFY("windowGrabber_t::selectWindowByWinId: "
               "cannot get window attributes\n"
              );
        return false;
    }

    Visual *visual;
    visual= winInfo.visual;

    if ( ! visual) // no visual ?
    {
        NOTIFY("windowGrabber_t::selectWindowByWinId: "
               "no visual available\n"
              );
        winId= None;
        return false;
    }

    FreeShmResources(); // just in case
    InitShmResources();

    union swapun   sw;
    // byte order
    sw.l = 1;
    cached.isLsbMachine = (sw.b[0]) ? 1 : 0;
    cached.rmask  = cached.gmask  = cached.bmask  = 0;
    cached.rshift = cached.gshift = cached.bshift = 0;
    cached.r8shift= cached.g8shift= cached.b8shift= 0;

    if (visual->c_class == TrueColor || visual->c_class == DirectColor)
    {
        unsigned int tmp;

        /* compute various shifty constants we'll need */
        cached.rmask = visual->red_mask;
        cached.gmask = visual->green_mask;
        cached.bmask = visual->blue_mask;

        cached.rshift = winlowbitnum((unsigned long) cached.rmask);
        cached.gshift = winlowbitnum((unsigned long) cached.gmask);
        cached.bshift = winlowbitnum((unsigned long) cached.bmask);

        cached.r8shift = 0;  tmp = cached.rmask >> cached.rshift;
        while (tmp >= 256) { tmp >>= 1;  cached.r8shift -= 1; }
        while (tmp < 128)  { tmp <<= 1;  cached.r8shift += 1; }

        cached.g8shift = 0;  tmp = cached.gmask >> cached.gshift;
        while (tmp >= 256) { tmp >>= 1;  cached.g8shift -= 1; }
        while (tmp < 128)  { tmp <<= 1;  cached.g8shift += 1; }

        cached.b8shift = 0;  tmp = cached.bmask >> cached.bshift;
        while (tmp >= 256) { tmp >>= 1;  cached.b8shift -= 1; }
        while (tmp < 128)  { tmp <<= 1;  cached.b8shift += 1; }
    }

    // if we're on an lsbfirst machine, or the image came from an lsbfirst
    // machine, we should flip the bytes around.  NOTE:  if we're on an
    // lsbfirst machine *and* the image came from an lsbfirst machine,
    // *don't* flip bytes, as it should work out

    int byte_order= ImageByteOrder(dpy);
    cached.flipBytes = ( cached.isLsbMachine && byte_order != LSBFirst) ||
                       (!cached.isLsbMachine && byte_order == LSBFirst);


    winId= newWinId;

    return true;
}

// -----------------------------------------------------------------
// Routine to let user select a window using the mouse
bool
windowGrabber_t::selectWindowByClicking(void)
{
    int status;
    Cursor cursor;
    XEvent event;
    Window target_win = None, root = RootWindow(dpy, DefaultScreen(dpy));
    int buttons = 0;

    // Warn the user
    beep();

    //--- Make the target cursor ---
    cursor= XCreateFontCursor(dpy, XC_crosshair);

    //--- Grab the pointer using target cursor, letting it room all over ---
    status= XGrabPointer(dpy,
                         root,
                         false,
                         ButtonPressMask|ButtonReleaseMask,
                         GrabModeSync,
                         GrabModeAsync,
                         root,
                         cursor,
                         CurrentTime
                        );
    if (status != GrabSuccess)
    {
        NOTIFY("windowGrabber_t::selectWindowByClicking "
               "Cannot grab the pointer"
              );

        XUngrabPointer(dpy, CurrentTime);      // Done with pointer
        XFlush(dpy);

        return false;
    }

    //--- Let the user select a window... ---
    while ((target_win == None) || (buttons != 0))
    {
        // allow one more event
        XAllowEvents(dpy, SyncPointer, CurrentTime);
        XWindowEvent(dpy, root, ButtonPressMask|ButtonReleaseMask, &event);
        switch (event.type)
        {
        case ButtonPress:
            if (target_win == None)
            {
                target_win = event.xbutton.subwindow; // window selected
                if (target_win == None)
                    target_win = root;
            }
            buttons++;
            break;
        case ButtonRelease:
            if (buttons > 0) // there may have been some down before we started
                buttons--;
            break;
        }
    }

    XUngrabPointer(dpy, CurrentTime);       // Done with pointer
    XFlush(dpy);

    target_win= XmuClientWindow(dpy, target_win);

    return selectWindowByWinId(target_win);
}

// -----------------------------------------------------------------
void
windowGrabber_t::beep(void)
{
    XBell(dpy, 50);
}

// -----------------------------------------------------------------
// Determine the pixmap size.
int
Image_Size(XImage *image)
{
    if(image->format != ZPixmap)
        return(image->bytes_per_line * image->height * image->depth);

    return(image->bytes_per_line * image->height);
}

#define lowbit(x) ((x) & (~(x) + 1))

// -----------------------------------------------------------------
// Get the XColors of all pixels in image - returns # of colors
int
windowGrabber_t::getXColors(XColor **colors)
{
    int       i, ncolors;
    Colormap  cmap  = winInfo.colormap;
    Visual   *visual= winInfo.visual;

#if 0
    if(use_installed)
        // assume the visual will be OK ...
        cmap = XListInstalledColormaps(dpy, winInfo->root, &i)[0];
#endif

    if ( ! cmap) return 0;

    ncolors= visual->map_entries;

    if ( ! (*colors = (XColor *) malloc (sizeof(XColor) * ncolors)))
    {
        NOTIFY("Out of memory allocating colors array\n");
        exit(1);   // que hacemos con esto??
    }

    if ((visual->c_class == DirectColor ) ||
        (visual->c_class == TrueColor   )
       )
    {
        Pixel red, green, blue, red1, green1, blue1;

        red   = green = blue = 0;
        red1  = lowbit(visual->red_mask);
        green1= lowbit(visual->green_mask);
        blue1 = lowbit(visual->blue_mask);
        for (i= 0; i < ncolors; i++)
        {
            (*colors)[i].pixel = red|green|blue;
            (*colors)[i].pad = 0;

            red += red1;
            if (red   > visual->red_mask  ) red   = 0;

            green += green1;
            if (green > visual->green_mask) green = 0;

            blue += blue1;
            if (blue  > visual->blue_mask ) blue  = 0;
        }

    }
    else
    {
        for (i= 0; i < ncolors; i++)
        {
            (*colors)[i].pixel= i;
            (*colors)[i].flags= DoRed|DoGreen|DoBlue;
            (*colors)[i].pad  = 0;
        }
    }

    XQueryColors(dpy, cmap, *colors, ncolors);

    return(ncolors);
}


//
// Shared Memory Managment
//
void
windowGrabber_t::InitShmResources(void)
{
    unsigned width = DisplayWidth(dpy, screen);
    unsigned height= DisplayHeight(dpy, screen);

    debugMsg(dbg_App_Verbose,
             "InitShmResources",
             "requested <%dx%d>",
             width,
             height
            );

    //int (*lastHandler)(Display *, XErrorEvent*);

    XError= false;
    //lastHandler= XSetErrorHandler(HandleXError);

    if( ! XShmQueryExtension (dpy))
    {
        debugMsg(dbg_App_Verbose,
                 "InitShmResources",
                 "MIT_SHM extension NOT found"
                 "reversion to grab display"
                );

        //XSetErrorHandler(lastHandler);

        return;
    }
    else
    {
        debugMsg(dbg_App_Verbose,
                 "InitShmResources",
                 "MIT_SHM extension found!"
                );
    }

    debugMsg(dbg_App_Verbose, "InitShmResources", "MIT_SHM extension found");

    ShmImage= XShmCreateImage (dpy,
                               winInfo.visual,
                               winInfo.depth,
                               ZPixmap,
                               NULL,
                               &shminfo,
                               width,
                               height
                              );

    if ( ! ShmImage)
    {
        NOTIFY("winGrabber_t::InitShmResources "
               " XShmCreateImage failed, reversing to grabdisplay"
              );

        //XSetErrorHandler(lastHandler);

        return;
    }

    ShmSize= ShmImage->bytes_per_line * ShmImage->height;

    debugMsg(dbg_App_Verbose,
             "winGrabber_t::InitShmResources",
             "ShmImage created bpl=%d, width=%d, height=%d (max. size= %d)\n",
             ShmImage->bytes_per_line,
             ShmImage->width,
             ShmImage->height,
             ShmSize
            );

    shminfo.shmid= shmget(IPC_PRIVATE, ShmSize, IPC_CREAT | 0777);

    if (shminfo.shmid < 0)
    {
        perror("Fail to create shared memory \n");
        XDestroyImage(ShmImage);
        ShmImage= NULL;

        //XSetErrorHandler(lastHandler);

        return;
    }

    shminfo.shmaddr = (char *)shmat(shminfo.shmid, 0, 0);

    if (shminfo.shmaddr == ((char *)-1))
    {
        perror("Fail to create shared memory \n");
        XDestroyImage(ShmImage);
        ShmImage= NULL;
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
        //XSetErrorHandler(lastHandler);

        return;
    }

    ShmImage->data= shminfo.shmaddr;
    shminfo.readOnly= false;

    if( ! XShmAttach(dpy, & shminfo))
    {
        debugMsg(dbg_App_Verbose,
                 "InitShmResources",
                 "XShmAttach failed, "
                 "reversion to grab display"
                );
        XDestroyImage(ShmImage);
        ShmImage= NULL;
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);

        //XSetErrorHandler(lastHandler);

        return;
    }

    XSync(dpy, false);

    debugMsg(dbg_App_Verbose, "InitShmResources", "XShmCreateImage done!!");

    if (XError)
    {
        XDestroyImage (ShmImage);
        ShmImage= NULL;
        shmdt (shminfo.shmaddr);
        shmctl (shminfo.shmid, IPC_RMID, 0);
    }

    //XSetErrorHandler(lastHandler);
}

void
windowGrabber_t::FreeShmResources(void)
{
    if (ShmImage)
    {
        XShmDetach(dpy, &shminfo);
        XDestroyImage(ShmImage);
        ShmImage= NULL;
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
    }
}

XImage*
windowGrabber_t::capturaVentana(Drawable window,
                                int x, int y,
                                int width, int height,
                                unsigned long plane_mask,
                                int format
                               )
{
    int size; // = w * h * (BitmapPad (theDisp) / 8);

    debugMsg(dbg_App_Verbose,
             "capturaVentana",
             " %dx%d:%d Maximun size: \t%d\n",
             width, height, winInfo.depth,
             ShmSize
            );

    if (ShmImage)
    {
        size= winInfo.width *
              winInfo.height *
              ShmImage->bytes_per_line / ShmImage->width;

        if (ShmSize < size)
        {
            FreeShmResources();
            InitShmResources();
        }
    }

    if (ShmImage)
    {
        if (    (width != ShmImage->width)
            || (height != ShmImage->height)
           )
        {
            XDestroyImage(ShmImage);

            ShmImage= XShmCreateImage (dpy,
                                       winInfo.visual,
                                       winInfo.depth,
                                       ZPixmap,
                                       NULL,
                                       &shminfo,
                                       width,
                                       height
                                      );

            if ( ! ShmImage)
            {
                NOTIFY("windowGrabber_t::capturaVentana: "
                       "XShmCreateImage failed, reversing to grabdisplay\n"
                      );
            }
            else
            {
                ShmImage->data= shminfo.shmaddr;

                debugMsg(dbg_App_Verbose,
                         "paint",
                         "XShmCreateImage bpl=%d, width=%d, height=%d\n",
                         ShmImage->bytes_per_line,
                         ShmImage->width,
                         ShmImage->height
                        );
            }
        }
    }

    if (ShmImage)
    {
//NOTIFY("windowGrabber_t::capturaVentana: XShmGetImage: <%dx%d+%d+%d>\n", width, height, x, y);
        if ( ! XShmGetImage(dpy,
                            window,
                            ShmImage,
                            x, y,
                            plane_mask
                          )
           )
        {
            NOTIFY("windowGrabber_t::capturaVentana: falla XShmGetImage\n");
            return NULL;
        }

        return ShmImage;
    }
    else
    {
//NOTIFY("XGetImage <%dx%d+%d+%d>\n", width, height, x, y);
        XImage *im= XGetImage(dpy,
                              window,
                              x, y,
                              width, height,
                              plane_mask,
                              format
                             );
        return im;
    }
}

void
windowGrabber_t::DestroyXImage(XImage *im)
{
    if ( ! ShmImage)
        XDestroyImage(im);
}

bool 
windowGrabber_t::setFrameRate(double fps)
{
    return false;
}

double 
windowGrabber_t::getFrameRate(void)
{
    return -1;
}

bool 
windowGrabber_t::setGrabSize(unsigned width, unsigned height)
{
    return false;
}

void 
windowGrabber_t::getGrabSize(unsigned *width, unsigned *height)
{
    *width = 0;
    *height= 0;
}

bool
windowGrabber_t::setGrabFormat(u32 fcc)
{
    return fcc == RGB24_FORMAT; // not really set, no complains setting RGB24
}

u32
windowGrabber_t::getGrabFormat(void)
{
    return RGB24_FORMAT;
}

sourceDescriptor_ref 
windowGrabber_t::getDescriptor(void) const
{
    return theSourceDescriptor;
}


/******************************************************************
*
*  X functions.
*
******************************************************************/

int
HandleXError (Display *dpy, XErrorEvent *xerror)
{
#ifdef __DEBUG
#if 0
    char msg[80];
    XID rid= xerror->resourceid;

    XGetErrorText(dpy, xerror->error_code, msg, 80);
    NOTIFY("HandleXError: X ERROR: rid = %ld 0x%lx\n\t\t%s\n", rid, rid, msg);
    fflush(stderr);
#endif
#endif

    XError= true;
    return 0;
}

int
HandleXIOError (Display *dpy)
{
#ifdef __DEBUG
    NOTIFY("HandleXIOError: !!!!!\n");
    fflush(stderr);
#endif

    XIOError= true;
    return 0;
}

