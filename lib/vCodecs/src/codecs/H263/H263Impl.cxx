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
// $Id: H263Impl.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "H263Impl.hh"
#include "config.hh"

#ifdef __SYMBIAN32__
#define MAXW 352
#define MAXH 288
#else
#define MAXW 1408
#define MAXH 1152
#endif

H263EncodeSession_t:: H263EncodeSession_t(void)
{
    img_cache= new yuvImage_t;

    img_cache->lum  = new unsigned char[MAXW*MAXH];
    img_cache->cromU= new unsigned char[MAXW*MAXH/4];
    img_cache->cromV= new unsigned char[MAXW*MAXH/4];

    w= h= 0;
    contador=MAX_INTER_SEGUIDAS;

    cont=suma=0;
};

H263EncodeSession_t::~H263EncodeSession_t(void)
{
    delete img_cache->lum;
    delete img_cache->cromU;
    delete img_cache->cromV;
    delete img_cache;
};

void H263EncodeSession_t::reset(unsigned w, unsigned h)
{
    assert( (w <= MAXW) && (h <= MAXH) && "H263Encode: too big");

    this->w= w;
    this->h= h;

    memset(img_cache->lum,    16, MAXW*MAXH);
    memset(img_cache->cromU, 128, MAXW*MAXH/4);
    memset(img_cache->cromV, 128, MAXW*MAXH/4);

    contador=MAX_INTER_SEGUIDAS;
};


H263DecodeSession_t::H263DecodeSession_t(void)
{
    img_cache= new yuvImage_t;

    img_cache->lum  = new u8[MAXW*MAXH];
    img_cache->cromU= new u8[MAXW*MAXH / 4];
    img_cache->cromV= new u8[MAXW*MAXH / 4];

    w= h= 0;
    contador=0;
};

H263DecodeSession_t:: ~H263DecodeSession_t(void)
{
    delete img_cache->lum;
    delete img_cache->cromU;
    delete img_cache->cromV;
    delete img_cache;
};

void H263DecodeSession_t::reset(unsigned w, unsigned h)
{
    assert( (w <= MAXW) && (h <= MAXH) && "H263Decode: too big");

    this->w= w;
    this->h= h;

    memset(img_cache->lum,    16, MAXW*MAXH);
    memset(img_cache->cromU, 128, MAXW*MAXH/4);
    memset(img_cache->cromV, 128, MAXW*MAXH/4);

    contador = 0;
};


H263EncodeSession_t *
H263CreateEncodeSession(void)
{
    return new H263EncodeSession_t;
}

H263DecodeSession_t *
H263CreateDecodeSession(void)
{
    return new H263DecodeSession_t;
}

void
H263DestroyEncodeSession(H263EncodeSession_t *s)
{
    delete s;
}

void
H263DestroyDecodeSession(H263DecodeSession_t *s)
{
    delete s;
}





