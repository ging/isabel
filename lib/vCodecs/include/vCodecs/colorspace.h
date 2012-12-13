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
// $Id: colorspace.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vcodecs_colorspace_h__
#define __vcodecs_colorspace_h__

#include <vCodecs/codec.h>

class colorspace_t
{
private:
    long int RY[256];
    long int GY[256];
    long int BY[256];
    long int RCr[256];
    long int GCr[256];
    long int BCr[256];
    long int RCb[256];
    long int GCb[256];
    long int BCb[256];
    long int YRGB[256];
    long int CrR[256];
    long int CrG[256];
    long int CbG[256];
    long int CbB[256];

    vDecoderArgs_t decoderArgs;
   
public:
    colorspace_t(void);
    ~colorspace_t(void);

    void YUV422itoRGB24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV422itoBGR24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV422PtoRGB24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV422PtoBGR24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV420PtoRGB24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV420PtoBGR24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV411PtoRGB24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV411PtoBGR24(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void RGB555toRGB24(unsigned char *dst,
                       int &dstWidth,
                       int &dstHeight,
                       unsigned char *src,
                       int srcWidth,
                       int srcHeight
                      );

    void RGB555toYUV420P(unsigned char *dst,
                         int &dstWidth,
                         int &dstHeight,
                         unsigned char *src,
                         int srcWidth,
                         int srcHeight
                        );

    void RGB565toRGB24(unsigned char *dst,
                       int &dstWidth,
                       int &dstHeight,
                       unsigned char *src,
                       int srcWidth,
                       int srcHeight
                      );

    void RGB565toYUV420P(unsigned char *dst,
                         int &dstWidth,
                         int &dstHeight,
                         unsigned char *src,
                         int srcWidth,
                         int srcHeight
                        );

    void RGB24toRGB555(unsigned char *dst,
                       int &dstWidth,
                       int &dstHeight,
                       unsigned char *src,
                       int srcWidth,
                       int srcHeight
                      );

    void RGB24toRGB565(unsigned char *dst,
                       int &dstWidth,
                       int &dstHeight,
                       unsigned char *src,
                       int srcWidth,
                       int srcHeight
                      );

    void RGB24toYUV422i(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void RGB24toYUV422P(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void RGB24toYUV420P(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void RGB24toYUV411P(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void BGR24toYUV422i(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void BGR24toYUV422P(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void BGR24toYUV420P(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void BGR24toYUV411P(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       );

    void YUV422itoYUV422P(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV422itoYUV420P(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV422PtoYUV422i(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV422PtoYUV420P(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV422PtoYUV411P(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV420PtoYUV422i(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV420PtoYUV411P(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    void YUV411PtoYUV420P(unsigned char *dst,
                          int &dstWidth,
                          int &dstHeight,
                          unsigned char *src,
                          int srcWidth,
                          int srcHeight
                         );

    // converts from RGB24 to BGR24 and viceversa
    void RAW24swap(unsigned char *dst,
                   int &dstWidth,
                   int &dstHeight,
                   unsigned char *src,
                   int srcWidth,
                   int srcHeight
                  );
};

extern colorspace_t* colorspace;

#endif

