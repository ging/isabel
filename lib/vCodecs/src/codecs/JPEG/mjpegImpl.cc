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
// $Id: mjpegImpl.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "mjpegImpl.h"


struct my_error_mgr {
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

    if(code < 0) {
        fprintf(stderr,"MJPEG Codec (Info): %d=%s\n", code, buf);
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

    fprintf(stderr,"MJPEG Codec (error/warning): %s\n", buf);
    longjmp(myerr->setjmp_buffer, 1);
}


class mjpegEncodeSession_t {
private:
    jpeg_compress_struct __cinfo;
    jpeg_error_mgr       __jerr;

public:
    mjpegEncodeSession_t(void) {
        __cinfo.err = jpeg_std_error(&__jerr);
        jpeg_create_compress(&__cinfo);
    }

    ~mjpegEncodeSession_t(void) {
        jpeg_destroy_compress(&__cinfo);
    }

    jpeg_compress_struct *cinfo(void) { return &__cinfo; }
};

class mjpegDecodeSession_t {
private:
    jpeg_decompress_struct  __cinfo;
    struct my_error_mgr     __jerr;

public:
    mjpegDecodeSession_t(void) {
        __cinfo.err              = jpeg_std_error(&__jerr.pub);
        __jerr.pub.emit_message  = my_error_printer;
        __jerr.pub.error_exit    = my_error_exit;
        __jerr.pub.output_message= my_error_printer2;

        jpeg_create_decompress(&__cinfo);
    }

    ~mjpegDecodeSession_t(void) {
            jpeg_destroy_decompress(&__cinfo);
    }

    jpeg_decompress_struct *cinfo(void) { return &__cinfo; }
    struct my_error_mgr    *jerr(void)  { return &__jerr; }
};


mjpegEncodeSession_t *
mjpegCreateEncodeSession(void)
{
    return new mjpegEncodeSession_t;
}

mjpegDecodeSession_t *
mjpegCreateDecodeSession(void)
{
    return new mjpegDecodeSession_t;
}

void
mjpegDestroyEncodeSession(mjpegEncodeSession_t *s)
{
    delete s;
}

void
mjpegDestroyDecodeSession(mjpegDecodeSession_t *s)
{
    delete s;
}

int
mjpegEncode(mjpegEncodeSession_t *s,
            unsigned char *outBuff,
            int            outBuffLen,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned int   width,
            unsigned int   height,
			int            quality

           )
{
    s->cinfo()->dct_method= JDCT_FASTEST;
    s->cinfo()->in_color_space= JCS_RGB;
    s->cinfo()->input_components= 3;
    s->cinfo()->image_width= width;
    s->cinfo()->image_height= height;

    jpeg_set_defaults(s->cinfo());

    s->cinfo()->comp_info[0].h_samp_factor = 2;
    s->cinfo()->comp_info[0].v_samp_factor = 1;
    s->cinfo()->comp_info[1].h_samp_factor = 1;
    s->cinfo()->comp_info[1].v_samp_factor = 1;
    s->cinfo()->comp_info[2].h_samp_factor = 1;
    s->cinfo()->comp_info[2].v_samp_factor = 1;
    s->cinfo()->write_JFIF_header = TRUE;
    s->cinfo()->restart_interval = 0; // restart markers may be used in RTP

    if (quality < 1) quality = 1;
    if (quality > 99) quality = 99;
    
    jpeg_set_quality(s->cinfo(), quality, 1);

    jpeg_dst_buffer(s->cinfo(), outBuff, (unsigned int*)&outBuffLen);

    JSAMPROW row_pointer[1];
    int row_stride= (s->cinfo()->image_width) * (s->cinfo()->input_components);
    jpeg_start_compress(s->cinfo(), TRUE);
    while (s->cinfo()->next_scanline < s->cinfo()->image_height) {
        row_pointer[0] = ((unsigned char*)inBuff) + s->cinfo()->next_scanline * row_stride;
        jpeg_write_scanlines(s->cinfo(), row_pointer, 1);
    }

    jpeg_finish_compress(s->cinfo());

    return outBuffLen;
}



int
mjpegDecode(mjpegDecodeSession_t *s,
            unsigned char *outBuff,
            int            outBuffLen,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned int  &width,
            unsigned int  &height 
		   )
{
    if(setjmp(s->jerr()->setjmp_buffer)) {
        fprintf(stderr,"mjpegDeCode: Corrupt JPEG frame\n");
        // Corrupt JPEG

        j_decompress_ptr cinfo= s->cinfo();
        JpegDataSource *src = (JpegDataSource *) cinfo -> src;
        free(src->data); // not freed by "destroy_decompress"

        jpeg_destroy_decompress(s->cinfo());

        s->cinfo()->err= jpeg_std_error(&(s->jerr()->pub));
        s->jerr()->pub.error_exit= my_error_exit;
        s->jerr()->pub.emit_message= my_error_printer;
        s->jerr()->pub.output_message= my_error_printer2;

        jpeg_create_decompress(s->cinfo());

        fprintf(stderr,"mjpegDecode: Returning NULL\n");
        return -1;
    }

    jpeg_src_buffer(s->cinfo(), inBuff, inBuffLen);
    jpeg_read_header(s->cinfo(), FALSE);

    s->cinfo()->out_color_space= JCS_RGB;
    s->cinfo()->do_fancy_upsampling= FALSE;
    s->cinfo()->do_block_smoothing= FALSE;
    s->cinfo()->dct_method= JDCT_FASTEST;
    s->cinfo()->dither_mode= JDITHER_NONE;

    unsigned int lines= 0;
    jpeg_start_decompress(s->cinfo());

    while (s->cinfo()->output_scanline < s->cinfo()->output_height) {
        unsigned char *linePtr= outBuff + 3 * lines * s->cinfo()->output_width;
        lines += jpeg_read_scanlines(s->cinfo(),
                                     &linePtr,
                                     1); //s->cinfo()->output_height-lines);
    }
    jpeg_finish_decompress(s->cinfo());

	width = s->cinfo()->output_width;
	height= s->cinfo()->output_height;

    return 0;
}

