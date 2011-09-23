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
// $Id: color.hh 10255 2007-05-31 15:44:25Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr_color_hh__
#define __gwr_color_hh__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <icf2/item.hh>

#define DEFAULT_COLOR "#000"
#define MAX_COLOR_LEN 256

class Mcursor_t;

class Color_t: public virtual item_t
{
public:

    Color_t(Display *theDpy, const char *color);

    virtual ~Color_t(void);

private:

    Color_t(const Color_t&) {
        // forbidden color copy to avoid freeing colors twice
        fprintf(stderr, "trying to copy color\n");
        exit(1);
    };

    Color_t& operator= (const Color_t&) {
        // forbidden color asssignment to avoid freeing colors twice
        fprintf(stderr, "trying to assign color\n");
        exit(1);
        return *this ; // to shut lint off
    };

    Status NameToXColor(const char *name, XID pixel);

    Display  *dpy;

    XColor   theColor;
    Colormap cmap;
    bool     reserved;

    friend   class Mcursor_t;

public:

    char     name[MAX_COLOR_LEN]; // you better don't touch it

             bool setColor(const char *color);
    unsigned long getPixel(void);

    virtual const char *className(void) const { return "Color_t"; }
};

#endif
