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
 * $Id: translate.h 22251 2011-04-12 10:37:44Z gabriel $
 * Pixel format translation.
 */

#ifndef _REFLIB_TRANSLATE_H
#define _REFLIB_TRANSLATE_H

/* Not used at this moment */
/*
#define TRANSLATE_PIXEL(pixel, table, bpp)              \
  ((CARD##bpp)(tbl_ptr[pixel >> 16 & 0xFF] |            \
               tbl_ptr[256 + (pixel >> 8 & 0xFF)] |     \
               tbl_ptr[512 + (pixel & 0xFF)]))
*/

typedef void (*TRANSFUNC_PTR)(void *dst_buf, FB_RECT *r, void *table);

void *gen_trans_table(RFB_PIXEL_FORMAT *fmt);

void transfunc_null(void *dst_buf, FB_RECT *r, void *table);
void transfunc8(void *dst_buf, FB_RECT *r, void *table);
void transfunc16(void *dst_buf, FB_RECT *r, void *table);
void transfunc32(void *dst_buf, FB_RECT *r, void *table);

#endif /* _REFLIB_TRANSLATE_H */
