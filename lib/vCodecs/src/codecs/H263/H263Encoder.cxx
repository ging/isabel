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
// $Id: H263Encoder.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "H263Encoder.hh"
#include "config.hh"

#ifdef WIN32
#include <sys/timeb.h>
static void gettimeofday(timeval *time, void * nada)
{
    _timeb __the_time;
    _ftime(&__the_time);
    time->tv_sec = __the_time.time;
    time->tv_usec = __the_time.millitm*1000;
}
#elif defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
// <sys/time.h> included in H263Encoder.hh
#else
#error "Define gettimeofday in your OS"
#endif


H263Encoder_t::H263Encoder_t(void)
: session(H263CreateEncodeSession()),
  mode(1),
  quality(70.0)
{
    gettimeofday(&time1,NULL);
    time2  = time1;
    BW     = 0;     // 0 bps
    maxBW  = 0;     // VBR by default;
    period = 500.0; // 500 msec.
    Q      = 50;    // Default CBR Q;
}

H263Encoder_t::~H263Encoder_t(void)
{
    H263DestroyEncodeSession(session);
}

int
H263Encoder_t::codifica(unsigned int   width,
                        unsigned int   height,
                        int            bitRate,
                        int            frameRate,
                        int            maxInter,
                        int            quality,
                        unsigned char *inBuff,
                        int            inBuffLen,
                        unsigned char *outBuff,
                        int            outBuffLen
                       )
{
    if ((width%16) || (height%16)) {
        fprintf(stderr,"H263Encoder_t::codifica: %dx%d not divided by 16\n",
               width,
               height
              );
        return -1;
    }

    yuvImage_t *img_yuv = new yuvImage_t;
    img_yuv->w= width;
    img_yuv->h= height;

    int size= width * height;

    img_yuv->lum  = inBuff;
    img_yuv->cromU= inBuff + size;
    img_yuv->cromV= inBuff + size + size / 4;

    //ahora en img_yuv ya la tenemos YUV411P

    H263EncodedImage_t *fr =
        cod.cod_pic(session, img_yuv, mode, maxBW?Q:quality);

    delete (img_yuv);

    if (fr == NULL) {
        fprintf(stderr,"Error al codificar de YUV411P hacia H.263\n");
        return -1;
    }

    //
    // trailer
    //

    if (fr->numBytes <= outBuffLen)
    {
        memcpy(outBuff, fr->buffer, fr->numBytes);
    }
    int numBytes= fr->numBytes;
    delete fr;

    //if CBR coding
    maxBW = bitRate;
    if (maxBW)
    {
        gettimeofday(&time2,NULL);
        double delta = (time2.tv_sec*1000 + time2.tv_usec/1000) -
                       (time1.tv_sec*1000 + time1.tv_usec /1000);

        //update BW
        BW += numBytes*8;
        if (delta>period)
        {
            BW = BW/period*1000.0;
            if (BW>(maxBW*2))
            {
                Q-=20;
            }
            else if (BW>maxBW*3/2)
            {
                Q-=10;
            }
            else if (BW>maxBW)
            {
                Q-=5;
            }
            else if (BW<maxBW)
            {
                Q+=5;
            }
            else if (BW<maxBW*3/2)
            {
                Q+=10;
            }
            else if (BW<maxBW*2)
            {
                Q+=5;
            }
            //check values
            if (Q<0)  Q=0;
            if (Q>100)Q=100;
            //printf("maxBW = %d, BW = %d, Q = %d\n",maxBW,BW,Q);
            //reset estimation values
            BW = 0;
            time1 = time2;
        }
    }

    return numBytes;
}

H263Decoder_t::H263Decoder_t(void)
: session(H263CreateDecodeSession()),
  lost(0)
{
}

H263Decoder_t::~H263Decoder_t(void)
{
    H263DestroyDecodeSession(session);
}

int
H263Decoder_t::decodifica(unsigned int  &width,
                          unsigned int  &height,
                          unsigned char *inBuff,
                          int            inBuffLen,
                          unsigned char *outBuff,
                          int            outBuffLen
                         )
{
    yuvImage_t *img_yuv = dec.dec_pic(session, inBuff);

    if (img_yuv == NULL) {
        fprintf(stderr,"H263Decoder_t::handleImage: "
               "Error decoding from H.263 to YUV\n"
              );
        return -1;
    }
    width = img_yuv->w;
    height= img_yuv->h;

    //
    // trailer
    //
    memcpy(outBuff, img_yuv->lum, width*height);
    outBuff += width*height;
    memcpy(outBuff, img_yuv->cromU, width*height/4);
    outBuff += width*height/4;
    memcpy(outBuff, img_yuv->cromV, width*height/4);

    if (img_yuv->lum)   delete img_yuv->lum;
    if (img_yuv->cromU) delete img_yuv->cromU;
    if (img_yuv->cromV) delete img_yuv->cromV;
    delete img_yuv;

    return 0;
}

