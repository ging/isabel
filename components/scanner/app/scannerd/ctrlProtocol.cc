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
/*
 * ctrlProtocol.cc
 *
 * Copyright (C) 1997 Dan Rodriksson ( djr@dit.upm.es )
 *
 * Interface definition file for scanner daemon control
 *
 */

#include <string.h>

#include <icf/general.h>
#include <icf/stdTask.h>

#include "ctrlProtocol.hh"
#include "scannerApp.hh"


binding_t<scanner_InterfaceDef> scanner_methodBinding[]=
{
  { "scanner_query_id",    &scanner_InterfaceDef::scanner_query_id    },
  { "scanner_scan",        &scanner_InterfaceDef::scanner_scan        },
  { "scanner_docsize",     &scanner_InterfaceDef::scanner_docsize     },
  { "scanner_size",        &scanner_InterfaceDef::scanner_size        },
  { "scanner_orientation", &scanner_InterfaceDef::scanner_orientation },
  { "scanner_autocrop",    &scanner_InterfaceDef::scanner_autocrop    },
  { "scanner_quality",     &scanner_InterfaceDef::scanner_quality     },
  { "scanner_format",      &scanner_InterfaceDef::scanner_format      },
  { "scanner_contrast",    &scanner_InterfaceDef::scanner_contrast    },
  { "scanner_brightness",  &scanner_InterfaceDef::scanner_brightness  },
  { "scanner_checklast",   &scanner_InterfaceDef::scanner_checklast   },

  //
  // common stuff
  //
  { "scanner_nop",         &scanner_InterfaceDef::scanner_nop         },
  { "scanner_bye",         &scanner_InterfaceDef::scanner_bye         },
  { "scanner_quit",        &scanner_InterfaceDef::scanner_quit        },
  { NULL, NULL }
};


#define APP ((scannerApp_t *)((scanner_Interface *)this)->get_owner())


char *
scanner_InterfaceDef::scanner_query_id(int argc, char **)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

    sprintf(retVal, "0x%x\n", interID);
    return retVal;
}



char *
scanner_InterfaceDef::scanner_scan(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->scan(argv[1])) {
      sprintf( retVal, "%s", "ERROR: cannot start scan\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}

char *
scanner_InterfaceDef::scanner_docsize(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 3);

  if (APP->theScanner->docsize(atoi(argv[1]),atoi(argv[2]))) {
      sprintf( retVal, "%s", "ERROR: cannot set doc size\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}

char *
scanner_InterfaceDef::scanner_size(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 3);

  if (APP->theScanner->size(atoi(argv[1]),atoi(argv[2]))) {
      sprintf( retVal, "%s", "ERROR: cannot set size\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}
char *
scanner_InterfaceDef::scanner_quality(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->quality(atoi(argv[1]))) {
      sprintf( retVal, "%s", "ERROR: cannot set quality\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}
char *
scanner_InterfaceDef::scanner_autocrop(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->autocrop(atoi(argv[1]))) {
      sprintf( retVal, "%s", "ERROR: cannot set autocrop\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}

char *
scanner_InterfaceDef::scanner_orientation(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->orientation(atoi(argv[1]))) {
      sprintf( retVal, "%s", "ERROR: cannot set orientation\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}
char *
scanner_InterfaceDef::scanner_contrast(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->contrast(atoi(argv[1]))) {
      sprintf( retVal, "%s", "ERROR: cannot set contrast\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}
char *
scanner_InterfaceDef::scanner_brightness(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->brightness(atoi(argv[1]))) {
      sprintf( retVal, "%s", "ERROR: cannot set brightness\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}
char *
scanner_InterfaceDef::scanner_format(int argc, char **argv)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 2);

  if (APP->theScanner->setformat(argv[1])) {
      sprintf( retVal, "%s", "ERROR: cannot set format\n");
  } else {
      sprintf( retVal, "%s", "OK\n");
  }
  return retVal;
}

char *
scanner_InterfaceDef::scanner_checklast(int argc, char **)
{
  static char retVal[512] = "OK\n";

  INTER_VRFY_ARGC(retVal, 1);

  switch( APP->theScanner->checklast()) {
      case -1:
          sprintf( retVal, "%s", "ERROR: cannot scan\n");
          break;
      case 0:
          sprintf( retVal, "%s", "OK\n");
          break;
      default:
          sprintf( retVal, "%s", "WAIT\n");
          break;
  }
  return retVal;
}
//
// old stuff
//
char *
scanner_InterfaceDef::scanner_nop(int argc, char **)
{
    static char retVal[512] = "OK\n";

    INTER_VRFY_ARGC(retVal, 1);

    return retVal;
}


char *
scanner_InterfaceDef::scanner_bye(int argc, char **)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

//    delete APP->theScanner;
//    delete (scanner_Interface *)this;
    return (char*)-1;
}


char *
scanner_InterfaceDef::scanner_quit(int, char **)
{
    delete APP->theScanner;
//    delete (scanner_Interface *)this;

    APP->shutdown();
//    exit(0);
    return (char *)-1;
}

