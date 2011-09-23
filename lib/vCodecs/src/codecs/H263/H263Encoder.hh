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
// $Id: H263Encoder.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __h263_encoder_hh__
#define __h263_encoder_hh__

#include "H263Impl.hh"
#include "codec.hh"


#ifdef WIN32
#include <winsock.h>
#endif
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <sys/time.h>
#endif

class H263Encoder_t
{
private:
    //
    // sesion
    //
    H263EncodeSession_t *session;

    int   mode;
    int   width;
    int   height;
    int   bitRate;
    int   frameRate;
    float quality;

    Cod cod;

    // timeval to estimate BW
    timeval time1;
    timeval time2;

    // BW and Q estimated
    // per period when using CBR
    unsigned long  BW;
    short Q;
    // Desired BW
    unsigned long maxBW;
    // estimation interval
    double period;

public:
    H263Encoder_t(void);

    ~H263Encoder_t(void);

    int codifica(unsigned int   width,
                 unsigned int   height,
                 int            bitRate,
                 int            frameRate,
                 int            maxInter,
                 int            quality,
                 unsigned char *inBuff,
                 int            inBuffLen,
                 unsigned char *outBuff,
                 int            outBuffLen
                );
};


class H263Decoder_t
{
private:
    //
    // sesion
    //
    H263DecodeSession_t *session;

    int   lost;
    int   width;
    int   height;
    int   bitRate;
    int   frameRate;
    float quality;

    Dec dec;

public:
    H263Decoder_t(void);

    ~H263Decoder_t(void);

    int decodifica(unsigned int  &width,
                   unsigned int  &height,
                   unsigned char *inBuff,
                   int            inBuffLen,
                   unsigned char *outBuff,
                   int            outBuffLen
                  );
};

#endif

