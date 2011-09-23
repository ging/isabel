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
/////////////////////////////////////////////////////////////////////////
//
// $Id: codec.h 22242 2011-04-08 12:41:52Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __acodec_codec_h__
#define __acodec_codec_h__


struct aCoder_t
{
    void *state;        // Se usa como quiere el codec
    int   format;       // compression format
    int   sampleRate;   // samples per second
    int   mcs_capture;  // milliseconds (MUST be calculated)
    int   bandwidth;    // bandwidth of codec (MUST be calculated)
    float ratio;        // bytes salida codificador / bytes entrada
    int   invRatio;
    int   bytesPerSample;
    int   minSamples; // numero de muestras que necesita como minimo el
                    // codificador (si ponemos mas deben ser multiplos de este)

    int (*Delete)(void *context);
    int (*Encode)(void *context,
                  unsigned char *inBuff,
                  int nSamples,
                  unsigned char *outBuff
                 );
};

struct aDecoder_t
{
    void *state;        // Se usa como quiere el codec
    int   format;       // compression format
    int   sampleRate;   // samples per second, sampling rate
    int   bytesPerSample;

    int (*Delete)(void *context);
    int (*Decode)(void *context,
                  unsigned char *inBuff,
                  int nBytes,
                  unsigned char *outBuff
                 );
};

#endif
