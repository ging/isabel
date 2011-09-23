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
 * ctrlProtocol.hh
 *
 * Copyright (C) 1997 Dan Rodriksson ( djr@dit.upm.es )
 *
 * Interface definition file for scanner daemon control
 *
 */

#ifndef __CTRLPROTOCOL_H__
#define __CTRLPROTOCOL_H__


#include <icf/general.h>
#include <icf/item.h>
#include <icf/stdTask.h>


struct scanner_InterfaceDef
{
    int  interID;

    scanner_InterfaceDef(void) { interID= (int)this; };
   
    char *scanner_query_id(int, char **);

    char *scanner_scan(int, char **);
    char *scanner_size(int, char **);
    char *scanner_docsize( int, char **);
    char *scanner_format(int, char **);
    char *scanner_quality(int, char **);
    char *scanner_orientation(int, char **);
    char *scanner_autocrop(int, char **);
    char *scanner_contrast(int, char **);
    char *scanner_brightness(int, char **);
    char *scanner_checklast(int argc, char **argv);
    //
    // common stuf
    //
    char *scanner_nop(int, char **);
    char *scanner_bye(int, char **);
    char *scanner_quit(int, char **);
};


EXTERNAL binding_t<scanner_InterfaceDef> scanner_methodBinding[];

typedef interface_t
    <scanner_InterfaceDef, scanner_methodBinding> scanner_Interface;

#endif
