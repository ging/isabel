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
// $Id: escritor.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __h263_escritor_hh__
#define __h263_escritor_hh__

#ifdef __SYMBIAN32__
#include "../../../include/vCodecs/general.h"
#else
#include <vCodecs/general.h>
#endif

class Escritor {
private:
    u8 *pict_array;
    unsigned int posicion;
    unsigned int quedan;

public:
    void reset (u8 *buf);
    void escribe_bits (int entrada, int n_bits);
    void alinea (void);
    unsigned int get_nBytes (void);
    unsigned int get_quedan (void);
};

#endif
