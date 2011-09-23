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
// $Id: txtWin.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __txtwin_hh__
#define __txtwin_hh__

#include <icf2/item.hh>

#include "color.hh"
#include "textItem.hh"

enum txtAlign_e
{
    TXT_CENTER,
    TXT_LEFT,
    TXT_RIGHT
};

class txtWin_t: public item_t
{
private:
    Display      *dpy;
    Window        winId;
    Window        rootWin;
    textItem_t   *tir;
    GC            globalGC;
    GC            maskGC;
    Color_t      *fgcolor;
    XFontStruct  *globalFontInfo;
    int           width, height; // window size

    void guessSize(int &w, int &h);
    void decorateWindow(txtAlign_e align);
    void drawText(Pixmap bg,
                  Pixmap mask,
                  int winWidth,
                  int winHeight,
                  txtAlign_e align
                 );
    void drawString(Pixmap bg,
                    Pixmap mask,
                    const char *msg,
                    int posX,
                    int posY,
                    int lenMsg
                   );

public:
    txtWin_t(Display     *theDpy,
             XFontStruct *fontInfo,
             GC           theGC,
             Window       rootWin,
             const char  *msg,
             int          posX,
             int          posY,
             txtAlign_e   align,
             Color_t     *color,
             float        fx,
             float        fy
            );

    virtual ~txtWin_t(void);

    virtual const char *className(void) const { return "txtWin_t"; }
};

#endif
