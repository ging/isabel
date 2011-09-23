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
// $Id: RTPRandom.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <stdlib.h>

#include <rtp/RTPRandom.hh>

RTPRandom_t::RTPRandom_t(void)
{
    numcalls = 0;
}

RTPRandom_t::~RTPRandom_t (void)
{
}

u8
RTPRandom_t::randomByte (void)
{
    return ((u8)(int)((256.0*(double)rand())/((double)RAND_MAX+1.0)));
}

u32
RTPRandom_t::random32(void)
{
    unsigned int seed;
    u8 byte;
    u32 retval;
    retval = 0;
    if (numcalls == 0)
    {
        numcalls++;
        seed = time(NULL);
        seed *= numcalls;
        seed -= clock();
        seed += (unsigned int)this;
        srand(seed);
    }
    byte = randomByte();
    retval |= ((u32)byte);
    byte = randomByte();
    retval |= (((u32)byte)<<8);
    byte = randomByte();
    retval |= (((u32)byte)<<16);
    byte = randomByte();
    retval |= (((u32)byte)<<24);

    return (u32)(labs(retval));
}

u16
RTPRandom_t::random16(void)
{
    unsigned int seed;
    u8 byte;
    u16 retval;
    retval = 0;
    numcalls++;
    seed = time(NULL);
    seed += numcalls;
    seed += clock();
    srand(seed);
    byte = randomByte();
    retval |= ((u16)byte);
    byte = randomByte();
    retval |= (((u16)byte)<<8);

    return (u16)(abs(retval));
}
