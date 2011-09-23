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
// $Id: ctrlProtocol.cc 10646 2007-08-24 12:42:18Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2007. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <string.h>

#include <icf2/general.h>
#include <icf2/stdTask.hh>

#include "transApp.hh"
#include "linkProtocol.hh"
#include "ctrlProtocol.hh"
#include "flow.hh"
#include "link.hh"

#define MY_INTER_VRFY_ARGC(b,x,msg)      \
    if(argc!= (x)) {                     \
        sprintf(b, "ERROR= %s", msg);    \
        return b;                        \
    }

#define MY_INTER_VRFY_ARGC_range(b,x,y, msg)          \
    if((argc< (x)) || (argc> (y))) {                  \
        sprintf(b, "ERROR= %s", msg);                 \
        return b;                                     \
    }

binding_t<webirouter_InterfaceDef> webirouter_methodBinding[]=
{
    { "ir7_query_id",        &webirouter_InterfaceDef::ir7_query_id },
    { "ir7_help",            &webirouter_InterfaceDef::ir7_help },
    { "help",                &webirouter_InterfaceDef::ir7_help },

    //
    // meta/santi
    //
    { "ir7_x_set_bw"    ,  &webirouter_InterfaceDef::ir7_x_set_bw     },
    { "ir7_version",       &webirouter_InterfaceDef::ir7_version },

    //
    // common stuff
    //
    { "ir7_nop",  &webirouter_InterfaceDef::ir7_nop   },
    { "ir7_bye",  &webirouter_InterfaceDef::ir7_bye   },
    { "ir7_quit", &webirouter_InterfaceDef::ir7_quit  },
    { NULL, NULL }
};


#define APP (static_cast<transApp_t *>(this->get_owner()))

char const *
webirouter_Interface::ir7_help(int argc, char **)
{
    static char retVal[1024];
    char *pointer=retVal;
    int i=0;
    printf("Control Commands Help:\n");
    while (webirouter_methodBinding[i].name) {
        strcpy(pointer, webirouter_methodBinding[i].name);
        pointer += strlen(webirouter_methodBinding[i].name);
        strcpy(pointer, "\n");
        pointer+=strlen("\n");
        i++;
    }
    return retVal;
}



char const *
webirouter_Interface::ir7_version(int argc, char **)
{
    return "Irouter&ICF2\n";
}

char const *
webirouter_Interface::ir7_query_id(int argc, char **)
{
    static char retVal[512];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_query_id()\n");

    sprintf(retVal, "0x%x\n", interID);
    return retVal;
}


//
// meta/santi
//
char const *
webirouter_Interface::ir7_x_set_bw(int argc, char **argv)
{
    static char retVal[512];

    MY_INTER_VRFY_ARGC(retVal, 3, "ir7_x_set_bw(linkName, bw)\n");

    link_t *l= APP->linkBinder->lookUp(argv[1]);

    if( ! l)
    {
        if(strcmp(argv[1], "default")== 0)
        {
            return "ERROR: no such link, (root of dist tree)\n";
        }
        else
        {
            return "ERROR: no such link\n";
        }
    }

    //l->adjustBandWidth(atoi(argv[2]));
    if (theHarbinger)
    {
        theHarbinger->adjustBandWidth(atoi(argv[2]));
    }

    return "OK\n";
}

//
// old stuff
//
char const *
webirouter_Interface::ir7_nop(int argc, char **)
{
    static char retVal[512];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_nop()\n");

    return "OK\n";
}


char const *
webirouter_Interface::ir7_bye(int argc, char **)
{
    static char retVal[512];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_bye()\n");

//    get_owner()->removeTask(this);
    return (char*)-1;
}


char const *
webirouter_Interface::ir7_quit(int argc, char **)
{
    static char retVal[512];

    MY_INTER_VRFY_ARGC(retVal, 1, "ir7_quit()\n");

    get_owner()->shutdown();
    return (char*)-1;
}

