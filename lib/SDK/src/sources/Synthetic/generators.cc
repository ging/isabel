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
// $Id: generators.cc 10820 2007-09-17 09:20:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>

#include <icf2/general.h>

#include "image.hh"
#include "generators.hh"


//
// syntheticSrc_t implementation
//
syntheticSrc_t::syntheticSrc_t(void)
: posX(0), posY(0), signoX(5), signoY(5)
{
    memset(&__grabState, 0, sizeof(__grabState));

    __grabState.width = 320;    // width of scanned image
    __grabState.height= 240;    // height of scanned image
    __grabState.side= minimun(__grabState.width, __grabState.height) / 3;

    __grabState.grab_number= 0;

}

syntheticSrc_t::~syntheticSrc_t(void)
{
}

bool
syntheticSrc_t::setImageSize(unsigned width, unsigned height)
{
    // recalculates position to keep the same aspect relation
    posX= posX * width / __grabState.width;
    posY= posY * height / __grabState.height;

    // set new size
    __grabState.width = width;    // width of scanned image
    __grabState.height= height;   // height of scanned image

#if 0
fprintf(stderr, "grab pars <%dx%d>\n",
        __grabState.width,
        __grabState.height
       );
#endif

    // calculates side
    __grabState.side= minimun(__grabState.width, __grabState.height) / 3;

    // readjust positions if needed; should never happen
    if(posX+__grabState.side >= __grabState.width)
        posX= __grabState.width  - __grabState.side;
    if(posY+__grabState.side >= __grabState.height)
        posY= __grabState.height - __grabState.side;

    return true;
}

void
syntheticSrc_t::getImageSize(unsigned *width, unsigned *height)
{
    *width = __grabState.width;
    *height= __grabState.height;
}



//
// syntheticRGB16Src_t implementation
//    pass 'false' for RGB24, 'true' for BGR24
//
syntheticRGB16Src_t::syntheticRGB16Src_t(bool do555)
: syntheticSrc_t(), __do555(do555)
{
    mallocBuffers();
}

syntheticRGB16Src_t::~syntheticRGB16Src_t(void)
{
    freeBuffers();
}

bool
syntheticRGB16Src_t::setImageSize(unsigned width, unsigned height)
{
    bool res= ::syntheticSrc_t::setImageSize(width, height);

    reallocBuffers();

    return res;
}

u32
syntheticRGB16Src_t::getGrabFormat(void)
{
    return __do555 ? RGB555_FORMAT : RGB565_FORMAT;
}

image_t*
syntheticRGB16Src_t::genImage(u32 timestamp)
{
    image_t *img=
        genImageRGB16(__buffer[__grabState.grab_number],
                      __grabState.width,
                      __grabState.height,
                      posX,
                      posY,
                      __grabState.side,
                      signoX,
                      signoY,
                      __do555,
                      timestamp
                     );

    __grabState.grab_number= (__grabState.grab_number+1)%NUMBUFFS;

    return img;
}

void
syntheticRGB16Src_t::mallocBuffers(void)
{
    __maxBufSize= __grabState.width  * __grabState.height * 2;
    for(int nb= 0; nb < NUMBUFFS; nb++)
        __buffer[nb]= (u8*)malloc(__maxBufSize);
}

void
syntheticRGB16Src_t::reallocBuffers(void)
{
    // calculates new buffers size
    unsigned newSize= __grabState.width  * __grabState.height * 2;

    // realloc buffers if needed
    if(newSize > __maxBufSize) {
        for(int nb= 0; nb < NUMBUFFS; nb++) {
            if(__buffer[nb]) free(__buffer[nb]);
            __buffer[nb]= (u8*)malloc(newSize);
        }
        __maxBufSize= newSize;
    }
}

void
syntheticRGB16Src_t::freeBuffers(void)
{
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        if(__buffer[nb]) free(__buffer[nb]);
        __buffer[nb]= NULL;
    }
}

//
// syntheticRAW24Src_t implementation
//    pass 'false' for RGB24, 'true' for BGR24
//
syntheticRAW24Src_t::syntheticRAW24Src_t(bool doBGR)
: syntheticSrc_t(), __doBGR(doBGR)
{
    mallocBuffers();
}

syntheticRAW24Src_t::~syntheticRAW24Src_t(void)
{
    freeBuffers();
}

bool
syntheticRAW24Src_t::setImageSize(unsigned width, unsigned height)
{
    bool res= ::syntheticSrc_t::setImageSize(width, height);

    reallocBuffers();

    return res;
}

u32
syntheticRAW24Src_t::getGrabFormat(void)
{
    return __doBGR ? BGR24_FORMAT : RGB24_FORMAT;
}

image_t*
syntheticRAW24Src_t::genImage(u32 timestamp)
{
    image_t *img=
        genImageRAW24(__buffer[__grabState.grab_number],
                      __grabState.width,
                      __grabState.height,
                      posX,
                      posY,
                      __grabState.side,
                      signoX,
                      signoY,
                      __doBGR,
                      timestamp
                     );

    __grabState.grab_number= (__grabState.grab_number+1)%NUMBUFFS;

    return img;
}

void
syntheticRAW24Src_t::mallocBuffers(void)
{
    __maxBufSize= __grabState.width  * __grabState.height * 3;
    for(int nb= 0; nb < NUMBUFFS; nb++)
        __buffer[nb]= (u8*)malloc(__maxBufSize);
}

void
syntheticRAW24Src_t::reallocBuffers(void)
{
    // calculates new buffers size
    unsigned newSize= __grabState.width  * __grabState.height * 3;

    // realloc buffers if needed
    if(newSize > __maxBufSize) {
        for(int nb= 0; nb < NUMBUFFS; nb++) {
            if(__buffer[nb]) free(__buffer[nb]);
            __buffer[nb]= (u8*)malloc(newSize);
        }
        __maxBufSize= newSize;
    }
}

void
syntheticRAW24Src_t::freeBuffers(void)
{
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        if(__buffer[nb]) free(__buffer[nb]);
        __buffer[nb]= NULL;
    }
}

//
// syntheticYUV422iSrc_t implementation
//
syntheticYUV422iSrc_t::syntheticYUV422iSrc_t(void): syntheticSrc_t()
{
    mallocBuffers();
}

syntheticYUV422iSrc_t::~syntheticYUV422iSrc_t(void)
{
    freeBuffers();
}

bool
syntheticYUV422iSrc_t::setImageSize(unsigned width, unsigned height)
{
    width &= ~0x1; // even

    bool res= ::syntheticSrc_t::setImageSize(width, height);

    reallocBuffers();

    return res;
}

u32
syntheticYUV422iSrc_t::getGrabFormat(void)
{
    return I422i_FORMAT;
}

image_t*
syntheticYUV422iSrc_t::genImage(u32 timestamp)
{
    image_t *img=
        genImageYUV422i(__buffer[__grabState.grab_number],
                        __grabState.width,
                        __grabState.height,
                        posX,
                        posY,
                        __grabState.side,
                        signoX,
                        signoY,
                        timestamp
                       );

    __grabState.grab_number= (__grabState.grab_number+1)%NUMBUFFS;

    return img;
}

void
syntheticYUV422iSrc_t::mallocBuffers(void)
{
    __maxBufSize= __grabState.width  * __grabState.height * 2;
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        __buffer[nb]= (u8*)malloc(__maxBufSize);
    }
}

void
syntheticYUV422iSrc_t::reallocBuffers(void)
{
    // calculates new buffers size
    unsigned newSize= __grabState.width  * __grabState.height * 2;

    // realloc buffers if needed
    if(newSize > __maxBufSize) {
        for(int nb= 0; nb < NUMBUFFS; nb++) {
            if(__buffer[nb]) free(__buffer[nb]);
            __buffer[nb]= (u8*)malloc(newSize);
        }
        __maxBufSize= newSize;
    }
}

void
syntheticYUV422iSrc_t::freeBuffers(void)
{
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        if(__buffer[nb]) free(__buffer[nb]);
        __buffer[nb]= NULL;
    }
}

//
// syntheticYUV422PSrc_t implementation
//
syntheticYUV422PSrc_t::syntheticYUV422PSrc_t(void): syntheticSrc_t()
{
    mallocBuffers();
}

syntheticYUV422PSrc_t::~syntheticYUV422PSrc_t(void)
{
    freeBuffers();
}

bool
syntheticYUV422PSrc_t::setImageSize(unsigned width, unsigned height)
{
    width &= ~0x1; // even

    bool res= ::syntheticSrc_t::setImageSize(width, height);

    reallocBuffers();

    return res;
}

u32
syntheticYUV422PSrc_t::getGrabFormat(void)
{
    return I422P_FORMAT;
}

image_t*
syntheticYUV422PSrc_t::genImage(u32 timestamp)
{
    image_t *img=
        genImageYUV422P(__buffer[__grabState.grab_number],
                        __grabState.width,
                        __grabState.height,
                        posX,
                        posY,
                        __grabState.side,
                        signoX,
                        signoY,
                        timestamp
                       );

    __grabState.grab_number= (__grabState.grab_number+1)%NUMBUFFS;

    return img;
}

void
syntheticYUV422PSrc_t::mallocBuffers(void)
{
    __maxBufSize= __grabState.width  * __grabState.height * 2;
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        __buffer[nb]= (u8*)malloc(__maxBufSize);
    }
}

void
syntheticYUV422PSrc_t::reallocBuffers(void)
{
    // calculates new buffers size
    unsigned newSize= __grabState.width  * __grabState.height * 2;

    // realloc buffers if needed
    if(newSize > __maxBufSize) {
        for(int nb= 0; nb < NUMBUFFS; nb++) {
            if(__buffer[nb]) free(__buffer[nb]);
            __buffer[nb]= (u8*)malloc(newSize);
        }
        __maxBufSize= newSize;
    }
}

void
syntheticYUV422PSrc_t::freeBuffers(void)
{
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        if(__buffer[nb]) free(__buffer[nb]);
        __buffer[nb]= NULL;
    }
}

//
// syntheticYUV420PSrc_t implementation
//
syntheticYUV420PSrc_t::syntheticYUV420PSrc_t(void): syntheticSrc_t()
{
    mallocBuffers();
}

syntheticYUV420PSrc_t::~syntheticYUV420PSrc_t(void)
{
    freeBuffers();
}

bool
syntheticYUV420PSrc_t::setImageSize(unsigned width, unsigned height)
{
    width  &= ~0x1; // even
    height &= ~0x1; // even

    bool res= ::syntheticSrc_t::setImageSize(width, height);

    reallocBuffers();

    return res;
}

u32
syntheticYUV420PSrc_t::getGrabFormat(void)
{
    return I420P_FORMAT;
}

image_t*
syntheticYUV420PSrc_t::genImage(u32 timestamp)
{
    image_t *img=
        genImageYUV420P(__buffer[__grabState.grab_number],
                        __grabState.width,
                        __grabState.height,
                        posX,
                        posY,
                        __grabState.side,
                        signoX,
                        signoY,
                        timestamp
                       );

    __grabState.grab_number= (__grabState.grab_number+1)%NUMBUFFS;

    return img;
}

void
syntheticYUV420PSrc_t::mallocBuffers(void)
{
    __maxBufSize= __grabState.width  * __grabState.height * 3 / 2;
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        __buffer[nb]= (u8*)malloc(__maxBufSize);
    }
}

void
syntheticYUV420PSrc_t::reallocBuffers(void)
{
    // calculates new buffers size
    unsigned newSize= __grabState.width  * __grabState.height * 3 / 2;

    // realloc buffers if needed
    if(newSize > __maxBufSize) {
        for(int nb= 0; nb < NUMBUFFS; nb++) {
            if(__buffer[nb]) free(__buffer[nb]);
            __buffer[nb]= (u8*)malloc(newSize);
        }
        __maxBufSize= newSize;
    }
}

void
syntheticYUV420PSrc_t::freeBuffers(void)
{
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        if(__buffer[nb]) free(__buffer[nb]);
        __buffer[nb]= NULL;
    }
}

//
// syntheticYUV411PSrc_t implementation
//
syntheticYUV411PSrc_t::syntheticYUV411PSrc_t(void): syntheticSrc_t()
{
    mallocBuffers();
}

syntheticYUV411PSrc_t::~syntheticYUV411PSrc_t(void)
{
    freeBuffers();
}

bool
syntheticYUV411PSrc_t::setImageSize(unsigned width, unsigned height)
{
    width &= ~0x3; // divisible by 4

    bool res= ::syntheticSrc_t::setImageSize(width, height);

    reallocBuffers();

    return res;
}

u32
syntheticYUV411PSrc_t::getGrabFormat(void)
{
    return I411P_FORMAT;
}

image_t*
syntheticYUV411PSrc_t::genImage(u32 timestamp)
{
    image_t *img=
        genImageYUV411P(__buffer[__grabState.grab_number],
                        __grabState.width,
                        __grabState.height,
                        posX,
                        posY,
                        __grabState.side,
                        signoX,
                        signoY,
                        timestamp
                       );

    __grabState.grab_number= (__grabState.grab_number+1)%NUMBUFFS;

    return img;
}

void
syntheticYUV411PSrc_t::mallocBuffers(void)
{
    __maxBufSize= __grabState.width  * __grabState.height * 3 / 2;

    for(int nb= 0; nb < NUMBUFFS; nb++) {
        __buffer[nb]= (u8*)malloc(__maxBufSize);
    }
}

void
syntheticYUV411PSrc_t::reallocBuffers(void)
{
    // calculates new buffers size
    unsigned newSize= __grabState.width  * __grabState.height * 3 / 2;

    // realloc buffers if needed
    if(newSize > __maxBufSize) {
        for(int nb= 0; nb < NUMBUFFS; nb++) {
            if(__buffer[nb]) free(__buffer[nb]);
            __buffer[nb]= (u8*)malloc(newSize);
        }
        __maxBufSize= newSize;
    }
}

void
syntheticYUV411PSrc_t::freeBuffers(void)
{
    for(int nb= 0; nb < NUMBUFFS; nb++) {
        if(__buffer[nb]) free(__buffer[nb]);
        __buffer[nb]= NULL;
    }
}

