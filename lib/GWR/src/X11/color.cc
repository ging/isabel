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
// $Id: color.cc 10255 2007-05-31 15:44:25Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <icf2/notify.hh>


#include "color.hh"

Color_t::Color_t(Display *theDpy, const char *color)
{
    debugMsg(dbg_App_Normal, "Color_t", "Constructor invoked");

    dpy= theDpy;

    cmap= DefaultColormap(dpy, DefaultScreen(dpy));

    memset(name, 0, MAX_COLOR_LEN);
    reserved= false;

    (void)setColor(color);
}

Color_t::~Color_t(void)
{
    debugMsg(dbg_App_Normal, "~Color_t", "Invocado destructor");

    if(reserved)
        XFreeColors(dpy, cmap, &theColor.pixel, 1, 0);
}


//
//  setColor
//
//  Allocates "color"
//  Returns TRUE is a new color is allocated
//  FALSE otherwise (tipically, the old color is kept)
//

bool
Color_t::setColor(const char *color)
{
    if(color == NULL) {
        return false;
    }

    debugMsg(dbg_App_Normal, "setColor", "new color: %s", color);

    if(*name) {
        debugMsg(dbg_App_Normal,
                 "setColor",
                 "old color was: %s (%lu), %s reserved",
                 name,
                 theColor.pixel,
                 reserved?"NOT":""
                );
    }

    if(!strcmp(name, color)) {
      debugMsg(dbg_App_Normal, "setColor", "Same color, nothing to do");
      return false; //same color, nothing to do
    }

    if(reserved)
        XFreeColors(dpy, cmap, &theColor.pixel, 1, 0);

    if(!NameToXColor(color, 1)) {
        // it is not a color name or it cannot be allocated
        NameToXColor(name, 1); // return to original
        return false;
    }

    // good color, keeping name
    sprintf(name, "%s", color);
    return true; // New color, should check NameToXColor
}

// Resolve name to actual X color.
Status
Color_t::NameToXColor(const char *name, XID pixel)
{
    theColor.flags= DoRed|DoGreen|DoBlue;

    if(name && *name) {
        if(!XParseColor(dpy,
                        cmap,
                        name,
                        &theColor
                       )) {

            NOTIFY("Color_t::NameToXColor:: "
                   "unknown color or bad color format: %s\n",
                   name
                  );
            reserved= false;
            return 0;
        }

        theColor.flags= DoRed|DoGreen|DoBlue;

        if(!XAllocColor(dpy,
                        cmap,
                        &theColor
                       )) {
            NOTIFY("Color_t::NameToXColor:: ", "%s\n\t\t%s\n",
                   "cannot allocate color, all colorcells allocated"
                   "and no matching cell found."
                  );
            reserved= false;
            return 0;
        }

        reserved= true;
    } else {
        theColor.pixel= pixel;
        XQueryColor(dpy,
                    cmap,
                    &theColor
                    );
        reserved= false;
    }

    return 1;
}

unsigned long
Color_t::getPixel(void)
{
    return theColor.pixel;
}

