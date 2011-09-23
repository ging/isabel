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

#ifndef __TONE_GENERATOR_H__
#define __TONE_GENERATOR_H__

class ToneGenerator_t
{
private:
    int sampleRate;  // sampling rate
    int freq;        // tone frequency

    int nth; // nth sample

public:

    ToneGenerator_t(int frequency, int samplingRate);

    ~ToneGenerator_t(void);

    /*
    * 'len' samples of simple tone are COPIED into 'dstBuf'
    * 'dstBuf' content, if any, is overriden
    * No boundaries checking is done, 
    */
    int read(unsigned char *dstBuf, int len);
};

#endif
