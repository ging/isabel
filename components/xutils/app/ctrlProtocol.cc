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
// $Id: ctrlProtocol.cc 20653 2010-06-23 13:20:32Z gabriel $
//
////////////////////////////////////////////////////////////////////////

#include <map>

#include <stdlib.h>

#include "ctrlProtocol.hh"
#include "XUtilsApp.hh"

const char *ERROR = "ERROR\n";
const char *OK    = "OK\n";

binding_t<xutils_InterfaceDef_t> xutils_methodBinding[]=
{

    { "xutils_query_id",      &xutils_InterfaceDef_t::xutils_query_id      },

    { "xutils_open_display",  &xutils_InterfaceDef_t::xutils_open_display  },
    { "xutils_close_display", &xutils_InterfaceDef_t::xutils_close_display },

    { "xutils_get_winid",     &xutils_InterfaceDef_t::xutils_get_winid     },

    { "xutils_map_window",    &xutils_InterfaceDef_t::xutils_map_window    },
    { "xutils_unmap_window",  &xutils_InterfaceDef_t::xutils_unmap_window  },

    { "xutils_set_geometry",  &xutils_InterfaceDef_t::xutils_set_geometry  },

    { "xutils_nop",           &xutils_InterfaceDef_t::xutils_nop           },
    { "xutils_quit",          &xutils_InterfaceDef_t::xutils_quit          },
    { "xutils_bye",           &xutils_InterfaceDef_t::xutils_bye           },

    { NULL, NULL }

};

#define APP     \
        static_cast<xutilsApp_t*>(      \
                static_cast<xutils_Interface_t *>(this)->get_owner()    \
        )

/*********************************************/


char const *
xutils_InterfaceDef_t::xutils_query_id (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 1);

    sprintf(retVal, "0x%x\n", interID);
    return retVal;
}


/*********************************************/


char const *
xutils_InterfaceDef_t::xutils_open_display (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 2);

    char const *dpyName= argv[1];

    XConn_t *xc= new XConn_t(dpyName);

    if ( ! xc->connected())
    {
        sprintf(retVal, " ERROR: unable to open dpy=[%s]", dpyName);
        return retVal;
    }

    APP->theXConns.insert(std::make_pair(xc->getId(), xc));

    sprintf(retVal, "%d\n", xc->getId());

    return retVal;
}

char const *
xutils_InterfaceDef_t::xutils_close_display (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 2);

    int dpyId= atoi(argv[1]);

    std::map<int, XConn_t *>::iterator iter= APP->theXConns.find(dpyId);

    if (iter == APP->theXConns.end()) // not found
    {
        return ERROR;
    }

    APP->theXConns.erase(dpyId);

    return OK;
}


/*********************************************/


//
// "xutils_get_winid(dpy,MODE,NAME,PARTIALMATCH,WHICH)"
// with:
//      dpyId= identifier of an opened display
//      MODE= {"TITLE", "PROPERTY"}
//      NAME a char* to search for
//      PARTIALMATCH= {"REGEXP", "PREFIX", "SUFFIX", "EXACT"}
//          if "PREFIX" searchs for NAME*
//          if "SUFFIX" searchs for *NAME
//          if "EXACT" searchs for exact match of NAME
//
//          if "REXEXP" will allow wildcards in NAME.
//                (TBP. Will substitute other options.)
//
//      WHICH which winId is returned
//          if "FIRST" returns the first winId that matches
//          if "ALL" returns all winIds that match
//
char const *
xutils_InterfaceDef_t::xutils_get_winid (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 6);

    int   dpyId = atoi(argv[1]);
    char *mode  = argv[2];
    char *name  = argv[3];
    char *match = argv[4];
    char *which = argv[5];

    searchMode_e searchMode;
    matchMode_e  matchMode;
    whichOne_e   whichOne;

    if (strcmp(mode, "TITLE") == 0)
        searchMode= TITLE;
    else if (strcmp(mode, "PROPERTY") == 0)
        searchMode= PROPERTY;
    else
        return ERROR;

    if (strcmp(match, "PREFIX") == 0)
        matchMode= PREFIX;
    else if (strcmp(match, "SUFFIX") == 0)
        matchMode= SUFFIX;
    else if (strcmp(match, "EXACT")  == 0)
        matchMode= EXACT;
    else
        return ERROR;

    if (strcmp(which, "FIRST") == 0)
        whichOne= FIRST;
    else if (strcmp(which, "ALL") == 0)
        whichOne= ALL;
    else
        return ERROR;

    std::map<int, XConn_t*>::iterator iter= APP->theXConns.find(dpyId);

    if ( iter == APP->theXConns.end()) // not found!
    {
        return ERROR;
    }

    XConn_t *xc= iter->second;

    char* winIdList= xc->getWinId(searchMode, name, matchMode, whichOne);

    if (winIdList == NULL)
    {
        return ERROR;
    }

    if (strlen(winIdList) > 0)
    {
        sprintf(retVal, "%s\n", winIdList);
        free(winIdList);
        return retVal;
    }

    return ERROR;
}


/*********************************************/

char const *
xutils_InterfaceDef_t::xutils_map_window (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 3);

    int dpyId  = atoi(argv[1]);
    int wId    = atoi(argv[2]);

    std::map<int, XConn_t *>::iterator iter= APP->theXConns.find(dpyId);

    if (iter == APP->theXConns.end()) // not found!
    {
        return ERROR;
    }

    XConn_t *xc= iter->second;

    xc->mapWindow(wId);

    return OK;
}

char const *
xutils_InterfaceDef_t::xutils_unmap_window (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 3);

    int dpyId  = atoi(argv[1]);
    int wId    = atoi(argv[2]);

    std::map<int, XConn_t *>::iterator iter= APP->theXConns.find(dpyId);

    if (iter == APP->theXConns.end()) // not found!
    {
        return ERROR;
    }

    XConn_t *xc= iter->second;

    xc->unmapWindow(wId);

    return OK;
}


char const *
xutils_InterfaceDef_t::xutils_set_geometry (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 4);

    int   dpyId  = atoi(argv[1]);
    int   wId    = atoi(argv[2]);
    char *geomStr= argv[3];

    std::map<int, XConn_t *>::iterator iter= APP->theXConns.find(dpyId);

    if (iter == APP->theXConns.end()) // not found!
    {
        return ERROR;
    }

    XConn_t *xc= iter->second;

    if (xc->setGeometry(wId, geomStr))
    {
        return OK;
    }

    return ERROR;
}


/*********************************************/


char const *
xutils_InterfaceDef_t::xutils_nop (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 1);

    return OK;
}


char const *
xutils_InterfaceDef_t::xutils_bye (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 1);

    (*APP)-(static_cast<xutils_Interface_t *>(this));
    return (char const *)-1;
}


char const *
xutils_InterfaceDef_t::xutils_quit (int argc, char **argv)
{
    static char retVal[2048];

    INTER_VRFY_ARGC(retVal, 1);

    APP->shutdown();
    return (char const *)-1;
}

