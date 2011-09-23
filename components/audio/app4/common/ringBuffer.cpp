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

#include <assert.h>
#include <string.h>

#include <icf2/notify.hh>

#include "ringBuffer.h"

RingBuffer_t::RingBuffer_t(int size, int initialPos)
: buffSize(size),
  initPos(initialPos)
{
    assert(   (initialPos >= 0)
           && (initialPos < size)
           && "RingBuffer_t:: Invalid initial Pos"
          );

    buffer= new i16[size];
    if ( ! buffer )
    {
        NOTIFY("rintBuffer_t:: out of memory\n");
        throw "Out of memory";
    }

    pend = buffer + size;

    pout = buffer;
    pin  = buffer + initPos;
    memset(buffer, 0, buffSize);
}

RingBuffer_t::~RingBuffer_t(void)
{
    delete []buffer;
}

i16
RingBuffer_t::mixSamples(i16 s1, i16 s2)
{
    i32 sample= (i32)s1 + (i32)s2;

    if (sample > 0x7fff)
    {
        sample = 0x7fff;
    }
    else if (sample < -0x7fff)
    {
        sample = -0x7fff;
    }

    return (i16)sample;
}

int
RingBuffer_t::Dequeue(i16* samples, int nsamples, bool mixed)
{
    int n = 0;
    int remain = pend - pout;
    if (pout == pin) return 0;
    if (remain > nsamples)
    {
        for (int i= 0; i < nsamples; i++)
        {
            u16 sample= mixed ? mixSamples (*samples, *pout): *pout;
            *samples= sample;
            pout++; n++; samples++;
            if (pout == pin)
            {
                return n;
            }
        }
    }
    else
    {
        for (int i= 0; i < remain; i++)
        {
            u16 sample= mixed ? mixSamples (*samples, *pout): *pout;
            *samples= sample;
            nsamples--; pout++; n++; samples++;
            if (pout == pin)
            {
                return n;
            }
        }
        pout = buffer;
        n += Dequeue(samples, nsamples, mixed);
    }
    return n;
}

int
RingBuffer_t::Enqueue(i16* samples, int nsamples)
{
    int n = 0;
    {
        int remain = pend - pin;
        if (remain > nsamples)
        {
            for (int i= 0; i < nsamples; ++i)
            {
                *pin = *samples;
                n++; pin++; samples++;
                if (pin == pout)
                {
                    // calculate with respect nsamples?
                    pout= pin + 1 < pend ? pin + 1 : buffer;
                }
            }
        }
        else
        {
            for (int i= 0; i < remain; ++i)
            {
                *pin = *samples;
                nsamples--;
                n++; pin++; samples++;
                if (pin == pout)
                {
                    // calculate with respect nsamples?
                    pout= pin + 1 < pend ? pin + 1 : buffer;
                }
            }
            pin= buffer;
            n += Enqueue(samples, nsamples);
        }
    }
    return n;
}

