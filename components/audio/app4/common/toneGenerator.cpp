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

#ifdef WIN32
// PI is not part of C++, some compilers require
// the following define to activate some match constants
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <icf2/general.h>

#include "toneGenerator.h"

ToneGenerator_t::ToneGenerator_t(int frequency, int samplingRate)
: sampleRate(samplingRate),
  freq(frequency),
  nth(0)
{
}

ToneGenerator_t::~ToneGenerator_t(void)
{
}

int
ToneGenerator_t::read(unsigned char *dstBuff, int len)
{
    i16 *samples= (i16*)dstBuff;

    for (int i= len / 2; i > 0; i--)
    {
        i16 nthsample= 16000.0 * sin((2.0 * M_PI * nth * freq) / sampleRate);

        *samples= nthsample;

        samples++; nth++;
    }

    return len;
}

