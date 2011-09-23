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
// $Id: random32.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "random32.hh"

u32
random32(void)
{
    u8 byte;
    u32 retval= 0;

    byte = 1+(int)(255*rand()/(RAND_MAX+1.0));
    retval |= ((u32)byte);
    byte = 1+(int)(255*rand()/(RAND_MAX+1.0));
    retval |= (((u32)byte)<<8);
    byte = 1+(int)(255*rand()/(RAND_MAX+1.0));
    retval |= (((u32)byte)<<16);
    byte = 1+(int)(255*rand()/(RAND_MAX+1.0));
    retval |= (((u32)byte)<<24);

    return (u32)(labs(retval));
}

