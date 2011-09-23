/* VNC Reflector
 * Copyright (C) 2001-2003 HorizonLive.com, Inc.  All rights reserved.
 *
 * This software is released under the terms specified in the file LICENSE,
 * included.  HorizonLive provides e-Learning and collaborative synchronous
 * presentation solutions in a totally Web-based environment.  For more
 * information about HorizonLive, please see our website at
 * http://www.horizonlive.com.
 *
 * This software was authored by Constantin Kaplinsky <const@ce.cctpu.edu.ru>
 * and sponsored by HorizonLive.com, Inc.
 *
 * $Id: rfblib.h 22251 2011-04-12 10:37:44Z gabriel $
 * RFB protocol definitions
 */

#ifndef _REFLIB_RFBLIB_H
#define _REFLIB_RFBLIB_H

/*
 * Define data types used in the RFB protocol.
 */

#ifndef CARD32

#define INT8    int8_t
#define CARD8   u_int8_t
#define INT16   int16_t
#define CARD16  u_int16_t
#define INT32   int32_t
#define CARD32  u_int32_t

#endif /* CARD32 */

/*
 * Rectangle.
 */

typedef struct _FB_RECT {
  CARD16 x;
  CARD16 y;
  CARD16 w;
  CARD16 h;
  CARD32 enc;                   /* Encoding type. */
  CARD16 src_x;                 /* CopyRect X source position. */
  CARD16 src_y;                 /* CopyRect Y source position. */
} FB_RECT;

/*
 * A macro to set x, y, w, h values in an FB_RECT structure.
 */

#define SET_RECT(rr, xx, yy, ww, hh)            \
{                                               \
  (rr)->x = (xx);                               \
  (rr)->y = (yy);                               \
  (rr)->w = (ww);                               \
  (rr)->h = (hh);                               \
}

/* FIXME: Non-consistent names. */

/*
 * RFB_PIXEL_FORMAT structure correspond to PIXEL_FORMAT as defined in
 * the RFB protocol specification. It describes pixel format used in
 * framebuffer updates sent by server to client.
 */

typedef struct _RFB_PIXEL_FORMAT {
  CARD8 bits_pixel;
  CARD8 color_depth;
  CARD8 big_endian;
  CARD8 true_color;
  CARD16 r_max;
  CARD16 g_max;
  CARD16 b_max;
  CARD8 r_shift;
  CARD8 g_shift;
  CARD8 b_shift;
  CARD8 unused[3];
} RFB_PIXEL_FORMAT;

/* Size of the pixel format structure as defined in the RFB protocol */
#define SZ_RFB_PIXEL_FORMAT  16

/*
 * RFB_SCREEN_INFO structure describes dimensions and format of the
 * screen (framebuffer).
 */

typedef struct _RFB_SCREEN_INFO {
  CARD16 width;
  CARD16 height;
  RFB_PIXEL_FORMAT pixformat;
  CARD32 name_length;
  CARD8 *name;
} RFB_SCREEN_INFO;

/*
 * Encodings
 */

#define RFB_ENCODING_RAW       0
#define RFB_ENCODING_COPYRECT  1
#define RFB_ENCODING_RRE       2
#define RFB_ENCODING_CORRE     4
#define RFB_ENCODING_HEXTILE   5
#define RFB_ENCODING_ZLIB      6
#define RFB_ENCODING_TIGHT     7
#define RFB_ENCODING_ZLIBHEX   8

#define RFB_ENCODING_COMPESSLEVEL0  0xFFFFFF00
#define RFB_ENCODING_COMPESSLEVEL9  0xFFFFFF09

#define RFB_ENCODING_QUALITYLEVEL0  0xFFFFFFE0
#define RFB_ENCODING_QUALITYLEVEL9  0xFFFFFFE9

#define RFB_ENCODING_LASTRECT       0xFFFFFF20
#define RFB_ENCODING_NEWFBSIZE      0xFFFFFF21

/*
 * Hextile encoding
 */

#define RFB_HEXTILE_RAW                   0x01
#define RFB_HEXTILE_BG_SPECIFIED          0x02
#define RFB_HEXTILE_FG_SPECIFIED          0x04
#define RFB_HEXTILE_ANY_SUBRECTS          0x08
#define RFB_HEXTILE_SUBRECTS_COLOURED     0x10

/*
 * Tight encoding
 */

#define RFB_TIGHT_EXPLICIT_FILTER         0x40
#define RFB_TIGHT_FILL                    0x80
#define RFB_TIGHT_JPEG                    0x90
#define RFB_TIGHT_MAX_SUBENCODING         0x90

#define RFB_TIGHT_FILTER_COPY             0x00
#define RFB_TIGHT_FILTER_PALETTE          0x01
#define RFB_TIGHT_FILTER_GRADIENT         0x02

#define RFB_TIGHT_MIN_TO_COMPRESS  12

/*
 * Macros and functions to compose/decompose bigger values from/into
 * byte arrays.
 */

#define buf_get_CARD8(buf)                      \
  (*((CARD8 *)buf))

#define buf_get_CARD16(buf)                     \
  ((CARD16)((CARD8 *)buf)[0] << 8 |             \
   (CARD16)((CARD8 *)buf)[1])

#define buf_get_CARD32(buf)                     \
  ((CARD32)((CARD8 *)buf)[0] << 24 |            \
   (CARD32)((CARD8 *)buf)[1] << 16 |            \
   (CARD32)((CARD8 *)buf)[2] << 8  |            \
   (CARD32)((CARD8 *)buf)[3]);

#define buf_put_CARD8(buf, value)               \
  *((CARD8 *)buf) = (CARD8)value;

#define buf_put_CARD16(buf, value)              \
{                                               \
  ((CARD8 *)buf)[0] = (CARD8)(value >> 8);      \
  ((CARD8 *)buf)[1] = (CARD8)value;             \
}

#define buf_put_CARD32(buf, value)              \
{                                               \
  ((CARD8 *)buf)[0] = (CARD8)(value >> 24);     \
  ((CARD8 *)buf)[1] = (CARD8)(value >> 16);     \
  ((CARD8 *)buf)[2] = (CARD8)(value >> 8);      \
  ((CARD8 *)buf)[3] = (CARD8)value;             \
}

/*
 * Functions
 */

void buf_get_pixfmt(void *buf, RFB_PIXEL_FORMAT *format);
void buf_put_pixfmt(void *buf, RFB_PIXEL_FORMAT *format);

void rfb_gen_challenge(CARD8 *buf);
void rfb_crypt(CARD8 *dst_buf, CARD8 *src_buf, unsigned char *password);

#endif /* _REFLIB_RFBLIB_H */
