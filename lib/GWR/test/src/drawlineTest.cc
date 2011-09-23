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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <X.h>
#include <Xlib.h>
#endif

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/sched.hh>

#include <gwr/displayTask.h>

//const char *colors[]= { "red", "yellow", "green", "blue", "brown", "white", "grey", "orange", "pink" };
const char *colors[]= { "red", "yellow", "green", "blue" };

const int numColors= sizeof(colors) / sizeof(const char *);

int
main(int argc, char **argv)
{ 
    int winWidth= 320, winHeight= 240;

    // PINTOR
    displayTask_ref dpyTask= NULL;
#ifdef WIN32
    dpyTask= createDisplayTask("GDI",NULL);
#else
    dpyTask= createDisplayTask("X11", NULL);
#endif

    sched_t app(128);
    app << (task_ref)((abstractTask_t *)dpyTask);

    //
    // verify depths
    //
    int vClass;
    int vDepth;
    dpyTask->getDefaultVisualCombination((unsigned*)&vDepth, &vClass);

    // VENTANAS
    displayTask_t::window_t *win;
    win = dpyTask->createWindow("draw line test", // title
                                NULL,             // no geometry
                                NULL,             // no parent
                                vDepth, vClass,
                                winWidth, winHeight
                               );

    win->map();

    win->setFgColor("green");

    const char *fontname= NULL;
    int size= 0;
    int style= 0;

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    fontname= "-*-courier-bold-r-*-*-24-*-*-*-*-*-iso8859-1";
    size= 0;
    style= 0;
#elif defined(__BUILD_FOR_WINXP)
    fontname= "Times New Roman";
    size= 24;
    style= 0;
#endif

    bool fontLoaded= win->loadFont(fontname, size, style);

    if ( ! fontLoaded)
    {
        NOTIFY("Could not load font \"%s\", trying to continue\n", fontname);
    }

    sleep(1);

    for (int i= 1; i < 20; i++)
    {
        win->clear();

        for (int c= 0; c < numColors; c++)
        {
            win->setFgColor(colors[c]);
            win->drawString(160, 120, "DRAW LINE TEXT", CENTER);

            win->drawLineInBG( 10,  10, 310,  10, i);
            win->drawLineInBG(310,  10, 310, 230, i);
            win->drawLineInBG(310, 230,  10, 230, i);
            win->drawLineInBG( 10, 230,  10,  10, i);
            sleep(1);
        }
    }

    dpyTask->destroyWindow(win);

    app - (task_ref)((abstractTask_t *)dpyTask);
}

