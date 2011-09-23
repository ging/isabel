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
// $Id: bufferTranslations.hh 10255 2007-05-31 15:44:25Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gwr__buffer_translations__hh__
#define __gwr__buffer_translations__hh__

#include <icf2/general.h>

#if 0
Si esto es particular de X, se debería declarar aqui.
Si es general, se deja en gwr/gwrTypes.hh y se 
borra esto
//
// should not be here
//
struct clutColor_t {
    u8 r;
    u8 g;
    u8 b;
};
#else
#include <gwr/gwr.h>
#endif

//
// rgb24 to xxx
//
void
stdc_translate_rgb24_to_luma8(
    void       *dst,
    void const *src,
    unsigned    numPixels
);
void
stdc_translate_rgb24_to_xxx8(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
);
void
stdc_translate_rgb24_to_rgb16(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
);
void
stdc_translate_rgb24_to_rgb24(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
);
void
stdc_translate_rgb24_to_rgb32(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
);

//
// rgb24 to xxx (with zoom!!!)
//
void
stdc_translate_rgb24_to_luma8_and_zoom(
    void       *dst,
    void const *src,
    unsigned    width,
    unsigned    height,
    float       zoomX,
    float       zoomY,
    unsigned    croppedWidth = 0,
    unsigned    croppedHeight= 0
);
void
stdc_translate_rgb24_to_xxx8_and_zoom(
    void       *dst,
    void const *src,
    unsigned    width,
    unsigned    height,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB,
    float       zoomX,
    float       zoomY,
    unsigned    croppedWidth= 0,
    unsigned    croppedHeight= 0
);
void
stdc_translate_rgb24_to_rgb16_and_zoom(
    void       *dst,
    void const *src,
    unsigned    width,
    unsigned    height,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB,
    float       zoomX,
    float       zoomY,
    unsigned    croppedWidth= 0,
    unsigned    croppedHeight= 0
);
void
stdc_translate_rgb24_to_rgb24_and_zoom(
    void       *dst,
    void const *src,
    unsigned    width,
    unsigned    height,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB,
    float       zoomX,
    float       zoomY,
    unsigned    croppedWidth= 0,
    unsigned    croppedHeight= 0
);
void
stdc_translate_rgb24_to_rgb32_and_zoom(
    void       *dst,
    void const *src,
    unsigned    width,
    unsigned    height,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB,
    float       zoomX,
    float       zoomY,
    unsigned    croppedWidth= 0,
    unsigned    croppedHeight= 0
);




//
// lookup table creation
//
void
createLookupTablesFromRGBMask(
    u32    rMask,
    u32    gMask,
    u32    bMask,
    u32   *redLookup,
    u32   *greenLookup,
    u32   *blueLookup,
    float  rGamma= 1.0,
    float  gGamma= 1.0,
    float  bGamma= 1.0
);

void
createPaletteFromRGBMask(
    u32          rMask,
    u32          gMask,
    u32          bMask,
    clutColor_t *pal
);
#endif
