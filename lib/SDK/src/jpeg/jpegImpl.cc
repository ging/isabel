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
// $Id: jpegImpl.cc 10576 2007-07-12 16:42:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "jpegImpl.hh"
#include <setjmp.h>

extern "C" {
#include <jpeglib.h>
}

#include "jpegmgr.hh"

struct my_error_mgr
{
    struct jpeg_error_mgr pub;    // "public" fields
    jmp_buf setjmp_buffer;        // for return to caller
};

typedef struct my_error_mgr *my_error_ptr;


static void
my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr= (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);

    longjmp(myerr->setjmp_buffer, 1);
}

static void
my_error_printer(j_common_ptr cinfo, int code)
{
    my_error_ptr myerr= (my_error_ptr) cinfo->err;
    char buf[JMSG_LENGTH_MAX];

    (*cinfo->err->format_message)(cinfo, buf);

    if (code < 0)
    {
        NOTIFY("MJPEG Codec (Info): %d=%s\n", code, buf);
        //if (cinfo->err->msg_code != 121)
        longjmp(myerr->setjmp_buffer, 1);
    }
}

static void
my_error_printer2(j_common_ptr cinfo)
{
    my_error_ptr myerr= (my_error_ptr) cinfo->err;
    char buf[JMSG_LENGTH_MAX];

    (*cinfo->err->format_message)(cinfo, buf);

    NOTIFY("MJPEG Codec (error/warning): %s\n", buf);
    longjmp(myerr->setjmp_buffer, 1);
}


class jpegEncodeSession_t
{
private:
    jpeg_compress_struct __cinfo;
    jpeg_error_mgr       __jerr;

public:
    jpegEncodeSession_t(void)
    {
        __cinfo.err = jpeg_std_error(&__jerr);
        jpeg_create_compress(&__cinfo);
    }

    ~jpegEncodeSession_t(void)
    {
        jpeg_destroy_compress(&__cinfo);
    }

    jpeg_compress_struct *cinfo(void) { return &__cinfo; }
};

class jpegDecodeSession_t
{
private:
    jpeg_decompress_struct  __cinfo;
    struct my_error_mgr     __jerr;

public:
    jpegDecodeSession_t(void)
    {
        __cinfo.err              = jpeg_std_error(&__jerr.pub);
        __jerr.pub.emit_message  = my_error_printer;
        __jerr.pub.error_exit    = my_error_exit;
        __jerr.pub.output_message= my_error_printer2;

        jpeg_create_decompress(&__cinfo);
    }

    ~jpegDecodeSession_t(void)
    {
        jpeg_destroy_decompress(&__cinfo);
    }

    jpeg_decompress_struct *cinfo(void) { return &__cinfo; }
    struct my_error_mgr    *jerr(void)  { return &__jerr; }
};


jpegEncodeSession_t *
jpegCreateEncodeSession(void)
{
    return new jpegEncodeSession_t;
}

jpegDecodeSession_t *
jpegCreateDecodeSession(void)
{
    return new jpegDecodeSession_t;
}

void
jpegDestroyEncodeSession(jpegEncodeSession_t *s)
{
    delete s;
}

void
jpegDestroyDecodeSession(jpegDecodeSession_t *s)
{
    delete s;
}

jpegEncodedImage_t *
jpegEncode(jpegEncodeSession_t *s,
           const u8 *b,
           unsigned w,
           unsigned h,
           int quality,
           int dri
          )
{
    jpegEncodedImage_t *retVal= new jpegEncodedImage_t;

    u32 size = w*h*3;
    if ( ! size)
    {
        NOTIFY("jpegEncode: size zero!. Returning NULL\n");
        return NULL;
    }
    u8 *buffer = (u8*)malloc(size);
    if ( ! buffer)
    {
        NOTIFY("jpegEncode: malloc failing. Returning NULL\n");
        return NULL;
    }

    retVal->w = w;
    retVal->h = h;
    retVal->buffer = NULL;
    retVal->lqt = NULL;
    retVal->cqt = NULL;
    retVal->bytesPopped = 0;

    s->cinfo()->dct_method= JDCT_IFAST;
    s->cinfo()->in_color_space= JCS_RGB;
    s->cinfo()->input_components= 3;
    s->cinfo()->image_width= w;
    s->cinfo()->image_height= h;

    jpeg_set_defaults(s->cinfo());

    s->cinfo()->comp_info[0].h_samp_factor = 2;
    s->cinfo()->comp_info[0].v_samp_factor = 1;
    s->cinfo()->comp_info[1].h_samp_factor = 1;
    s->cinfo()->comp_info[1].v_samp_factor = 1;
    s->cinfo()->comp_info[2].h_samp_factor = 1;
    s->cinfo()->comp_info[2].v_samp_factor = 1;
    s->cinfo()->write_JFIF_header = TRUE;
    s->cinfo()->restart_interval = dri;

    // quality conversion from % to linear quality
    //quality= 1200-11*quality;
    int factor = quality;
    if (quality < 1) factor = 1;
    if (quality > 99) factor = 99;
    if (quality < 50)
        quality = 5000/factor;
    else
        quality = 200 - factor*2;

    jpeg_set_linear_quality(s->cinfo(), quality, 1);

    jpeg_dst_buffer(s->cinfo(), buffer, (unsigned*)&retVal->numBytes);

    JSAMPROW row_pointer[1];
    int row_stride= (s->cinfo()->image_width) * (s->cinfo()->input_components);
    jpeg_start_compress(s->cinfo(), TRUE);
    while (s->cinfo()->next_scanline < s->cinfo()->image_height)
    {
        row_pointer[0] = ((u8*)b) + s->cinfo()->next_scanline * row_stride;
        jpeg_write_scanlines(s->cinfo(), row_pointer, 1);
    }

    jpeg_finish_compress(s->cinfo());

    retVal->buffer = (u8*)malloc(retVal->numBytes);
    if ( ! retVal->buffer)
    {
        NOTIFY("jpegEncode: malloc failing. Returning NULL\n");
        return NULL;
    }
    memcpy (retVal->buffer, buffer, retVal->numBytes);
    retVal->type = 64;
    retVal->typeSpecific = 0;
    retVal->quality = quality;
    retVal->dri = dri;
    retVal->lqt = (u8*)malloc(64);
    retVal->cqt = (u8*)malloc(64);
    u8 *aux = NULL;
    for (unsigned int i = 0; i < retVal -> numBytes; i++)
    {
        if ( (buffer[i] == 0xff) && (buffer[i+1] == 0xdb) )
        {
            aux = buffer + i;
            break;
        }
    }
    memcpy (retVal->lqt, aux+5, 64);
    memcpy (retVal->cqt, aux+74, 64);

#if 0
    jpegSaveFile(retVal, "/tmp/Image", true);
#endif

    free (buffer);
    return retVal;
}



u8 *
jpegDecode(jpegDecodeSession_t *s, jpegEncodedImage_t *img)
{
    u8 *retVal = (u8*)malloc(img->w*img->h*3);
    if ( ! retVal)
    {
        NOTIFY("jpegDecode: malloc failing. Returning NULL\n");
        return NULL;
    }

    if (setjmp(s->jerr()->setjmp_buffer))
    {
        NOTIFY("jpegDeCode: Corrupt JPEG frame\n");
        // Corrupt JPEG

        j_decompress_ptr cinfo= s->cinfo();
        JpegDataSource *src = (JpegDataSource *) cinfo -> src;
        free(src->data); // not freed by "destroy_decompress"

        jpeg_destroy_decompress(s->cinfo());
#if 0
jpegSaveFile (img, "/tmp/mala", true);
#endif
        free(retVal);

        s->cinfo()->err= jpeg_std_error(&(s->jerr()->pub));
        s->jerr()->pub.error_exit= my_error_exit;
        s->jerr()->pub.emit_message= my_error_printer;
        s->jerr()->pub.output_message= my_error_printer2;

        jpeg_create_decompress(s->cinfo());

        NOTIFY("jpegDecode: Returning NULL\n");
        return NULL;
    }

    jpeg_src_buffer(s->cinfo(), img->buffer, img->numBytes);
    jpeg_read_header(s->cinfo(), FALSE);

    s->cinfo()->out_color_space= JCS_RGB;
    s->cinfo()->do_fancy_upsampling= FALSE;
    s->cinfo()->do_block_smoothing= FALSE;
    s->cinfo()->dct_method= JDCT_IFAST;
    s->cinfo()->dither_mode= JDITHER_NONE;
    s->cinfo()->restart_interval= img -> dri;

    unsigned int lines= 0;
    jpeg_start_decompress(s->cinfo());

    while (s->cinfo()->output_scanline < s->cinfo()->output_height)
    {
        u8 *linePtr= retVal + 3 * lines * s->cinfo()->output_width;
        lines += jpeg_read_scanlines(s->cinfo(),
                                     &linePtr,
                                     1 //s->cinfo()->output_height-lines);
                                    );
    }
    jpeg_finish_decompress(s->cinfo());

    return retVal;
}

image_t*
jpegLoadFile(FILE *f)
{
    fseek(f, 0L, SEEK_SET); // paranoid, not needed

    unsigned char b[2]= {0, 0};
    fread(b, 2, 1, f);
    if ( (b[0] != 0xff) || (b[1] != 0xd8) )
    {
        return NULL;
    }

    fseek(f, 0L, SEEK_SET); // paranoid, not needed

    jpeg_decompress_struct  cinfo;
    jpeg_error_mgr          jerr;

    cinfo.err= jpeg_std_error(&jerr);
//    jerr.error_exit=my_error_exit;
//    jerr.emit_message=my_error_printer;
//    jerr.output_message=my_error_printer2;

    jpeg_create_decompress(&cinfo);


    jpeg_stdio_src(&cinfo, f);

    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
    {
        jpeg_destroy_decompress(&cinfo);
        return NULL;
    }

    cinfo.out_color_space=JCS_RGB;
    cinfo.do_fancy_upsampling = FALSE;
    cinfo.do_block_smoothing = FALSE;
    cinfo.dct_method = JDCT_IFAST;//FLOAT;
    cinfo.dither_mode = JDITHER_NONE;


    unsigned int lines=0;
    (void) jpeg_start_decompress(&cinfo);

    u8 *retVal= (u8*)malloc(cinfo.output_width*cinfo.output_height*3);

    while (cinfo.output_scanline < cinfo.output_height)
    {
      u8 *linePtr[1024];
      linePtr[0]= retVal+3*lines*cinfo.output_width;
      lines += jpeg_read_scanlines(&cinfo, linePtr, 1);
    }

    (void) jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    image_t *img= new image_t(retVal,
                              3 * cinfo.output_width * cinfo.output_height,
                              RGB24_FORMAT,
                              cinfo.output_width,
                              cinfo.output_height,
                              0
                             );

    free(retVal);

    return img;
}


// esto es una chapuza
// lo pongo hasta decidir como modelarlo correctamente
#define JPEG_EXTENSION ".jpg"
#define TXT_EXTENSION  ".txt"

bool
jpegSaveFile(image_t *img, char *const path, bool addExt)
{
    assert("jpegSaveFile requieres jpeg encoding, then save file");

    return false;

#if 0
    char *fname= (char*)malloc (strlen(path) + strlen (JPEG_EXTENSION) + 1);
    int fd;
    if ( ! fname)
    {
        fprintf(stderr, "jpegSaveFileFromRGB24:: Out of memory\n");
        return false;
    }
    sprintf (fname, "%s%s", path, addExt? JPEG_EXTENSION: "");

    if ((fd = open (fname, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0)
    {
        fprintf (stderr, "jpegSaveFileFromRGB24:: ni abrir ficheros\n");
        free(fname);
        return false;
    }

    write (fd, img->buffer, (size_t)(img->numBytes));
    close (fd);
    free(fname);

    char *fnametxt = (char*)malloc (strlen(path) + strlen (TXT_EXTENSION) + 1);
    if ( ! fnametxt)
    {
        fprintf(stderr, "jpegSaveFileFromRGB24:: Out of memory\n");
        return false;
    }
    sprintf (fnametxt, "%s%s", path, TXT_EXTENSION);

    FILE *file = fopen(fnametxt, "w");
    for (unsigned i = 0; i < img->numBytes; i+=4)
    {
        fprintf (file, "Pos: %d -- Val: %x\t%x\t%x\t%x\n",
                 i,
                 img->buffer[i],
                 img->buffer[i+1],
                 img->buffer[i+2],
                 img->buffer[i+3]
                );
    }
    fclose (file);
    free (fnametxt);
    return true;
#endif
}

