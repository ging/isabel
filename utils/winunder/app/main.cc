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
// $Id:$
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <icf2/ql.hh>

void
help(const char *progname)
{
    printf("usage: %s [-display dpyname] -winOver winId -winUnder winId\n",
           progname
          );
    printf("\twindow <winUnder> is restacked under <winOver>\n");
}

#define DISPLAY_OPTION  "-display"
#define WINOVER_OPTION  "-winOver"
#define WINUNDER_OPTION "-winUnder"

typedef                   ql_t<Window>    winIdList_t;
typedef smartReference_t< ql_t<Window> >  winIdList_ref;


winIdList_ref
getAncestors(Display *dpy, Window rootId, Window winId)
{
    Window dummy, parent;
    Window *children= NULL;
    unsigned int nchildren;

    if (winId == rootId) return NULL;

    if ( ! XQueryTree(dpy, winId, &dummy, &parent, &children, &nchildren) )
    {
        printf("XQueryTree devuelve error\n");
        return NULL;
    }
    if (children) { XFree((char *)children); children= NULL; }

    winIdList_ref retVal= getAncestors(dpy, rootId, parent);

    if ( ! retVal.isValid())
    {
        retVal= new winIdList_t();
    }

    retVal->insert(parent);

    return retVal;
}

void
printWinList(winIdList_ref wilr)
{
    for (winIdList_t::iterator_t i= wilr->begin(); i != wilr->end(); i++)
    {
        Window w= static_cast<Window>(i);

        printf("%ld ", w);
    }
    printf("\n");
}

int
main(int argc, char *argv[])
{
    const char *progname= argv[0];

    Window winOverID= 0, winUnderID= 0;

    for (int i= 1; i < argc; i++)
    {
        if (argv[i][0]== '-')
        {
            if (strcmp(argv[i], DISPLAY_OPTION)== 0)
            {
                i++;
                if (i < argc)
                {
                    char *b= (char *)malloc(2048);
                    sprintf(b, "DISPLAY=%s", argv[i]);

                    putenv(b);
                }
                else
                {
                    printf("Option `%s' requires a parameter\n",
                           DISPLAY_OPTION
                          );
                }
            }
            else if (strcmp(argv[i], WINOVER_OPTION)== 0)
            {
                i++;
                if (i < argc)
                {
                    winOverID= strtoul(argv[i], NULL, 10);
                }
                else
                {
                    printf("Option `%s' requires a parameter\n",
                           WINOVER_OPTION
                          );
                }
            }
            else if (strcmp(argv[i], WINUNDER_OPTION)== 0)
            {
                i++;
                if (i < argc)
                {
                    winUnderID= strtoul(argv[i], NULL, 10);
                }
                else
                {
                    printf("Option `%s' requires a parameter\n",
                           WINUNDER_OPTION
                          );
                }
            }
            else
            {
                printf("Unknown option: `%s'\n", argv[i]);
                help(progname);
                exit(1);
            }
        }
        else
        {
            help(progname);
            exit(1);
        }
    }

    if ( (winOverID == 0) || (winUnderID == 0) )
    {
        help(progname);
        exit(1);
    }

    Display *dpy= XOpenDisplay(NULL);

    if ( ! dpy )
    {
        printf("dtX11_t::dtX11_t: cannot open display %s, "
               "bailing out\n",
               getenv("DISPLAY")
              );

        exit(1);
    }

    // get older ancestors which are different (not root): ensures sibling
    Window rootID;

    rootID= DefaultRootWindow(dpy);
    printf("ROOT ID= %ld\n", rootID);

    winIdList_ref overAncestors, underAncestors;

    overAncestors = getAncestors(dpy, rootID, winOverID);
    overAncestors->insert(winOverID);
    underAncestors= getAncestors(dpy, rootID, winUnderID);
    underAncestors->insert(winUnderID);

    printf("Ancestros de over: ");
    printWinList(overAncestors);
    printf("Ancestros de under: ");
    printWinList(underAncestors);

    // now get siblings of latest common ancestor
    while (overAncestors->head() == underAncestors->head())
    {
        overAncestors->behead();
        underAncestors->behead();
    }

    winOverID= overAncestors->head();
    winUnderID= underAncestors->head();

    printf("Applying command to %ld %ld\n", winOverID, winUnderID);

    Window wins[2];

    wins[0]= winOverID;
    wins[1]= winUnderID;

    XRestackWindows(dpy, wins, 2);

    XSync(dpy, False);

    return 0; // keeps compiler happy
}

