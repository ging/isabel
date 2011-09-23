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
// $Id: txtWin.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>

#include <icf2/notify.hh>

#include "txtWin.hh"
#include "titlesApp.hh"

#define maximun(x,y) (x)>(y)?(x):(y)

//
// txtWin_t implementation
//

txtWin_t::txtWin_t(Display     *theDpy,
                   XFontStruct *fontInfo,
                   GC           globalGC,
                   Window       rootWin,
                   const char  *msg,
                   int          posX,
                   int          posY,
                   txtAlign_e   align,
                   Color_t     *color,
                   float        fx,
                   float        fy
                  )
: dpy(theDpy),
  rootWin(rootWin),
  globalGC(globalGC),
  fgcolor(color),
  globalFontInfo(fontInfo)
{
    debugMsg(dbg_App_Normal, "txtWin_t", "Constructor invoked");

    // Create the Window that holds the text
    //
    XWindowAttributes xwa;
    XSetWindowAttributes swa;

    XGetWindowAttributes(dpy,
                         rootWin,
                         &xwa
                         );

    swa.background_pixel= 1;
    swa.override_redirect= True;
    swa.save_under       = True;
    swa.backing_store    = WhenMapped;

    winId= XCreateWindow(dpy,
                         rootWin,
                         0, 0,
                         120,             // win width
                         60,              // win height
                         0,               // border_width
                         xwa.depth,       // depth
                         InputOutput,     // class
                         CopyFromParent,  // visual
                         CWBackPixel | CWOverrideRedirect |
                             CWSaveUnder|CWBackingStore,
                         &swa
                        );

    debugMsg(dbg_App_Normal,
             "drawText",
             "drawing message at (%d, %d), align=%s",
             posX,
             posY,
             align == TXT_RIGHT ? "RIGHT" :
                 (align == TXT_LEFT ? "LEFT" : "CENTER")
            );

    tir= new textItem_t(msg);

    decorateWindow(align);

    switch (align)
    {
    case TXT_CENTER:
        posX -= width/2;
        break;
    case TXT_LEFT:
        // do nothing
        break;
    case TXT_RIGHT:
        posX -= width;
        break;
    default:
        NOTIFY("txtWin_t::drawText", "unknown alignment (%d)", align);
        return;
    }

    XMoveWindow(dpy, winId, posX, posY);

    XMapWindow(dpy, winId);
    XClearWindow(dpy, winId);
    XRaiseWindow(dpy, winId);
    XFlush(dpy);

    debugMsg(dbg_App_Normal, "txtWin_t", "Created text window 0x%lx", winId);
}


txtWin_t::~txtWin_t(void)
{
    debugMsg(dbg_App_Normal, "~txtWin_t", "Destructor invoked");

    if (winId != None)
    {
        XClearWindow(dpy, winId);
        XDestroyWindow(dpy, winId);
        XSync(dpy, False);
    }
    XFreeGC(dpy, maskGC);

    delete tir;
}


void
txtWin_t::drawString(Pixmap bg,
                     Pixmap mask,
                     const char *msg,
                     int posX,
                     int posY,
                     int lenMsg
                    )
{
    debugMsg(dbg_Show_Always, //dbg_App_Normal,
             "drawString",
             "drawing msg [%s] at real (%d, %d)",
             msg,
             posX,
             posY
            );

    XSetForeground(dpy, globalGC, BlackPixel(dpy, DefaultScreen(dpy)));

    for (int delta_x= -1; delta_x< 2; delta_x++)
    {
       for (int delta_y= -1; delta_y< 2; delta_y++)
       {
            XDrawString(dpy,
                        mask,
                        maskGC,
                        posX+delta_x,
                        posY+delta_y,
                        msg,
                        lenMsg
                       );
        }
    }

    XSetForeground(dpy, globalGC, fgcolor->getPixel());

    XDrawString(dpy,
                bg,
                globalGC,
                posX,
                posY,
                msg,
                lenMsg
               );
}

void
txtWin_t::drawText(Pixmap bg,
                   Pixmap mask,
                   int winWidth,
                   int winHeight,
                   txtAlign_e align
                  )
{
    unsigned int widthMsg, lenMsg;
    unsigned int txtPosX, txtPosY;


    ql_t<string> *msgs= tir->getText();

    unsigned int fontHeight= globalFontInfo->ascent + globalFontInfo->descent;

    txtPosY= globalFontInfo->ascent + 1; // plus 1 due to shade effect

    for (ql_t<string>::iterator_t it= msgs->begin();
         it != msgs->end();
         it++, txtPosY += fontHeight
        )
    {
        string piece= it;

        const char *msg= piece.c_str();

        lenMsg= strlen(msg);
        widthMsg= XTextWidth(globalFontInfo, msg, lenMsg);

        switch(align)
        {
        case TXT_CENTER:
            txtPosX= (winWidth - widthMsg)/2;
            break;
        case TXT_LEFT:
            txtPosX= 1;
            // do nothing
            break;
        case TXT_RIGHT:
            txtPosX= winWidth - widthMsg;
            break;
        default:
            NOTIFY("txtWin_t::drawText", "unknown alignment (%d)", align);
            return;
        }

        drawString(bg, mask, msg, txtPosX, txtPosY, lenMsg);
    }
}

void
txtWin_t::guessSize(int &w, int &h)
{
    w= 0, h= 0;

    int fontHeight= globalFontInfo->ascent + globalFontInfo->descent;

    ql_t<string> *msgs= tir->getText();

    for (ql_t<string>::iterator_t it= msgs->begin();
         it != msgs->end();
         it++, h += fontHeight
        )
    {
        string piece= it;

        const char *msg= piece.c_str();

        int lenMsg= strlen(msg);
        int widthMsg= XTextWidth(globalFontInfo, msg, lenMsg);
        if (widthMsg > w)
            w= widthMsg;
    }

    // add 2 due to shade effect
    h += 2;
    w += 2;
}

void
txtWin_t::decorateWindow(txtAlign_e align)
{
    Pixmap background=None, winMask= None;

    XWindowAttributes xwa;

    XGetWindowAttributes(dpy, rootWin, &xwa);

    guessSize(width, height);

    winMask   =XCreatePixmap(dpy,rootWin,width,height,1);
    background=XCreatePixmap(dpy,rootWin,width,height,xwa.depth);

    XGCValues gcvalues;

    gcvalues.function      = GXcopy;
    gcvalues.cap_style     = CapRound;
    gcvalues.join_style    = JoinRound;
    gcvalues.font          = globalFontInfo->fid;

    unsigned long gc_mask= GCFunction | GCCapStyle | GCJoinStyle | GCFont;

    maskGC= XCreateGC(dpy, winMask, gc_mask, &gcvalues);

    // background
    XSetForeground(dpy, globalGC, BlackPixel(dpy, DefaultScreen(dpy)));
    XSetFillStyle (dpy, globalGC, FillSolid);
    XFillRectangle(dpy, background, globalGC, 0, 0, width, height);

    // Window's mask
    XSetForeground(dpy, maskGC, 0x0);
    XSetBackground(dpy, maskGC, 0x0);
    XSetFillStyle (dpy, maskGC, FillOpaqueStippled);
    XFillRectangle(dpy, winMask, maskGC, 0, 0, width, height);
    XSetFillStyle (dpy, maskGC, FillSolid);
    XSetBackground(dpy, maskGC, 0x0);
    XSetForeground(dpy, maskGC, 0xffffffff);

    XSetFunction(dpy, globalGC, GXcopy);
    XSetFunction(dpy, maskGC, GXcopy);

    drawText(background, winMask, width, height, align);

    XResizeWindow(dpy, winId, width, height);


#if 1
    XShapeCombineMask(dpy,
                      winId,
                      ShapeBounding,
                      0,0,
                      winMask,
                      ShapeSet
                     );
#endif

    XSetWindowAttributes swa;
    unsigned long mask= CWBackPixmap;

    swa.background_pixmap= background;
    XChangeWindowAttributes(dpy, winId, mask, &swa);

    XClearWindow(dpy, winId);

    XFreePixmap(dpy, background);
    XFreePixmap(dpy, winMask);
}

