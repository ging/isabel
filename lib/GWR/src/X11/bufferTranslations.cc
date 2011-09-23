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
// $Id: bufferTranslations.cc 10255 2007-05-31 15:44:25Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "bufferTranslations.hh"

#include <stdio.h>
#include <math.h>


//
// MIN macro
//
#define MIN(a,b) ((a)<(b)?(a):(b))

static bool
isSmallZoom(float z) {
    return sqrt((z-1.0)*(z-1.0))< 0.05;
}

//
// luma routines
//
static u8 div3_lookup[768]=
{
      0,   0,   0,   1,   1,   1,   2,   2,   2,   3,   3,   3,
      4,   4,   4,   5,   5,   5,   6,   6,   6,   7,   7,   7,
      8,   8,   8,   9,   9,   9,  10,  10,  10,  11,  11,  11, 
     12,  12,  12,  13,  13,  13,  14,  14,  14,  15,  15,  15, 
     16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19, 
     20,  20,  20,  21,  21,  21,  22,  22,  22,  23,  23,  23, 
     24,  24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27, 
     28,  28,  28,  29,  29,  29,  30,  30,  30,  31,  31,  31, 
     32,  32,  32,  33,  33,  33,  34,  34,  34,  35,  35,  35, 
     36,  36,  36,  37,  37,  37,  38,  38,  38,  39,  39,  39, 
     40,  40,  40,  41,  41,  41,  42,  42,  42,  43,  43,  43, 
     44,  44,  44,  45,  45,  45,  46,  46,  46,  47,  47,  47, 
     48,  48,  48,  49,  49,  49,  50,  50,  50,  51,  51,  51, 
     52,  52,  52,  53,  53,  53,  54,  54,  54,  55,  55,  55, 
     56,  56,  56,  57,  57,  57,  58,  58,  58,  59,  59,  59, 
     60,  60,  60,  61,  61,  61,  62,  62,  62,  63,  63,  63, 
     64,  64,  64,  65,  65,  65,  66,  66,  66,  67,  67,  67, 
     68,  68,  68,  69,  69,  69,  70,  70,  70,  71,  71,  71, 
     72,  72,  72,  73,  73,  73,  74,  74,  74,  75,  75,  75, 
     76,  76,  76,  77,  77,  77,  78,  78,  78,  79,  79,  79, 
     80,  80,  80,  81,  81,  81,  82,  82,  82,  83,  83,  83, 
     84,  84,  84,  85,  85,  85,  86,  86,  86,  87,  87,  87, 
     88,  88,  88,  89,  89,  89,  90,  90,  90,  91,  91,  91, 
     92,  92,  92,  93,  93,  93,  94,  94,  94,  95,  95,  95, 
     96,  96,  96,  97,  97,  97,  98,  98,  98,  99,  99,  99, 
    100, 100, 100, 101, 101, 101, 102, 102, 102, 103, 103, 103, 
    104, 104, 104, 105, 105, 105, 106, 106, 106, 107, 107, 107, 
    108, 108, 108, 109, 109, 109, 110, 110, 110, 111, 111, 111, 
    112, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 115, 
    116, 116, 116, 117, 117, 117, 118, 118, 118, 119, 119, 119, 
    120, 120, 120, 121, 121, 121, 122, 122, 122, 123, 123, 123, 
    124, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127, 
    128, 128, 128, 129, 129, 129, 130, 130, 130, 131, 131, 131, 
    132, 132, 132, 133, 133, 133, 134, 134, 134, 135, 135, 135, 
    136, 136, 136, 137, 137, 137, 138, 138, 138, 139, 139, 139, 
    140, 140, 140, 141, 141, 141, 142, 142, 142, 143, 143, 143, 
    144, 144, 144, 145, 145, 145, 146, 146, 146, 147, 147, 147, 
    148, 148, 148, 149, 149, 149, 150, 150, 150, 151, 151, 151, 
    152, 152, 152, 153, 153, 153, 154, 154, 154, 155, 155, 155, 
    156, 156, 156, 157, 157, 157, 158, 158, 158, 159, 159, 159, 
    160, 160, 160, 161, 161, 161, 162, 162, 162, 163, 163, 163, 
    164, 164, 164, 165, 165, 165, 166, 166, 166, 167, 167, 167, 
    168, 168, 168, 169, 169, 169, 170, 170, 170, 171, 171, 171, 
    172, 172, 172, 173, 173, 173, 174, 174, 174, 175, 175, 175, 
    176, 176, 176, 177, 177, 177, 178, 178, 178, 179, 179, 179, 
    180, 180, 180, 181, 181, 181, 182, 182, 182, 183, 183, 183, 
    184, 184, 184, 185, 185, 185, 186, 186, 186, 187, 187, 187, 
    188, 188, 188, 189, 189, 189, 190, 190, 190, 191, 191, 191, 
    192, 192, 192, 193, 193, 193, 194, 194, 194, 195, 195, 195, 
    196, 196, 196, 197, 197, 197, 198, 198, 198, 199, 199, 199, 
    200, 200, 200, 201, 201, 201, 202, 202, 202, 203, 203, 203, 
    204, 204, 204, 205, 205, 205, 206, 206, 206, 207, 207, 207, 
    208, 208, 208, 209, 209, 209, 210, 210, 210, 211, 211, 211, 
    212, 212, 212, 213, 213, 213, 214, 214, 214, 215, 215, 215, 
    216, 216, 216, 217, 217, 217, 218, 218, 218, 219, 219, 219, 
    220, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 223, 
    224, 224, 224, 225, 225, 225, 226, 226, 226, 227, 227, 227, 
    228, 228, 228, 229, 229, 229, 230, 230, 230, 231, 231, 231, 
    232, 232, 232, 233, 233, 233, 234, 234, 234, 235, 235, 235, 
    236, 236, 236, 237, 237, 237, 238, 238, 238, 239, 239, 239, 
    240, 240, 240, 241, 241, 241, 242, 242, 242, 243, 243, 243, 
    244, 244, 244, 245, 245, 245, 246, 246, 246, 247, 247, 247, 
    248, 248, 248, 249, 249, 249, 250, 250, 250, 251, 251, 251, 
    252, 252, 252, 253, 253, 253, 254, 254, 254, 255, 255, 255
};


void
stdc_translate_rgb24_to_luma8(
    void       *dst,
    void const *src,
    unsigned    numPixels
)
{
#ifdef __BUILD_FOR_INTEL
    u8 const *in = reinterpret_cast<u8 const *>(src);
    u8       *out= reinterpret_cast<u8       *>(dst);

    for(unsigned i= 0; i< numPixels/4; i++) {

      unsigned luma0=
        (
            in[12*i+0]+
            in[12*i+1]+
            in[12*i+2]
        );
      out[4*i+0]= div3_lookup[luma0];

      unsigned luma1=
        (
            in[12*i+3]+
            in[12*i+4]+
            in[12*i+5]
        );
      out[4*i+1]= div3_lookup[luma1];

      unsigned luma2=
        (
            in[12*i+6]+
            in[12*i+7]+
            in[12*i+8]
        );
      out[4*i+2]= div3_lookup[luma2];

      unsigned luma3=
        (
            in[12*i+9]+
            in[12*i+10]+
            in[12*i+11]
        );
      out[4*i+3]= div3_lookup[luma3];
    }
#else
#endif
}

void
stdc_translate_rgb24_to_xxx8(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
)
{
    u8 const *in = reinterpret_cast<u8 const *>(src);
    u8       *out= reinterpret_cast<u8       *>(dst);

    for(unsigned i= 0; i< npixels/4; i++) {
        u8 s0r= in[0*3+0];
        u8 s0g= in[0*3+1];
        u8 s0b= in[0*3+2];

        u8 s1r= in[1*3+0];
        u8 s1g= in[1*3+1];
        u8 s1b= in[1*3+2];

        u8 s2r= in[2*3+0];
        u8 s2g= in[2*3+1];
        u8 s2b= in[2*3+2];

        u8 s3r= in[3*3+0];
        u8 s3g= in[3*3+1];
        u8 s3b= in[3*3+2];

        u8 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];
        u8 x1= transTblR[s1r] | transTblG[s1g] | transTblB[s1b];
        u8 x2= transTblR[s2r] | transTblG[s2g] | transTblB[s2b];
        u8 x3= transTblR[s3r] | transTblG[s3g] | transTblB[s3b];

        out[0]= x0;
        out[1]= x1;
        out[2]= x2;
        out[3]= x3;

        in += 4*3;
        out+= 4;
    }

#if 0
    for(unsigned i= 0; i< npixels%4; i++) {
        u8 s0r= src[0*3+0];
        u8 s0g= src[0*3+1];
        u8 s0b= src[0*3+2];

        u8 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

        dst[i]= x0;

        src+= 3;
        dst+= 1;
    }
#endif

}

void
stdc_translate_rgb24_to_rgb16(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
)
{
    u8  const *in = reinterpret_cast<u8  const *>(src);
    u16       *out= reinterpret_cast<u16       *>(dst);

    for(unsigned i= 0; i< npixels; i++) {
        u8 s0r= in[0];
        u8 s0g= in[1];
        u8 s0b= in[2];

        u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

        out[0]= x0;

        in += 3;
        out+= 1;
    }
}

void
stdc_translate_rgb24_to_rgb24(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
)
{
    u8  const *in = reinterpret_cast<u8  const *>(src);
    u8        *out= reinterpret_cast<u8        *>(dst);

    for(unsigned i= 0; i< npixels; i++) {
        u8 s0r= in[0];
        u8 s0g= in[1];
        u8 s0b= in[2];

        u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

#ifdef __BUILD_FOR_BIG_ENDIAN
        out[0]= (x0>>16)&0xff;
        out[1]= (x0>> 8)&0xff;
        out[2]= (x0>> 0)&0xff;
#else
        out[2]= (x0>>16)&0xff;
        out[1]= (x0>> 8)&0xff;
        out[0]= (x0>> 0)&0xff;
#endif

        in += 3;
        out+= 3;
    }
}

void
stdc_translate_rgb24_to_rgb32(
    void       *dst,
    void const *src,
    unsigned    npixels,
    u32        *transTblR,
    u32        *transTblG,
    u32        *transTblB
)
{
    u8  const *in = reinterpret_cast<u8  const *>(src);
    u32       *out= reinterpret_cast<u32       *>(dst);

    for(unsigned i= 0; i< npixels; i++) {
        u8 s0r= in[0];
        u8 s0g= in[1];
        u8 s0b= in[2];

        u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

        out[0]= x0;

        in += 3;
        out+= 1;
    }
}

void
stdc_translate_rgb24_to_luma8_and_zoom(
    void       *dst,
    void const *src,
    unsigned    width,
    unsigned    height,
    float       zoomX,
    float       zoomY,
    unsigned    croppedWidth,
    unsigned    croppedHeight
)
{
    if(croppedWidth == 0) croppedWidth = width;
    if(croppedHeight== 0) croppedHeight= height;


    float actualZoomX= (zoomX<0
        ?(float)croppedWidth/(float)width
        :zoomX
      );
    float actualZoomY= (zoomY<0
        ?(float)croppedHeight/(float)height
        :zoomY
      );


    unsigned zoomedWidth= int(width*actualZoomX);
    unsigned zoomedHeight= int(height*actualZoomY);

    unsigned croppedZoomedWidth = MIN(
        zoomedWidth ,
        croppedWidth
      );
    unsigned croppedZoomedHeight= MIN(
        zoomedHeight,
        croppedHeight
      );


    if((croppedZoomedWidth == width) &&
       (croppedZoomedHeight== height) &&
       (isSmallZoom(actualZoomX))     &&
       (isSmallZoom(actualZoomY))
      )
    {
        stdc_translate_rgb24_to_luma8(
            dst,
            src,
            width*height
        );

        return;
    }



    //
    // letstart by zooming image
    //
    static  unsigned const MAX_ZOOM_WIDTH= 4096;
    static  unsigned const MAX_ZOOM_HEIGHT= 4096;
            unsigned int xindex [MAX_ZOOM_WIDTH];
            unsigned int yindex [MAX_ZOOM_HEIGHT];


    unsigned   inBpp= 3;
    unsigned   curry= 0;
    float      deltaX= 1/actualZoomX;
    float      deltaY= 1/actualZoomY;

    for(unsigned iter= 0; iter < croppedZoomedHeight; iter++)
        yindex[iter]=         int(float(iter) * deltaY);

    for(unsigned iter= 0; iter < croppedZoomedWidth; iter++)
        xindex[iter]= inBpp * int(float(iter) * deltaX);


    u8 const *in = reinterpret_cast<u8 const *>(src);
    u8       *out= reinterpret_cast<u8       *>(dst);

    for(unsigned iy= 0; iy< croppedZoomedHeight; iy++) {
        while(curry  < yindex[iy])  {
            in+=  width*inBpp;
            curry++;
        }
        for(unsigned ix= 0; ix< croppedZoomedWidth; ix++) {
            const u8 *aux= in+xindex[ix];
            unsigned luma=
            (
                aux[0]+
                aux[1]+
                aux[2]
            );
            out[0]= div3_lookup[luma];
            out++;
        }
        for(unsigned ix= croppedZoomedWidth; ix< croppedWidth; ix++) {
            out[0]= 0;
            out++;
        }
    }
}

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
    unsigned    croppedWidth,
    unsigned    croppedHeight
)
{
    if(croppedWidth == 0) croppedWidth = width;
    if(croppedHeight== 0) croppedHeight= height;


    float actualZoomX= (zoomX<0
        ?(float)croppedWidth/(float)width
        :zoomX
      );
    float actualZoomY= (zoomY<0
        ?(float)croppedHeight/(float)height
        :zoomY
      );


    unsigned zoomedWidth= int(width*actualZoomX);
    unsigned zoomedHeight= int(height*actualZoomY);

    unsigned croppedZoomedWidth = MIN(
        zoomedWidth ,
        croppedWidth
      );
    unsigned croppedZoomedHeight= MIN(
        zoomedHeight,
        croppedHeight
      );


    if((croppedZoomedWidth == width) &&
       (croppedZoomedHeight== height) &&
       (isSmallZoom(actualZoomX))     &&
       (isSmallZoom(actualZoomY))
      )
    {
        stdc_translate_rgb24_to_xxx8(
            dst,
            src,
            width*height,
            transTblR,
            transTblG,
            transTblB
        );

        return;
    }



    //
    // letstart by zooming image
    //
    static  unsigned const MAX_ZOOM_WIDTH= 4096;
    static  unsigned const MAX_ZOOM_HEIGHT= 4096;
            unsigned int xindex [MAX_ZOOM_WIDTH];
            unsigned int yindex [MAX_ZOOM_HEIGHT];


    unsigned   inBpp= 3;
    unsigned   curry= 0;
    float      deltaX= 1/actualZoomX;
    float      deltaY= 1/actualZoomY;

    for(unsigned iter= 0; iter < croppedZoomedHeight; iter++)
        yindex[iter]=         int(float(iter) * deltaY);

    for(unsigned iter= 0; iter < croppedZoomedWidth; iter++)
        xindex[iter]= inBpp * int(float(iter) * deltaX);


    u8 const *in = reinterpret_cast<u8 const *>(src);
    u8 *out= reinterpret_cast<u8*>(dst);
    for(unsigned iy= 0; iy< croppedZoomedHeight; iy++) {
        while(curry  < yindex[iy])  {
            in+=  width*inBpp;
            curry++;
        }
        for(unsigned ix= 0; ix< croppedZoomedWidth; ix++) {
            const u8 *aux= in+xindex[ix];
            u8 s0r= aux[0];
            u8 s0g= aux[1];
            u8 s0b= aux[2];

            u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

            out[0]= (u8)(x0);
            out++;
        }
        for(unsigned ix= croppedZoomedWidth; ix< croppedWidth; ix++) {
            out[0]= 0;
            out++;
        }
    }
}

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
    unsigned    croppedWidth,
    unsigned    croppedHeight
)
{
    if(croppedWidth == 0) croppedWidth = width;
    if(croppedHeight== 0) croppedHeight= height;


    float actualZoomX= (zoomX<0
        ?(float)croppedWidth/(float)width
        :zoomX
      );
    float actualZoomY= (zoomY<0
        ?(float)croppedHeight/(float)height
        :zoomY
      );


    unsigned zoomedWidth= int(width*actualZoomX);
    unsigned zoomedHeight= int(height*actualZoomY);

    unsigned croppedZoomedWidth = MIN(
        zoomedWidth ,
        croppedWidth
      );
    unsigned croppedZoomedHeight= MIN(
        zoomedHeight,
        croppedHeight
      );


    if((croppedZoomedWidth == width) &&
       (croppedZoomedHeight== height) &&
       (isSmallZoom(actualZoomX))     &&
       (isSmallZoom(actualZoomY))
      )
    {
        stdc_translate_rgb24_to_rgb16(
            dst,
            src,
            width*height,
            transTblR,
            transTblG,
            transTblB
        );

        return;
    }



    //
    // letstart by zooming image
    //
    static  unsigned const MAX_ZOOM_WIDTH= 4096;
    static  unsigned const MAX_ZOOM_HEIGHT= 4096;
            unsigned int xindex [MAX_ZOOM_WIDTH];
            unsigned int yindex [MAX_ZOOM_HEIGHT];


    unsigned   inBpp= 3;
    unsigned   curry= 0;
    float      deltaX= 1/actualZoomX;
    float      deltaY= 1/actualZoomY;

    for(unsigned iter= 0; iter < croppedZoomedHeight; iter++)
        yindex[iter]=         int(float(iter) * deltaY);

    for(unsigned iter= 0; iter < croppedZoomedWidth; iter++)
        xindex[iter]= inBpp * int(float(iter) * deltaX);


    u8  const *in = reinterpret_cast<u8 const *>(src);
    u16 *out= reinterpret_cast<u16*>(dst);
    for(unsigned iy= 0; iy< croppedZoomedHeight; iy++) {
        while(curry  < yindex[iy])  {
            in+=  width*inBpp;
            curry++;
        }
        for(unsigned ix= 0; ix< croppedZoomedWidth; ix++) {
            const u8 *aux= in+xindex[ix];
            u8 s0r= aux[0];
            u8 s0g= aux[1];
            u8 s0b= aux[2];

            u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

            out[0]= (u16)(x0);
            out++;
        }
        for(unsigned ix= croppedZoomedWidth; ix< croppedWidth; ix++) {
            out[0]= 0;
            out++;
        }
    }
}

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
    unsigned    croppedWidth,
    unsigned    croppedHeight
)
{
    if(croppedWidth == 0) croppedWidth = width;
    if(croppedHeight== 0) croppedHeight= height;


    float actualZoomX= (zoomX<0
        ?(float)croppedWidth/(float)width
        :zoomX
      );
    float actualZoomY= (zoomY<0
        ?(float)croppedHeight/(float)height
        :zoomY
      );


    unsigned zoomedWidth= int(width*actualZoomX);
    unsigned zoomedHeight= int(height*actualZoomY);

    unsigned croppedZoomedWidth = MIN(
        zoomedWidth ,
        croppedWidth
      );
    unsigned croppedZoomedHeight= MIN(
        zoomedHeight,
        croppedHeight
      );


    if((croppedZoomedWidth == width) &&
       (croppedZoomedHeight== height) &&
       (isSmallZoom(actualZoomX))     &&
       (isSmallZoom(actualZoomY))
      )
    {
        stdc_translate_rgb24_to_rgb24(
            dst,
            src,
            width*height,
            transTblR,
            transTblG,
            transTblB
        );

        return;
    }



    //
    // letstart by zooming image
    //
    static  unsigned const MAX_ZOOM_WIDTH= 4096;
    static  unsigned const MAX_ZOOM_HEIGHT= 4096;
            unsigned int xindex [MAX_ZOOM_WIDTH];
            unsigned int yindex [MAX_ZOOM_HEIGHT];


    unsigned   inBpp= 3;
    unsigned   curry= 0;
    float      deltaX= 1/actualZoomX;
    float      deltaY= 1/actualZoomY;

    for(unsigned iter= 0; iter < croppedZoomedHeight; iter++)
        yindex[iter]=         int(float(iter) * deltaY);

    for(unsigned iter= 0; iter < croppedZoomedWidth; iter++)
        xindex[iter]= inBpp * int(float(iter) * deltaX);


    u8  const *in = reinterpret_cast<u8 const *>(src);
    u8  *out= reinterpret_cast<u8 *>(dst);
    for(unsigned iy= 0; iy< croppedZoomedHeight; iy++) {
        while(curry  < yindex[iy])  {
            in+=  width*inBpp;
            curry++;
        }
        for(unsigned ix= 0; ix< croppedZoomedWidth; ix++) {
            const u8 *aux= in+xindex[ix];
            u8 s0r= aux[0];
            u8 s0g= aux[1];
            u8 s0b= aux[2];

            u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

#ifdef __BUILD_FOR_BIG_ENDIAN
            out[0]= (x0>>16)&0xff;
            out[1]= (x0>> 8)&0xff;
            out[2]= (x0>> 0)&0xff;
#else
            out[2]= (x0>>16)&0xff;
            out[1]= (x0>> 8)&0xff;
            out[0]= (x0>> 0)&0xff;
#endif
            out+= 3;
        }
        for(unsigned ix= croppedZoomedWidth; ix< croppedWidth; ix++) {
            out[0]= 0;
            out++;
        }
    }
}

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
    unsigned    croppedWidth,
    unsigned    croppedHeight
)
{
    if(croppedWidth == 0) croppedWidth = width;
    if(croppedHeight== 0) croppedHeight= height;


    float actualZoomX= (zoomX<0
        ?(float)croppedWidth/(float)width
        :zoomX
      );
    float actualZoomY= (zoomY<0
        ?(float)croppedHeight/(float)height
        :zoomY
      );


    unsigned zoomedWidth= int(width*actualZoomX);
    unsigned zoomedHeight= int(height*actualZoomY);

    unsigned croppedZoomedWidth = MIN(
        zoomedWidth ,
        croppedWidth
      );
    unsigned croppedZoomedHeight= MIN(
        zoomedHeight,
        croppedHeight
      );


    if((croppedZoomedWidth == width) &&
       (croppedZoomedHeight== height) &&
       (isSmallZoom(actualZoomX))     &&
       (isSmallZoom(actualZoomY))
      )
    {
        stdc_translate_rgb24_to_rgb32(
            dst,
            src,
            width*height,
            transTblR,
            transTblG,
            transTblB
        );

        return;
    }



    //
    // letstart by zooming image
    //
    static  unsigned const MAX_ZOOM_WIDTH= 4096;
    static  unsigned const MAX_ZOOM_HEIGHT= 4096;
            unsigned int xindex [MAX_ZOOM_WIDTH];
            unsigned int yindex [MAX_ZOOM_HEIGHT];


    unsigned   inBpp= 3;
    unsigned   curry= 0;
    float      deltaX= 1/actualZoomX;
    float      deltaY= 1/actualZoomY;

    for(unsigned iter= 0; iter < croppedZoomedHeight; iter++)
        yindex[iter]=         int(float(iter) * deltaY);

    for(unsigned iter= 0; iter < croppedZoomedWidth; iter++)
        xindex[iter]= inBpp * int(float(iter) * deltaX);


    u8  const *in = reinterpret_cast<u8 const *>(src);
    u32 *out= reinterpret_cast<u32*>(dst);
    for(unsigned iy= 0; iy< croppedZoomedHeight; iy++) {
        while(curry  < yindex[iy])  {
            in+=  width*inBpp;
            curry++;
        }
        for(unsigned ix= 0; ix< croppedZoomedWidth; ix++) {
            const u8 *aux= in+xindex[ix];
            u8 s0r= aux[0];
            u8 s0g= aux[1];
            u8 s0b= aux[2];

            u32 x0= transTblR[s0r] | transTblG[s0g] | transTblB[s0b];

            out[0]= (u32)(x0);
            out++;
        }
        for(unsigned ix= croppedZoomedWidth; ix< croppedWidth; ix++) {
            out[0]= 0;
            out++;
        }
    }
}



//
// lookup table creation
//
void
createLookupTablesFromRGBMask(
    u32  rMask,
    u32  gMask,
    u32  bMask,
    u32 *redLookup,
    u32 *greenLookup,
    u32 *blueLookup,
    float rGamma,
    float gGamma,
    float bGamma
)
{
    u8 redDisplacement= 0;
    u8 greenDisplacement= 0;
    u8 blueDisplacement= 0;

    while((rMask&0x80000000)== 0) {
      rMask<<= 1;
      redDisplacement++;
    }

    while((gMask&0x80000000)== 0) {
      gMask<<= 1;
      greenDisplacement++;
    }

    while((bMask&0x80000000)== 0) {
      bMask<<= 1;
      blueDisplacement++;
    }

    float rExp= 1/rGamma;
    float gExp= 1/gGamma;
    float bExp= 1/bGamma;
    for(int i= 0; i< 256; i++) {
      unsigned r= unsigned(255.0*pow(float(i)/255.0, rExp));
      unsigned g= unsigned(255.0*pow(float(i)/255.0, gExp));
      unsigned b= unsigned(255.0*pow(float(i)/255.0, bExp));
      redLookup  [i]= ((r<<24)&rMask)>> redDisplacement;
      greenLookup[i]= ((g<<24)&gMask)>> greenDisplacement;
      blueLookup [i]= ((b<<24)&bMask)>> blueDisplacement;
    }
}

void
createPaletteFromRGBMask(
    u32          rMask,
    u32          gMask,
    u32          bMask,
    clutColor_t *pal
)
{
    u8 redDisplacement= 0;
    u8 greenDisplacement= 0;
    u8 blueDisplacement= 0;


    while((rMask&0x80000000)== 0) {
      rMask<<= 1;
      redDisplacement++;
    }
    while((gMask&0x80000000)== 0) {
      gMask<<= 1;
      greenDisplacement++;
    }
    while((bMask&0x80000000)== 0) {
      bMask<<= 1;
      blueDisplacement++;
    }

    rMask>>= redDisplacement;
    gMask>>= greenDisplacement;
    bMask>>= blueDisplacement;

    for(unsigned i= 0; i< 256; i++) {
      u32 rBits= i&rMask;
      u32 gBits= i&gMask;
      u32 bBits= i&bMask;

      rBits<<= redDisplacement;
      gBits<<= greenDisplacement;
      bBits<<= blueDisplacement;

//      rBits|= (rBits>>3);
//      gBits|= (gBits>>3);
//      bBits|= (bBits>>3);

      pal[i].r= (rBits>> 24);
      pal[i].g= (gBits>> 24);
      pal[i].b= (bBits>> 24);
    }
}

