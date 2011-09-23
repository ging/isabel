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
// $Id: image.cc 10825 2007-09-17 10:23:25Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/icfTime.hh>

#include "image.hh"

#define SAVE_TO_FILES 0

#if SAVE_TO_FILES
static int numSeq=0;
#endif


image_t*
genImageRAW24(u8 *buffer,
              unsigned width,
              unsigned height,
              unsigned &posX,
              unsigned &posY,
              unsigned side,
              int &signoX,
              int &signoY,
              bool doBGR,
              u32 timestamp
             )
{
    memset(buffer, 0, width*height*3);

    for (unsigned i= posX; i < posX+side; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            for (unsigned rgb= 2; rgb < 3; rgb++)
            {
                buffer[(i+j*width)*3+rgb]= 255;
            }
        }
    }

    if (posX+side >= width)  signoX= -1;
    if (posX <= 0) signoX= 1;
    if (posY+side >= height) signoY= -1;
    if (posY <= 0) signoY= 1;

    posX+= signoX;
    posY+= signoY;

#if SAVE_TO_FILES
    char head[256], fileName[1024];
    unsigned __Width= width;
    unsigned __Height= height;

    sprintf (fileName, "/tmp/kk%03d.ppm", numSeq++);
    FILE *out= fopen (fileName, "w");

    // imprime en formato P6
    sprintf (head, "P6\n%d %d\n255\n", __Width, __Height);
    fwrite (head, 1, strlen (head), out);

    unsigned npixels= __Width * __Height;

    fwrite(buffer, 1, 3*npixels, out);

    fclose (out);

#endif

    image_t *img= new image_t(buffer,
                              3 * width * height,
                              doBGR ? BGR24_FORMAT : RGB24_FORMAT,
                              width,
                              height,
                              timestamp
                             );

    return img;
}

image_t*
genImageYUV422i(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               )
{
    unsigned size= width * height;

    u8  *buf8 = buffer;
    for (unsigned i= 0; i < size / 2; i++)
    {
        *buf8=  16; buf8++;
        *buf8= 128; buf8++;
        *buf8=  16; buf8++;
        *buf8= 128; buf8++;
    }

    for (unsigned i= posX; i < posX+side; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            buffer[2*(i+j*width)]= 41;
        }
    }
    for (unsigned i= posX/2; i < (posX+side)/2; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            buffer[4*(i+j*width/2)+1]= 239;
        }
    }
    //for (unsigned i= posX/2; i < (posX+side)/2; i++)
    //{
    //    for (unsigned j= posY; j < posY+side; j++)
    //    {
    //        buffer[4*(i+j*width/2)+3]= 128;
    //    }
    //}

    if (posX+side >= width)  signoX= -1;
    if (posX <= 0) signoX= 1;
    if (posY+side >= height) signoY= -1;
    if (posY <= 0) signoY= 1;

    posX+= signoX;
    posY+= signoY;

    image_t *img= new image_t(buffer,
                              size * 2,
                              I422i_FORMAT,
                              width,
                              height,
                              timestamp
                             );

    return img;
}

image_t*
genImageYUV422P(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               )
{
    unsigned size= width * height;

    u8 *lum  = buffer;
    u8 *cromU= buffer + size;
    u8 *cromV= buffer + size + size / 2;

    memset(lum,    16, size);
    memset(cromU, 128, size/2);
    memset(cromV, 128, size/2);

    for (unsigned i= posX; i < posX+side; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            lum[i+j*width]= 41;
        }
    }
    for (unsigned i= posX/2; i < (posX+side)/2; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            cromU[i+j*width/2]= 239;
        }
    }
    //for (unsigned i= posX/2; i < (posX+side)/2; i++)
    //{
    //    for (unsigned j= posY; j < posY+side; j++)
    //    {
    //        cromV[i+j*width/2]= 128;
    //    }
    //}

    if (posX+side >= width)  signoX= -1;
    if (posX <= 0) signoX= 1;
    if (posY+side >= height) signoY= -1;
    if (posY <= 0) signoY= 1;

    posX+= signoX;
    posY+= signoY;

    image_t *img= new image_t(buffer,
                              size * 2, 
                              I422P_FORMAT,
                              width,
                              height,
                              timestamp
                             );

    return img;
}

image_t*
genImageYUV420P(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               )
{
    unsigned size= width * height;

    u8 *lum  = buffer;
    u8 *cromU= buffer + size;
    u8 *cromV= buffer + size + size / 4;

#if 1
    for (unsigned j= 0; j < height; j++)
    {
        for (unsigned i= 0; i < width; i++)
        {
            lum[j * width + i] = i + j + posX * 3;
        }
    }

    for (unsigned j= 0; j < height; j++)
    {
        for (unsigned i= 0; i < width/4; i++)
        {
            cromU[j * width/4 + i] = 128 + j + posX * 2;
            cromV[j * width/4 + i] =  64 + i + posX * 5;
        }
    }

    posX++;
    if (posX >= 512)
    {
        posX = 0;
    }
#else
    memset(lum,    16, size);
    memset(cromU, 128, size / 4);
    memset(cromV, 128, size / 4);

    for (unsigned i= posX; i < posX+side; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            lum[i+j*width]= 41;
        }
    }
    for (unsigned i= posX/2; i < (posX+side)/2; i++)
    {
        for (unsigned j= posY/2; j < (posY+side)/2; j++)
        {
            cromU[i+j*width/2]= 239;
        }
    }
    //for (unsigned i= posX/2; i < (posX+side)/2; i++)
    //{
    //    for (unsigned j= posY/2; j < (posY+side)/2; j++)
    //    {
    //        cromV[i+j*width/2]= 128;
    //    }
    //}

    if (posX+side >= width)  signoX= -1;
    if (posX <= 0) signoX= 1;
    if (posY+side >= height) signoY= -1;
    if (posY <= 0) signoY= 1;

    posX+= signoX;
    posY+= signoY;
#endif

    image_t *img= new image_t(buffer,
                              size * 3 / 2,
                              I420P_FORMAT,
                              width,
                              height,
                              timestamp
                             );

    return img;
}

image_t*
genImageYUV411P(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               )
{
    unsigned size= width * height;

    u8 *lum  = buffer;
    u8 *cromU= buffer + size;
    u8 *cromV= buffer + size + size / 4;

    memset(lum,    16, size);
    memset(cromU, 128, size / 4);
    memset(cromV, 128, size / 4);

    for (unsigned i= posX; i < posX+side; i++)
    {
        for (unsigned j= posY; j < posY+side; j++)
        {
            lum[i+j*width]= 41;
        }
    }
    for (unsigned i= posX/4; i < (posX+side)/4; i++)
    {
        for (unsigned j= posY; j < (posY+side); j++)
        {
            cromU[i+j*width/4]= 239;
        }
    }
    //for (unsigned i= posX/4; i < (posX+side)/4; i++)
    //{
    //    for (unsigned j= posY; j < (posY+side); j++)
    //    {
    //        cromV[i+j*width/4]= 128;
    //    }
    //}

    if (posX+side >= width)  signoX= -1;
    if (posX <= 0) signoX= 1;
    if (posY+side >= height) signoY= -1;
    if (posY <= 0) signoY= 1;

    posX+= signoX;
    posY+= signoY;

    image_t *img= new image_t(buffer,
                              size * 3 / 2,
                              I411P_FORMAT,
                              width,
                              height,
                              timestamp
                             );

    return img;
}


