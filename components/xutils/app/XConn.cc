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
// $Id: XConn.cc 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>

#include <X11/Xutil.h>

#include <icf2/ql.hh>

#include "XConn.hh"

bool
strMatch(char * const s1, char * const s2, matchMode_e match)
{
    switch (match)
    {
    case PREFIX:    // "s1" matches with "s2*"
        {
            return s1 == strstr(s1, s2);
        }
    case SUFFIX:    // "s1" matches with "*s2"
        {
            char *ult= NULL;

            int len1= strlen(s1),
                len2= strlen(s2);

            int pos= len1 - len2;
            if (pos < 0)    // don't fit
                return false;

            ult= s1 + pos;

            return ult == strstr(ult, s2);
        }
    case EXACT:  // if EXACT or UNKNOWN, return usual equality
    default:
       break;
    }

    return strcmp(s1, s2) == 0;
}

// ATENCION!
// Este codigo esta copiado del winGrabber
// hay que sacarlo factor comun
// de momento lo COPIO aqui para ir acabando el componente
// pero esto es una mierda
//
Window
getWinIdByName(Display * dpy,
               Window top,
               char * const name,
               matchMode_e match)
{
    Window *children=NULL, dummy;
    unsigned int nchildren;
    Window w= None;
    char *window_name;

    // To be changed to XGetWMName
    if (XFetchName(dpy, top, &window_name))
    {
        bool res= strMatch(window_name, name, match);
        XFree(window_name);
        if (res)   // found a match
        {
            return top;
        }
    }

    if ( ! XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren))
    {
        return None;
    }

    for (unsigned i=0; i<nchildren; i++)
    {
        w = getWinIdByName(dpy, children[i], name, match);
        if (w != None)
        {
            break;
        }
    }

    if (children) XFree ((char *)children);

    return w;
}

typedef                  ql_t<Window>   intList_t;
typedef smartReference_t<ql_t<Window> > intList_ref;

intList_ref
getWinIdListByName(Display * dpy,
                   Window top,
                   char * const name,
                   matchMode_e match
                  )
{
    Window *children=NULL, dummy;
    unsigned int nchildren;
    char *window_name;
    intList_ref retVal= new intList_t;

    // To be changed to XGetWMName
    if (XFetchName(dpy, top, &window_name))
    {
        if (strMatch(window_name, name, match))
        {
            retVal->insert(top);
        }
        XFree(window_name);
    }

    if ( ! XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren))
    {
        return retVal;
    }

    for (unsigned i=0; i<nchildren; i++)
    {
        intList_ref sonList = getWinIdListByName(dpy, children[i], name, match);
        if (sonList->len() > 0)
        {
            intList_t *l= sonList;
            retVal->insert(*l);
        }
    }

    if (children) XFree((char *)children);

    return retVal;
}

char*
getWindowListByName(Display * dpy,
                    Window top,
                    char * const name,
                    matchMode_e match,
                    whichOne_e whichOne
                   )
{
    char *retVal= NULL;

    switch (whichOne)
    {
    case FIRST:
        {
            Window w= getWinIdByName(dpy, top, name, match);
            if (w != None)
            {
                retVal= (char*)malloc(10);
                sprintf(retVal, "%ld", (long)w);
            }
            break;
        }
    case ALL:
        {
            intList_ref il= getWinIdListByName(dpy, top, name, match);
            for (intList_t::iterator_t i= il->begin();
                 i != il->end();
                 i++
                )
            {
                Window w= static_cast<Window>(i);
                if (retVal == NULL)
                {
                    retVal= (char*)malloc(2048);
                    retVal[0]=0;
                    sprintf(retVal, "%ld", w);
                }
                else
                {
                    sprintf(retVal, "%s,%ld", retVal, w);
                }
            }
            break;
        }
    default:
        break;
    }

    return retVal;
}


Window
getWindowByProp(Display * dpy, Window top, char * const name, matchMode_e match)
{
    // TBP
fprintf(stderr, "To Be Provided\n");
    return None;
}


int XConn_t::numXConns= 1;

XConn_t::XConn_t(char const *ndpyName)
: id(numXConns++),
  dpyName(strdup(ndpyName))
{
    dpy= XOpenDisplay(dpyName);

    if (dpy)
    {
        screen= DefaultScreen(dpy);
        rootWindow= RootWindow(dpy, screen);
    }
}

XConn_t::~XConn_t(void)
{
    id= -1;    // paranoic
    if (dpy)
    {
        XCloseDisplay(dpy);
    }
    free (dpyName);
}

char*
XConn_t::getWinId (searchMode_e mode,
                   char *const name,
                   matchMode_e match,
                   whichOne_e whichOne
                  )
{
    switch(mode)
    {
    case TITLE:
        return getWindowListByName (dpy,
                                    rootWindow,
                                    name,
                                    match,
                                    whichOne
                                   );
    case PROPERTY:
        //return getWindowByProp (dpy, rootWindow, name, match, whichOne);
        return NULL;
    default:
        fprintf(stderr, "UNKOWN mode in getWinId [%d]\n", mode);
    }

    return NULL;
}


void
XConn_t::mapWindow(Window wId)
{
    XMapWindow (dpy, wId);

    XFlush(dpy);
}


void
XConn_t::unmapWindow(Window wId)
{
    XUnmapWindow (dpy, wId);

    XFlush(dpy);
}

void
XConn_t::resize(Window wId, unsigned w, unsigned h)
{
    if (wId == rootWindow)
        return;

    XSizeHints hints;

    XResizeWindow(dpy, wId, w, h);

    XSync(dpy, False);

    hints.flags= PSize|PMinSize|PMaxSize;
    hints.width                          =
        hints.min_width= hints.max_width = w ;
    hints.height= hints.min_height       =
        hints.max_height                 = h ;
    XSetWMNormalHints(dpy, wId, &hints);

    XSync(dpy, False);

    XResizeWindow(dpy, wId, w, h);

    XSync(dpy, False);
}

void
XConn_t::reposition(Window wId, int x, int y)
{
    XMoveWindow(dpy, wId, x, y);

    XSizeHints sizehints;

    sizehints.flags=
        USPosition | USSize  |
        PPosition  | PSize   ;
        sizehints.x           = x;
        sizehints.y           = y;

    XSetWMNormalHints(dpy, wId, &sizehints);

    XSync(dpy, False);
}


bool
XConn_t::setGeometry (Window wId, char const *geomStr)
{
    if (wId == rootWindow)
        return false;

    if (geomStr)
    {
        // Copiado de displayTask_t
        bool doReposition= false;
        bool doResize    = false;

        unsigned width;
        unsigned height;
        int      x;
        int      y;
        int res= XParseGeometry(geomStr, &x, &y, &width, &height);

        if(res & (XValue    |     YValue)) doReposition= true;
        if(res & (WidthValue|HeightValue)) doResize    = true;

        if (doResize) resize(wId, width, height);
        if (doReposition) reposition(wId, x, y);
    }

    XFlush(dpy);

    return true;
}

