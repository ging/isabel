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
// $Id: bmpEnc.cc 10565 2007-07-11 16:18:59Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <icf2/general.h>
#include <icf2/item.hh>

#include "bmpImpl.hh"
#include "bmp.h"

void bmp_t::writeNBytes(const u8* data, int numbytes)
{
    memcpy ( image+bytesWritten, data, numbytes);
    bytesWritten += numbytes;
};


//
// write BMP24 from RAW-RGB24 (no colormaps)
//
u8*
bmp_t::encodeFromRaw(const u8* rawbuf, unsigned w, unsigned h)
{
  bytesWritten = 0;

  int nbits = 24;   // just TrueColor BMP
  int nc = 0;       // no colormap

  // mind the padding
  bperlin = ((w * nbits + 31) / 32) * 4;   /* # bytes written per line */

  int size;
  /* compute size of whole BMP  */
  size = 14 +                /* size of bitmap file header */
         40 +                /* size of bitmap info header */
         (nc * 4) +          /* size of colormap */
         bperlin * h;        /* size of image data */

  image= (u8 *) malloc ( size );

  // start writing BMP
 
  bitMapFileHeader_t bmfh;

  bmfh.bfType     = 19778; //magic number = "BM"
  bmfh.bfSize     = size;
  bmfh.bfxHotSpot = 0;
  bmfh.bfyHotSpot = 0;
  bmfh.bfOffBits  = 14 + 40 + (nc * 4); 
 
  writeNBytes( (u8*) &bmfh.bfType    , sizeof(bmfh.bfType));
  writeNBytes( (u8*) &bmfh.bfSize    , sizeof(bmfh.bfSize));
  writeNBytes( (u8*) &bmfh.bfxHotSpot, sizeof(bmfh.bfxHotSpot));
  writeNBytes( (u8*) &bmfh.bfyHotSpot, sizeof(bmfh.bfyHotSpot));
  writeNBytes( (u8*) &bmfh.bfOffBits , sizeof(bmfh.bfOffBits));

  // write BMP Header

  bitMapInfoHeader_t bmih;

  bmih.biSize         = 40;
  bmih.biWidth        = w;
  bmih.biHeight       = h;
  bmih.biPlanes       = 1;
  bmih.biBitCount     = 24;
  bmih.biCompression  = BI_RGB;
  bmih.biSizeImage    = bperlin*h;
  bmih.biXPelsPerMeter= 75 * 39;     // ask XV-code ?
  bmih.biYPelsPerMeter= 75 * 39;
  bmih.biClrUsed      = nc;
  bmih.biClrImportant = nc;

  /*
  writeNBytes( (u8*) &bmih.biSize          , sizeof(bmih.biSize          ));
  writeNBytes( (u8*) &bmih.biWidth         , sizeof(bmih.biWidth         ));
  writeNBytes( (u8*) &bmih.biHeight        , sizeof(bmih.biHeight        ));
  writeNBytes( (u8*) &bmih.biPlanes        , sizeof(bmih.biPlanes        ));
  writeNBytes( (u8*) &bmih.biBitCount      , sizeof(bmih.biBitCount      ));
  writeNBytes( (u8*) &bmih.biCompression   , sizeof(bmih.biCompression   ));
  writeNBytes( (u8*) &bmih.biSizeImage     , sizeof(bmih.biSizeImage     ));
  writeNBytes( (u8*) &bmih.biXPelsPerMeter , sizeof(bmih.biXPelsPerMeter ));
  writeNBytes( (u8*) &bmih.biYPelsPerMeter , sizeof(bmih.biYPelsPerMeter ));
  writeNBytes( (u8*) &bmih.biClrUsed       , sizeof(bmih.biClrUsed       ));
  writeNBytes( (u8*) &bmih.biClrImportant  , sizeof(bmih.biClrImportant  ));
  */

  // en este caso se puede hacer
  writeNBytes( (u8*) &bmih , sizeof(bmih));

  //putint(fp, 40);         /* biSize: size of bitmap info header */
  //putint(fp, w);          /* biWidth */
  //putint(fp, h);          /* biHeight */
  //putshort(fp, 1);        /* biPlanes:  must be '1' */
  //putshort(fp, nbits);    /* biBitCount: 1,4,8, or 24 */
  //putint(fp, BI_RGB);     /* biCompression:  BI_RGB, BI_RLE8 or BI_RLE4 */
  //putint(fp, bperlin*h);  /* biSizeImage:  size of raw image data */
  //putint(fp, 75 * 39);    /* biXPelsPerMeter: (75dpi * 39" per meter) */
  //putint(fp, 75 * 39);    /* biYPelsPerMeter: (75dpi * 39" per meter) */
  //putint(fp, nc);         /* biClrUsed: # of colors used in cmap */
  //putint(fp, nc);         /* biClrImportant: same as above */


  // Done with headers : write out the image
 
  writeBMP24(rawbuf, w, h);

  return image;

};

void
bmp_t::writeBMP24(const u8* rawbuf, unsigned w , unsigned h)
{

  int bpad = bperlin-3*w;

  u8 *zero_array= new u8[bpad];
  memset(zero_array, 0, bpad);

  for (int j=h-1; j>=0; j--) {
 
        for (unsigned i=0; i<w ; i++) {
                for (int z=2; z>=0; z--)
                        writeNBytes( rawbuf + j*w*3 + (i*3)+z , 1);
                //writeNBytes( rawbuf + j*w*3 + i*3  , 3 );
        };

        // padding at end of line
        writeNBytes( zero_array, bpad );
  };

  delete [] zero_array;
};

u8*
bmp_t::encodeFromClut8(const u8* buf,
                       const u8* colormap,
                       unsigned numColors,
                       unsigned w,
                       unsigned h
                      )
// write BMP8 from CLUT8
{

  bytesWritten = 0;

  int nbits = 8;   //  BMP with 8-bit colors
  int nc    = numColors;      

  // mind the padding
  bperlin = ((w * nbits + 31) / 32) * 4;   /* # bytes written per line */

  int size;
  /* compute size of whole BMP  */
  size = 14 +                /* size of bitmap file header */
         40 +                /* size of bitmap info header */
         (nc * 4) +          /* size of colormap */
         bperlin * h;        /* size of image data */

  image= (u8 *) malloc ( size );

  // start writing BMP
 
  bitMapFileHeader_t bmfh;

  bmfh.bfType     = 19778; //magic number = "BM"
  bmfh.bfSize     = size;
  bmfh.bfxHotSpot = 0;
  bmfh.bfyHotSpot = 0;
  bmfh.bfOffBits  = 14 + 40 + (nc * 4); 
 
  writeNBytes( (u8*) &bmfh.bfType    , sizeof(bmfh.bfType));
  writeNBytes( (u8*) &bmfh.bfSize    , sizeof(bmfh.bfSize));
  writeNBytes( (u8*) &bmfh.bfxHotSpot, sizeof(bmfh.bfxHotSpot));
  writeNBytes( (u8*) &bmfh.bfyHotSpot, sizeof(bmfh.bfyHotSpot));
  writeNBytes( (u8*) &bmfh.bfOffBits , sizeof(bmfh.bfOffBits));

  // write BMP Header

  bitMapInfoHeader_t bmih;

  bmih.biSize         = 40;
  bmih.biWidth        = w;
  bmih.biHeight       = h;
  bmih.biPlanes       = 1;
  bmih.biBitCount     = 8;
  bmih.biCompression  = BI_RGB;
  bmih.biSizeImage    = bperlin*h;
  bmih.biXPelsPerMeter= 75 * 39;     // ask XV-code ?
  bmih.biYPelsPerMeter= 75 * 39;
  bmih.biClrUsed      = nc;
  bmih.biClrImportant = nc;

  /*
  writeNBytes( (u8*) &bmih.biSize          , sizeof(bmih.biSize          ));
  writeNBytes( (u8*) &bmih.biWidth         , sizeof(bmih.biWidth         ));
  writeNBytes( (u8*) &bmih.biHeight        , sizeof(bmih.biHeight        ));
  writeNBytes( (u8*) &bmih.biPlanes        , sizeof(bmih.biPlanes        ));
  writeNBytes( (u8*) &bmih.biBitCount      , sizeof(bmih.biBitCount      ));
  writeNBytes( (u8*) &bmih.biCompression   , sizeof(bmih.biCompression   ));
  writeNBytes( (u8*) &bmih.biSizeImage     , sizeof(bmih.biSizeImage     ));
  writeNBytes( (u8*) &bmih.biXPelsPerMeter , sizeof(bmih.biXPelsPerMeter ));
  writeNBytes( (u8*) &bmih.biYPelsPerMeter , sizeof(bmih.biYPelsPerMeter ));
  writeNBytes( (u8*) &bmih.biClrUsed       , sizeof(bmih.biClrUsed       ));
  writeNBytes( (u8*) &bmih.biClrImportant  , sizeof(bmih.biClrImportant  ));
  */

  // en este caso se puede hacer todo en un bloque
  writeNBytes( (u8*) &bmih , sizeof(bmih));

  // Done with headers : write out the image
 
  writeBMP8( buf, colormap, nc, w, h );

  return image;

}

void
bmp_t::writeBMP8(const u8* buf,
                 const u8* colormap,
                 unsigned numColors,
                 unsigned w,
                 unsigned h
                )
{
  u8 zero= 0;

  // write BitMap ColorMap from CLUT Colormap

  for (unsigned i=0; i < numColors; i++) {
        writeNBytes( (u8*) colormap + i*3, 3);
          writeNBytes( &zero, 1);
  };

  // write image - invert pixel's order
 
  int bpad = bperlin-w;
  u8 *zero_array= new u8[bpad];
  memset(zero_array, 0, bpad);

  for (int j= h-1; j >= 0; j--) {

        writeNBytes((u8*)buf + j*w, w);

        // padding at end of line
        writeNBytes(zero_array, bpad);
  };

  delete []zero_array;
};


u8*
bmp24EncodeFromRawRGB24(u8* rawbuf, unsigned width, unsigned height)
{
    bmp_t bmp;

    //NOTIFY("codificando de RAW a BMP24\n");

    u8* buffer= bmp.encodeFromRaw( rawbuf, width, height );

    return buffer;
};


u8*
bmp8EncodeFromClut8(const u8* buf,
                    const u8* colormap,
                    unsigned numColors,
                    unsigned width,
                    unsigned height
                   )
{
    bmp_t bmp;

    //NOTIFY("codificando de CLUT8 a BMP8\n");

    u8 *buffer= bmp.encodeFromClut8(buf, colormap, numColors, width, height);

    return buffer;
};


