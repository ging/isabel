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
 * $Id: translate.c 22251 2011-04-12 10:37:44Z gabriel $
 * Pixel format translation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>

#include "rfblib.h"
#include "reflector.h"
#include "async_io.h"
#include "translate.h"
#include "client_io.h"

#define SWAP_PIXEL8(pixel)  (pixel)

#define SWAP_PIXEL16(pixel)                     \
  (((pixel) << 8 & 0xFF00) |                    \
   ((pixel) >> 8 & 0x00FF))

#define SWAP_PIXEL32(pixel)                     \
  (((pixel) << 24 & 0xFF000000) |               \
   ((pixel) << 8  & 0x00FF0000) |               \
   ((pixel) >> 8  & 0x0000FF00) |               \
   ((pixel) >> 24 & 0x000000FF))

static void *gen_trans_table8(RFB_PIXEL_FORMAT *fmt);
static void *gen_trans_table16(RFB_PIXEL_FORMAT *fmt);
static void *gen_trans_table32(RFB_PIXEL_FORMAT *fmt);

void *gen_trans_table(RFB_PIXEL_FORMAT *fmt)
{
  switch(fmt->bits_pixel) {
  case 8:
    return gen_trans_table8(fmt);
  case 16:
    return gen_trans_table16(fmt);
  case 32:
    return gen_trans_table32(fmt);
  }
  return NULL;
}

#define DEFINE_GEN_TRANS_TABLE(bpp)                                     \
                                                                        \
static void *gen_trans_table##bpp(RFB_PIXEL_FORMAT *fmt)                \
{                                                                       \
  CARD##bpp *table;                                                     \
  CARD##bpp r, g, b;                                                    \
  int c;                                                                \
                                                                        \
  /* Allocate space for 3 tables for 8-bit R, G, B components */        \
  table = malloc(256 * 3 * sizeof(CARD##bpp));                          \
                                                                        \
  /* Fill in translation tables */                                      \
  if (table != NULL) {                                                  \
    for (c = 0; c < 256; c++) {                                         \
      r = (CARD##bpp)((c * fmt->r_max + 127) / 255 << fmt->r_shift);    \
      g = (CARD##bpp)((c * fmt->g_max + 127) / 255 << fmt->g_shift);    \
      b = (CARD##bpp)((c * fmt->b_max + 127) / 255 << fmt->b_shift);    \
      if ((fmt->big_endian != 0) ==                                     \
          (g_screen_info.pixformat.big_endian != 0)) {                  \
        table[c] = r;                                                   \
        table[256 + c] = g;                                             \
        table[512 + c] = b;                                             \
      } else {                                                          \
        table[c] = SWAP_PIXEL##bpp(r);                                  \
        table[256 + c] = SWAP_PIXEL##bpp(g);                            \
        table[512 + c] = SWAP_PIXEL##bpp(b);                            \
      }                                                                 \
    }                                                                   \
  }                                                                     \
                                                                        \
  return (void *)table;                                                 \
}

DEFINE_GEN_TRANS_TABLE(8)
DEFINE_GEN_TRANS_TABLE(16)
DEFINE_GEN_TRANS_TABLE(32)

void transfunc_null(void *dst_buf, FB_RECT *r, void *table)
{
  CARD32 *fb_ptr;
  CARD32 *dst_ptr = (CARD32 *)dst_buf;
  int y;

  fb_ptr = &g_framebuffer[r->y * g_fb_width + r->x];

  for (y = 0; y < r->h; y++) {
    memcpy(dst_ptr, fb_ptr, r->w * sizeof(CARD32));
    fb_ptr += g_fb_width;
    dst_ptr += r->w;
  }
}

/*
 * Primitive implementation of pixel translation function for BGR233
 * mode. Not used, written only for performance evaluation (by the
 * way, performance is not that bad.
 */

#define DEFINE_DUMB_TRANSFUNC8                                  \
                                                                \
void transfunc8(void *dst_buf, FB_RECT *r, void *table)         \
{                                                               \
  CARD32 *fb_ptr;                                               \
  CARD8 *dst_ptr = (CARD8 *)dst_buf;                            \
  int x, y;                                                     \
                                                                \
  fb_ptr = &g_framebuffer[r->y * g_fb_width + r->x];            \
  for (y = 0; y < r->h; y++) {                                  \
    for (x = 0; x < r->w; x++) {                                \
      *dst_ptr++ =                                              \
        ((((*fb_ptr >> 16 & 0xFF) * 7 + 127) / 255) |           \
         (((*fb_ptr >> 8 & 0xFF) * 7 + 127) / 255 << 3) |       \
         (((*fb_ptr & 0xFF) * 3 + 127) / 255 << 6));            \
      fb_ptr++;                                                 \
    }                                                           \
    fb_ptr += (g_fb_width - r->w);                              \
  }                                                             \
}

/*
 * Straghtforward implementation of pixel translation function using
 * lookup tables. Its performance does not depend on pixel data and is
 * always proportional to the amount of pixel data to translate. This
 * function is currently not used.
 */

#define DEFINE_TRANSFUNC(bpp)                                   \
                                                                \
void transfunc##bpp(void *dst_buf, FB_RECT *r, void *table)     \
{                                                               \
  CARD32 *fb_ptr;                                               \
  CARD##bpp *dst_ptr = (CARD##bpp *)dst_buf;                    \
  CARD##bpp *tbl_ptr = (CARD##bpp *)table;                      \
  int x, y;                                                     \
                                                                \
  fb_ptr = &g_framebuffer[r->y * g_fb_width + r->x];            \
  for (y = 0; y < r->h; y++) {                                  \
    for (x = 0; x < r->w; x++) {                                \
      *dst_ptr++ = (tbl_ptr[*fb_ptr >> 16 & 0xFF] |             \
                    tbl_ptr[256 + (*fb_ptr >> 8 & 0xFF)] |      \
                    tbl_ptr[512 + (*fb_ptr & 0xFF)]);           \
      fb_ptr++;                                                 \
    }                                                           \
    fb_ptr += (g_fb_width - r->w);                              \
  }                                                             \
}

/*
 * Alternative implementation of pixel translation function. This
 * function is more efficient when there are many neighbouring pixels
 * of the same color (quite common situation). Otherwise, it's a bit
 * slower than its straightforward equivalent.
 */

#define DEFINE_TRANSFUNC_ALT(bpp)                               \
                                                                \
void transfunc##bpp(void *dst_buf, FB_RECT *r, void *table)     \
{                                                               \
  CARD32 *fb_ptr;                                               \
  CARD32 fb_pixel;                                              \
  CARD##bpp *dst_ptr = (CARD##bpp *)dst_buf;                    \
  CARD##bpp *tbl_r = (CARD##bpp *)table;                        \
/*  CARD##bpp *tbl_g = tbl_r + 256; */                          \
/*  CARD##bpp *tbl_b = tbl_g + 256; */                          \
  CARD##bpp pixel = 0;                                          \
  int x, y, w, h;                                               \
                                                                \
  fb_ptr = &g_framebuffer[r->y * g_fb_width + r->x];            \
  w = r->w;                                                     \
  h = r->h;                                                     \
                                                                \
  /* Make sure fb_pixel != *fb_ptr */                           \
  fb_pixel = ~(*fb_ptr);                                        \
                                                                \
  for (y = 0; y < h; y++) {                                     \
    for (x = 0; x < w; x++) {                                   \
      if (fb_pixel != *fb_ptr++) {                              \
        fb_pixel = *(fb_ptr - 1);                               \
        pixel = (tbl_r[fb_pixel >> 16 & 0xFF] |                 \
                 tbl_r[256 + (fb_pixel >> 8 & 0xFF)] |          \
                 tbl_r[512 + (fb_pixel & 0xFF)]);               \
      }                                                         \
      *dst_ptr++ = pixel;                                       \
    }                                                           \
    fb_ptr += (g_fb_width - w);                                 \
  }                                                             \
}

DEFINE_TRANSFUNC_ALT(8)
DEFINE_TRANSFUNC_ALT(16)
DEFINE_TRANSFUNC_ALT(32)

