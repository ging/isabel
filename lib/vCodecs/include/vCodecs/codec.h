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
// $Id: codec.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vcodecs_codec_h__
#define __vcodecs_codec_h__

#include <vCodecs/general.h>

#ifdef __SYMBIAN32__
//  Include Files
#include <e32base.h>    // CBase
#include <e32std.h>
#else
//#include <vCodecs/fourcc.h>
#endif

// forget them!
#undef __IMPORT
#undef __EXPORT

#ifdef _USRDLL
#define __IMPORT  __declspec(dllexport)
#define __EXPORT  __declspec(dllexport)
#else
#ifdef __SYMBIAN32__
#define __IMPORT  IMPORT_C
#define __EXPORT  EXPORT_C
#else
#ifdef WIN32
#define __IMPORT  __declspec(dllexport)
#define __EXPORT  __declspec(dllexport)
#else
#define __IMPORT
#define __EXPORT
#endif
#endif
#endif

#define FOURCC( a, b, c, d ) \
  ( (u32)(a) | ( (u32)(b) << 8 ) | ( (u32)(c) << 16 ) | ( (u32)(d) << 24 ) )


//-----------------------------------
// colorspaces and pixel format names
//-----------------------------------

const u32 RGB24_FORMAT= FOURCC('R','B','G','3');
const u32 BGR24_FORMAT= FOURCC('B','G','R','3');
const u32 I422i_FORMAT= FOURCC('Y','U','Y','V');
const u32 I422P_FORMAT= FOURCC('Y','V','1','2');
const u32 I420P_FORMAT= FOURCC('I','4','2','0');
const u32 I411P_FORMAT= FOURCC('Y','4','1','B');

// const u32 PNG_FORMAT = FOURCC('P','N','G','X');
// const u32 TIFF_FORMAT= FOURCC('T','I','F','F');
// const u32 GIF_FORMAT = FOURCC('G','I','F','X');
// const u32 JPEG_FORMAT= FOURCC('J','P','E','G');

const u32 CELB_FORMAT = FOURCC('I','C','L','B');
const u32 MJPEG_FORMAT= FOURCC('M','J','G','P');
const u32 MPEG1_FORMAT= FOURCC('M','P','G','1');
const u32 MPEG2_FORMAT= FOURCC('M','P','G','2');
const u32 H263_FORMAT = FOURCC('H','2','6','3');
const u32 H263P_FORMAT= FOURCC('2','6','3','+');
const u32 H264_FORMAT = FOURCC('H','2','6','4');
const u32 MPEG4_FORMAT= FOURCC('M','P','G','4');
const u32 XVID_FORMAT = FOURCC('X','V','I','D');
const u32 DIVX_FORMAT = FOURCC('D','I','V','X');

//-----------------------------------
// Auxiliar functions for colorspaces
//-----------------------------------

__IMPORT const char*
vGetFormatNameById(u32 format);

__IMPORT u32
vGetFormatIdByName(const char *fmtname);

__IMPORT int
vFrameSize(int width, int height, u32 format);


//-----------------------------------
// Coder and Decoder args and structs
//-----------------------------------

__IMPORT typedef struct
{
    unsigned int   width;
    unsigned int   height;
    int            bitRate;   // bits per second, ALWAYS CBR
    float          frameRate;
    int            maxInter;  // emit one INTRA frame every n frames
    int            quality;   // only used in NO CBR codecs (as MJPEG)
    u32            format;
} vCoderArgs_t;

__IMPORT typedef struct
{
    unsigned int width;
    unsigned int height;
    u32          format;
} vDecoderArgs_t;

__IMPORT typedef struct
{
   u32 format; // output FourCC
   vCoderArgs_t params;

   void *encoderContext;

   void (*Delete)(void *context);

   int (*Encode)(void          *context,
                 unsigned char *inBuff,
                 int            inBuffLen,
                 unsigned char *outBuff,
                 int            outBuffLen
                );

   void (*GetParams)(void *context, vCoderArgs_t *params);

} vCoder_t;

__IMPORT typedef struct
{
   u32 format; // input FourCC
   vDecoderArgs_t params;

   void *decoderContext;

   void (*Delete)(void *context);

   int (*Decode)(void          *context,
                 unsigned char *inBuff,
                 int            inBuffLen,
                 unsigned char *outBuff,
                 int            outBuffLen
                );

   void (*GetParams)(void *context, vDecoderArgs_t *params);

} vDecoder_t;

//-----------------------------------
// Functions over particular codec
//-----------------------------------

__IMPORT void
vDeleteCoder(vCoder_t *coder);

__IMPORT void
vDeleteDecoder(vDecoder_t *decoder);

__IMPORT int
vEncode(vCoder_t      *coder,
        unsigned char *inBuff,
        int            inBuffLen,
        unsigned char *outBuff,
        int            outBuffLen
       );

__IMPORT int
vDecode(vDecoder_t    *decoder,
        unsigned char *inBuff,
        int            inBuffLen,
        unsigned char *outBuff,
        int            outBuffLen
       );

__IMPORT void
vGetParams(vCoder_t *coder, vCoderArgs_t *params);

__IMPORT void
vGetParams(vDecoder_t *decoder, vDecoderArgs_t *params);

#endif

