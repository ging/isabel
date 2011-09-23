/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: MPEG1imp.h 10492 2007-06-29 17:02:05Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __mpeg1_imp_h__
#define __mpeg1_imp_h__

#include <icf2/general.h>

// MPEG1 structs

struct MPEG1Header_t
{
    u16 word1;

    u8 P:3;
    u8 E:1;
    u8 B:1;
    u8 S:1;
    u8 N:1;
    u8 AN:1;

    u8 FFC:3;
    u8 FFV:1;
    u8 BFC:3;
    u8 FBV:1;
};

struct mpeg1FragData_t
{
    u16 temporal_reference;
    u8  picture_type;
    u8  FFC;
};

#endif

