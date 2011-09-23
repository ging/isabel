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
// $Id: lector.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "lector.hh"


/*********************************************************
  Inicializa el escritor, y lo asocia con el buffer 'buf'
**********************************************************/
void
Lector::reset (u8 *buf)
{
    pict_array=buf;

    posicion=0;
    quedan=8;
}


/**************************************************
  Devuelve en un int (max 32 bits) alineados por la
derecha n_bits del stream.

   Va leyendo en un buffer de 8 bits del pict_array.
   Lleva un puntero de los bits que quedan por leer
del buffer y otro de la posicion en el pict_array.
****************************************************/
unsigned int
Lector::lee_bits (int n_bits)
{
    unsigned int salida=0;
    u8 buffer;
    int leidos=8-quedan;

    buffer = pict_array[posicion];
    // Rotamos buffer el n. de bits que hemos leido de este octeto
    buffer <<= (leidos);

    while (n_bits > 0) {
        if (quedan == 0) {
            posicion++;
            buffer = pict_array[posicion];
            quedan=8;
        }

        salida <<= 1;
        if (buffer & 0x80) {
            salida |= 1;  // ponemos a 1 bit
        }
        buffer <<= 1;
        n_bits--;
        quedan--;
    }

    return salida;
}

/**************************************************
   Retrocede n_bits en la cadena de bits. Para ello
actualiza los valores de posicion y quedan
***************************************************/
void
Lector::devuelve_bits (int n_bits)
{
    int leidos=8-quedan;

    if (n_bits <= leidos) {
        quedan += n_bits;
    } else {
        n_bits -= leidos;
        posicion--;
        quedan=0;

        while (n_bits>8) {
            posicion--;
            n_bits -= 8;
        }
        quedan += n_bits;
    }
}


/**********************************************
   Avanza el cursor de forma que lo siguiente
que se lea este byte-aligned
   Devuelve el numeros de bits alineados
**********************************************/
int
Lector::alinea (void)
{
    int temp;

    if ( (quedan>0) && (quedan<8) ) {
        temp=quedan;
        quedan=0;
        return temp;
    } else {
        return 0;
    }
}
