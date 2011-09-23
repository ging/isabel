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
// $Id: parseCmdLine.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/parseCmdLine.hh>

optionDef_t::optionDef_t(const char *n, int id, const char *desc)
{
    if(n && n[0]) {
        if(n[0]== '@') {
           optName= strdup(n+1);
           optHasArg= 1;
        } else {
            optName= strdup(n);
            optHasArg= 0;
        }

        optId= id;
    } else {
        debugMsg(dbg_App_Normal, "optionDef_t", "Invalid parameter string\n");
    }

    if(desc)
        optDesc= strdup(desc);
    else
        optDesc= NULL;
}

optionDef_t::~optionDef_t(void)
{
    if(optName)
        free(optName);

    if(optDesc)
        free(optDesc);
}



appParam_t::appParam_t(int id, const char *v)
{
    parId= id;

    parValue= NULL;
    if(v)
        parValue= strdup(v);
}

appParam_t::~appParam_t(void)
{
    if(parValue)
        free(parValue);
}



appParamList_t *
getOpt(optionDefList_t &optList, int &argc, argv_t &argv)
{
//  int skip;

    appParamList_t  *parList= new appParamList_t;

    if(argc< 0) return parList;

    for(unsigned i= 0; i< unsigned(argc); i++) {
        if(argv[i][0]== '-') {
//          skip= 0;

            optionDefList_t::iterator_t j;

            for(j= optList.begin(); j != optList.end(); j++) {

                optionDef_t* o= static_cast<optionDef_t*>(j);

                if(strcmp(argv[i]+1, o->optName)== 0) {
                    if(o->optHasArg && (i+1< unsigned(argc))) {
                        *parList<< new appParam_t(o->optId, argv[i+1]);
//                      skip= 1;
                    } else if(!o->optHasArg) {
                        *parList<< new appParam_t(o->optId, NULL);
                    }
                }
            }
        }
    }

    return parList;
}
