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
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <icf2/general.h>

class RingBuffer_t
{
private:
    const int buffSize;
    const int initPos;

    i16 *buffer;
    i16 *pin;
    i16 *pout;
    i16 *pend;

    inline i16 mixSamples(i16 s1, i16 s2);

public:

    /**
    * Build an audio ring buffer of 'size' samples, with initial
    * writing pos at 'initialPos' (thus, there is an starting
    * silence till buffer is filled up to 'initialPos'
    * Throw exception if memory is not allocated
    * Assert 'initialPos' is within the boundaries of ring buffer
    * @param size number of samples (16bits) to store
    * @param initialPos Input data will be written at this position
    */
    RingBuffer_t(int size, int initialPos);
    virtual ~RingBuffer_t(void);

    /**
    * Get audio data from the ring bffer and copy or mix them into samples
    * @param pBuffer direccion del buffer de salida.
    * @param length Longitud del buffer de salida.
    * @param mixed if audio samples are mixed or overriden
    * @return Numero de bytes leidos.
    */
    int Dequeue(i16* samples, int nsamples, bool mixed);

    /**
    * Put audio data into the ring buffer
    * @param pBuffer direccion del buffer de salida.
    * @param length Longitud del buffer de salida.
    * @return Numero de bytes leidos.
    */
    int Enqueue(i16* samples, int nsamples);
};

#endif
