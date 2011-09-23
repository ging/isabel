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
// $Id: bmpDec.cc 10567 2007-07-11 16:53:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/item.hh>

#include "bmpImpl.hh"
#include "bmp.h"

// macros for reading in dwords and words

#define GULONG4(bp) ( (unsigned long)(bp)[0] + \
                     ((unsigned long)(bp)[1] <<  8) + \
                     ((unsigned long)(bp)[2] << 16) + \
                     ((unsigned long)(bp)[3] << 24))

#define GULONG2(bp) ((unsigned long)(bp)[0] + ((unsigned long)(bp)[1] << 8))
#define GUINT2(bp)  ((unsigned int )(bp)[0] + ((unsigned int )(bp)[1] << 8))

bmp_t::bmp_t(void)
: cmap(NULL),
  bytesRead(0)
{
}

bmp_t::~bmp_t (void)
{
    if (cmap != NULL)
    {
        delete [] cmap;
    }
}

int
bmp_t::readNBytes (u8 *buffer, int size)
{
    memcpy(buffer, image + bytesRead , size);
    bytesRead += size;

    return READOK;
}

int
bmp_t::load (u8 *buf, int size)
{
    if (cmap != NULL)
    {
        delete [] cmap;
        cmap= NULL;
    }

    image = buf;

    u8 header[WIN_HEADER_LEN]; // largest we'll need for the header

    // read the file bmp header

    readNBytes(header, FILE_HEADER_LEN);

    // check file header (first 2 bytes == 'BM')

    if ( header[0] != 'B' || header[1] != 'M')
    {
        return NOTBMPFILE;
    }

    // get variables from Header headerfer

    bmfh.bfType     = header[0] << 8  + header[1];
    bmfh.bfSize     = GULONG4( &header[2] );
    bmfh.bfxHotSpot = GUINT2 ( &header[6] );
    bmfh.bfyHotSpot = GUINT2 ( &header[8] );
    bmfh.bfOffBits  = GULONG4( &header[10]);

    // read in size of info header (4 bytes)
    // to find out class of BMP (WIN / OS2)

    readNBytes(header, 4);

    bmih.biSize = GULONG4(&header[0]);

    switch (bmih.biSize) {
    case WIN_HEADER_LEN:
        bmpType = bmp_win;
        // read in the rest of the info header
        readNBytes(header + 4,   WIN_HEADER_LEN - 4);

        bmih.biWidth         = GULONG4(&header[4]);
        bmih.biHeight        = GULONG4(&header[8]);
        bmih.biPlanes        = GUINT2(&header[12]);
        bmih.biBitCount      = GUINT2(&header[14]);
        bmih.biCompression   = GULONG4(&header[16]);
        bmih.biSizeImage     = GULONG4(&header[20]);
        bmih.biXPelsPerMeter = GULONG4(&header[24]);
        bmih.biYPelsPerMeter = GULONG4(&header[28]);
        bmih.biClrUsed       = GULONG4(&header[32]);
        bmih.biClrImportant  = GULONG4(&header[36]);
        break;

    case OS2_HEADER_LEN:
        bmpType = bmp_os2;
        // read in the rest of the info header
        readNBytes(header + 4,   OS2_HEADER_LEN - 4);

        bmih.biWidth         = GULONG2(&header[4]);
        bmih.biHeight        = GULONG2(&header[6]);
        bmih.biPlanes        = GUINT2(&header[8]);
        bmih.biBitCount      = GUINT2(&header[10]);
        bmih.biCompression   = BI_RGB;
        bmih.biSizeImage     = 0;
        bmih.biXPelsPerMeter = 0;
        bmih.biYPelsPerMeter = 0;
        bmih.biClrUsed       = 0;
        bmih.biClrImportant  = 0;
        break;

    default:
        return BADBMPASPECT;
    }

    // done with header read, now check consistency :

    if (   bmih.biBitCount != 1 && bmih.biBitCount != 4
        && bmih.biBitCount != 8 && bmih.biBitCount != 24)
    {
        return BADBITCOUNT;
    }

    if ( (     bmih.biCompression != BI_RGB
           &&  bmih.biCompression != BI_RLE8
           &&  bmih.biCompression != BI_RLE4
         )
         || (bmih.biCompression == BI_RLE8 && bmih.biBitCount != 8)
         || (bmih.biCompression == BI_RLE4 && bmih.biBitCount != 4)
       )
    {
        return BADCOMPRESSION;
    }

    if (bmih.biPlanes != 1)
    {
        return BADPLANES;
    }

    debugMsg (dbg_App_Normal,
              "bmp_t",
              "LoadBMP:\tbfSize=%d, bfOffBits=%d",
              bmfh.bfSize,
              bmfh.bfOffBits
             );
    debugMsg (dbg_App_Normal,
              "bmp_t",
              "\t\tbiSize=%d, biWidth=%d, biHeight=%d,biPlanes=%d",
              bmih.biSize,
              bmih.biWidth,
              bmih.biHeight,
              bmih.biPlanes
             );
    debugMsg (dbg_App_Normal,
              "bmp_t",
              "\t\tbiBitCount=%d, biCompression=%d,biSizeImage=%d",
              bmih.biBitCount,
              bmih.biCompression,
              bmih.biSizeImage
             );
    debugMsg (dbg_App_Normal,
              "bmp_t",
              "\t\tbiX,YPelsPerMeter=%d,%d  biClrUsed=%d,biClrImp=%d\n",
              bmih.biXPelsPerMeter,
              bmih.biYPelsPerMeter,
              bmih.biClrUsed,
              bmih.biClrImportant
             );

    // ready to read in colormap, if any

    int ncols= 0;  // number of colors used

    // number of bytes per color (4 in win and 3 in os2)
    int n= (bmpType == bmp_win) ? 4 : 3;

    if (bmih.biBitCount != 24)
    {
        // colors in table
        ncols = (bmih.biClrUsed) ? bmih.biClrUsed : (1 << bmih.biBitCount);

        cmap= new u8[n*ncols];

        // read whole colortable
        if (readNBytes( cmap,  n*ncols) != READOK )
            return ERRORREADCOLORTABLE;

        // if debug , print colors
        /*
        debugMsg (dbg_App_Normal, "bmp_t", "LoadBMP:  BMP colormap:  (RGB order)\n");
        for (int i=0; i<ncols; i++)
        {
            debugMsg (dbg_App_Normal,
                      "bmp_t",
                      "%02x%02x%02x  ",
                      cmap[i+2],
                      cmap[i+1],
                      cmap[i+0]
                     );
        }
        */

    }
    // done with colormap

    // read byte pad to beginning of pixel data, if any

    int bpad;
    if (( bpad= bmfh.bfOffBits - ( FILE_HEADER_LEN + bmih.biSize + n*ncols )) > 0)
        bytesRead += bpad;

    // raster points to beginning of pixels
    raster = buf + bytesRead;

    return BMPOK;
}

u8*
bmp_t::decodeToRGB24 (void)
{
    int pixel;
    int padw;
    int bitnum;
    u8 *pi;

    // the raw image will be buffered in pic
    //u8 *pic= u8 new[ bmih.biWidth * bmih.biHeight * 3 ];
    // no compila de repente !!!!!!!!!

    u8 *pic= (u8*) malloc( bmih.biWidth * bmih.biHeight * 3 );
    u8 *p=pic;

    int Bpp= (bmpType == bmp_win) ? 4 : 3;

    switch (bmih.biBitCount)
    {
    case 1:
        // Width padded to be a multiple of 32
        padw = ((bmih.biWidth + 31)/32) * 32;

        for (int i= bmih.biHeight-1; i >= 0; i--)
        {
            pi = raster + (i* padw / 8);

            bitnum=0;
            for (unsigned j= 0; j < bmih.biWidth; j++)
            {
                if (bitnum < 7)
                {
                    pixel = (*pi >> (7-bitnum)) & 1;
                    bitnum++;
                }
                else
                {
                    pixel = *pi++ & 1;
                    bitnum = 0;
                }

                *p++= cmap[Bpp*pixel+2];
                *p++= cmap[Bpp*pixel+1];
                *p++= cmap[Bpp*pixel+0];
            }
        }
        break;
    case 4:
        if (bmih.biCompression == BI_RLE4)
        {
            u8 *pic8= (u8*) malloc( bmih.biWidth * bmih.biHeight);

            rle4_2_bmp(pic8);

            // here pic8 contains clut pixels ( not BMP pixels)
            // pi recorre estos pixels
            pi=pic8;
            for (unsigned j= 0; j < bmih.biWidth * bmih.biHeight; j++)
            {
                pixel = *pi++;
                // p recorre raw field

                *p++= cmap[Bpp*pixel+2];
                *p++= cmap[Bpp*pixel+1];
                *p++= cmap[Bpp*pixel+0];
            }

            free(pic8);
        }
        else
        {
            // Width padded to a multiple of 8pix (32 bits)
            padw = ((bmih.biWidth + 7)/8) * 8;

            for (int i= bmih.biHeight-1; i >= 0; i--)
            {
                pi = raster + (i * padw / 2 );

                unsigned j, nybnum= 0;
                for (j= nybnum= 0; j < bmih.biWidth; j++, nybnum++)
                {
                    if ((nybnum & 1) == 0)
                    {
                         pixel = (*pi & 0xF0) >> 4;
                         nybnum = 0;
                    }
                    else
                    {
                         pixel = (*pi++ & 0x0F);
                    }

                    *p++= cmap[Bpp*pixel+2];
                    *p++= cmap[Bpp*pixel+1];
                    *p++= cmap[Bpp*pixel+0];
                }
            }
        }
        break;
    case 8:
        if (bmih.biCompression == BI_RLE8)
        {
            u8 *pic8= (u8*) malloc( bmih.biWidth * bmih.biHeight);

            rle8_2_bmp(pic8);

            // here pic8 contains clut pixels ( not BMP pixels)
            pi=pic8;
            for (unsigned j= 0; j < bmih.biWidth * bmih.biHeight; j++)
            {
                pixel = *pi++;

                *p++= cmap[Bpp*pixel+2];
                *p++= cmap[Bpp*pixel+1];
                *p++= cmap[Bpp*pixel+0];
            }

            free(pic8);
        }
        else
        {
            // Width padded to a multiple of 4pix (32 bits)
            padw = ((bmih.biWidth + 3)/4) * 4;

            for (int i= bmih.biHeight-1; i >= 0; i--)
            {
                pi = raster + (i * padw);

                for (unsigned j= 0; j < bmih.biWidth; j++)
                {
                    pixel = *pi++;

                    *p++= cmap[Bpp*pixel+2];
                    *p++= cmap[Bpp*pixel+1];
                    *p++= cmap[Bpp*pixel+0];
                }
            }
        }
        break;
    case 24:  // truecolor
        int padb = (4 - ((bmih.biWidth*3) % 4)) & 0x03;

        // # of pad bytes to read at EOscanline

        for (int i= bmih.biHeight-1; i >= 0; i--)
        {
            pi = raster + (i * (bmih.biWidth * 3 + padb));

            for (unsigned j= 0; j < bmih.biWidth; j++)
            {
                *p++= *(pi + 2);
                *p++= *(pi + 1);
                *p++= *pi;

                pi += 3;
            }
        }
        break;
    }

    return pic;
}


void bmp_t::rle4_2_bmp(u8 *pic8)
{
    u8 *pp;            // recorre pic8
    u8 c,c1;
    u8 *p=raster;      // recorre raster (BMP)

    int i;
    int x;
    int y;
    int h= bmih.biHeight;
    int w= bmih.biWidth;

    x = y = 0;
    pp = pic8 + x + (h-y-1)*w;

    while (y < h)
    {
        c = *p++;

        if (c) // encoded mode
        {
            c1 = *p++;
            for (i=0; i<c; i++,x++,pp++)
                *pp = (i&1) ? (c1 & 0x0f) : ((c1>>4)&0x0f);
        }
        else // c==0x00 :  escape codes
        {
            c = *p++;

            if (c == 0x00) // end of line
            {
                x=0;  y++;  pp = pic8 + x + (h-y-1)*w;
            }

            else if (c == 0x01) break;  // end of pic8

            else if (c == 0x02)         // delta
            {
                c = *p++;  x += c;
                c = *p++;  y += c;
                pp = pic8 + x + (h-y-1)*w;
            }

            else // absolute mode
            {
                for (i=0; i<c; i++, x++, pp++)
                {
                    if ((i&1) == 0) c1 = *p++;
                    *pp = (i&1) ? (c1 & 0x0f) : ((c1>>4)&0x0f);
                }

                if (((c&3)==1) || ((c&3)==2)) p++;  // read pad byte
            }
        }  // escape processing
    }
}

void bmp_t::rle8_2_bmp(u8 *pic8)
{
    u8 *pp;            // recorre pic8
    u8 c,c1;
    u8 *p=raster;      // recorre raster (BMP)

    int i;
    int x;
    int y;
    int h=bmih.biHeight;
    int w=bmih.biWidth;

    x = y = 0;
    pp = pic8 + x + (h-y-1)*w;

    while (y < h)
    {
        c = *p++;

        if (c) // encoded mode
        {
            c1 = *p++;
            for (i= 0; i < c; i++, x++, pp++)
            {
                *pp = c1;
            }
        }
        else // c==0x00  :  escape codes
        {
            c = *p++;

            if (c == 0x00) // end of line
            {
                x=0;  y++;  pp = pic8 + x + (h-y-1)*w;
            }

            else if (c == 0x01) break;               /* end of pic8 */

            else if (c == 0x02) /* delta */
            {
                c = *p++;  x += c;
                c = *p++;  y += c;
                pp = pic8 + x + (h-y-1)*w;
            }

            else // absolute mode
            {
                for (i= 0; i < c; i++, x++, pp++)
                {
                    c1 = *p++;
                    *pp = c1;
                }

                if (c & 1) p++;  /* odd length run: read an extra pad byte */
            }
        }  // escape processing
    }
}


void
bmp_t::printError (int error, char *s)
{
    switch (error)
    {
    case BMPOK:
        strcpy (s, "bmp OK");
        break;
    case READOK:
        strcpy (s, "READ OK");
        break;
    case ERRORREAD:
        strcpy (s, "ERROR reading data");
        break;
    case ERRORREADFILEHEADER:
        strcpy (s, "ERROR reading file header");
        break;
    case ERRORREADINFOHEADER:
        strcpy (s, "ERROR reading info header");
        break;
    case ERRORREADCOLORTABLE:
        strcpy (s, "ERROR reading color table");
        break;
    case BADBITCOUNT:
        strcpy (s, "Bad Bit Count (not 1,4,8,24)");
        break;
    case BADCOMPRESSION:
        strcpy (s, "ERROR unknown compression type");
        break;
    case BADPLANES:
        strcpy (s, "ERROR bitPlanes must be 1");
        break;
    case NOTBMPFILE:
        strcpy (s, "ERROR not bmp file");
        break;
    default:
        strcpy (s, "ERROR UNKOWN");
    }
}


//
// load BMP from filehandle
//
image_t*
bmpLoadFile(FILE *f)
{
    size_t filesize;

    // find the size of the file
    fseek(f, 0L, SEEK_SET);  // unnecessary
    fseek(f, 0L, SEEK_END);
    filesize= ftell(f);
    fseek(f, 0L, SEEK_SET);

    u8 *buffer= (u8*)malloc (filesize);
    if ( ! buffer)
    {
        NOTIFY("bmpLoadFile: run out of memory\n");
        return NULL;
    }

    // fill buffer
    if (fread (buffer, 1, filesize, f) != filesize)
    {
        NOTIFY("bmpLoadFile: error reading %d bytes from file [%s]\n",
               filesize,
               strerror(errno)
              );
        free(buffer);
        return NULL;
    };

    bmp_t bmp;
    int res= bmp.load(buffer, filesize);

    if (res != bmp_t::BMPOK)
    {
        char sres[1024];
        bmp.printError(res, sres);
        NOTIFY("bmpLoadFile: failed to load file, error=%s\n", sres);

        free(buffer);

        return NULL;
    }

    int w= bmp.getWidth();
    int h= bmp.getHeight();

    u8 *buff= bmp.decodeToRGB24();
    image_t *img= new image_t(buff,
                              3 * w * h,
                              RGB24_FORMAT,
                              w,
                              h,
                              0
                             );

    free(buff);
    free(buffer);

    return img;
}

