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
// $Id: item.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <icf2/general.h>
#include <icf2/item.hh>


int item_t::debugMask= 0;
int item_t::debugLevel= 0;
FILE * item_t::debugFile= stderr;

item_t::binding_t item_t::bindMask[]={
    {"dbg_Show_Always",     dbg_Show_Always},

    {"dbg_No_Msg",          dbg_No_Msg},

    {"dbg_K_Normal",        dbg_K_Normal},
    {"dbg_K_Verbose",       dbg_K_Verbose},
    {"dbg_K_Paranoic",      dbg_K_Paranoic},
    {"dbg_K_Internal",      dbg_K_Internal},

    {"dbg_Lib_Low",         dbg_Lib_Low},
    {"dbg_Lib_Info",        dbg_Lib_Info},
    {"dbg_Lib_Normal",      dbg_Lib_Normal},
    {"dbg_Lib_Verbose",     dbg_Lib_Verbose},
    {"dbg_Lib_Paranoic",    dbg_Lib_Paranoic},
    {"dbg_Lib_Internal",    dbg_Lib_Internal},

    {"dbg_App_Low",         dbg_App_Low},
    {"dbg_App_Info",        dbg_App_Info},
    {"dbg_App_Normal",      dbg_App_Normal},
    {"dbg_App_Verbose",     dbg_App_Verbose},
    {"dbg_App_Paranoic",    dbg_App_Paranoic},
    {"dbg_App_Internal",    dbg_App_Internal},

    {NULL,                  dbg_No_Msg}
};

item_t::debugMask_e
item_t::strToMask(char *s) {
    int n=0;
    while (bindMask[n].name) {
        if (strcmp(bindMask[n].name, s)==0)
            return bindMask[n].mask;
        n++;
    }
    return dbg_No_Msg;
}

void
item_t::setDebugLevel(int level)
{
    // Compatibilidad con lo anterior

    if (level == app_normal)
        debugMask = debugMask | dbg_App_Normal;

    else if (level <= app_verbose)
        debugMask = debugMask | dbg_App_Normal | dbg_App_Verbose;

    else if (level <= app_paranoic)
        debugMask = debugMask | dbg_App_Normal | dbg_App_Verbose |
                    dbg_App_Paranoic;

    else if (level <= k_normal)
        debugMask = debugMask | dbg_App_Normal | dbg_App_Verbose |
                    dbg_App_Paranoic | dbg_K_Normal;

    else if (level <= k_verbose)
        debugMask = debugMask | dbg_App_Normal | dbg_App_Verbose |
                    dbg_App_Paranoic | dbg_K_Normal | dbg_K_Verbose;

    else
        debugMask = debugMask | dbg_App_Normal | dbg_App_Verbose |
                    dbg_App_Paranoic | dbg_K_Normal | dbg_K_Verbose |
                    dbg_K_Paranoic;
}

void
item_t::_debugMsg(debugMask_e mask, char const *method,
                  char const *msg,  va_list ap)
{

    if (((debugMask & mask)==0) && (mask!=dbg_Show_Always))
        return;

    size_t len= strlen(className()) + strlen(method) + strlen(msg) + 64;
    char *b= new char [len];
    int aux = debugMask & (!dbg_App_Low & !dbg_App_Info & !dbg_App_Normal);
    if (aux)
    {
        sprintf(b, "[ %08lu@0x%08lx ] %s::%s:: %s\n",
                (unsigned long)itemId,
                (unsigned long)this,
                className(),
                method,
                msg
               );
    }
    else
    {
        sprintf(b, "%s::%s:: %s\n", className(), method, msg);
    }

    vfprintf(debugFile, b, ap);
    fflush(debugFile);

    delete [] b;
}


u32
registerItem(const item_t *)
{
    static u32 counter= 0;

    u32 retVal= counter++;

    return retVal;
}

