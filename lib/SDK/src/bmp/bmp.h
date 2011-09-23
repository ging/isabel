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
// $Id: bmp.h 10567 2007-07-11 16:53:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __bmp_hh__
#define __bmp_hh__

#define BI_RGB  0        /* Compression type */
#define BI_RLE8 1
#define BI_RLE4 2


#define FILE_HEADER_LEN 14

#define WIN_HEADER_LEN 40
#define OS2_HEADER_LEN 12

#include <icf2/general.h>
#include <icf2/item.hh>

class bmp_t: public item_t {
public:

    bmp_t(void);

    virtual ~bmp_t (void);

    enum bmpType_e {
        bmp_win, bmp_os2
    };

    enum bmpError_e {
        BMPOK,
        READOK,
        ERRORREAD,
        ERRORREADFILEHEADER,
        ERRORREADINFOHEADER,
        ERRORREADCOLORTABLE,
        BADBITCOUNT,
        BADCOMPRESSION,
        BADPLANES,
        BADBMPASPECT,
        NOTBMPFILE
    };


    // Header structure definition

    typedef struct tagBITMAPFILEHEADER {
        u16 bfType;
        u32 bfSize;        // Size of file in bytes
        u16 bfxHotSpot;    // Not used
        u16 bfyHotSpot;    // Not used
        u32 bfOffBits;     // Offset of image bits from start of header

    } bitMapFileHeader_t;

    typedef struct tagBITMAPINFOHEADER {
        u32 biSize;           // Size of info header in bytes
        u32 biWidth;          // Image width in pixels
        u32 biHeight;         // Image height in pixels
        u16 biPlanes;         // Planes. Must == 1
        u16 biBitCount;       // Bits per pixels. Must be 1, 4, 8 or 24
        u32 biCompression;    // Compression type
        u32 biSizeImage;      // Size of image in bytes
        u32 biXPelsPerMeter;  // X pixels per meter
        u32 biYPelsPerMeter;  // Y pixels per meter
        u32 biClrUsed;        // Number of colormap entries (0 == max)
        u32 biClrImportant;   // Number of important colors

    } bitMapInfoHeader_t;

    virtual void printError (int error, char *s);
    virtual int load (u8 *buf, int size);
    virtual u8* decodeToRGB24 (void);
    virtual u8* encodeFromRaw  (const u8* rawbuf, unsigned w, unsigned h);
    virtual u8* encodeFromClut8(const u8* buf,
                                const u8* colmap,
                                unsigned numColors,
                                unsigned w,
                                unsigned h
                               );

    virtual int getSize(void)   { return bytesWritten;  }
    virtual int getWidth(void)  { return bmih.biWidth;  }
    virtual int getHeight(void) { return bmih.biHeight; }

    virtual const char *className(void) const { return "bmp_t"; }

private:

    bmpError_e error;
    bmpType_e  bmpType;

    bitMapFileHeader_t bmfh;
    bitMapInfoHeader_t bmih;
    u8 *cmap;

    u8  *image;   // pointer to whole BMP
    u8  *raster;  // pointer to pixels

    int  readNBytes (u8 *buffer, int size);
    void writeNBytes(const u8 *buffer, int size);
    void writeBMP24 (const u8 *buffer, unsigned w, unsigned h);
    void writeBMP8  (const u8 *buffer,
                     const u8 *colormap,
                     unsigned nc,
                     unsigned w,
                     unsigned h
                    );

    void rle4_2_bmp(u8 *pic8);
    void rle8_2_bmp(u8 *pic8);

    int bytesRead;     // used in load
    int bytesWritten;  // used in encodeFromRaw
    int bperlin;       // bytes per line in BitMap
};

#endif

