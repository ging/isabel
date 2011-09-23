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
#include <icf2/notify.hh>
#include <string.h>
#include "mixerbuffer.hh"

// -----------------------------------------------------------------------------
// MixerBuffer::MixerBuffer
// 
// -----------------------------------------------------------------------------
//
MixerBuffer::MixerBuffer(void)
{
    minOffset = 0;
    memset(buffer, 0, BUFFER_SIZE);
}

// -----------------------------------------------------------------------------
// MixerBuffer::~MixerBuffer
// 
// -----------------------------------------------------------------------------
//
MixerBuffer::~MixerBuffer()
{
}

// -----------------------------------------------------------------------------
// MixerBuffer::getMinOffset
// devuelve el puntero de lectura + un retraso
// -----------------------------------------------------------------------------
//
int
MixerBuffer::getMinOffset() {
    return minOffset + delay;
}

// -----------------------------------------------------------------------------
// MixerBuffer::isBetweenPointers
// Copia los datos en la posicion que se le diga.
// -----------------------------------------------------------------------------
//
bool
MixerBuffer::isBetweenPointers(unsigned int down, unsigned int up, unsigned int p) {
    if (down < up) { // down y up en el mismo ciclo
        if (down <= p && p <= up) {
            return true;
        }
    } // up esta en el siguiente ciclo de down
    else {
        if (p <= up || p >= down) {
            return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// MixerBuffer::write
// Copia los datos en la posicion que se le diga.
// -----------------------------------------------------------------------------
//
int
MixerBuffer::write(int offset, const unsigned char* pBuffer, int length, bool force)
{

    unsigned int realOffset = offset%BUFFER_SIZE;

    //NOTIFY("--> write min=%d real=%d \n", minOffset, realOffset);
    // Esta offset entre minOffset y el máximo buffer tolerado (MAX_BUFFER)?
    unsigned int down = minOffset;
    unsigned int up   = (minOffset+BUFFER_SIZE/2)%BUFFER_SIZE;
    if (!isBetweenPointers(down, up, realOffset)) {

        //NOTIFY("xxx MB Desincronizado min=%d real=%d \n", minOffset, realOffset);
		if (!force) {
			NOTIFY("Paquete descartado por MixerBuffer\n");
			return -1;
		}
        realOffset = (minOffset + delay)%BUFFER_SIZE;
    }

    int wrapAround = realOffset + length - BUFFER_SIZE;

    if (wrapAround <= 0) {
        //memcpy(buffer + realOffset, pBuffer, length);
        mix(buffer + realOffset, pBuffer, length);
        return (realOffset + length);
    }

    else {
        //memcpy(buffer + realOffset, pBuffer, length - wrapAround);
        //memcpy(buffer, pBuffer + length - wrapAround, wrapAround);

        mix(buffer + realOffset, pBuffer, length - wrapAround);
        mix(buffer, pBuffer + length - wrapAround, wrapAround);

        return wrapAround;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::mix
// Supone audio de 16 bits
// -----------------------------------------------------------------------------
//
void
MixerBuffer::mix(void* dest, const void* source, size_t length)
{
    short* des = (short*)dest;
    const short* sou = (const short*)source;

    size_t len = length/2;

	int temp = 0;

    for (size_t i = 0; i < len; i++) {
		temp = des[i] + sou[i];
		if (temp > 32767)         // 32767 = MAX_SIGNED_SHORT
			des[i] = 32767;
		else if (temp < -32767)
			des[i] = -32767;
		else
			des[i] = (short int)temp;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::heartBeat
// 
// -----------------------------------------------------------------------------
//
int
MixerBuffer::read(unsigned char* pBuffer,int length) {
    static int beats = 0;

    // Escribo al dispositivo el siguiente grupo de datos
    // y pongo la parte que he escrito a cero
    int wrapAround = minOffset + length - BUFFER_SIZE;


    if (wrapAround <= 0) { // Esta todo seguido
		writeAndClean(pBuffer,buffer + minOffset, length);
        minOffset = minOffset + length;
    }
    else { // parte al final y parte al principio
        writeAndClean(pBuffer,buffer + minOffset, length - wrapAround);
        writeAndClean(pBuffer + length - wrapAround,buffer, wrapAround);
        minOffset = wrapAround;
    }

    // Si el offset queda al final lo pongo al principio
    // para que no se envie un write de cero bytes
    // en el siguiente ciclo.
    if (minOffset == BUFFER_SIZE)
        minOffset = 0;

    return length;
}

// -----------------------------------------------------------------------------
// MixerBuffer::writeBuffer
// 
// -----------------------------------------------------------------------------
//
void MixerBuffer::writeAndClean(unsigned char * obuf, unsigned char* buf, int length) {
    memcpy(obuf,buf,length);
    memset(buf, 0x00 , length);
}

