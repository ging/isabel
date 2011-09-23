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
// $Id: escritor.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "escritor.hh"

//
// Inicializa el escritor, y lo asocia con el buffer 'buf'
//
void
Escritor::reset (u8 *buf)
{
    pict_array=buf;
    posicion=0;
    quedan=8;
}


//
// Introduce n_bits de un int (max 32 bits) en el buffer.
// Primero mete los mas significativos.
//
// Cada elemento del pict_array tiene 8 bits.
// Lleva un puntero de la posicion en el pict_array y otro
// de los bits que quedan libres por escribir en esa posicion.
//
void
Escritor::escribe_bits (int entrada, int n_bits)
{
    while (n_bits > 0) {
        if (quedan == 0) {
            posicion++;
            quedan=8;
        }

        if (entrada & (0x1 << n_bits-1)) {
            pict_array[posicion] |= (0x1 << quedan-1);  /* ponemos a 1 bit */
        } else {
            pict_array[posicion] &= ~(0x1 << quedan-1);  /* ponemos a 0 bit */
        }
        n_bits--;
        quedan--;
    }
}


//
// Avanza el cursor de forma que lo siguiente
// que se escriba este byte-aligned
//
void
Escritor::alinea (void)
{
    if ( (quedan>0) && (quedan<8) ) {
        quedan=0;
    }
}


//
// Devuelve el numero de bytes escritos.
//
unsigned int
Escritor::get_nBytes (void)
{
    return posicion+1;
}


unsigned int
Escritor::get_quedan (void)
{
    return quedan;
}

