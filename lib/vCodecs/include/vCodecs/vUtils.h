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
// $Id: vUtils.h 22043 2011-02-16 16:30:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __video_utils_h__
#define __video_utils_h__

#include <stdlib.h>

#ifdef __SYMBIAN32__
#include "vCodecs/codec.h"
#else
#include <vCodecs/codec.h>
#endif

__IMPORT void
vSmooth(unsigned char *inBuff,
        unsigned char *outBuff,
        unsigned       inW,
        unsigned       inH,
        unsigned       outW,
        unsigned       outH,
        u32            format
       );

__IMPORT int
vRescale(unsigned char *inBuff,
         unsigned       inBuffLen,
         unsigned char *outBuff,
         unsigned       outBuffLen,
         unsigned       inW,
         unsigned       inH,
         unsigned       outW,
         unsigned       outH,
         u32            format
        );

__IMPORT int
vPutImage(unsigned char *inBuff,
          unsigned       inBuffLen,
          unsigned char *outBuff,
          unsigned       outBuffLen,
          unsigned       inW,
          unsigned       inH,
          unsigned       outW,
          unsigned       outH,
          unsigned       posX,
          unsigned       posY,
          unsigned       totalW,
          unsigned       totalH,
          u32            format,
          unsigned char *mask = NULL,
          bool           invertMask = false
         );

__IMPORT void
vSetMaskRect(unsigned char *mask,
             unsigned       W,
             unsigned       H,
             unsigned       posX,
             unsigned       posY,
             unsigned       totalW,
             unsigned       totalH,
             bool           val,
             u32            format
            );

__IMPORT int
vSetMask(unsigned char *outBuff,
         unsigned       outBuffLen,
         unsigned char *mask,
         unsigned       W,
         unsigned       H,
         unsigned       totalW,
         unsigned       totalH,
         bool           val,
         u32            format
        );

#endif

