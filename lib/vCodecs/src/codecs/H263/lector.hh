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
// $Id: lector.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __lector_hh__
#define __lector_hh__

#ifdef __SYMBIAN32__
#include "../../../include/vCodecs/general.h"
#else
#include <vCodecs/general.h>
#endif

class Lector {
private:
    u8 *pict_array;
    unsigned int posicion;
    unsigned int quedan;

public:
    void reset (u8 *buf);
    unsigned int lee_bits (int n_bits);
    void devuelve_bits (int n_bits);
    int alinea (void);
};

#endif
