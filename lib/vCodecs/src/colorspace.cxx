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
// $Id: colorspace.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <vCodecs/colorspace.h>
#include <vCodecs/codecs.h>


typedef unsigned char u8;
typedef   signed char i8;

colorspace_t * colorspace = new colorspace_t;

//
// Explanation:
// let be a RGB image with size w*h
//
// YUV422P: luma=w*h cromU=w/2*h cromV= w/2*h
//          U,V one per two pixels horizontally
// YUV420P: luma=w*h cromU=w/2*h/2 cromV= w/2*h/2
//          U,V one per four pixels, two horizontally and two vertically
// YUV411P: luma=w*h cromU=w/4*h cromV= w/4*h
//          U,V one per four pixels horizontally
//
// YUV422i: YUV422 interleaved Y U Y V
//
//    <------ w ------>
//  ^ -----------------
//  | |               |
//  | |               |
//    |      RGB      |
//  h |               |
//    |               |
//  | |               |
//  | |               |
//  v -----------------
//
//    <------ w ------>   <- w/2->  <- w/2->
//  ^ -----------------   --------  --------
//  | |               |   |      |  |      |
//  | |               |   |      |  |      |
//    |    YUV422P    |   |      |  |      |
//  h |     (luma)    |   | crom |  | crom |
//    |               |   |  U   |  |  V   |
//  | |               |   |      |  |      |
//  | |               |   |      |  |      |
//  v -----------------   --------  --------
//
//    <------ w ------>   <- w/2->  <- w/2->
//  ^ -----------------   --------  -------- ^
//  | |               |   |      |  |      | |
//  | |               |   | crom |  | crom | h/2
//    |    YUV420P    |   |  U   |  |  V   | |
//  h |     (luma)    |   --------  -------- v
//    |               |
//  | |               |
//  | |               |
//  v -----------------
//
//    <------ w ------>   <w/4>  <w/4>
//  ^ -----------------   ----  ----
//  | |               |   |  |  |  |
//  | |               |   |c |  |c |
//    |    YUV411P    |   |r |  |r |
//  h |     (luma)    |   |o |  |o |
//    |               |   |m |  |m |
//  | |               |   |U |  |V |
//  | |               |   |  |  |  |
//  v -----------------   ----  ----
//

void ConvertYUVtoRGB(u8 *src0,u8 *src1,u8 *src2,u8 *dst_ori,int width,int height);
void ConvertYUVtoBGR(u8 *src0,u8 *src1,u8 *src2,u8 *dst_ori,int width,int height);
void ConvertRGBtoYUV(u8 *src0,u8 *src1,u8 *src2,u8 *dst_ori,int width,int height);
void ConvertBGRtoYUV(u8 *src0,u8 *src1,u8 *src2,u8 *dst_ori,int width,int height);


/** Tablas de conversion utilizadas. Se multiplica todo por 32768 para eliminar los calculos en flotante.
  * Por eso al final se divide por 32768 (desplazando 15 hacia la derecha).
    Y   = 0.257R + 0.504G + 0.098B + 16
    Cb=U= 0.439R - 0.368G - 0.071B + 128
    Cr=V=-0.148R - 0.291G + 0.439B + 128

    R = 1.164(Y-16) + 1.596(Cb - 128)
    G = 1.164(Y-16) - 0.813(Cb - 128) - 0.391(Cr - 128)
    B = 1.164(Y-16)                   + 2.018(Cr - 128)
  */

#define C01 0.257
#define C02 0.504
#define C03 0.098

#define C04 0.439
#define C05 -0.368
#define C06 -0.071

#define C07 -0.148
#define C08 -0.291
#define C09 0.439

#define C10 1.164
#define C11 1.596
#define C12 0.813
#define C13 0.391
#define C14 2.018

colorspace_t::colorspace_t(void)
{
    long int aux;

    for ( int i = 0 ; i < 256 ; i++ )
    {
        aux = (long int)(i*C01*32768); // 0.257*32768
        RY[i]  = aux;
        aux = (long int)(i*C02*32768); // 0.504*32768
        GY[i]  = aux;
        aux = (long int)(i*C03*32768);  // 0.098*32768
        BY[i]  = aux;

        aux = (long int)(i*C04*32768); // 0.439*32768
        RCr[i] = aux;
        aux = (long int)(i*C05*32768);
        GCr[i] = aux;
        aux = (long int)(i*C06*32768);
        BCr[i] = aux;

        aux = (long int)(i*C07*32768); // 0.148*32768
        RCb[i] = aux;
        aux = (long int)(i*C08*32768);
        GCb[i] = aux;
        aux = (long int)(i*C09*32768);
        BCb[i] = aux;

        aux = (long int)((i-16)*C10*32768); // 1.164*32768
        YRGB[i]= aux;

        aux = (long int)((i-128)*C11*32768); // 1.596*32768
        CrR[i] = aux;

        aux = (long int)((i-128)*C12*32768); // 0.813*32768
        CrG[i] = aux;
        aux = (long int)((i-128)*C13*32768); // 0.391*32768
        CbG[i] = aux;

        aux = (long int)((i-128)*C14*32768); // 2.018*32768
        CbB[i] = aux;
    }

    decoderArgs.width = 1024;
    decoderArgs.height=  768;
    decoderArgs.format= I420P_FORMAT;
}

colorspace_t::~colorspace_t(void)
{    
}

void
colorspace_t::YUV422itoRGB24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *RGB= dst;
    // YUYV pattern
    unsigned char *Y= src;
    unsigned char *U= src + 1;
    unsigned char *V= src + 3;

    long int aux, aux1, aux2, aux3;

    for ( int i = 0 ; i < srcSize; i+= 2 )
    {
        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y+= 2;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y+= 2;

        U+=4; V+=4;
    }
}

void
colorspace_t::YUV422itoBGR24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *RGB= dst;
    // YUYV pattern
    unsigned char *Y= src;
    unsigned char *U= src + 1;
    unsigned char *V= src + 3;

    long int aux, aux1, aux2, aux3;

    for ( int i = 0 ; i < srcSize; i+= 2 )
    {
        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[0]= aux3;

        RGB+= 3;
        Y+= 2;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[0]= aux3;

        RGB+= 3;
        Y+= 2;

        U+=4; V+=4;
    }
}

void
colorspace_t::YUV422PtoRGB24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *RGB= dst;
    unsigned char *Y= src;
    unsigned char *U= src + srcSize;
    unsigned char *V= src + srcSize + srcSize / 2;

    long int aux, aux1, aux2, aux3;

    for ( int i = 0 ; i < srcSize; i+= 2 )
    {
        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y++;

        U++; V++;
    }
}

void
colorspace_t::YUV422PtoBGR24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    int srcSize = srcWidth * srcHeight;
    dstWidth = srcWidth;
    dstHeight= srcHeight;

    unsigned char *BGR= dst;
    unsigned char *Y= src;
    unsigned char *U= src + srcSize;
    unsigned char *V= src + srcSize + srcSize / 2;

    long int aux, aux1, aux2, aux3;

    for ( int i = 0 ; i < srcSize; i+= 2 )
    {
        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        BGR[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        BGR[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        BGR[0]= aux3;

        BGR+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        BGR[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        BGR[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        BGR[0]= aux3;

        BGR+= 3;
        Y++;

        U++; V++;
    }
}

void
colorspace_t::YUV420PtoRGB24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    int srcSize = srcWidth * srcHeight;
    dstWidth = srcWidth;
    dstHeight= srcHeight;

    unsigned char *RGB= dst;
    unsigned char *Y= src;
    unsigned char *U= src + srcSize;
    unsigned char *V= src + srcSize + srcSize / 4;

    long int aux, aux1, aux2, aux3;
    unsigned char *Uaux, *Vaux;

    for (int i = 0 ; i < srcHeight; i++ )
    {
        Uaux= U; Vaux= V;
        for (int j = 0 ; j < srcWidth; j+= 2 )
        {
            aux = YRGB[*Y];
            aux1 = (aux + CrR[*V])>>15;
            aux2 = (aux - CrG[*V] - CbG[*U])>>15;
            aux3 = (aux + CbB[*U])>>15;
            if (aux1 > 255) aux1= 255;
            else if (aux1 < 0 ) aux1 = 0;
            RGB[0]= aux1;
            if (aux2 > 255) aux2= 255;
            else if (aux2 < 0 ) aux2 = 0;
            RGB[1]= aux2;
            if (aux3 > 255) aux3= 255;
            else if (aux3 < 0 ) aux3 = 0;
            RGB[2]= aux3;

            RGB+= 3;
            Y++;

            aux = YRGB[*Y];
            aux1 = (aux + CrR[*V])>>15;
            aux2 = (aux - CrG[*V] - CbG[*U])>>15;
            aux3 = (aux + CbB[*U])>>15;
            if (aux1 > 255) aux1= 255;
            else if (aux1 < 0 ) aux1 = 0;
            RGB[0]= aux1;
            if (aux2 > 255) aux2= 255;
            else if (aux2 < 0 ) aux2 = 0;
            RGB[1]= aux2;
            if (aux3 > 255) aux3= 255;
            else if (aux3 < 0 ) aux3 = 0;
            RGB[2]= aux3;

            RGB+= 3;
            Y++;

            U++; V++;
        }
        if ((i % 2) == 0) {
            U= Uaux; V= Vaux;
        }
    }
}

void
colorspace_t::YUV420PtoBGR24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *BGR= dst;
    unsigned char *Y= src;
    unsigned char *U= src + srcSize;
    unsigned char *V= src + srcSize + srcSize / 4;

    long int aux, aux1, aux2, aux3;
    unsigned char *Uaux, *Vaux;

    for (int i = 0 ; i < srcHeight; i++ )
    {
        Uaux= U; Vaux= V;
        for (int j = 0 ; j < srcWidth; j+= 2 )
        {
            aux = YRGB[*Y];
            aux1 = (aux + CrR[*V])>>15;
            aux2 = (aux - CrG[*V] - CbG[*U])>>15;
            aux3 = (aux + CbB[*U])>>15;
            if (aux1 > 255) aux1= 255;
            else if (aux1 < 0 ) aux1 = 0;
            BGR[2]= aux1;
            if (aux2 > 255) aux2= 255;
            else if (aux2 < 0 ) aux2 = 0;
            BGR[1]= aux2;
            if (aux3 > 255) aux3= 255;
            else if (aux3 < 0 ) aux3 = 0;
            BGR[0]= aux3;

            BGR+= 3;
            Y++;

            aux = YRGB[*Y];
            aux1 = (aux + CrR[*V])>>15;
            aux2 = (aux - CrG[*V] - CbG[*U])>>15;
            aux3 = (aux + CbB[*U])>>15;
            if (aux1 > 255) aux1= 255;
            else if (aux1 < 0 ) aux1 = 0;
            BGR[2]= aux1;
            if (aux2 > 255) aux2= 255;
            else if (aux2 < 0 ) aux2 = 0;
            BGR[1]= aux2;
            if (aux3 > 255) aux3= 255;
            else if (aux3 < 0 ) aux3 = 0;
            BGR[0]= aux3;

            BGR+= 3;
            Y++;

            U++; V++;
        }
        if ((i % 2) == 0) {
            U= Uaux; V= Vaux;
        }
    }
}

void
colorspace_t::YUV411PtoRGB24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *RGB= dst;
    unsigned char *Y= src;
    unsigned char *U= src + srcSize;
    unsigned char *V= src + srcSize * 5 / 4;

#if 0
    ConvertYUVtoRGB(Y, U, V, RGB, srcWidth, srcHeight);
#else
    long int aux, aux1, aux2, aux3;

    for ( int i = 0 ; i < srcSize; i+=4 )
    {
        // De cada 4 valores de Y se sacan 4 pixel RGB
        // pero solo se utilizan para ello 1 valor de U y otro de V

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        RGB[0]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        RGB[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        RGB[2]= aux3;

        RGB+= 3;
        Y++;

        U++; V++;
    }
#endif
}

void
colorspace_t::YUV411PtoBGR24(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *BGR= dst;

    unsigned char *Y= src;
    unsigned char *U= src + srcSize;
    unsigned char *V= src + srcSize * 5 / 4;

#if 0
    ConvertYUVtoBGR(Y, U, V, BGR, srcWidth, srcHeight);
#else
    long int aux, aux1, aux2, aux3;

    for ( int i = 0 ; i < srcSize; i+=4 )
    {
        // De cada 4 valores de Y se sacan 4 pixel BGR
        // pero solo se utilizan para ello 1 valor de U y otro de V

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        BGR[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        BGR[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        BGR[0]= aux3;

        BGR+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        BGR[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        BGR[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        BGR[0]= aux3;

        BGR+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        BGR[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        BGR[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        BGR[0]= aux3;

        BGR+= 3;
        Y++;

        aux = YRGB[*Y];
        aux1 = (aux + CrR[*V])>>15;
        aux2 = (aux - CrG[*V] - CbG[*U])>>15;
        aux3 = (aux + CbB[*U])>>15;
        if (aux1 > 255) aux1= 255;
        else if (aux1 < 0 ) aux1 = 0;
        BGR[2]= aux1;
        if (aux2 > 255) aux2= 255;
        else if (aux2 < 0 ) aux2 = 0;
        BGR[1]= aux2;
        if (aux3 > 255) aux3= 255;
        else if (aux3 < 0 ) aux3 = 0;
        BGR[0]= aux3;

        BGR+= 3;
        Y++;

        U++; V++;
    }
#endif
}

#define SCALEBITS 8
#define ONE_HALF  (1 << (SCALEBITS - 1))
#define FIX(x)          ((int) ((x) * (1L<<SCALEBITS) + 0.5))

void
RGB16toRGB24(unsigned char *dst, unsigned char *src, int npixels, bool do555)
{
    // RGB565: rrrrrggg gggbbbbb
    // RGB555: xrrrrrgg gggbbbbb
    unsigned char R, G, B;

    unsigned char Rmask= 0xf8,
                  Gmask= do555 ? 0xf8 : 0xfc,
                  Bmask= 0xf8;
    unsigned char Rbits= do555 ? 7 : 8,
                  Gbits= do555 ? 2 : 3,
                  Bbits= 3;
    unsigned char Rdup= 5,
                  Gdup= do555 ? 5 : 6,
                  Bdup= 5;

    for ( int i = 0 ; i < npixels; i++ )
    {
        unsigned short tmp= *(unsigned short *)src;

        R= Rmask & (tmp >> Rbits);
        R |= R >> Rdup; // fill lower bits repeating higher ones
        *dst++ = R;
        G= Gmask & (tmp >> Gbits);
        G |= G >> Gdup; // fill lower bits repeating higher ones
        *dst++ = G;
        B= Bmask & (tmp << Bbits);
        B |= B >> Bdup; // fill lower bits repeating higher ones
        *dst++ = B;

        src+= 2;
    }
}

void
colorspace_t::RGB565toRGB24(unsigned char *dst,
                            int &dstWidth,
                            int &dstHeight,
                            unsigned char *src,
                            int srcWidth,
                            int srcHeight
                           )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGB565toRGB24");

    dstWidth = srcWidth;
    dstHeight= srcHeight;

    int dstSize = dstWidth * dstHeight;

    RGB16toRGB24(dst, src, dstSize, false);
}

void
RGB16toYUV420P(unsigned char *dst,
               int &dstWidth,
               int &dstHeight,
               unsigned char *src,
               int srcWidth,
               int srcHeight,
               bool do555
              )
{
    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight & ~0x1; // even
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 4;

    int wrap= srcWidth - dstWidth;

    unsigned short RGB;
    unsigned char R, G, B;
    unsigned int Rsum, Gsum, Bsum;

    unsigned char Rmask= 0xf8,
                  Gmask= do555 ? 0xf8 : 0xfc,
                  Bmask= 0xf8;
    unsigned char Rbits= do555 ? 7 : 8,
                  Gbits= do555 ? 2 : 3,
                  Bbits= 3;
    unsigned char Rdup= 5,
                  Gdup= do555 ? 5 : 6,
                  Bdup= 5;

    for ( int j = 0 ; j < dstHeight; j+= 2 )
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            RGB= *(unsigned short*)src;
            R = RGB >> Rbits & Rmask; R |= R >> Rdup; 
            G = RGB >> Gbits & Gmask; G |= G >> Gdup; 
            B = RGB << Bbits & Bmask; B |= B >> Bdup; 

            Y[0]= (FIX(0.29900) * R +
                   FIX(0.58700) * G +
                   FIX(0.11400) * B +
                   ONE_HALF) >> SCALEBITS;

            Rsum= R; Gsum= G; Bsum= B;

            RGB= *(unsigned short*)(src+2);
            R = RGB >> Rbits & Rmask; R |= R >> Rdup; 
            G = RGB >> Gbits & Gmask; G |= G >> Gdup; 
            B = RGB << Bbits & Bmask; B |= B >> Bdup; 

            Y[1]= (FIX(0.29900) * R +
                   FIX(0.58700) * G +
                   FIX(0.11400) * B +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= R; Gsum+= G; Bsum+= B;

            src += 2*srcWidth;
            Y += dstWidth;

            RGB= *(unsigned short*)src;
            R = RGB >> Rbits & Rmask; R |= R >> Rdup; 
            G = RGB >> Gbits & Gmask; G |= G >> Gdup; 
            B = RGB << Bbits & Bmask; B |= B >> Bdup; 

            Y[0]= (FIX(0.29900) * R +
                   FIX(0.58700) * G +
                   FIX(0.11400) * B +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= R; Gsum+= G; Bsum+= B;

            RGB= *(unsigned short*)(src+2);
            R = RGB >> Rbits & Rmask; R |= R >> Rdup; 
            G = RGB >> Gbits & Gmask; G |= G >> Gdup; 
            B = RGB << Bbits & Bmask; B |= B >> Bdup; 

            Y[1]= (FIX(0.29900) * R +
                   FIX(0.58700) * G +
                   FIX(0.11400) * B +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= R; Gsum+= G; Bsum+= B;

            src += (-2*srcWidth + 2 * 2);
            Y += (-dstWidth + 2);

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum
                     - FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U++; V++;
        }
        src += 2*(srcWidth+wrap);
        Y += dstWidth;
    }
}

void
colorspace_t::RGB565toYUV420P(unsigned char *dst,
                              int &dstWidth,
                              int &dstHeight,
                              unsigned char *src,
                              int srcWidth,
                              int srcHeight
                             )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR565toYUV42oP");

    RGB16toYUV420P(dst, dstWidth, dstHeight, src, srcWidth, srcHeight, false);
}

void
colorspace_t::RGB555toRGB24(unsigned char *dst,
                            int &dstWidth,
                            int &dstHeight,
                            unsigned char *src,
                            int srcWidth,
                            int srcHeight
                           )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGB555toRGB24");

    dstWidth = srcWidth;
    dstHeight= srcHeight;

    int dstSize = dstWidth * dstHeight;

    RGB16toRGB24(dst, src, dstSize, true);
}

void
colorspace_t::RGB555toYUV420P(unsigned char *dst,
                              int &dstWidth,
                              int &dstHeight,
                              unsigned char *src,
                              int srcWidth,
                              int srcHeight
                             )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR565toYUV42oP");

    RGB16toYUV420P(dst, dstWidth, dstHeight, src, srcWidth, srcHeight, true);
}

void
colorspace_t::RGB24toRGB555(unsigned char *dst,
                            int &dstWidth,
                            int &dstHeight,
                            unsigned char *src,
                            int srcWidth,
                            int srcHeight
                           )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR24toRGB555");

    dstWidth = srcWidth ;
    dstHeight= srcHeight;

    int dstSize= dstWidth * dstHeight;

    unsigned short *dst16= (unsigned short *)dst;

    for ( int i = 0 ; i < dstSize; i++)
    {
        // RGB555: xrrrrrgg gggbbbbb
        unsigned short R= (unsigned short)(0xf8 & *src++);
        unsigned short G= (unsigned short)(0xf8 & *src++);
        unsigned short B= (unsigned short)(0xf8 & *src++);

        *dst16++=   (R << 10) | (G << 3) | B ;
    }
}

void
colorspace_t::RGB24toRGB565(unsigned char *dst,
                            int &dstWidth,
                            int &dstHeight,
                            unsigned char *src,
                            int srcWidth,
                            int srcHeight
                           )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR24toRGB565");

    dstWidth = srcWidth ;
    dstHeight= srcHeight;

    int dstSize= dstWidth * dstHeight;

    unsigned short *dst16= (unsigned short *)dst;

    for ( int i = 0 ; i < dstSize; i++)
    {
        // rrrrrggg gggbbbbb
        unsigned short R= (unsigned short)(0xf8 & *src++);
        unsigned short G= (unsigned short)(0xfc & *src++);
        unsigned short B= (unsigned short)(0xf8 & *src++);

        *dst16++=   (R << 11) | (G << 3) | B ;
    }
}

void
colorspace_t::RGB24toYUV422i(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR24toYUV422i");

    unsigned int Rsum, Gsum, Bsum;

    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + 1;
    unsigned char *V= dst + 3;
    unsigned char *RGB= src;

    int wrap= srcWidth - dstWidth;

    for ( int j = 0 ; j < dstHeight; j++)
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= (FIX(0.29900) * RGB[0] +
                   FIX(0.58700) * RGB[1] +
                   FIX(0.11400) * RGB[2] +
                   ONE_HALF) >> SCALEBITS;

            Rsum= RGB[0]; Gsum= RGB[1]; Bsum= RGB[2];

            RGB += 3;
            Y+= 2;

            Y[0]= (FIX(0.29900) * RGB[0] +
                   FIX(0.58700) * RGB[1] +
                   FIX(0.11400) * RGB[2] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= RGB[0]; Gsum+= RGB[1]; Bsum+= RGB[2];

            RGB+= 3;
            Y+= 2;

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum
                     - FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U+= 4; V+= 4;
        }
        RGB += 3*wrap;
    }
}

void
colorspace_t::RGB24toYUV422P(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR24toYUV422P");

    unsigned int Rsum, Gsum, Bsum;

    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 2;
    unsigned char *RGB= src;

    int wrap= srcWidth - dstWidth;

    for ( int j = 0 ; j < dstHeight; j++)
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= (FIX(0.29900) * RGB[0] +
                   FIX(0.58700) * RGB[1] +
                   FIX(0.11400) * RGB[2] +
                   ONE_HALF) >> SCALEBITS;

            Rsum= RGB[0]; Gsum= RGB[1]; Bsum= RGB[2];

            RGB += 3;
            Y++;

            Y[0]= (FIX(0.29900) * RGB[0] +
                   FIX(0.58700) * RGB[1] +
                   FIX(0.11400) * RGB[2] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= RGB[0]; Gsum+= RGB[1]; Bsum+= RGB[2];

            RGB+= 3;
            Y++;

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum
                     - FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U++; V++;
        }
        RGB += 3*wrap;
    }
}

void
colorspace_t::RGB24toYUV420P(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR24toYUV42oP");

    unsigned int Rsum, Gsum, Bsum;

    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight & ~0x1; // even
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 4;
    unsigned char *RGB= src;

    int wrap= srcWidth - dstWidth;

#if 0
    memset(U, 0, dstSize/4);
    memset(V, 0, dstSize/4);
    for ( int j = 0 ; j < dstHeight; j+= 2 )
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= ((  RY[RGB[0]] +  GY[RGB[1]] +  BY[RGB[2]])>>15) +  16;
            Rsum= RGB[0]; Bsum= RGB[1]; Gsum= RGB[2];

            Y[1]= ((  RY[RGB[3]] +  GY[RGB[4]] +  BY[RGB[5]])>>15) +  16;
            Rsum+= RGB[3]; Bsum+= RGB[4]; Gsum+= RGB[5];
            RGB += 3*srcWidth;
            Y += dstWidth;

            Y[0]= ((  RY[RGB[0]] +  GY[RGB[1]] +  BY[RGB[2]])>>15) +  16;
            Rsum+= RGB[0]; Bsum+= RGB[1]; Gsum+= RGB[2];

            Y[1]= ((  RY[RGB[3]] +  GY[RGB[4]] +  BY[RGB[4]])>>15) +  16;
            Rsum+= RGB[3]; Bsum+= RGB[4]; Gsum+= RGB[4];
            RGB += -3*srcWidth + 2 * 3;
            Y += -dstWidth + 1;

            //U[0] =(( RCr[Rsum] + GCr[Gsum] + BCr[Bsum])>>15) + 128;
            //V[0] =(( RCb[Rsum] + GCb[Gsum] + BCb[Bsum])>>15) + 128;

            U++; V++;
        }
        RGB += 3*(srcWidth + wrap);
        Y += dstWidth;
    }
#else
    for ( int j = 0 ; j < dstHeight; j+= 2 )
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= (FIX(0.29900) * RGB[0] +
                   FIX(0.58700) * RGB[1] +
                   FIX(0.11400) * RGB[2] +
                   ONE_HALF) >> SCALEBITS;

            Rsum= RGB[0]; Gsum= RGB[1]; Bsum= RGB[2];

            Y[1]= (FIX(0.29900) * RGB[3] +
                   FIX(0.58700) * RGB[4] +
                   FIX(0.11400) * RGB[5] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= RGB[3]; Gsum+= RGB[4]; Bsum+= RGB[5];

            RGB += 3*srcWidth;
            Y += dstWidth;

            Y[0]= (FIX(0.29900) * RGB[0] +
                   FIX(0.58700) * RGB[1] +
                   FIX(0.11400) * RGB[2] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= RGB[0]; Gsum+= RGB[1]; Bsum+= RGB[2];

            Y[1]= (FIX(0.29900) * RGB[3] +
                   FIX(0.58700) * RGB[4] +
                   FIX(0.11400) * RGB[5] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= RGB[3]; Gsum+= RGB[4]; Bsum+= RGB[5];

            RGB += (-3*srcWidth + 2 * 3);
            Y += (-dstWidth + 2);

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum
                     - FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U++; V++;
        }
        RGB += 3*(srcWidth+wrap);
        Y += dstWidth;
    }
#endif
}

void
colorspace_t::RGB24toYUV411P(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RGBR24toYUV411");

    dstWidth = srcWidth  & ~0x3; // divisible by 4
    dstHeight= srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 4;
    unsigned char *RGB= src;

    int wrap= srcWidth - dstWidth;

#if 0
    ConvertRGBtoYUV(Y, U, V, RGB, width, height);
#else
    for ( int j = 0 ; j < dstHeight; j++ )
    {
        for ( int i = 0 ; i < dstWidth; i+= 4 )
        {
            *Y =((  RY[RGB[0]] +  GY[RGB[1]] +  BY[RGB[2]])>>15) +  16;
            *U =(( RCb[RGB[0]] + GCb[RGB[1]] + BCb[RGB[2]])>>15) + 128;
            *V =(( RCr[RGB[0]] + GCr[RGB[1]] + BCr[RGB[2]])>>15) + 128;

            RGB += 3;
            Y++;

            *Y =((  RY[RGB[0]] +  GY[RGB[1]] +  BY[RGB[2]])>>15) +  16;

            RGB += 3;
            Y++;
            *Y =((  RY[RGB[0]] +  GY[RGB[1]] +  BY[RGB[2]])>>15) +  16;

            RGB += 3;
            Y++;
            *Y =((  RY[RGB[0]] +  GY[RGB[1]] +  BY[RGB[2]])>>15) +  16;

            RGB += 3;
            Y++; U++; V++;
        }
        RGB += 3*wrap;
    }
#endif
}

void
colorspace_t::BGR24toYUV422i(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at BGR24toYUV422i");

    unsigned int Rsum, Gsum, Bsum;

    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + 1;
    unsigned char *V= dst + 3;
    unsigned char *BGR= src;

    int wrap= srcWidth - dstWidth;

    for ( int j = 0 ; j < dstHeight; j++)
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= (FIX(0.29900) * BGR[2] +
                   FIX(0.58700) * BGR[1] +
                   FIX(0.11400) * BGR[0] +
                   ONE_HALF) >> SCALEBITS;

            Rsum= BGR[2]; Gsum= BGR[1]; Bsum= BGR[0];

            BGR += 3;
            Y+= 2;

            Y[0]= (FIX(0.29900) * BGR[2] +
                   FIX(0.58700) * BGR[1] +
                   FIX(0.11400) * BGR[0] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= BGR[2]; Gsum+= BGR[1]; Bsum+= BGR[0];

            BGR+= 3;
            Y+= 2;

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum
                     - FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U+= 4; V+= 4;
        }
        BGR += 3*wrap;
    }
}
void
colorspace_t::BGR24toYUV422P(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at BGRR24toYUV422P");

    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 2;
    unsigned char *BGR= src;

    unsigned int Rsum, Gsum, Bsum;

    int wrap= srcWidth - dstWidth;

    for ( int j = 0 ; j < dstHeight; j++)
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= (FIX(0.29900) * BGR[2] +
                   FIX(0.58700) * BGR[1] +
                   FIX(0.11400) * BGR[0] +
                   ONE_HALF) >> SCALEBITS;

            Rsum= BGR[2]; Gsum= BGR[1]; Bsum= BGR[0];

            BGR += 3;
            Y++;

            Y[0]= (FIX(0.29900) * BGR[2] +
                   FIX(0.58700) * BGR[1] +
                   FIX(0.11400) * BGR[0] +
                   ONE_HALF) >> SCALEBITS;

            Rsum+= BGR[2]; Gsum+= BGR[1]; Bsum+= BGR[0];

            BGR+= 3;
            Y++;

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum -
                       FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U++; V++;
        }
        BGR += 3*wrap;
    }
}

void
colorspace_t::BGR24toYUV420P(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at BGRR24toYUV420P");

    dstWidth = srcWidth  & ~0x1; // even
    dstHeight= srcHeight & ~0x1; // even
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 4;
    unsigned char *BGR= src;

    unsigned int Rsum, Gsum, Bsum;

    int wrap= srcWidth - dstWidth;

    for ( int j = 0 ; j < dstHeight; j+= 2 )
    {
        for ( int i = 0 ; i < dstWidth; i+= 2 )
        {
            Y[0]= (FIX(0.29900) * BGR[2] +
                   FIX(0.58700) * BGR[1] +
                   FIX(0.11400) * BGR[0] + ONE_HALF) >> SCALEBITS;

            Rsum= BGR[2]; Gsum= BGR[1]; Bsum= BGR[0];

            Y[1]= (FIX(0.29900) * BGR[5] +
                   FIX(0.58700) * BGR[4] +
                   FIX(0.11400) * BGR[3] + ONE_HALF) >> SCALEBITS;

            Rsum+= BGR[5]; Gsum+= BGR[4]; Bsum+= BGR[3];

            BGR += 3*srcWidth;
            Y += dstWidth;

            Y[0]= (FIX(0.29900) * BGR[2] +
                   FIX(0.58700) * BGR[1] +
                   FIX(0.11400) * BGR[0] + ONE_HALF) >> SCALEBITS;

            Rsum+= BGR[2]; Gsum+= BGR[1]; Bsum+= BGR[0];

            Y[1]= (FIX(0.29900) * BGR[5] +
                   FIX(0.58700) * BGR[4] +
                   FIX(0.11400) * BGR[3] + ONE_HALF) >> SCALEBITS;

            Rsum+= BGR[5]; Gsum+= BGR[4]; Bsum+= BGR[3];

            BGR += -3*srcWidth + 2 * 3;
            Y += -dstWidth + 2;

            U[0] = ((- FIX(0.16874) * Rsum
                     - FIX(0.33126) * Gsum +
                       FIX(0.50000) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            V[0] = ((  FIX(0.50000) * Rsum
                     - FIX(0.41869) * Gsum -
                       FIX(0.08131) * Bsum +
                       4 * ONE_HALF - 1) >> (SCALEBITS + 2)) + 128;

            U++; V++;
        }
        BGR += 3*(srcWidth+wrap);
        Y += dstWidth;
    }
}

void
colorspace_t::BGR24toYUV411P(unsigned char *dst,
                             int &dstWidth,
                             int &dstHeight,
                             unsigned char *src,
                             int srcWidth,
                             int srcHeight
                            )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at BGRR24toYUV411");

    dstWidth = srcWidth  & ~0x3; // divisible by 4
    dstHeight= srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y= dst;
    unsigned char *U= dst + dstSize;
    unsigned char *V= dst + dstSize + dstSize / 4;
    unsigned char *BGR= src;

    int wrap= srcWidth - dstWidth;

#if 0
    ConvertBGRtoYUV(Y, U, V, BGR, width, height);
#else
    for ( int j = 0 ; j < dstHeight; j++ )
    {
        for ( int i = 0 ; i < dstWidth; i+=4 )
        {
            *Y =((  RY[BGR[2]] +  GY[BGR[1]] +  BY[BGR[0]])>>15) +  16;
            *U =(( RCb[BGR[2]] + GCb[BGR[1]] + BCb[BGR[0]])>>15) + 128;
            *V =(( RCr[BGR[2]] + GCr[BGR[1]] + BCr[BGR[0]])>>15) + 128;

            BGR += 3;
            Y++;

            *Y =((  RY[BGR[2]] +  GY[BGR[1]] +  BY[BGR[0]])>>15) +  16;

            BGR += 3;
            Y++;
            *Y =((  RY[BGR[2]] +  GY[BGR[1]] +  BY[BGR[0]])>>15) +  16;

            BGR += 3;
            Y++;
            *Y =((  RY[BGR[2]] +  GY[BGR[1]] +  BY[BGR[0]])>>15) +  16;

            BGR += 3;
            Y++; U++; V++;
        }
        BGR += 3*wrap;
    }
#endif
}

/*

u8 div3_lookup[768]=
{
  0,   0,   1,   1,   1,   2,   2,   2,
  3,   3,   3,   4,   4,   4,   5,   5,
  5,   6,   6,   6,   7,   7,   7,   8,
  8,   8,   9,   9,   9,  10,  10,  10,
 11,  11,  11,  12,  12,  12,  13,  13,
 13,  14,  14,  14,  15,  15,  15,  16,
 16,  16,  17,  17,  17,  18,  18,  18,
 19,  19,  19,  20,  20,  20,  21,  21,
 21,  22,  22,  22,  23,  23,  23,  24,
 24,  24,  25,  25,  25,  26,  26,  26,
 27,  27,  27,  28,  28,  28,  29,  29,
 29,  30,  30,  30,  31,  31,  31,  32,
 32,  32,  33,  33,  33,  34,  34,  34,
 35,  35,  35,  36,  36,  36,  37,  37,
 37,  38,  38,  38,  39,  39,  39,  40,
 40,  40,  41,  41,  41,  42,  42,  42,
 43,  43,  43,  44,  44,  44,  45,  45,
 45,  46,  46,  46,  47,  47,  47,  48,
 48,  48,  49,  49,  49,  50,  50,  50,
 51,  51,  51,  52,  52,  52,  53,  53,
 53,  54,  54,  54,  55,  55,  55,  56,
 56,  56,  57,  57,  57,  58,  58,  58,
 59,  59,  59,  60,  60,  60,  61,  61,
 61,  62,  62,  62,  63,  63,  63,  64,
 64,  64,  65,  65,  65,  66,  66,  66,
 67,  67,  67,  68,  68,  68,  69,  69,
 69,  70,  70,  70,  71,  71,  71,  72,
 72,  72,  73,  73,  73,  74,  74,  74,
 75,  75,  75,  76,  76,  76,  77,  77,
 77,  78,  78,  78,  79,  79,  79,  80,
 80,  80,  81,  81,  81,  82,  82,  82,
 83,  83,  83,  84,  84,  84,  85,  85,
 85,  86,  86,  86,  87,  87,  87,  88,
 88,  88,  89,  89,  89,  90,  90,  90,
 91,  91,  91,  92,  92,  92,  93,  93,
 93,  94,  94,  94,  95,  95,  95,  96,
 96,  96,  97,  97,  97,  98,  98,  98,
 99,  99,  99, 100, 100, 100, 101, 101,
101, 102, 102, 102, 103, 103, 103, 104,
104, 104, 105, 105, 105, 106, 106, 106,
107, 107, 107, 108, 108, 108, 109, 109,
109, 110, 110, 110, 111, 111, 111, 112,
112, 112, 113, 113, 113, 114, 114, 114,
115, 115, 115, 116, 116, 116, 117, 117,
117, 118, 118, 118, 119, 119, 119, 120,
120, 120, 121, 121, 121, 122, 122, 122,
123, 123, 123, 124, 124, 124, 125, 125,
125, 126, 126, 126, 127, 127, 127, 128,
128, 128, 129, 129, 129, 130, 130, 130,
131, 131, 131, 132, 132, 132, 133, 133,
133, 134, 134, 134, 135, 135, 135, 136,
136, 136, 137, 137, 137, 138, 138, 138,
139, 139, 139, 140, 140, 140, 141, 141,
141, 142, 142, 142, 143, 143, 143, 144,
144, 144, 145, 145, 145, 146, 146, 146,
147, 147, 147, 148, 148, 148, 149, 149,
149, 150, 150, 150, 151, 151, 151, 152,
152, 152, 153, 153, 153, 154, 154, 154,
155, 155, 155, 156, 156, 156, 157, 157,
157, 158, 158, 158, 159, 159, 159, 160,
160, 160, 161, 161, 161, 162, 162, 162,
163, 163, 163, 164, 164, 164, 165, 165,
165, 166, 166, 166, 167, 167, 167, 168,
168, 168, 169, 169, 169, 170, 170, 170,
171, 171, 171, 172, 172, 172, 173, 173,
173, 174, 174, 174, 175, 175, 175, 176,
176, 176, 177, 177, 177, 178, 178, 178,
179, 179, 179, 180, 180, 180, 181, 181,
181, 182, 182, 182, 183, 183, 183, 184,
184, 184, 185, 185, 185, 186, 186, 186,
187, 187, 187, 188, 188, 188, 189, 189,
189, 190, 190, 190, 191, 191, 191, 192,
192, 192, 193, 193, 193, 194, 194, 194,
195, 195, 195, 196, 196, 196, 197, 197,
197, 198, 198, 198, 199, 199, 199, 200,
200, 200, 201, 201, 201, 202, 202, 202,
203, 203, 203, 204, 204, 204, 205, 205,
205, 206, 206, 206, 207, 207, 207, 208,
208, 208, 209, 209, 209, 210, 210, 210,
211, 211, 211, 212, 212, 212, 213, 213,
213, 214, 214, 214, 215, 215, 215, 216,
216, 216, 217, 217, 217, 218, 218, 218,
219, 219, 219, 220, 220, 220, 221, 221,
221, 222, 222, 222, 223, 223, 223, 224,
224, 224, 225, 225, 225, 226, 226, 226,
227, 227, 227, 228, 228, 228, 229, 229,
229, 230, 230, 230, 231, 231, 231, 232,
232, 232, 233, 233, 233, 234, 234, 234,
235, 235, 235, 236, 236, 236, 237, 237,
237, 238, 238, 238, 239, 239, 239, 240,
240, 240, 241, 241, 241, 242, 242, 242,
243, 243, 243, 244, 244, 244, 245, 245,
245, 246, 246, 246, 247, 247, 247, 248,
248, 248, 249, 249, 249, 250, 250, 250,
251, 251, 251, 252, 252, 252, 253, 253,
253, 254, 254, 254, 255, 255, 255, 255
};

#if 1
u_char UV_lookup_base[511]=
{
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   1,   2,
  2,   3,   4,   5,   5,   6,   7,   8,
  8,   9,  10,  11,  11,  12,  13,  14,
 14,  15,  16,  17,  17,  18,  19,  20,
 20,  21,  22,  23,  23,  24,  25,  26,
 26,  27,  28,  29,  29,  30,  31,  32,
 32,  33,  34,  35,  35,  36,  37,  38,
 38,  39,  40,  41,  41,  42,  43,  44,
 44,  45,  46,  47,  47,  48,  49,  50,
 50,  51,  52,  53,  53,  54,  55,  56,
 56,  57,  58,  59,  59,  60,  61,  62,
 62,  63,  64,  65,  65,  66,  67,  68,
 68,  69,  70,  71,  71,  72,  73,  74,
 74,  75,  76,  77,  77,  78,  79,  80,
 80,  81,  82,  83,  83,  84,  85,  86,
 86,  87,  88,  89,  89,  90,  91,  92,
 92,  93,  94,  95,  95,  96,  97,  98,
 98,  99, 100, 101, 101, 102, 103, 104,
104, 105, 106, 107, 107, 108, 109, 110,
110, 111, 112, 113, 113, 114, 115, 116,
116, 117, 118, 119, 119, 120, 121, 122,
122, 123, 124, 125, 125, 126, 127, 128,

128, 129, 130, 131, 131, 132, 133, 134,
134, 135, 136, 137, 137, 138, 139, 140,
140, 141, 142, 143, 143, 144, 145, 146,
146, 147, 148, 149, 149, 150, 151, 152,
152, 153, 154, 155, 155, 156, 157, 158,
158, 159, 160, 161, 161, 162, 163, 164,
164, 165, 166, 167, 167, 168, 169, 170,
170, 171, 172, 173, 173, 174, 175, 176,
176, 177, 178, 179, 179, 180, 181, 182,
182, 183, 184, 185, 185, 186, 187, 188,
188, 189, 190, 191, 191, 192, 193, 194,
194, 195, 196, 197, 197, 198, 199, 200,
200, 201, 202, 203, 203, 204, 205, 206,
206, 207, 208, 209, 209, 210, 211, 212,
212, 213, 214, 215, 215, 216, 217, 218,
218, 219, 220, 221, 221, 222, 223, 224,
224, 225, 226, 227, 227, 228, 229, 230,
230, 231, 232, 233, 233, 234, 235, 236,
236, 237, 238, 239, 239, 240, 241, 242,
242, 243, 244, 245, 245, 246, 247, 248,
248, 249, 250, 251, 251, 252, 253, 254,
254, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255
};
#else
char UV_lookup_base[511]=
{
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -128, -128,
  -128, -128, -128, -128, -128, -128, -127, -126,
  -126, -125, -124, -123, -123, -122, -121, -120,
  -120, -119, -118, -117, -117, -116, -115, -114,
  -114, -113, -112, -111, -111, -110, -109, -108,
  -108, -107, -106, -105, -105, -104, -103, -102,
  -102, -101, -100,  -99,  -99,  -98,  -97,  -96,
   -96,  -95,  -94,  -93,  -93,  -92,  -91,  -90,
   -90,  -89,  -88,  -87,  -87,  -86,  -85,  -84,
   -84,  -83,  -82,  -81,  -81,  -80,  -79,  -78,
   -78,  -77,  -76,  -75,  -75,  -74,  -73,  -72,
   -72,  -71,  -70,  -69,  -69,  -68,  -67,  -66,
   -66,  -65,  -64,  -63,  -63,  -62,  -61,  -60,
   -60,  -59,  -58,  -57,  -57,  -56,  -55,  -54,
   -54,  -53,  -52,  -51,  -51,  -50,  -49,  -48,
   -48,  -47,  -46,  -45,  -45,  -44,  -43,  -42,
   -42,  -41,  -40,  -39,  -39,  -38,  -37,  -36,
   -36,  -35,  -34,  -33,  -33,  -32,  -31,  -30,
   -30,  -29,  -28,  -27,  -27,  -26,  -25,  -24,
   -24,  -23,  -22,  -21,  -21,  -20,  -19,  -18,
   -18,  -17,  -16,  -15,  -15,  -14,  -13,  -12,
   -12,  -11,  -10,   -9,   -9,   -8,   -7,   -6,
    -6,   -5,   -4,   -3,   -3,   -2,   -1,    0,

  0,   1,   2,   3,   3,   4,   5,   6,
  6,   7,   8,   9,   9,  10,  11,  12,
 12,  13,  14,  15,  15,  16,  17,  18,
 18,  19,  20,  21,  21,  22,  23,  24,
 24,  25,  26,  27,  27,  28,  29,  30,
 30,  31,  32,  33,  33,  34,  35,  36,
 36,  37,  38,  39,  39,  40,  41,  42,
 42,  43,  44,  45,  45,  46,  47,  48,
 48,  49,  50,  51,  51,  52,  53,  54,
 54,  55,  56,  57,  57,  58,  59,  60,
 60,  61,  62,  63,  63,  64,  65,  66,
 66,  67,  68,  69,  69,  70,  71,  72,
 72,  73,  74,  75,  75,  76,  77,  78,
 78,  79,  80,  81,  81,  82,  83,  84,
 84,  85,  86,  87,  87,  88,  89,  90,
 90,  91,  92,  93,  93,  94,  95,  96,
 96,  97,  98,  99,  99, 100, 101, 102,
102, 103, 104, 105, 105, 106, 107, 108,
108, 109, 110, 111, 111, 112, 113, 114,
114, 115, 116, 117, 117, 118, 119, 120,
120, 121, 122, 123, 123, 124, 125, 126,
126, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127
};
#endif

u8 *UV_lookup=(u8*)(UV_lookup_base)+256;


void
ConvertRGBtoYUV(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    //
    // actual formula is
    //
    //   Y= 0.3R+0.5G+0.2B
    //   U= 0.493*B-0.493*Y;
    //   V= 0.877*G-0.877*Y;
    //
    // but turboPeg uses:
    //
    //   Y= (R+G+B)/3
    //   U= (B-Y)*0.75
    //   V= (G-Y)*0.75
    //
    u8 *out1= src0;
    u8 *out2= src1;
    u8 *out3= src2;
    u8 *src = dst_ori;
    int myn = 0;
    for (unsigned i= 0; i< height; i+= 2)
    {
        myn++;
        for (unsigned j= 0; j< width; j+= 2)
        {
            int w3 = 3*width;
            out1[  0]= div3_lookup[src[0]+src[1]+src[2]];
            out1[  1]= div3_lookup[src[3]+src[4]+src[5]];
            out1[width+0]= div3_lookup[src[w3+0]+src[w3+1]+src[w3+2]];
            out1[width+1]= div3_lookup[src[w3+3]+src[w3+4]+src[w3+5]];
            out2[  0]= UV_lookup[(src[2]+src[w3+5]-out1[0]-out1[width+1])>>1];  // /2
            out3[  0]= UV_lookup[-(src[1]+src[w3+4]-out1[0]-out1[width+1])>>1];// /-2

            src += 6;//2*3;
            out1+= 2;
            out2+= 1;
            out3+= 1;
        }
        src +=width*3;
        out1+=width;
    }
}

void
ConvertBGRtoYUV(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    //
    // actual formula is
    //
    //   Y= 0.3R+0.5G+0.2B
    //   U= 0.493*B-0.493*Y;
    //   V= 0.877*G-0.877*Y;
    //
    // but turboPeg uses:
    //
    //   Y= (R+G+B)/3
    //   U= (B-Y)*0.75
    //   V= (G-Y)*0.75
    //
    u8 *out1= src0;
    u8 *out2= src1;
    u8 *out3= src2;
    u8 *src = dst_ori;
    for (unsigned i= 0; i< height; i+= 2)
    {
        for (unsigned j= 0; j< width; j+= 2)
        {
            out1[  0]= div3_lookup[src[0]+src[1]+src[2]];
            out1[  1]= div3_lookup[src[3]+src[4]+src[5]];
            out1[width+0]= div3_lookup[src[width*3+0]+src[width*3+1]+src[width*3+2]];
            out1[width+1]= div3_lookup[src[width*3+3]+src[width*3+4]+src[width*3+5]];
            out2[  0]= UV_lookup[(src[2]+src[width*3+5]-out1[0]-out1[width+1])/2];
            out3[  0]= UV_lookup[(src[1]+src[width*3+4]-out1[1]-out1[width+1])/2];

            src += 2*3;
            out1+= 2;
            out2+= 1;
            out3+= 1;
        }
        src +=width*3;
        out1+=width;
    }
}

u8
saturateByte_lookup_base[]=
{
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 1, 2, 3, 4, 5, 6, 7,
8, 9, 10, 11, 12, 13, 14, 15,
16, 17, 18, 19, 20, 21, 22, 23,
24, 25, 26, 27, 28, 29, 30, 31,
32, 33, 34, 35, 36, 37, 38, 39,
40, 41, 42, 43, 44, 45, 46, 47,
48, 49, 50, 51, 52, 53, 54, 55,
56, 57, 58, 59, 60, 61, 62, 63,
64, 65, 66, 67, 68, 69, 70, 71,
72, 73, 74, 75, 76, 77, 78, 79,
80, 81, 82, 83, 84, 85, 86, 87,
88, 89, 90, 91, 92, 93, 94, 95,
96, 97, 98, 99, 100, 101, 102, 103,
104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122, 123, 124, 125, 126, 127,
128, 129, 130, 131, 132, 133, 134, 135,
136, 137, 138, 139, 140, 141, 142, 143,
144, 145, 146, 147, 148, 149, 150, 151,
152, 153, 154, 155, 156, 157, 158, 159,
160, 161, 162, 163, 164, 165, 166, 167,
168, 169, 170, 171, 172, 173, 174, 175,
176, 177, 178, 179, 180, 181, 182, 183,
184, 185, 186, 187, 188, 189, 190, 191,
192, 193, 194, 195, 196, 197, 198, 199,
200, 201, 202, 203, 204, 205, 206, 207,
208, 209, 210, 211, 212, 213, 214, 215,
216, 217, 218, 219, 220, 221, 222, 223,
224, 225, 226, 227, 228, 229, 230, 231,
232, 233, 234, 235, 236, 237, 238, 239,
240, 241, 242, 243, 244, 245, 246, 247,
248, 249, 250, 251, 252, 253, 254, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255
};

u8 *saturateByte_lookup= saturateByte_lookup_base+512;


short UV_lookup_base2[512]=
{
-341, -340, -338, -337, -336, -334, -333, -332,
-330, -329, -328, -326, -325, -324, -322, -321,
-320, -318, -317, -316, -314, -313, -312, -310,
-309, -308, -306, -305, -304, -302, -301, -300,
-298, -297, -296, -294, -293, -292, -290, -289,
-288, -286, -285, -284, -282, -281, -280, -278,
-277, -276, -274, -273, -272, -270, -269, -268,
-266, -265, -264, -262, -261, -260, -258, -257,
-256, -254, -253, -252, -250, -249, -248, -246,
-245, -244, -242, -241, -240, -238, -237, -236,
-234, -233, -232, -230, -229, -228, -226, -225,
-224, -222, -221, -220, -218, -217, -216, -214,
-213, -212, -210, -209, -208, -206, -205, -204,
-202, -201, -200, -198, -197, -196, -194, -193,
-192, -190, -189, -188, -186, -185, -184, -182,
-181, -180, -178, -177, -176, -174, -173, -172,

-170, -169, -168, -166, -165, -164, -162, -161,
-160, -158, -157, -156, -154, -153, -152, -150,
-149, -148, -146, -145, -144, -142, -141, -140,
-138, -137, -136, -134, -133, -132, -130, -129,
-128, -126, -125, -124, -122, -121, -120, -118,
-117, -116, -114, -113, -112, -110, -109, -108,
-106, -105, -104, -102, -101, -100,  -98,  -97,
 -96,  -94,  -93,  -92,  -90,  -89,  -88,  -86,
 -85,  -84,  -82,  -81,  -80,  -78,  -77,  -76,
 -74,  -73,  -72,  -70,  -69,  -68,  -66,  -65,
 -64,  -62,  -61,  -60,  -58,  -57,  -56,  -54,
 -53,  -52,  -50,  -49,  -48,  -46,  -45,  -44,
 -42,  -41,  -40,  -38,  -37,  -36,  -34,  -33,
 -32,  -30,  -29,  -28,  -26,  -25,  -24,  -22,
 -21,  -20,  -18,  -17,  -16,  -14,  -13,  -12,
 -10,   -9,   -8,   -6,   -5,   -4,   -2,   -1,

   0,    1,    2,    4,    5,    6,    8,    9,
  10,   12,   13,   14,   16,   17,   18,   20,
  21,   22,   24,   25,   26,   28,   29,   30,
  32,   33,   34,   36,   37,   38,   40,   41,
  42,   44,   45,   46,   48,   49,   50,   52,
  53,   54,   56,   57,   58,   60,   61,   62,
  64,   65,   66,   68,   69,   70,   72,   73,
  74,   76,   77,   78,   80,   81,   82,   84,
  85,   86,   88,   89,   90,   92,   93,   94,
  96,   97,   98,  100,  101,  102,  104,  105,
 106,  108,  109,  110,  112,  113,  114,  116,
 117,  118,  120,  121,  122,  124,  125,  126,
 128,  129,  130,  132,  133,  134,  136,  137,
 138,  140,  141,  142,  144,  145,  146,  148,
 149,  150,  152,  153,  154,  156,  157,  158,
 160,  161,  162,  164,  165,  166,  168,  169,

 170,  172,  173,  174,  176,  177,  178,  180,
 181,  182,  184,  185,  186,  188,  189,  190,
 192,  193,  194,  196,  197,  198,  200,  201,
 202,  204,  205,  206,  208,  209,  210,  212,
 213,  214,  216,  217,  218,  220,  221,  222,
 224,  225,  226,  228,  229,  230,  232,  233,
 234,  236,  237,  238,  240,  241,  242,  244,
 245,  246,  248,  249,  250,  252,  253,  254,
 256,  257,  258,  260,  261,  262,  264,  265,
 266,  268,  269,  270,  272,  273,  274,  276,
 277,  278,  280,  281,  282,  284,  285,  286,
 288,  289,  290,  292,  293,  294,  296,  297,
 298,  300,  301,  302,  304,  305,  306,  308,
 309,  310,  312,  313,  314,  316,  317,  318,
 320,  321,  322,  324,  325,  326,  328,  329,
 330,  332,  333,  334,  336,  337,  338,  340
};

short *UV_lookup2=UV_lookup_base2+256;


void
ConvertYUVtoRGB(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    //
    // actual formula is
    //
    //   Y= 0.3R+0.5G+0.2B
    //   U= 0.493*B-0.493*Y;
    //   V= 0.877*G-0.877*Y;
    //
    // but turboPeg uses:
    //
    //   Y= (R+G+B)/3   |     R= Y-(4/3)*(U-V)
    //   U= (B-Y)*0.75  | --> B= Y+(4/3)*U
    //   V= (G-Y)*0.75  |     G= Y+(4/3)*V
    //

    u8 const *in1= src0;
    u8 const *in2= src1;
    u8 const *in3= src2;
    u8 *out = dst_ori;
    for (unsigned i= 0; i< height; i+= 2)
    {
        for (unsigned j= 0; j< width; j+= 2)
        {
            int tU= int((in2[0]-128)/0.493);
            int tV= int((in3[0]-128)/0.877);

//            int tU= UV_lookup2[char(in2[0])];
//            int tV= UV_lookup2[char(in3[0])];
            out[0]= saturateByte_lookup[tU+int(in1[0])];
            out[2]= saturateByte_lookup[tV+int(in1[0])];
            out[1]= saturateByte_lookup[int(in1[0])-tU-tV];

            out[3]= saturateByte_lookup[tU+int(in1[1])];
            out[5]= saturateByte_lookup[tV+int(in1[1])];
            out[4]= saturateByte_lookup[int(in1[1])-tU-tV];

            out[3*width+0]= saturateByte_lookup[tU+int(in1[width+0])];
            out[3*width+2]= saturateByte_lookup[tV+int(in1[width+0])];
            out[3*width+1]= saturateByte_lookup[int(in1[width+0])-tU-tV];

            out[3*width+3]= saturateByte_lookup[tU+int(in1[width+1])];
            out[3*width+5]= saturateByte_lookup[tV+int(in1[width+1])];
            out[3*width+4]= saturateByte_lookup[int(in1[width+1])-tU-tV];

            in1 += 2;
            in2 += 1;
            in3 += 1;
            out += 6; //3*2;
        }
        in1+=width;
        out+=width*3;
    }
}

void
tpeg_convert_YUV_to_BGR24(u8 *dst, u8 const *src, unsigned w, unsigned h)
{
    //
    // actual formula is
    //
    //   Y= 0.3R+0.5G+0.2B
    //   U= 0.493*B-0.493*Y;
    //   V= 0.877*G-0.877*Y;
    //
    // but turboPeg uses:
    //
    //   Y= (R+G+B)/3
    //   U= (B-Y)*0.75
    //   V= (G-Y)*0.75
    //
    u8 *out= dst;
    const u8 *in1= src;
    const u8 *in2= src+w*h;
    const u8 *in3= src+w*h+((w*h)>>2);
    for (unsigned i= 0; i< h; i+= 2)
    {
        for (unsigned j= 0; j< w; j+= 2)
        {
            int tU= int((in2[0]-128)/0.493);
            int tV= int((in3[0]-128)/0.877);
            out[2]= tU+in1[0];
            out[1]= tV+in1[0];
            out[0]= in1[0]+in1[0]+in1[0]-out[1]-out[2];

            out[5]= tU+in1[1];
            out[4]= tV+in1[1];
            out[3]= in1[1]+in1[1]+in1[1]-out[4]-out[5];

            out[3*w+2]= tU+in1[w+0];
            out[3*w+1]= tV+in1[w+0];
            out[3*w+0]= in1[w]+in1[w]+in1[w]-out[1]-out[2];

            out[3*w+5]= tU+in1[w+1];
            out[3*w+4]= tV+in1[w+1];
            out[3*w+3]= in1[w+1]+in1[w+1]+in1[w+1]-out[3*w+4]-out[3*w+5];

            in1 += 2;
            in2 += 1;
            in3 += 1;
            out += 3*2;
        }
        in1+=w;
        out+=w*3;
    }
}


*/

void
ConvertYUVtoRGB(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    u8 y11,y21;
    u8 y12,y22;
    i8 u,v;
    int i,j;

    u8 *py1,*py2,*pu,*pv;
    u8 *d1, *d2;

    int aux;


    d1 = dst_ori;
    d2 = d1 + width*3;

    py1 = src0; pu = src1; pv = src2;
    py2 = py1 + width;

    for (j = 0; j < height; j += 2)
    {
//        line j + 0
        for (i = 0; i < width; i += 2)
        {
            u = *pu++ - 128;
            v = *pv++ - 128;

//u=v=0;

            y11 = *py1++;
            y12 = *py1++;

            y21 = *py2++;
            y22 = *py2++;

            aux = (32768*y11 + 45941*v)/32768;
//            aux = (int) (y11 + 1.402*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (int) (32768*y11 - 11272*u - 23396*v)/32768;
//            aux = (int) (y11 - 0.344*u - 0.714*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (int) (32768*y11 + 58065*u)/32768;
//            aux = (int) (y11 + 1.772*u);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (32768*y12 + 45941*v)/32768;
//            aux = (int) (y12 + 1.402*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (int) (32768*y12 - 11272*u - 23396*v)/32768;
//            aux = (int) (y12 - 0.344*u - 0.714*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (int) (32768*y12 + 58065*u)/32768;
//            aux = (int) (y12 + 1.772*u);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (32768*y21 + 45941*v)/32768;
//            aux = (int) (y21 + 1.402*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (int) (32768*y21 - 11272*u - 23396*v)/32768;
//            aux = (int) (y21 - 0.344*u - 0.714*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (int) (32768*y21 + 58065*u)/32768;
//            aux = (int) (y21 + 1.772*u);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (32768*y22 + 45941*v)/32768;
//            aux = (int) (y22 + 1.402*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (int) (32768*y22 - 11272*u - 23396*v)/32768;
//            aux = (int) (y22 - 0.344*u - 0.714*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (int) (32768*y22 + 58065*u)/32768;
//            aux = (int) (y22 + 1.772*u);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;
        }
        d1 += (width * 3);
        d2 += (width * 3);
        py1 += width;
        py2 += width;
    }
}

void
ConvertYUVtoBGR(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    u8 y11,y21;
    u8 y12,y22;
    i8 u,v;
    int i,j;

    u8 *py1,*py2,*pu,*pv;
    u8 *d1, *d2;

    int aux;

    d1 = dst_ori;
    d2 = d1 + width*3;

    py1 = src0; pu = src1; pv = src2;
    py2 = py1 + width;

    for (j = 0; j < height; j += 2)
    {
//        line j + 0
        for (i = 0; i < width; i += 2)
        {
            u = *pu++ - 128;
            v = *pv++ - 128;

//u=v=0;

            y11 = *py1++;
            y12 = *py1++;

            y21 = *py2++;
            y22 = *py2++;
//------------------
            aux = (int) (32768*y11 + 58065*u)/32768;
//            aux = (int) (y11 + 1.772*u);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (int) (32768*y11 - 11272*u - 23396*v)/32768;
//            aux = (int) (y11 - 0.344*u - 0.714*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (32768*y11 + 45941*v)/32768;
//            aux = (int) (y11 + 1.402*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;
//------------------------------

            aux = (int) (32768*y12 + 58065*u)/32768;
//            aux = (int) (y12 + 1.772*u);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (int) (32768*y12 - 11272*u - 23396*v)/32768;
//            aux = (int) (y12 - 0.344*u - 0.714*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;

            aux = (32768*y12 + 45941*v)/32768;
//            aux = (int) (y12 + 1.402*v);
            if      (aux>255) *d1++ = 255;
            else if (aux<0)   *d1++ = 0;
            else              *d1++ = aux;
//---------------------------

            aux = (int) (32768*y21 + 58065*u)/32768;
//            aux = (int) (y21 + 1.772*u);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (int) (32768*y21 - 11272*u - 23396*v)/32768;
//            aux = (int) (y21 - 0.344*u - 0.714*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (32768*y21 + 45941*v)/32768;
//            aux = (int) (y21 + 1.402*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

//---------------------------

            aux = (int) (32768*y22 + 58065*u)/32768;
//            aux = (int) (y22 + 1.772*u);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (int) (32768*y22 - 11272*u - 23396*v)/32768;
//            aux = (int) (y22 - 0.344*u - 0.714*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

            aux = (32768*y22 + 45941*v)/32768;
//            aux = (int) (y22 + 1.402*v);
            if      (aux>255) *d2++ = 255;
            else if (aux<0)   *d2++ = 0;
            else              *d2++ = aux;

//---------------------------
        }
        d1 += (width * 3);
        d2 += (width * 3);
        py1 += width;
        py2 += width;
    }
}

void
ConvertRGBtoYUV(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    int aux;
    int i,j;
    int r,g,b;

    u8 *py1,*py2,*pu,*pv;
    u8 *d1, *d2;

    d1 = dst_ori;
    d2 = d1 + width*3;

    py1 = src0; pu = src1; pv = src2;
    py2 = py1 + width;

    for (j = 0; j < height; j += 2)
    {
//        line j + 0
        for (i = 0; i < width; i+= 2)
        {
            r = *d1++;
            g = *d1++;
            b = *d1++;

            aux = (-5538*r - 10846*g + 16384*b)/32768+128;
//            aux = (int) (-0.169*r - 0.331*g + 0.5*b)    + 128;
            if      (aux>255) *pu++ = 255;
            else if (aux<0)   *pu++ = 0;
            else              *pu++ = aux;

            aux = (16384*r - 13730*g - 2654*b)/32768+128;
//            aux = (int) (0.5*r   - 0.419*g - 0.081*b)  + 128;
            if      (aux>255) *pv++ = 255;
            else if (aux<0)   *pv++ = 0;
            else              *pv++ = aux;


//*pu++=0;
//*pv++=0;
            aux = (9798*r +16941*g +3277*b)/32768;
//            aux = (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py1++ = 255;
            else if (aux<0)   *py1++ = 0;
            else              *py1++ = aux;

            r = *d1++;
            g = *d1++;
            b = *d1++;
            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux =  (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py1++ = 255;
            else if (aux<0)   *py1++ = 0;
            else              *py1++ = aux;

            r = *d2++;
            g = *d2++;
            b = *d2++;

            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux =  (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py2++ = 255;
            else if (aux<0)   *py2++ = 0;
            else              *py2++ = aux;

            r = *d2++;
            g = *d2++;
            b = *d2++;

            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux = (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py2++ = 255;
            else if (aux<0)   *py2++ = 0;
            else              *py2++ = aux;
        }
        d1 += (width * 3);
        d2 += (width * 3);
        py1 += width;
        py2 += width;
    }
}


void
ConvertBGRtoYUV(unsigned char *src0,
                unsigned char *src1,
                unsigned char *src2,
                unsigned char *dst_ori,
                int width,
                int height
               )
{
    int aux;
    int i,j;
    int r,g,b;

    u8 *py1,*py2,*pu,*pv;
    u8 *d1, *d2;


    d1 = dst_ori;
    d2 = d1 + width*3;

    py1 = src0; pu = src1; pv = src2;
    py2 = py1 + width;


    for (j = 0; j < height; j += 2)
    {
//        line j + 0
        for (i = 0; i < width; i+= 2)
        {
            b = *d1++;
            g = *d1++;
            r = *d1++;

            aux = (-5538*r - 10846*g + 16384*b)/32768+128;
//            aux = (int) (-0.169*r - 0.331*g + 0.5*b)    + 128;
            if      (aux>255) *pu++ = 255;
            else if (aux<0)   *pu++ = 0;
            else              *pu++ = aux;

            aux = (16384*r - 13730*g - 2654*b)/32768+128;
//           aux = (int) (0.5*r   - 0.419*g - 0.081*b)  + 128;
            if      (aux>255) *pv++ = 255;
            else if (aux<0)   *pv++ = 0;
            else              *pv++ = aux;


//*pu++=0;
//*pv++=0;

            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux = (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py1++ = 255;
            else if (aux<0)   *py1++ = 0;
            else              *py1++ = aux;

            b = *d1++;
            g = *d1++;
            r = *d1++;

            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux =  (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py1++ = 255;
            else if (aux<0)   *py1++ = 0;
            else              *py1++ = aux;

            b = *d2++;
            g = *d2++;
            r = *d2++;

            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux =  (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py2++ = 255;
            else if (aux<0)   *py2++ = 0;
            else              *py2++ = aux;

            b = *d2++;
            g = *d2++;
            r = *d2++;

            aux = (9798*r + 19235*g + 3736*b)/32768;
//            aux = (int) (0.299*r + 0.587*g + 0.114*b);
            if      (aux>255) *py2++ = 255;
            else if (aux<0)   *py2++ = 0;
            else              *py2++ = aux;
        }
        d1 += (width * 3);
        d2 += (width * 3);
        py1 += width;
        py2 += width;
    }
}


void
colorspace_t::YUV422itoYUV422P(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                               )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV422itoYUV422P");
    assert( ((srcWidth % 2) == 0) && " YUV422itoYUV422P odd width");

    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int dstSize = dstWidth * dstHeight;

    u8 *yuyv= src;

    u8 *dy= dst;
    u8 *du= dst + dstSize;
    u8 *dv= dst + dstSize + dstSize / 2;

    for (int i= 0; i < dstSize / 2; i++)
    {
        u8 b1= *yuyv; yuyv++; // first byte
        u8 b2= *yuyv; yuyv++; // second byte
        u8 b3= *yuyv; yuyv++; // third byte
        u8 b4= *yuyv; yuyv++; // fourth byte

        *dy= b1; dy++;
        *du= b2; du++;
        *dy= b3; dy++;
        *dv= b4; dv++;
    }
}

void
colorspace_t::YUV422itoYUV420P(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                               )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV422itoYUV420P");
    assert( ((srcWidth % 2) == 0) && " YUV422itoYUV420P odd width");

    dstWidth = srcWidth;         // must be event
    dstHeight= srcHeight & ~0x1; // even
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y1= src;
    unsigned char *U1= src + 1;
    unsigned char *V1= src + 3;
    unsigned char *Y2= dst;
    unsigned char *U2= dst + dstSize;
    unsigned char *V2= dst + dstSize + dstSize / 4;


    for (int i= 0; i < dstSize; i++)
    {
        Y2[i]= Y1[2*i];
    }

    u8 *d = U2;
    u8 *o1= U1;
    u8 *o2= U1 + 2*srcWidth;
    for (int i= 0; i < srcHeight/2; i++)
    {
        for (int j= 0; j < srcWidth/2; j++)
        {
            d[0]= (o1[0] + o2[0])/2;
            d++;
            o1+= 4;
            o2+= 4;
        }
        o1+= 2*srcWidth;
        o2+= 2*srcWidth;
    }
    d = V2;
    o1= V1;
    o2= V1 + 2*srcWidth;
    for (int i= 0; i < srcHeight/2; i++)
    {
        for (int j= 0; j < srcWidth/2; j++)
        {
            d[0]= (o1[0] + o2[0])/2;
            d++;
            o1+= 4;
            o2+= 4;
        }
        o1+= 2*srcWidth;
        o2+= 2*srcWidth;
    }
}


void
colorspace_t::YUV422PtoYUV422i(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                               )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV422PtoYUV422i");
    assert( ((srcWidth % 2) == 0) && " YUV422PtoYUV422i odd width");

    dstWidth = srcWidth;
    dstHeight= srcHeight;
    int dstSize = dstWidth * dstHeight;

    u8 *Olum  = src;
    u8 *OcromU= src + dstSize;
    u8 *OcromV= src + dstSize + dstSize / 2;

    u8 *yuyv  = dst;

    for (int i= 0; i < dstSize / 2; i++)
    {
        u8 Y1= *Olum; Olum++;
        u8 Y2= *Olum; Olum++;
        u8 U = *OcromU; OcromU++;
        u8 V = *OcromV; OcromV++;

        *yuyv= Y1; yuyv++;
        *yuyv= U;  yuyv++;
        *yuyv= Y2; yuyv++;
        *yuyv= V;  yuyv++;
    }
}

void
colorspace_t::YUV422PtoYUV420P(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                               )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV422PtoYUV420P");
    assert( ((srcWidth % 2) == 0) && " YUV422PtoYUV420P odd width");

    dstWidth = srcWidth;         // must be even
    dstHeight= srcHeight & ~0x1; // even
    int srcSize = srcWidth * srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y1= src;
    unsigned char *U1= src + srcSize;
    unsigned char *V1= src + srcSize + srcSize / 2;
    unsigned char *Y2= dst;
    unsigned char *U2= dst + dstSize;
    unsigned char *V2= dst + dstSize + dstSize / 4;

    memcpy(Y2, Y1, dstSize);

    u8 *d = U2;
    u8 *o1= U1;
    u8 *o2= U1 + srcWidth/2;
    int i= 0;
    for (i= 0; i < dstHeight/2; i++)
    {
        for (int j= 0; j < dstWidth/2; j++)
        {
            d[0]= (o1[0] + o2[0])/2;
            d++;
            o1++;
            o2++;
        }
        o1+= srcWidth/2;
        o2+= srcWidth/2;
    }
    d = V2;
    o1= V1;
    o2= V1 + srcWidth/2;
    for (i= 0; i < dstHeight/2; i++)
    {
        for (int j= 0; j < dstWidth/2; j++)
        {
            d[0]= (o1[0] + o2[0])/2;
            d++;
            o1++;
            o2++;
        }
        o1+= srcWidth/2;
        o2+= srcWidth/2;
    }
}



void
colorspace_t::YUV422PtoYUV411P(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                              )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV422PtoYUV411P");
    assert( ((srcWidth % 2) == 0) && " YUV422PtoYUV411P odd width");

    dstWidth = srcWidth & ~0x3; // divisible by 4
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *Y1= src;
    unsigned char *U1= src + srcSize;
    unsigned char *V1= src + srcSize + srcSize / 2;
    unsigned char *Y2= dst;
    unsigned char *U2= dst + dstSize;
    unsigned char *V2= dst + dstSize + dstSize / 4;

    u8 *o, *d;

    if (srcWidth == srcHeight)
    {
        memcpy(Y2, Y1, dstSize);
    }
    else
    {
        o= Y1;
        d= Y2;
        for (int j= 0; j < dstHeight; j++)
        {
            memcpy(d, o, dstWidth);
            o += srcWidth;
            d += dstWidth;
        }
    }

    o= U1;
    d= U2;
    for (int i= 0; i < dstSize / 4; i++)
    {
        d[0]= (o[0] + o[1])/2;
        d++;
        o += 2;
    }
    o= V1;
    d= V2;
    for (int i= 0; i < dstSize / 4; i++)
    {
        d[0]= (o[0] + o[1])/2;
        d++;
        o += 2;
    }
}

void
colorspace_t::YUV420PtoYUV422i(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                              )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV420PtoYUV422i");
    assert( ((srcWidth  % 2) == 0) && " YUV420PtoYUV422i odd width");
    assert( ((srcHeight % 2) == 0) && " YUV420PtoYUV422i odd height");

    dstWidth = srcWidth;  // must be even
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;

    unsigned char *oY= src;
    unsigned char *oU= src + srcSize;
    unsigned char *oV= src + srcSize + srcSize / 4;
    unsigned char *dY= dst;
    unsigned char *dU= dst + 1;
    unsigned char *dV= dst + 3;


    for (int i= 0; i < srcSize; i++)
    {
        dY[2*i]= oY[i];
    }

    u8 *d1= dU;
    u8 *d2= dU + 2*srcWidth;
    u8 *o = oU;
    for (int i= 0; i < srcHeight/2; i++)
    {
        for (int j= 0; j < srcWidth/2; j++)
        {
            d1[0]= o[0];
            d2[0]= o[0];
            d1+= 4;
            d2+= 4;
            o++;
        }
        d1+= 2*srcWidth;
        d2+= 2*srcWidth;
    }

    d1= dV;
    d2= dV + 2*srcWidth;
    o = oV;
    for (int i= 0; i < srcHeight/2; i++)
    {
        for (int j= 0; j < srcWidth/2; j++)
        {
            d1[0]= o[0];
            d2[0]= o[0];
            d1+= 4;
            d2+= 4;
            o++;
        }
        d1+= 2*srcWidth;
        d2+= 2*srcWidth;
    }
}

void
colorspace_t::YUV420PtoYUV411P(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                              )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV420PtoYUV411P");
    assert( ((srcWidth  % 2) == 0) && " YUV420PtoYUV411P odd width");
    assert( ((srcHeight % 2) == 0) && " YUV420PtoYUV411P odd height");

    dstWidth = srcWidth & ~0x3; // divisible by 4
    dstHeight= srcHeight;
    int srcSize = srcWidth * srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *oY= src;
    unsigned char *oU= src + srcSize;
    unsigned char *oV= src + srcSize + srcSize / 4;
    unsigned char *dY= dst;
    unsigned char *dU= dst + dstSize;
    unsigned char *dV= dst + dstSize + dstSize / 4;

    if (srcWidth == dstWidth)
    {
        memcpy(dY, oY, dstSize);
    }
    else
    {
        for (int j= 0; j < dstHeight; j++)
        {
            memcpy(dY, oY, dstWidth);
            oY += srcWidth;
            dY += dstWidth;
        }
    }

#if 0
    memcpy(dU, oU, dstSize / 4);

    memcpy(dV, oV, dstSize / 4);
#else
    u8 *d1= dU;
    u8 *d2= dU + srcWidth/4;
    u8 *o = oU;
    for (int i= 0; i < srcHeight/2; i++)
    {
        for (int j= 0; j < srcWidth/4; j++)
        {
            d1[0]= (o[0] + o[1])/2;
            d2[0]= d1[0];
            d1++;
            d2++;
            o += 2;
        }
        d1+= srcWidth/4;
        d2+= srcWidth/4;
    }
    d1= dV;
    d2= dV + srcWidth/4;
    o = oV;
    for (int i= 0; i < srcHeight/2; i++)
    {
        for (int j= 0; j < srcWidth/4; j++)
        {
            d1[0]= (o[0] + o[1])/2;
            d2[0]= d1[0];
            d1++;
            d2++;
            o += 2;
        }
        d1+= srcWidth/4;
        d2+= srcWidth/4;
    }
#endif
}

void
colorspace_t::YUV411PtoYUV420P(unsigned char *dst,
                               int &dstWidth,
                               int &dstHeight,
                               unsigned char *src,
                               int srcWidth,
                               int srcHeight
                               )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at YUV411PtoYUV420P");
    assert( ((srcWidth  % 4) == 0) && " YUV411PtoYUV420P not width % 4");

    dstWidth = srcWidth;         // must be even
    dstHeight= srcHeight & ~0x1; // even
    int srcSize = srcWidth * srcHeight;
    int dstSize = dstWidth * dstHeight;

    unsigned char *oY= src;
    unsigned char *oU= src + srcSize;
    unsigned char *oV= src + srcSize + srcSize / 4;
    unsigned char *dY= dst;
    unsigned char *dU= dst + dstSize;
    unsigned char *dV= dst + dstSize + dstSize / 4;

    memcpy(dY, oY, dstSize);

    u8 *d = dU;
    u8 *o1= oU;
    u8 *o2= oU + srcWidth / 4;
    for (int i= 0; i < dstHeight/2; i++)
    {
        for (int j= 0; j < dstWidth/4; j++)
        {
            d[0]= (o1[0] + o2[0])/2;
            d[1]= d[0]; // what's better?
            d+= 2;
            o1++;
            o2++;
        }
        o1+= srcWidth/4;
        o2+= srcWidth/4;
    }

    d = dV;
    o1= oV;
    o2= oV + srcWidth / 4;
    for (int i= 0; i < dstHeight/2; i++)
    {
        for (int j= 0; j < dstWidth/4; j++)
        {
            d[0]= (o1[0] + o2[0])/2;
            d[1]= d[0]; // what's better?
            d+= 2;
            o1++;
            o2++;
        }
        o1+= srcWidth/4;
        o2+= srcWidth/4;
    }
}

// converts from RGB24 to BGR24 and viceversa
void
colorspace_t::RAW24swap(unsigned char *dst,
                        int &dstWidth,
                        int &dstHeight,
                        unsigned char *src,
                        int srcWidth,
                        int srcHeight
                       )
{
    assert( (srcWidth > 0) && (srcHeight > 0) && " failed at RAW24swap");

    dstWidth = srcWidth;
    dstHeight= srcHeight;

    int dstSize= dstWidth * dstHeight;

    for (int i= 0; i < dstSize; i++)
    {
        dst[2]= src[0];
        dst[1]= src[1];
        dst[0]= src[2];

        src += 3;
        dst += 3;
    }
}

