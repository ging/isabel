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
// $Id: main.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include "sessionManager.h"
#include "returnCode.h"

#define UPDATE_DAY   3
#define UPDATE_MONTH 11
#define UPDATE_YEAR  2005

#ifdef WIN32
#include "Excpt.h"
#endif

#include <vCodecs/codecs.h>


sessionManager_t *APP = NULL;

int main(int argc, char *argv[])
{
    putenv("MCU_USE_ONLY_IPv4=1");

    NOTIFY("/-----------------------------------\\\n"
           " - MCU - %d/%d/%d -\n"
           " - IP protocol is %s -\n"
           "\\-----------------------------------/\n\n",
           UPDATE_DAY,
           UPDATE_MONTH,
           UPDATE_YEAR,
           getenv("MCU_USE_ONLY_IPv4")?"IPv4":"IPv6"
          );

    //--------------------------------
    // Initializes vCodecs lib
    // called only once in an
    // application program.
    //--------------------------------
    vCodecInit();

    APP = new sessionManager_t(argc,argv);
    APP->run();

    return 0;
}

