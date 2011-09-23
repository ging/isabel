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
// $Id: txtWinMgr.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __txtwinmngr_hh__
#define __txtwinmngr_hh__

#include <map>

#include <X11/X.h>
#include <X11/Xlib.h>

#include "color.hh"
#include "txtWin.hh"

class txtWinMgr_t
{
private:
    Display     *dpy;

    Window       rootWin;

    Color_t      fgcolor;
    XFontStruct *fontInfo;
    GC           theGC;

    float        fx, fy; // for scaling

    map<int, txtWin_t*> theTextWinHolder;
    int numMsgs;

public:
    txtWinMgr_t(Display *theDpy);

    virtual ~txtWinMgr_t(void);

    void virtualDisplaySize(int VW, int VH);

    //
    // Titles
    bool LoadFont(const char *fontname);
    bool setTextColor(const char *color);
    int  setText(const char *msg, int x, int y, txtAlign_e align);
    bool EraseText(int msgId);
    void EraseAllTexts(void);
};

#endif
