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
// $Id: txtWinMgr.cc 20759 2010-07-05 10:30:36Z gabriel $
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

#include "txtWinMgr.hh"

#define maximun(x,y) (x)>(y)?(x):(y)

//
// scale functions
//
int
scaleX(int posX, float fx)
{
    return (int)floor((float)posX * fx + 0.5);
}

int
scaleY(int posY, float fy)
{
    return (int)floor((float)posY * fy + 0.5);
}

//
// txtWinMgr_t implementation
//

txtWinMgr_t::txtWinMgr_t(Display *theDpy)
: dpy(theDpy),
  fgcolor(theDpy, "Yellow"),
  fx(1.0), fy(1.0), // default scale (1.0,1.0)
  numMsgs(0)
{
    const char *fontname= "-*-courier-bold-r-*-*-24-*-*-*-*-*-iso8859-1";

    rootWin= RootWindow(dpy, DefaultScreen(dpy));

    // loading font
    if ((fontInfo= XLoadQueryFont(dpy, fontname)) == NULL)
    {
        NOTIFY("txtWinMgr_t::txtWinMgr_t: Cannot open %s font", fontname);
    }
    else
    {
        NOTIFY("txtWinMgr_t::txtWinMgr_t: Opened %s font", fontname);
    }

    XGCValues gcvalues;

    gcvalues.foreground= WhitePixel(dpy, DefaultScreen(dpy));
    gcvalues.background= BlackPixel(dpy, DefaultScreen(dpy));

    unsigned long gc_mask= GCForeground | GCBackground;

    if (fontInfo)
    {
        gcvalues.font= fontInfo->fid;
        gc_mask |= GCFont ;
    }

    theGC= XCreateGC(dpy, rootWin, gc_mask, &gcvalues);
}


txtWinMgr_t::~txtWinMgr_t(void)
{
}


void
txtWinMgr_t::virtualDisplaySize(int VW, int VH)
{
    int RW= DisplayWidth(dpy, DefaultScreen(dpy));
    int RH= DisplayHeight(dpy, DefaultScreen(dpy));

    fx= float(RW)/float(VW);
    fy= float(RH)/float(VH);

    // the following sentence keeps the original aspect ratio
    // of the virtual display
    // it may be removed, and the aspect ratio is not kept
    if (fx > fy)
        fx= fy;
    else
        fy= fx;

    NOTIFY("txtWinMgr_t::virtualDisplaySize: "
           "Real=%dx%d Virtual=%dx%d, scale (%f,%f)",
           RW, RH,
           VW, VH,
           fx, fy
          );
}


//
// Titles
bool
txtWinMgr_t::LoadFont(const char *fontname)
{
    XFontStruct *newFontInfo;

    if ((newFontInfo= XLoadQueryFont(dpy, fontname)) == NULL)
    {
        NOTIFY("txtWinMgr_t::LoadFont: Cannot open %s font\n", fontname);
        return false;
    }

    NOTIFY("txtWinMgr_t::LoadFont: Opened %s font", fontname);

    if (fontInfo)
        XFreeFont(dpy, fontInfo);

    fontInfo= newFontInfo;

    XSetFont(dpy, theGC, fontInfo->fid);

    return true;
}


bool
txtWinMgr_t::setTextColor(const char *color)
{
    if (fgcolor.setColor(color))
    {
        return true;
    }
    return false;
}


int
txtWinMgr_t::setText(const char *msg, int x, int y, txtAlign_e align)
{
    int retCode;

    NOTIFY("txtWinMgr_t::setText: "
           "Setting message [%s] at (%d, %d), align=%s",
           msg,
           x,
           y,
           align == TXT_CENTER ? "CENTER" :
                 (align == TXT_LEFT ? "LEFT" : "RIGHT")
          );

    txtWin_t *twin= new txtWin_t(dpy,
                                 fontInfo,
                                 theGC,
                                 rootWin,
                                 msg,
                                 scaleX(x, fx),
                                 scaleY(y, fy),
                                 align,
                                 &fgcolor,
                                 fx,
                                 fy
                                );

    retCode= numMsgs;
    theTextWinHolder[numMsgs]= twin;
    numMsgs++;

    return retCode;
}


void
txtWinMgr_t::EraseAllTexts(void)
{
    map<int, txtWin_t*>::iterator iter;

    for (iter= theTextWinHolder.begin();
         iter != theTextWinHolder.end();
         iter++
        )
    {
        theTextWinHolder.erase(iter->first);
        delete iter->second;
    }

    XSync(dpy,false);
}

bool
txtWinMgr_t::EraseText(int msgId)
{
    map<int, txtWin_t*>::iterator iter= theTextWinHolder.find(msgId);

    if (iter == theTextWinHolder.end()) // not found!
    {
        return false;
    }

    theTextWinHolder.erase(msgId);

    delete iter->second;

    return true;
}


