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
// $Id: tiffDec.cc 10570 2007-07-12 08:58:20Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <tiffio.h>

#include "tiffImpl.hh"

void
TIFFErrorHndlr(const char *module, const char *fmt, va_list ap)
{
    char buf[8192];

    vsprintf(buf, fmt, ap);

    NOTIFY("TIFF error: %s\n", buf);
}

void
TIFFWarningHndlr(const char *module, const char *fmt, va_list ap)
{
    char buf[8192];

    vsprintf(buf, fmt, ap);

    NOTIFY("TIFF warning: %s\n", buf);
}

image_t*
tiffLoadFile(FILE *f)
{
    TIFF *tif= NULL;
    short bps, spp, photo, orient;
    long unsigned w, h;

    TIFFSetErrorHandler(TIFFErrorHndlr);
    TIFFSetWarningHandler(TIFFWarningHndlr);

    fseek(f, 0L, SEEK_SET);
    fflush(f);

    tif= TIFFFdOpen (fileno(f), "err", "r");
    if (tif == NULL)
    {
        NOTIFY("tiffLoadFile: cannot open TIFF file\n");
        return NULL;
    }

    // flip orientation so that image comes in X order
    TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orient);
    switch (orient)
    {
    case ORIENTATION_TOPLEFT:
    case ORIENTATION_TOPRIGHT:
    case ORIENTATION_LEFTTOP:
    case ORIENTATION_RIGHTTOP: orient = ORIENTATION_BOTLEFT;   break;

    case ORIENTATION_BOTRIGHT:
    case ORIENTATION_BOTLEFT:
    case ORIENTATION_RIGHTBOT:
    case ORIENTATION_LEFTBOT:  orient = ORIENTATION_TOPLEFT;   break;
    }

    TIFFSetField(tif, TIFFTAG_ORIENTATION, orient);

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,  &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bps);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
    TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);

    unsigned numBytes= 4 * w * h; // ALPHA CHANNEL!!
    uint32 *raster= (uint32*)_TIFFmalloc(numBytes);

    if ( ! TIFFReadRGBAImage (tif, w, h, raster, 1))
    {
        NOTIFY("Errors loading TIFF file\n");
        free (raster);
        raster= NULL;
    }

    // pass to RGBA to RGB
    u8 *p= (u8*)raster;
    for (unsigned i= 0; i < w * h; i++)
    {
        p[3*i+0]= p[4*i+0];
        p[3*i+1]= p[4*i+1];
        p[3*i+2]= p[4*i+2];
    }

    image_t *img= new image_t(p,
                              3 * w * h,
                              RGB24_FORMAT,
                              w,
                              h,
                              0
                             );

    free(raster);

    TIFFClose(tif);

    return img;
}

