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
// $Id: CellBImpl.cxx 10673 2007-08-30 10:54:34Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>

#ifdef WIN32
int
random(void)
{
    return rand();
}
#endif

#include "CellBImpl.hh"

//define VERBOSE for useful information on CellB coding/decoding
//#define VERBOSE

class cellbEncodeSession_t
{
public:
    unsigned w;
    unsigned h;

    u8 *lumaCache;
    i8 *timeCache;

    cellbEncodeSession_t(void)
    {
        lumaCache= new u8[65536];
        timeCache= new i8[65536];

        w= h= 0;
    }
    ~cellbEncodeSession_t(void)
    {
        delete[] lumaCache;
        delete[] timeCache;
    }

    void reset(unsigned w, unsigned h)
    {
        assert (w < 1024 && h < 1024); // cual es el tamano maximo?

        this->w= w;
        this->h= h;

        memset(lumaCache, 0, 65536);
        memset(timeCache, 0, 65536);
    };
};

class cellbDecodeSession_t
{
public:
    unsigned w;
    unsigned h;

    u8 *imgCache;

    cellbDecodeSession_t(void)
    {
        imgCache= new u8[3*1024*1024];

        w= h= 0;
    }
    ~cellbDecodeSession_t(void)
    {
        delete[] imgCache;
    }

    void reset(unsigned w, unsigned h)
    {
        assert (w < 1024 && h < 1024);

        this->w= w;
        this->h= h;

        memset(imgCache, 0, 3*1024*1024);
    };
};


cellbEncodeSession_t *
cellbCreateEncodeSession(void)
{
    return new cellbEncodeSession_t();
}

cellbDecodeSession_t *
cellbCreateDecodeSession(void)
{
    return new cellbDecodeSession_t;
}

void
cellbDestroyEncodeSession(cellbEncodeSession_t *s)
{
    delete s;
}

void
cellbDestroyDecodeSession(cellbDecodeSession_t *s)
{
    delete s;
}



//
// some table required
//
static struct {
    u8 y1;
    u8 y2;
} yyTable[256]=
{
    {  16,  20  }, {  16,  24  }, {  16,  32  }, {  16,  48  },
    {  16,  64  }, {  16,  80  }, {  16, 112  }, {  16, 144  },
    {  16, 176  }, {  16, 208  }, {  16, 240  }, {  20,  24  },
    {  24,  28  }, {  24,  32  }, {  24,  40  }, {  28,  32  },
    {  32,  36  }, {  32,  40  }, {  32,  48  }, {  32,  64  },
    {  32,  80  }, {  32,  96  }, {  36,  40  }, {  40,  44  },
    {  40,  48  }, {  40,  56  }, {  44,  48  }, {  48,  52  },
    {  48,  56  }, {  48,  64  }, {  48,  80  }, {  48,  96  },
    {  48, 112  }, {  48, 144  }, {  48, 176  }, {  48, 208  },
    {  48, 240  }, {  52,  56  }, {  56,  60  }, {  56,  64  },
    {  56,  72  }, {  60,  64  }, {  64,  68  }, {  64,  72  },
    {  64,  80  }, {  64,  96  }, {  64, 112  }, {  64, 128  },
    {  68, 128  }, {  72,  76  }, {  72,  80  }, {  72,  88  },
    {  76,  80  }, {  80,  84  }, {  80,  88  }, {  80,  96  },
    {  80, 112  }, {  80, 128  }, {  80, 144  }, {  80, 176  },
    {  80, 208  }, {  80, 240  }, {  84,  88  }, {  88,  92  },
    {  88,  96  }, {  88, 104  }, {  92,  96  }, {  96, 100  },
    {  96, 104  }, {  96, 112  }, {  96, 128  }, {  96, 144  },
    {  96, 160  }, { 100, 104  }, { 104, 108  }, { 104, 112  },
    { 104, 120  }, { 108, 112  }, { 112, 116  }, { 112, 120  },
    { 112, 128  }, { 112, 144  }, { 112, 160  }, { 112, 176  },
    { 112, 208  }, { 112, 240  }, { 120, 128  }, { 120, 136  },
    { 128, 136  }, { 128, 144  }, { 128, 160  }, { 128, 176  },
    { 128, 192  }, { 136, 144  }, { 136, 152  }, { 144, 152  },
    { 144, 160  }, { 144, 176  }, { 144, 192  }, { 144, 208  },
    { 144, 240  }, { 152, 160  }, { 152, 168  }, { 160, 168  },
    { 160, 176  }, { 160, 192  }, { 160, 208  }, { 160, 224  },
    { 168, 176  }, { 168, 184  }, { 176, 184  }, { 176, 192  },
    { 176, 208  }, { 176, 224  }, { 176, 240  }, { 184, 192  },
    { 184, 200  }, { 192, 200  }, { 192, 208  }, { 192, 224  },
    { 192, 240  }, { 200, 208  }, { 200, 216  }, { 208, 216  },
    { 208, 224  }, { 208, 240  }, { 216, 232  }, { 224, 240  },

    {  20 , 16  }, {  24 , 16  }, {  32 , 16  }, {  48 , 16  },
    {  64 , 16  }, {  80 , 16  }, { 112 , 16  }, { 144 , 16  },
    { 176 , 16  }, { 208 , 16  }, { 240 , 16  }, {  24 , 20  },
    {  28 , 24  }, {  32 , 24  }, {  40 , 24  }, {  32 , 28  },
    {  36 , 32  }, {  40 , 32  }, {  48 , 32  }, {  64 , 32  },
    {  80 , 32  }, {  96 , 32  }, {  40 , 36  }, {  44 , 40  },
    {  48 , 40  }, {  56 , 40  }, {  48 , 44  }, {  52 , 48  },
    {  56 , 48  }, {  64 , 48  }, {  80 , 48  }, {  96 , 48  },
    { 112 , 48  }, { 144 , 48  }, { 176 , 48  }, { 208 , 48  },
    { 240 , 48  }, {  56 , 52  }, {  60 , 56  }, {  64 , 56  },
    {  72 , 56  }, {  64 , 60  }, {  68 , 64  }, {  72 , 64  },
    {  80 , 64  }, {  96 , 64  }, { 112 , 64  }, { 128 , 64  },
    { 128 , 68  }, {  76 , 72  }, {  80 , 72  }, {  88 , 72  },
    {  80 , 76  }, {  84 , 80  }, {  88 , 80  }, {  96 , 80  },
    { 112 , 80  }, { 128 , 80  }, { 144 , 80  }, { 176 , 80  },
    { 208 , 80  }, { 240 , 80  }, {  88 , 84  }, {  92 , 88  },
    {  96 , 88  }, { 104 , 88  }, {  96 , 92  }, { 100 , 96  },
    { 104 , 96  }, { 112 , 96  }, { 128 , 96  }, { 144 , 96  },
    { 160 , 96  }, { 104 ,100  }, { 108 ,104  }, { 112 ,104  },
    { 120 ,104  }, { 112 ,108  }, { 116 ,112  }, { 120 ,112  },
    { 128 ,112  }, { 144 ,112  }, { 160 ,112  }, { 176 ,112  },
    { 208 ,112  }, { 240 ,112  }, { 128 ,120  }, { 136 ,120  },
    { 136 ,128  }, { 144 ,128  }, { 160 ,128  }, { 176 ,128  },
    { 192 ,128  }, { 144 ,136  }, { 152 ,136  }, { 152 ,144  },
    { 160 ,144  }, { 176 ,144  }, { 192 ,144  }, { 208 ,144  },
    { 240 ,144  }, { 160 ,152  }, { 168 ,152  }, { 168 ,160  },
    { 176 ,160  }, { 192 ,160  }, { 208 ,160  }, { 224 ,160  },
    { 176 ,168  }, { 184 ,168  }, { 184 ,176  }, { 192 ,176  },
    { 208 ,176  }, { 224 ,176  }, { 240 ,176  }, { 192 ,184  },
    { 200 ,184  }, { 200 ,192  }, { 208 ,192  }, { 224 ,192  },
    { 240 ,192  }, { 208 ,200  }, { 216 ,200  }, { 216 ,208  },
    { 224 ,208  }, { 240 ,208  }, { 232 ,216  }, { 240 ,224  }
};


static struct {
    u8 uu;
    u8 vv;
} uvTable[256]=
{
    {  16,  16  }, {  16,  48  }, {  16,  80  }, {  16, 112  },
    {  16, 144  }, {  16, 176  }, {  16, 208  }, {  16, 240  },
    {  48,  16  }, {  48,  48  }, {  48,  80  }, {  48, 112  },
    {  48, 144  }, {  48, 176  }, {  48, 208  }, {  48, 240  },
    {  64, 112  }, {  64, 128  }, {  64, 144  }, {  64, 160  },
    {  64, 176  }, {  80,  16  }, {  80,  48  }, {  80,  80  },
    {  80,  96  }, {  80, 112  }, {  80, 128  }, {  80, 144  },
    {  80, 160  }, {  80, 176  }, {  80, 192  }, {  80, 208  },
    {  80, 240  }, {  96,  80  }, {  96,  96  }, {  96, 112  },
    {  96, 128  }, {  96, 144  }, {  96, 160  }, {  96, 176  },
    {  96, 192  }, {  96, 208  }, { 104, 128  }, { 104, 136  },
    { 104, 133  }, { 104, 152  }, { 104, 160  }, { 112,  16  },
    { 112,  48  }, { 112,  64  }, { 112,  80  }, { 112,  96  },
    { 112, 112  }, { 112, 120  }, { 112, 128  }, { 112, 136  },
    { 112, 144  }, { 112, 152  }, { 112, 160  }, { 112, 168  },
    { 112, 176  }, { 112, 192  }, { 112, 208  }, { 112, 224  },
    { 112, 240  }, { 120, 112  }, { 120, 120  }, { 120, 128  },
    { 120, 136  }, { 120, 144  }, { 120, 152  }, { 120, 160  },
    { 120, 168  }, { 120, 176  }, { 128,  64  }, { 128,  80  },
    { 128,  96  }, { 128, 104  }, { 128, 112  }, { 128, 120  },
    { 128, 128  }, { 128, 136  }, { 128, 144  }, { 128, 152  },
    { 128, 160  }, { 128, 168  }, { 128, 176  }, { 128, 184  },
    { 128, 192  }, { 128, 208  }, { 128, 224  }, { 132, 136  },
    { 132, 140  }, { 132, 148  }, { 132, 152  }, { 132, 104  },
    { 136, 104  }, { 136, 112  }, { 136, 120  }, { 136, 128  },
    { 136, 132  }, { 136, 136  }, { 136, 140  }, { 136, 144  },
    { 136, 148  }, { 136, 152  }, { 136, 156  }, { 136, 160  },
    { 136, 168  }, { 136, 176  }, { 136, 184  }, { 140, 132  },
    { 140, 136  }, { 140, 140  }, { 140, 144  }, { 140, 148  },
    { 140, 152  }, { 140, 156  }, { 144,  16  }, { 144,  48  },
    { 144,  64  }, { 144,  80  }, { 144,  96  }, { 144, 104  },
    { 144, 112  }, { 144, 120  }, { 144, 128  }, { 144, 132  },
    { 144, 136  }, { 144, 140  }, { 144, 140  }, { 144, 148  },
    { 144, 152  }, { 144, 156  }, { 144, 160  }, { 144, 168  },
    { 144, 176  }, { 144, 184  }, { 144, 192  }, { 144, 208  },
    { 144, 224  }, { 144, 240  }, { 148, 132  }, { 148, 136  },
    { 148, 140  }, { 148, 144  }, { 148, 148  }, { 148, 152  },
    { 148, 156  }, { 152, 104  }, { 152, 112  }, { 152, 120  },
    { 152, 128  }, { 152, 132  }, { 152, 136  }, { 152, 140  },
    { 152, 144  }, { 152, 148  }, { 152, 152  }, { 152, 156  },
    { 152, 160  }, { 152, 168  }, { 152, 176  }, { 152, 184  },
    { 156, 136  }, { 156, 140  }, { 156, 144  }, { 156, 148  },
    { 156, 152  }, { 160,  64  }, { 160,  80  }, { 160,  96  },
    { 160, 104  }, { 160, 112  }, { 160, 120  }, { 160, 128  },
    { 160, 136  }, { 160, 144  }, { 160, 152  }, { 160, 160  },
    { 160, 168  }, { 160, 176  }, { 160, 184  }, { 160, 192  },
    { 160, 208  }, { 160, 224  }, { 168, 112  }, { 168, 120  },
    { 168, 128  }, { 168, 136  }, { 168, 144  }, { 168, 152  },
    { 168, 160  }, { 168, 168  }, { 168, 176  }, { 176,  16  },
    { 176,  48  }, { 176,  64  }, { 176,  80  }, { 176,  96  },
    { 176, 112  }, { 176, 120  }, { 176, 128  }, { 176, 136  },
    { 176, 144  }, { 176, 152  }, { 176, 160  }, { 176, 168  },
    { 176, 176  }, { 176, 192  }, { 176, 208  }, { 176, 224  },
    { 176, 240  }, { 184, 128  }, { 184, 136  }, { 184, 144  },
    { 184, 152  }, { 184, 160  }, { 192,  80  }, { 192,  96  },
    { 192, 112  }, { 192, 128  }, { 192, 144  }, { 192, 160  },
    { 192, 176  }, { 192, 192  }, { 192, 208  }, { 208,  16  },
    { 208,  48  }, { 208,  80  }, { 208,  96  }, { 208, 112  },
    { 208, 128  }, { 208, 144  }, { 208, 160  }, { 208, 176  },
    { 208, 192  }, { 208, 208  }, { 208, 240  }, { 224, 112  },
    { 224, 128  }, { 224, 144  }, { 224, 160  }, { 224, 176  },
    { 240,  16  }, { 240,  48  }, { 240,  80  }, { 240, 112  },
    { 240, 144  }, { 240, 176  }, { 240, 208  }, { 240, 240  },
    {   0,   0  }, {   0,   0  }, {   0,   0  }, {   0,   0  }
};



//
// some lookups required
//
static float sqrtQ16LookupTable[16];
static u8 yyLookupTable[4096];
static u8 uvLookupTable[4096];


#define DIST(U,V,tU,tV)				\
(						\
	((U)-int(tU))*((U)-int(tU)) +		\
	((V)-int(tV))*((V)-int(tV))		\
)
static class __initializeLookups_t
{
public:
     __initializeLookups_t(void) {
          for(int q= 0; q< 16; q++)
                sqrtQ16LookupTable[q]= sqrt(q/(16.0-q));

          for(unsigned i= 0; i< 4096; i++) {
              int u0= i & 0x3f;
              int v0= i >> 6;


              unsigned dist= 0xffffffff;
              unsigned nearest= 0xff;

              for(unsigned j= 0; j< 252; j++) {
                  unsigned newDist= DIST(u0, v0, uvTable[j].uu>>2, uvTable[j].vv>>2);
                  if(newDist< dist) {
                      nearest= j;
                      dist   = newDist;
                  }
              }

              uvLookupTable[i]= nearest;
          }

          for(unsigned i= 0; i< 4096; i++) {
              int y1= i & 0x3f;
              int y2= i >> 6;

              unsigned dist= 0xffffffff;
              unsigned nearest= 0xff;

              for(unsigned j= 0; j< 128; j++) {
                  unsigned newDist= DIST(y1, y2, yyTable[j].y1>>2, yyTable[j].y2>>2);
                  if(newDist< dist) {
                      nearest= j;
                      dist   = newDist;
                  }
              }

              yyLookupTable[i]= nearest;
          }
     }
} __initializeLookups;


//
// dirty work ahead
//
static u16
getLuma(const u8 *ptr, unsigned stride)
{
    u16 retVal= 0;

    for(unsigned i= 0; i< 4; i++) {
        for(unsigned j= 0; j< 4*3; j++) {
            retVal+= *ptr;
            ptr++;
        }
        ptr+= stride;
    }

#if 0
    return retVal/3;
#else
    return (retVal*85)>>8;
#endif
}

void
cellbEncodeCell(const u8 *imgPtr, u32 stride, u8 *outPtr, u16 referenceLuma)
{
    const u8 *ptr= imgPtr;

    int   yAcum = 0;
    int   yAcum2= 0;
    int   gAcum = 0;
    int   bAcum = 0;
    float yyMean= 0;
    float yyMean2= 0;
    float uuMean= 0;
    float vvMean= 0;

    u16      code= 0;
    unsigned q   = 0;

    referenceLuma*= 3;

    for(unsigned jj= 0; jj< 4; jj++) {
        for(unsigned ii= 0; ii< 4; ii++) {
            //
            // actual formula is 0.3R+0.5G+0.2B
            //
            int curY= int(ptr[0])+ int(ptr[1])+ int(ptr[2]);


            yAcum += curY;
            yAcum2+= curY*curY;


            //
            // calcular ´gAcum´, ´bAcum´
            //
            gAcum+= ptr[1];
            bAcum+= ptr[2];

            //
            // calcular ´code´ an ´q´
            //
            if(curY>= referenceLuma) {
               code<<= 1;
               code |= 1;
               q++;
            } else {
               code<<= 1;
               code |= 0;
            }
            ptr+= 3;
        }

        ptr+= stride;
    }

#if 0
    uuMean= 0.493*bAcum-(0.493/3)*yAcum;
    vvMean= 0.877*gAcum-(0.877/3)*yAcum;

    yyMean = float(yAcum )/16; yAcum>>= 4;
    yyMean2= float(yAcum2)/16;
    uuMean /= 16; uuMean+= 128;
    vvMean /= 16; vvMean+= 128;
#else
    uuMean= float(128+(126*bAcum-42*yAcum)/(4096));
    vvMean= float(128+(224*gAcum-74*yAcum)/(4096));

    yyMean = float(yAcum/16); yAcum/= 16;
    yyMean2= float(yAcum2/16);
#endif

    //
    // normalize yyMean, yyMean2
    //
    yyMean /= 3;
    yyMean2/= 3*3;


    //
    // find sigma
    //
    float sigma = sqrt(yyMean2 - yyMean*yyMean);


    //
    // find y1 and y2
    //
    u8 yy1;
    u8 yy2;


    if(yyMean<  16) yyMean=  16;
    if(yyMean> 240) yyMean= 240;

    if(q== 16) {
        yy1= yy2= u8(yyMean);    /* all pixels are the same */
    } else {
        float d= sqrtQ16LookupTable[q];

        float tmpY1= yyMean - sigma*d;
        float tmpY2= yyMean + sigma/d;

        if (tmpY1<  16.0) tmpY1=  16.0;
        if (tmpY2> 240.0) tmpY2= 240.0;

        /* Round values to nearest integer */
        yy1= (u8)(tmpY1+0.5);
        yy2= (u8)(tmpY2-0.5);
    }


    //
    // find yy codeword
    //
    yy1= (yy1+2)>>2;
    yy2= (yy2+2)>>2;

    u8 yyCode= yyLookupTable[yy2<<6|yy1];


    //
    // find uu codeword
    //
    u8 uu= ((u8)(uuMean)+2)>> 2;
    u8 vv= ((u8)(vvMean)+2)>> 2;

    u8 uvCode= uvLookupTable[vv<<6|uu];


    //
    // hight bit of code must be zero
    //
    if(code & (1<< 15)) {
        //
        // u8 swp= yy1;
        //
        // yy1= yy2;
        // yy2= swp;
        //

        code^= 0xffff;

        yyCode+=128;
    }


    /////////////////////////////////////////////////////////
    //
    // at this point code, yyCode and uvCode contains
    // the cellb info
    //
    /////////////////////////////////////////////////////////

    outPtr[0]= code>>8;
    outPtr[1]= code& 0xff;
    outPtr[2]= uvCode;
    outPtr[3]= yyCode;
}

static u8 *
cellbEmitSkip(u8 *outPtr, unsigned nSkips)
{
    outPtr[0]= 0x80 | ((nSkips-1) & 0x1f);

    outPtr++;

    return outPtr;
}


extern cellbEncodedImage_t *
cellbEncode(cellbEncodeSession_t *s, const u8 *b, unsigned w, unsigned h)
{
    cellbEncodedImage_t *retVal= new cellbEncodedImage_t;

    unsigned encodeW= 4*(w/4);
    unsigned encodeH= 4*(h/4);

    if((s->w!=encodeW) || (s->h!= encodeH)) {
        s->reset(encodeW, encodeH);
    }


    unsigned bpp        = 3;
    unsigned rowStride  = bpp*w;
    unsigned cellsPerRow= encodeW/4;
    unsigned numCells   = (encodeW*encodeH)/(4*4);

    const u8 *imgPtr= b;
    u8       *outPtr= (u8*)malloc(4*numCells);

    retVal->buffer= outPtr;
    retVal->w     = encodeW;
    retVal->h     = encodeH;

    unsigned tSkips= 0;
    unsigned xSkips= 0;
    unsigned nSkips= 0;

    for(unsigned currCell= 0; currCell< numCells; currCell++) {

        //
        // get luma for this cell
        //
        u16 cachedLuma = s->lumaCache[currCell];
        u16 currentLuma= getLuma(imgPtr, rowStride-bpp*4);


        //
        // Facts at this point:
        //   + ´luma´ is the actual luma multiplied by 16 (4 bits)
        //     so it's a 12 bit value.
        //
        //   + cellb quatization disregards 2 bits(at least).
        //
        //   + we consider a change in the last 2 bits a ´small change´
        //
        // So 4+2+2 = 8 bits are not checked. Only 4 higher order
        // bits are considered for detecting changes.
        //
        u8 checkLuma= currentLuma>>8;

        u8 lumaMaskIndex= checkLuma|cachedLuma;
        u8 lumaMask[]=
        {
            0xfe,
            0xfe,
            0xff, 0xff,
            0xff,0xff, 0xff, 0xff,
            0xff,0xff, 0xff, 0xff, 0xff,0xff, 0xfe, 0xfe
        };
        u8 deltaVal[]=
        {
            0,
            0, 1,
            1, 1, 2, 2,
            2, 2,
            2, 2, 1, 1,
            1, 0,
            0
        };

        u8 deltaLuma=lumaMask[lumaMaskIndex]&(checkLuma ^ cachedLuma);


        s->timeCache[currCell]-= deltaVal[lumaMaskIndex];

        if(deltaLuma || (s->timeCache[currCell]< 0)) {
            //
            // compress
            //

            //
            // start by saving the luma in the cache
            //
            s->lumaCache[currCell]= checkLuma;

            s->timeCache[ currCell   ] = 24+(random()%32);
            if(currCell>0 && currCell<numCells-1) {
              s->timeCache[(currCell+1)]-= 8*deltaLuma ;
              s->timeCache[(currCell-1)]-= 8*deltaLuma ;
            }


            //
            // scale the luma (4 bits = divide by 16)
            //
            currentLuma>>= 4;

            if(nSkips) {
               outPtr= cellbEmitSkip(outPtr, nSkips);
               nSkips= 0;
               xSkips++;
            }

            cellbEncodeCell(imgPtr,
                            rowStride-4*bpp,
                            outPtr, currentLuma
                           );
            outPtr+= 4;
        } else {
            //
            // skip
            //
            tSkips++;
            nSkips++;
            if(nSkips== 32) {
               outPtr= cellbEmitSkip(outPtr, nSkips);
               nSkips= 0;
               xSkips++;
            }

            if(lumaMaskIndex<2 || lumaMaskIndex>12)
                s->timeCache[currCell]+= 1;
        }

        imgPtr+= bpp*4;
        if((currCell%cellsPerRow)== cellsPerRow-1) {
            imgPtr+= (w-encodeW)*bpp;
            imgPtr+= rowStride*(4-1);
        }
    }

    retVal->numBytes= outPtr-retVal->buffer;

#if VERBOSE
    printf("encSize=%5d\tskips=%4d %4d (%6.2f%%)\t",
           retVal->numBytes,
           tSkips,
           xSkips,
           100.0*float(numCells-tSkips)/numCells
          );
#endif

#if 0
    if(float(numCells-tSkips)/numCells>0.20) {
        int h= open("/dev/audio", O_WRONLY);

        write(h, yyTable, sizeof(yyTable)/4);

        close(h);
    }
#endif

    return retVal;
}



static void
cellbDecodeCell(u8 *ptr, u32 stride, u16 code, u8 uvCode, u8 yyCode)
{
    u8 ry1= yyTable[yyCode]. y1;
    u8 ry2= yyTable[yyCode]. y2;

    u8 ruu= uvTable[uvCode].uu;
    u8 rvv= uvTable[uvCode].vv;

    i16 R[2];
    i16 G[2];
    i16 B[2];


#if 0
    B[0]=i16(float(int(ruu)-128)/0.493+ry1);
    G[0]=i16(float(int(rvv)-128)/0.877+ry1);
    R[0]=i16(3*int(ry1)-G[0]-B[0]);
#else
    B[0]=((int(ruu)-128)*519)/256+ry1;
    G[0]=((int(rvv)-128)*291)/256+ry1;
    R[0]=(int(ry1)+int(ry1)+int(ry1)-G[0]-B[0]);
#endif

    if(R[0]<   0) R[0]=   0;
    if(R[0]> 255) R[0]= 255;
    if(G[0]<   0) G[0]=   0;
    if(G[0]> 255) G[0]= 255;
    if(B[0]<   0) B[0]=   0;
    if(B[0]> 255) B[0]= 255;

#if 0
    B[1]=i16(float(int(ruu)-128)/ 0.493+ry2);
    G[1]=i16(float(int(rvv)-128)/ 0.877+ry2);
    R[1]=i16(3*int(ry2)-G[1]-B[1]);
#else
    B[1]=((int(ruu)-128)*519)/256+ry2;
    G[1]=((int(rvv)-128)*291)/256+ry2;
    R[1]=(int(ry2)+int(ry2)+int(ry2)-G[1]-B[1]);
#endif

    if(R[1]<   0) R[1]=   0;
    if(R[1]> 255) R[1]= 255;
    if(G[1]<   0) G[1]=   0;
    if(G[1]> 255) G[1]= 255;
    if(B[1]<   0) B[1]=   0;
    if(B[1]> 255) B[1]= 255;



#ifdef __LOTSA_REGISTERS
    u32 *bw= (u32*)ptr;
    u8   sel;

    u32 x0;
    u32 x1;
    u32 x2;


    //
    // first scan
    //
    x0= x1= x2= 0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<<  0;
    x0 |= G[sel]<<  8;
    x0 |= B[sel]<< 16;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<< 24;
    x1 |= G[sel]<<  0;
    x1 |= B[sel]<<  8;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x1 |= R[sel]<< 16;
    x1 |= G[sel]<< 24;
    x2 |= B[sel]<<  0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x2 |= R[sel]<<  8;
    x2 |= G[sel]<< 16;
    x2 |= B[sel]<< 24;

    bw[0]= x0;
    bw[1]= x1;
    bw[2]= x2;

    bw+= 3*w/4;


    //
    // second scan
    //
    x0= x1= x2= 0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<<  0;
    x0 |= G[sel]<<  8;
    x0 |= B[sel]<< 16;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<< 24;
    x1 |= G[sel]<<  0;
    x1 |= B[sel]<<  8;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x1 |= R[sel]<< 16;
    x1 |= G[sel]<< 24;
    x2 |= B[sel]<<  0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x2 |= R[sel]<<  8;
    x2 |= G[sel]<< 16;
    x2 |= B[sel]<< 24;

    bw[0]= x0;
    bw[1]= x1;
    bw[2]= x2;
    bw+= 3*w/4;


    //
    // third scan
    //
    x0= x1= x2= 0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<<  0;
    x0 |= G[sel]<<  8;
    x0 |= B[sel]<< 16;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<< 24;
    x1 |= G[sel]<<  0;
    x1 |= B[sel]<<  8;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x1 |= R[sel]<< 16;
    x1 |= G[sel]<< 24;
    x2 |= B[sel]<<  0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x2 |= R[sel]<<  8;
    x2 |= G[sel]<< 16;
    x2 |= B[sel]<< 24;

    bw[0]= x0;
    bw[1]= x1;
    bw[2]= x2;
    bw+= 3*w/4;


    //
    // fourth scan
    //
    x0= x1= x2= 0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<<  0;
    x0 |= G[sel]<<  8;
    x0 |= B[sel]<< 16;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x0 |= R[sel]<< 24;
    x1 |= G[sel]<<  0;
    x1 |= B[sel]<<  8;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x1 |= R[sel]<< 16;
    x1 |= G[sel]<< 24;
    x2 |= B[sel]<<  0;

    sel=(code&(1<<15))?1:0; code<<= 1;
    x2 |= R[sel]<<  8;
    x2 |= G[sel]<< 16;
    x2 |= B[sel]<< 24;

    bw[0]= x0;
    bw[1]= x1;
    bw[2]= x2;
    bw+= 3*w/4;

#else

    u8 rr[2]; rr[0]= R[0]; rr[1]= R[1];
    u8 gg[2]; gg[0]= G[0]; gg[1]= G[1];
    u8 bb[2]; bb[0]= B[0]; bb[1]= B[1];
    for(unsigned jj= 0; jj< 4; jj++) {
            u8 sel=(code&(1<<15))>>15;

            ptr[3*0  ]= (rr[sel]+ptr[3*(0-1)+0])/2;
            ptr[3*0+1]= (gg[sel]+ptr[3*(0-1)+1])/2;
            ptr[3*0+2]= (bb[sel]+ptr[3*(0-1)+2])/2;

            code<<= 1;
        for(unsigned ii= 1; ii< 4; ii++) {
            u8 sel=(code&(1<<15))>>15;

#if 0
            ptr[3*ii  ]= (3*rr[sel]+ptr[3*(ii-1)+0])/4;
            ptr[3*ii+1]= (3*gg[sel]+ptr[3*(ii-1)+1])/4;
            ptr[3*ii+2]= (3*bb[sel]+ptr[3*(ii-1)+2])/4;
#else
            ptr[3*ii  ]= rr[sel];
            ptr[3*ii+1]= gg[sel];
            ptr[3*ii+2]= bb[sel];
#endif

            code<<= 1;
        }
        ptr+= stride;
    }
#endif
}

int
cellbDecode(cellbDecodeSession_t *s,
            unsigned width,
            unsigned height,
            unsigned char *outBuff,
            int outBuffLen,
            unsigned char *inBuff,
            int inBuffLen
           )
{
    unsigned bpp= 3;

    if (outBuffLen < (int) (bpp * width * height) )
    {
        return -1;
    }

    unsigned numBytes= inBuffLen;

    if((s->w != width) || (s->h != height)) {
        s->reset(width, height);
    }

    unsigned rowStride  = bpp*width;
    unsigned cellsPerRow= width/4;
    unsigned numCells   = (width*height)/(4*4);

    const u8 *imgPtr= inBuff;
    u8       *outPtr= s->imgCache;

    unsigned currCell= 0;
    for (unsigned currByte = 0; currByte < numBytes; )
    {
        assert(currCell< numCells);

        if(imgPtr[currByte] & 0x80) {
            //
            // skip cells
            //
            currCell+= 1 + (imgPtr[currByte]&0x1f) ;

            currByte++;
        } else {

            //
            // decode cell
            //
            unsigned xCell= currCell%cellsPerRow;
            unsigned yCell= currCell/cellsPerRow;

            u8 *ptr= outPtr+ bpp*yCell*4*width+bpp*xCell*4;

            u16 code  = (imgPtr[currByte+0]<< 8)|(imgPtr[currByte+1]) ;
            u8  uvCode= (imgPtr[currByte+2]);
            u8  yyCode= (imgPtr[currByte+3]);

            cellbDecodeCell(ptr, rowStride, code, uvCode, yyCode);

#if 0
            u8 *cachePtr= s->imgCache+ bpp*yCell*4*width+bpp*xCell*4;

            u32 *p1= (u32*)ptr;
            u32 *p2= (u32*)cachePtr;

            p2[0]= p1[0];
            p2[1]= p1[1];
            p2[2]= p1[2];

            p1+= (width*bpp)/4;
            p2+= (width*bpp)/4;

            p2[0]= p1[0];
            p2[1]= p1[1];
            p2[2]= p1[2];

            p1+= (width*bpp)/4;
            p2+= (width*bpp)/4;

            p2[0]= p1[0];
            p2[1]= p1[1];
            p2[2]= p1[2];

            p1+= (width*bpp)/4;
            p2+= (width*bpp)/4;

            p2[0]= p1[0];
            p2[1]= p1[1];
            p2[2]= p1[2];
#endif

            currCell++;

            currByte+= 4;
        }
    }

    memcpy(outBuff, s->imgCache, bpp*width*height);

    return bpp * width * height;
}

