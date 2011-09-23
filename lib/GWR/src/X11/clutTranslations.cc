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
// $Id: clutTranslations.cc 10344 2007-06-11 10:42:20Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

NOTE: this code is incomplete, we keep it for future

#include <icf2/general.h>
#include <icf2/notify.hh>

//
// implementation details
//
struct rgb24_clut8_retval_t
{
    u8 *b;
    u8 *p;
};

rgb24_clut8_retval_t *
trans_RGB24_to_CLUT8(u8 *workBuffer, unsigned nPixels)
{
    rgb24_clut8_retval_t *retVal= new rgb24_clut8_retval_t;

    clutColor_t pal[256];

    u32 redLookup[256];
    u32 greenLookup[256];
    u32 blueLookup[256];

    u32 rMask= 0xE0;
    u32 gMask= 0x1C;
    u32 bMask= 0x03;

    createLookupTablesFromRGBMask(
        rMask,
        gMask,
        bMask,
        redLookup,
        greenLookup,
        blueLookup
    );
    createPaletteFromRGBMask(
        rMask,
        gMask,
        bMask,
        pal
    );

    u8 *newWorkBuffer= (u8*)malloc(5*nPixels);

    stdc_translate_rgb24_to_xxx8(
        newWorkBuffer,
        workBuffer,
        nPixels,
        redLookup,
        greenLookup,
        blueLookup
    );

    retVal->b= newWorkBuffer;
    retVal->p= (u8*)malloc(256*3);

    for(unsigned i= 0; i< 256; i++) {
        retVal->p[3*i+0]= pal[i].r;
        retVal->p[3*i+1]= pal[i].g;
        retVal->p[3*i+2]= pal[i].b;
    }

    return retVal;
}

rgb24_clut8_retval_t *
trans_BGR24_to_CLUT8(u8 *workBuffer, unsigned nPixels)
{
    rgb24_clut8_retval_t *retVal= new rgb24_clut8_retval_t;

    clutColor_t pal[256];

    u32 redLookup[256];
    u32 greenLookup[256];
    u32 blueLookup[256];

    u32 rMask= 0xE0;
    u32 gMask= 0x1C;
    u32 bMask= 0x03;

    createLookupTablesFromRGBMask(
        rMask,
        gMask,
        bMask,
        redLookup,
        greenLookup,
        blueLookup
    );
    createPaletteFromRGBMask(
        bMask,
        gMask,
        rMask,
        pal
    );

    u8 *newWorkBuffer= (u8*)malloc(5*nPixels);

    stdc_translate_rgb24_to_xxx8(
        newWorkBuffer,
        workBuffer,
        nPixels,
        redLookup,
        greenLookup,
        blueLookup
    );

    retVal->b= newWorkBuffer;
    retVal->p= (u8*)malloc(256*3);

    for(unsigned i= 0; i< 256; i++) {
        retVal->p[3*i+0]= pal[i].r;
        retVal->p[3*i+1]= pal[i].g;
        retVal->p[3*i+2]= pal[i].b;
    }

    return retVal;
}



