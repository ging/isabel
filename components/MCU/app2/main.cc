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
// $Id: main.cc 7973 2006-01-26 18:50:49Z sirvent $
//
/////////////////////////////////////////////////////////////////////////


#include "sessionManager.h"
#include "returnCode.h"

#define UPDATE_DAY   1
#define UPDATE_MONTH 2
#define UPDATE_YEAR  2009

#ifdef WIN32
#include "Excpt.h"
#endif

#include <vCodecs/codecs.h>


sessionManager_t *APP = NULL;

int
main(int argc, char *argv[])
{

    NOTIFY("/-----------------------------------\\\n"
           " - MCU - %d/%d/%d -\n"
           "\\-----------------------------------/\n\n",
           UPDATE_DAY,
           UPDATE_MONTH,
           UPDATE_YEAR
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

