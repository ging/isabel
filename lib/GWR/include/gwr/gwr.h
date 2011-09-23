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
// $Id: gwr.h 10215 2007-05-25 14:02:30Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr_types_hh__
#define __gwr_types_hh__

#include <icf2/general.h>

#define CLASS_STATICGRAY  0
#define CLASS_STATICCOLOR 2
#define CLASS_PSEUDOCOLOR 3
#define CLASS_TRUECOLOR   4

struct clutColor_t {
    u8 r;
    u8 g;
    u8 b;
};


#endif
