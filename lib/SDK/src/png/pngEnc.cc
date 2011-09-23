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
// $Id: pngEnc.cc 10574 2007-07-12 16:08:15Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/item.hh>

#include <png.h>
#include "pngImpl.hh"

#define DEFAULT_FILTER         PNG_FILTER_NONE
#define DEFAULT_COMPRESSION    Z_BEST_COMPRESSION
#define DEFAULT_DISPLAY_GAMMA  2.20

struct pngOptions_t {
    png_byte filter;
    int      compLevel;
    float    gamma;
    int      interlace;
} defaultVals= {DEFAULT_FILTER,
                DEFAULT_COMPRESSION,
                DEFAULT_DISPLAY_GAMMA,
                0
               };

class pngEnc_t:public virtual item_t
{
private:

  u8            *buffer; // the compressed PNG
  long unsigned  __numBytes;
public:
    pngEnc_t (void) {
        buffer= NULL;
        __numBytes= 0;
    };
    ~pngEnc_t (void) {
        __numBytes= 0;  // excessively paranoid
        if (buffer)
            free (buffer);
    };

    unsigned  getNumBytes (void) {
        assert (__numBytes != 0);
        return __numBytes;
    };
    u8       *encodeFromRGB24 (const u8 *pic, unsigned w, unsigned h);

    virtual const char *className(void) const { return "pngEnc"; };
};

/*** local functions ***/

static void
png_my_error(png_struct *png_ptr, char const *message)
{
  fprintf (stderr, " libpng error: %s", message);

  longjmp(png_ptr->jmpbuf, 1);
}

static void
png_my_warning(png_struct *png_ptr, char const *message)
{
  if (!png_ptr)
    return;

  fprintf (stderr, " libpng warning: %s", message);
}

void
write_row_callback (png_structp png_ptr, png_uint_32 row, int pass)
{
    fprintf(stderr,
            "Written on (%p), row=%ld, pass=%d\n",
            png_ptr->io_ptr,
            row,
            pass
           );
}

void
user_write_data (png_structp png_ptr, png_bytep data, png_size_t length)
{
  u8 *p= (u8*)png_ptr->io_ptr;

//fprintf(stderr, "about to write from %p to %p, %d bytes\n", data, p, length);

  assert (data != NULL);
  memcpy (p, data, length);
  p += length;
  png_ptr->io_ptr= (png_voidp)p;

//fprintf(stderr, "written from %p to %p, %d bytes\n", data, p, length);
}

u8*
pngEnc_t::encodeFromRGB24(const u8 *pic, unsigned w, unsigned h)
{
    png_struct *png_ptr;
    png_info   *info_ptr;
    int         i, linesize = 0, numberOfPasses;
    u8         *p, *retVal;

#if 0
    {
        static int pmframe=0;

        char head[4096], outfilename[4096];

        int fd;

        char cname[4096];
        sprintf (cname, "pm%03d.pm", pmframe);
        fprintf (stderr, "salvando a %s\n", cname);

        if ((fd = open (cname, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
            fprintf (stderr, "Error opening output file %s\n", outfilename);
        } else {
            // imprime en formato P6
            sprintf (head, "P6\n%d %d\n255\n", w, h);
            write (fd, head, strlen(head));

            write(fd, pic, 3*w*h);

            close(fd);
        }
        pmframe++;
    }
#endif

//fprintf (stderr, "START encodeFromRGB24 lib PNG version=%s\n", PNG_LIBPNG_VER_STRING);

    if (!(buffer= (u8*) malloc (3*(w*h+(w*h>>8)+32*1024)))) {
        NOTIFY("pngEnc_t::pngEncodeFromRGB24 " "malloc failure");
        return NULL;
    }

    if ((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                           NULL,
                                           png_my_error,
                                           png_my_warning)) == NULL) {
        NOTIFY("pngEnc_t::pngEncodeFromRGB24 " "malloc failure");
        return NULL;
    }

    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        NOTIFY("pngEnc_t::pngEncodeFromRGB24 " "malloc failure");
        return NULL;
    }

    if (setjmp(png_ptr->jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return NULL;
    }

    //png_init_io(png_ptr, fp);
    png_set_write_fn(png_ptr,
                     (png_voidp)buffer, // a lo bestia!
                     user_write_data,
                     NULL
                    );

    //png_ptr->io_ptr= (png_voidp)buffer; // a lo bestia!

    //png_set_write_status_fn (png_ptr, write_row_callback);

    //png_set_filter(png_ptr, 0, defaultVals.filter);

#if 0
    png_set_compression_level(png_ptr, defaultVals.compLevel);
#else
    png_set_compression_level(png_ptr, Z_BEST_SPEED);
#endif

#if 0
    info_ptr->width= w;
    info_ptr->height= h;

    info_ptr->interlace_type= defaultVals.interlace;

    // FULLCOLOR for RGB24
    linesize= 3*w;
    info_ptr->color_type= PNG_COLOR_TYPE_RGB;
    info_ptr->bit_depth= 8;

    info_ptr->gamma= 1.0/defaultVals.gamma;
    info_ptr->valid |= PNG_INFO_gAMA;
#else
   png_set_IHDR(png_ptr,
                info_ptr,
                w,                          // Width
                h,                          // Height
                8,                          // bit_depth
                PNG_COLOR_TYPE_RGB,         // PNG_COLOR_TYPE_???
                PNG_INTERLACE_NONE,         // PNG_INTERLACE_????
                PNG_COMPRESSION_TYPE_BASE,
                PNG_FILTER_TYPE_BASE);

#endif

    png_write_info(png_ptr, info_ptr);

#if 0
    if(info_ptr->bit_depth < 8)
        png_set_packing(png_ptr);
#endif

//fprintf (stderr, "5 encodeFromRGB24 passes=%d\n", numberOfPasses);

#if 0
//fprintf (stderr, "5.0.1 encodeFromRGB24 (no interlacing)\n");
        //png_byte row_pointers[h][w];
        //memcpy (row_pointers, pic, 3*w*h);
        p= const_cast<u8*>(pic);
        png_write_image (png_ptr, (png_bytepp)p);
#else
//fprintf (stderr, "5.0.0 encodeFromRGB24 (interlacing!!)\n");
        linesize=3*w;
        //if (defaultVals.interlace)
            numberOfPasses= png_set_interlace_handling(png_ptr);
        //else
            //numberOfPasses= 1;

        for(i = 0; i < numberOfPasses; i++) {
            p= const_cast<u8*>(pic);
            for(long unsigned j = 0; j < h; j++) {
                png_write_row(png_ptr, p);
                p += linesize;
            }
        }
    //}
#endif

    info_ptr->text= NULL;

    png_convert_from_time_t(&(info_ptr->mod_time), time(NULL));
    info_ptr->valid |= PNG_INFO_tIME;

    png_write_end(png_ptr, info_ptr);

    __numBytes= (u8*)png_ptr->io_ptr - buffer;
    png_destroy_write_struct(&png_ptr, &info_ptr);

    retVal= (u8*)malloc (1024*1024);
    memcpy (retVal, buffer, __numBytes);

//fprintf (stderr, "END encodeFromRGB24, numBytes=%lu\n", __numBytes);

    return retVal;
}

u8 *
pngEncodeFromRGB24(const u8 *pic, unsigned w, unsigned h)
{
    pngEnc_t  pImage;

    u8 *buffer= pImage.encodeFromRGB24 (pic, w, h);

#if 0
    {
        static int pngframe=0;

        int fd;
        char cname[4096];
        sprintf (cname, "PNG%03d.png", pngframe);
        fprintf (stderr, "salvando a %s\n", cname);

        if ((fd = open (cname, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
            fprintf (stderr, "ni abrir ficheros\n");
        else {
            write (fd, retVal->buffer, retVal->numBytes);
            close (fd);
        }
        pngframe++;
    }
#endif

    return buffer;
}

