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
// $Id: H263Impl.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

/****************************************************************************

   Descripcion:
          Definicion de las clases sesion para cod. y dec.
          Definicion de las imagenes yuv y H263

****************************************************************************/

#ifndef __H263_implementation__hh__
#define __H263_implementation__hh__

#include <stdlib.h>

#ifdef __SYMBIAN32__
#include "../../include/vCodecs/general.h"
#else
#include <vCodecs/general.h>
#endif

class H263EncodedImage_t
{
public :
    u8           *buffer;
    unsigned      numBytes;
    unsigned      w;
    unsigned      h;
    unsigned int  nGOBs;
    unsigned int  nMBs;
    unsigned int  nMBTotal;
    unsigned      GOBsPopped;
    unsigned      MBsPopped;
    u8            source;
    u8            inter;
    u8            quant;
    u8            mode;
    unsigned int *GOBp;
    unsigned int *GOBq;
    unsigned int *MBp;
    unsigned int *MBq;

    H263EncodedImage_t(void) {};

    ~H263EncodedImage_t(void) {
        delete buffer; buffer= NULL;
        delete GOBp;  GOBp= NULL;
        delete GOBq;  GOBq= NULL;
        delete MBp;   MBp= NULL;
        delete MBq;   MBq= NULL;
    }
};

class yuvImage_t
{
public:
    u8       *lum;
    u8       *cromU;
    u8       *cromV;
    unsigned  w;
    unsigned  h;

    yuvImage_t(void)
    : lum(NULL),
      cromU(NULL),
      cromV(NULL)
    {}

    ~yuvImage_t(void)
    {
    }
};


class H263EncodeSession_t
{
public:
    unsigned contador;

    unsigned w;
    unsigned h;

    yuvImage_t *img_cache;

    H263EncodeSession_t(void);
    ~H263EncodeSession_t(void);

    void reset(unsigned w, unsigned h);

    unsigned cont, suma;
};


class H263DecodeSession_t
{
public:
    unsigned w;
    unsigned h;

    unsigned contador;

    yuvImage_t *img_cache;

    H263DecodeSession_t(void);
    ~H263DecodeSession_t(void);

    void reset(unsigned w, unsigned h);
};


H263EncodeSession_t *H263CreateEncodeSession(void);
H263DecodeSession_t *H263CreateDecodeSession(void);

void H263DestroyEncodeSession(H263EncodeSession_t *);
void H263DestroyDecodeSession(H263DecodeSession_t *);



#endif
