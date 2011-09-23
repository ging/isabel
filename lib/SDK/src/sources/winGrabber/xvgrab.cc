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
// $Id: xvgrab.cc 58 2001-06-12 09:22:06Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xmd.h>

#include "xvgrab.hh"

int xv_grab=0;

union swapun {
  CARD32 l;
  CARD16 s;
  CARD8  b[sizeof(CARD32)];
};


/**************************************/
int
convertImage(XImage *image,
             XColor *colors,
             int ncolors,
             XWindowAttributes *xwap,
             grabImage_t *gImage,
             struct windowGrabber_t::cached_t &cached
            )
{
  /* attempts to conver the image from whatever weird-ass format it might
     be in into something E-Z to deal with (a 24-bit image).
     Returns '1' on success. */

  /* this code owes a lot to 'xwdtopnm.c', part of the pbmplus package,
     written by Jef Poskanzer */

  Visual         *visual= xwap->visual;
  int             i, j;
  CARD8          *bptr, tmpbyte;
  CARD16         *sptr, sval;
  CARD32         *lptr, lval;
  CARD8          *pptr, *lineptr;
  int            bits_used, bits_per_item, bit_shift, bit_order;
  int            bits_per_pixel;
  CARD32         pixvalue, pixmask;
  CARD32         rval, gval, bval;
  union swapun   sw;

  u8   *grabPic;
  int   gbits;
  int   gWIDE, gHIGH;

  /* quiet compiler warnings */
  sval = 0;
  lval = 0;
  bit_shift = 0;
  pixvalue  = 0;

  if (image->bitmap_unit != 8 && image->bitmap_unit != 16 &&
      image->bitmap_unit != 32) {
    fprintf(stderr, "Can't deal with this display.\n");
    fprintf(stderr, "Returned image bitmap_unit (%d) non-standard.\n",
            image->bitmap_unit
           );
    return 0;
  }

  if (!ncolors && visual->c_class != TrueColor) {
    fprintf(stderr, "Can't deal with this display.\n");
    fprintf(stderr, "Only TrueColor displays can have no colormap.\n");
    return 0;
  }

  /* build the 'global' grabPic stuff */
  gWIDE = image->width;  gHIGH = image->height;

  grabPic = (u8*) malloc((size_t) gWIDE * gHIGH * 3);
  if (!grabPic) {
    fprintf(stderr, "unable to malloc grabPic in convertImage()");
    exit(1);
  }
  pptr = grabPic;

  if (visual->c_class == TrueColor || visual->c_class == DirectColor ||
      ncolors > 256) {
    gbits = 24;
  }
  else {
    gbits = 8;
  }


  bits_per_item = image->bitmap_unit;
  bits_used = bits_per_item;
  bits_per_pixel = image->bits_per_pixel;

  if (bits_per_pixel == 32) pixmask = 0xffffffff;
  else pixmask = (((CARD32) 1) << bits_per_pixel) - 1;

  bit_order = image->bitmap_bit_order;

  for (i=0; i<image->height; i++) {
    lineptr = (u8*) image->data + (i * image->bytes_per_line);
    bptr = ((CARD8  *) lineptr) - 1;
    sptr = ((CARD16 *) lineptr) - 1;
    lptr = ((CARD32 *) lineptr) - 1;
    bits_used = bits_per_item;

    for (j=0; j<image->width; j++) {

      /* get the next pixel value from the image data */

      if (bits_used == bits_per_item) {  /* time to move on to next b/s/l */
        switch (bits_per_item) {
        case 8:  bptr++;  break;
        case 16: sptr++;  sval = *sptr;
                 if (cached.flipBytes) {   /* swap CARD16 */
                   sw.s = sval;
                   tmpbyte = sw.b[0];
                   sw.b[0] = sw.b[1];
                   sw.b[1] = tmpbyte;
                   sval = sw.s;
                 }
                 break;
        case 32: lptr++;  lval = *lptr;
                 if (cached.flipBytes) {   /* swap CARD32 */
                   sw.l = lval;
                   tmpbyte = sw.b[0];
                   sw.b[0] = sw.b[3];
                   sw.b[3] = tmpbyte;
                   tmpbyte = sw.b[1];
                   sw.b[1] = sw.b[2];
                   sw.b[2] = tmpbyte;
                   lval = sw.l;
                 }
                 break;
        }

        bits_used = 0;
        if (bit_order == MSBFirst) bit_shift = bits_per_item - bits_per_pixel;
                              else bit_shift = 0;
      }

      switch (bits_per_item) {
      case 8:  pixvalue = (*bptr >> bit_shift) & pixmask;  break;
      case 16: pixvalue = ( sval >> bit_shift) & pixmask;  break;
      case 32: pixvalue = ( lval >> bit_shift) & pixmask;  break;
      }

      if (bit_order == MSBFirst) bit_shift -= bits_per_pixel;
                            else bit_shift += bits_per_pixel;
      bits_used += bits_per_pixel;

      /* okay, we've got the next pixel value in 'pixvalue' */

      if (visual->c_class == TrueColor || visual->c_class == DirectColor) {
        /* in either case, we have to take the pixvalue and
           break it out into individual r,g,b components */
        rval = (pixvalue & cached.rmask) >> cached.rshift;
        gval = (pixvalue & cached.gmask) >> cached.gshift;
        bval = (pixvalue & cached.bmask) >> cached.bshift;

        if (visual->c_class == DirectColor) {
          /* use rval, gval, bval as indicies into colors array */

          *pptr++ = (rval < (CARD32)ncolors) ? (colors[rval].red   >> 8) : 0;
          *pptr++ = (gval < (CARD32)ncolors) ? (colors[gval].green >> 8) : 0;
          *pptr++ = (bval < (CARD32)ncolors) ? (colors[bval].blue  >> 8) : 0;
        }

        else {   /* TrueColor */
          /* have to map rval,gval,bval into 0-255 range */
          *pptr++ = (cached.r8shift >= 0)
                        ? (rval << cached.r8shift)
                        : (rval >> (-cached.r8shift));
          *pptr++ = (cached.g8shift >= 0)
                        ? (gval << cached.g8shift)
                        : (gval >> (-cached.g8shift));
          *pptr++ = (cached.b8shift >= 0)
                        ? (bval << cached.b8shift)
                        : (bval >> (-cached.b8shift));
        }
      }

      else { /* all others: StaticGray, StaticColor, GrayScale, PseudoColor */
        /* use pixel value as an index into colors array */

        if (pixvalue >= (CARD32)ncolors) {
            fprintf(stderr, "convertImage(): pixvalue >= ncolors");
            //exit(1);
            free(grabPic);
            return 0;
        }

        *pptr++ = (colors[pixvalue].red)   >> 8;
        *pptr++ = (colors[pixvalue].green) >> 8;
        *pptr++ = (colors[pixvalue].blue)  >> 8;
      }
    }
  }

  gImage->grabPic = grabPic;
  gImage->gbits   = 24;
  gImage->gWIDE   = gWIDE;
  gImage->gHIGH   = gHIGH;

  return 1;
}

