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
// $Id: XConn.hh 20653 2010-06-23 13:20:32Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __XCONN_HH__
#define __XCONN_HH__

#include <X11/X.h>
#include <X11/Xlib.h>

enum searchMode_e { TITLE, PROPERTY };
enum matchMode_e  { PREFIX, SUFFIX, EXACT };
enum whichOne_e   { FIRST, ALL };

class XConn_t
{
private:
    static int numXConns;

    int      id;
    Display *dpy;
    int      screen;
    Window   rootWindow;
    char    *dpyName;

    void resize(Window wId, unsigned w, unsigned h);
    void reposition(Window wId, int x, int y);

public:

    XConn_t(char const *ndpyName);

    ~XConn_t(void);

    int         getId(void)            { return id; };
    char *const getDpyName(void) const { return dpyName; };

    bool        connected(void) { return dpy != NULL; };

    char*       getWinId    (searchMode_e mode,
                             char *const name, 
                             matchMode_e match,
                             whichOne_e whichOne
                            );
    void        mapWindow   (Window wId);
    void        unmapWindow (Window wId);

    bool        setGeometry (Window wId, char const *geomStr);
};

#endif

