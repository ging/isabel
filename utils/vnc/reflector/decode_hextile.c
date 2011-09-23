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
 * $Id: decode_hextile.c 22251 2011-04-12 10:37:44Z gabriel $
 * Decoding Hextile-encoded rectangles.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>

#include "rfblib.h"
#include "reflector.h"
#include "async_io.h"
#include "logging.h"
#include "host_io.h"

static CARD8 s_subenc;
static CARD8 s_num_subrects;
static CARD32 s_bg, s_fg;
static FB_RECT s_rect, s_tile;

static void rf_host_hextile_subenc(void);
static void rf_host_hextile_raw(void);
static void rf_host_hextile_hex(void);
static void rf_host_hextile_subrects(void);

static void hextile_fill_subrect(CARD8 pos, CARD8 dim);
static void hextile_next_tile(void);

/* FIXME: Evil/buggy servers can overflow this buffer */
static CARD32 hextile_buf[256 + 2];

void setread_decode_hextile(FB_RECT *r)
{
  s_rect = *r;

  s_tile.x = s_rect.x;
  s_tile.y = s_rect.y;
  s_tile.w = (s_rect.w < 16) ? s_rect.w : 16;
  s_tile.h = (s_rect.h < 16) ? s_rect.h : 16;
  aio_setread(rf_host_hextile_subenc, NULL, sizeof(CARD8));
}

static void rf_host_hextile_subenc(void)
{
  int data_size;

  /* Copy data for saving in a file if necessary */
  fbs_spool_byte(cur_slot->readbuf[0]);

  s_subenc = cur_slot->readbuf[0];
  if (s_subenc & RFB_HEXTILE_RAW) {
    data_size = s_tile.w * s_tile.h * sizeof(CARD32);
    aio_setread(rf_host_hextile_raw, hextile_buf, data_size);
    return;
  }
  data_size = 0;
  if (s_subenc & RFB_HEXTILE_BG_SPECIFIED) {
    data_size += sizeof(CARD32);
  } else {
    fill_fb_rect(&s_tile, s_bg);
  }
  if (s_subenc & RFB_HEXTILE_FG_SPECIFIED)
    data_size += sizeof(CARD32);
  if (s_subenc & RFB_HEXTILE_ANY_SUBRECTS)
    data_size += sizeof(CARD8);
  if (data_size) {
    aio_setread(rf_host_hextile_hex, hextile_buf, data_size);
  } else {
    hextile_next_tile();
  }
}

static void rf_host_hextile_raw(void)
{
  int row;
  CARD32 *from_ptr;
  CARD32 *fb_ptr;

  fbs_spool_data(hextile_buf, s_tile.w * s_tile.h * sizeof(CARD32));

  from_ptr = hextile_buf;
  fb_ptr = &g_framebuffer[s_tile.y * (int)g_fb_width + s_tile.x];

  /* Just copy raw data into the framebuffer */
  for (row = 0; row < s_tile.h; row++) {
    memcpy(fb_ptr, from_ptr, s_tile.w * sizeof(CARD32));
    from_ptr += s_tile.w;
    fb_ptr += g_fb_width;
  }

  hextile_next_tile();
}

static void rf_host_hextile_hex(void)
{
  CARD32 *from_ptr = hextile_buf;
  int data_size;

  /* Get background and foreground colors */
  if (s_subenc & RFB_HEXTILE_BG_SPECIFIED) {
    s_bg = *from_ptr++;
    fill_fb_rect(&s_tile, s_bg);
  }
  if (s_subenc & RFB_HEXTILE_FG_SPECIFIED) {
    s_fg = *from_ptr++;
  }

  if (s_subenc & RFB_HEXTILE_ANY_SUBRECTS) {
    fbs_spool_data(hextile_buf, (from_ptr - hextile_buf) * sizeof(CARD32) + 1);
    s_num_subrects = *((CARD8 *)from_ptr);
    if (s_subenc & RFB_HEXTILE_SUBRECTS_COLOURED) {
      data_size = 6 * (unsigned int)s_num_subrects;
    } else {
      data_size = 2 * (unsigned int)s_num_subrects;
    }
    if (data_size > 0) {
      aio_setread(rf_host_hextile_subrects, NULL, data_size);
      return;
    }
  } else {
    fbs_spool_data(hextile_buf, (from_ptr - hextile_buf) * sizeof(CARD32));
  }

  hextile_next_tile();
}

/* FIXME: Not as efficient as it could be. */
static void rf_host_hextile_subrects(void)
{
  CARD8 *ptr;
  CARD8 pos, dim;
  int i;

  ptr = cur_slot->readbuf;

  if (s_subenc & RFB_HEXTILE_SUBRECTS_COLOURED) {
    fbs_spool_data(ptr, s_num_subrects * 6);
    for (i = 0; i < (int)s_num_subrects; i++) {
      memcpy(&s_fg, ptr, sizeof(s_fg));
      ptr += sizeof(s_fg);
      pos = *ptr++;
      dim = *ptr++;
      hextile_fill_subrect(pos, dim);
    }
  } else {
    fbs_spool_data(ptr, s_num_subrects * 2);
    for (i = 0; i < (int)s_num_subrects; i++) {
      pos = *ptr++;
      dim = *ptr++;
      hextile_fill_subrect(pos, dim);
    }
  }

  hextile_next_tile();
}

/********************/
/* Helper functions */
/********************/

static void hextile_fill_subrect(CARD8 pos, CARD8 dim)
{
  int pos_x, pos_y, dim_w, dim_h;
  int x, y, skip;
  CARD32 *fb_ptr;

  pos_x = pos >> 4 & 0x0F;
  pos_y = pos & 0x0F;
  fb_ptr = &g_framebuffer[(s_tile.y + pos_y) * (int)g_fb_width +
                          (s_tile.x + pos_x)];

  /* Optimization for 1x1 subrects */
  if (dim == 0) {
    *fb_ptr = s_fg;
    return;
  }

  /* Actually, we should add 1 to both dim_h and dim_w. */
  dim_w = dim >> 4 & 0x0F;
  dim_h = dim & 0x0F;
  skip = g_fb_width - (dim_w + 1);

  for (y = 0; y <= dim_h; y++) {
    for (x = 0; x <= dim_w; x++) {
      *fb_ptr++ = s_fg;
    }
    fb_ptr += skip;
  }
}

static void hextile_next_tile(void)
{
  if (s_tile.x + 16 < s_rect.x + s_rect.w) {
    /* Next tile in the same row */
    s_tile.x += 16;
    if (s_tile.x + 16 < s_rect.x + s_rect.w)
      s_tile.w = 16;
    else
      s_tile.w = s_rect.x + s_rect.w - s_tile.x;
  } else if (s_tile.y + 16 < s_rect.y + s_rect.h) {
    /* First tile in the next row */
    s_tile.x = s_rect.x;
    s_tile.w = (s_rect.w < 16) ? s_rect.w : 16;
    s_tile.y += 16;
    if (s_tile.y + 16 < s_rect.y + s_rect.h)
      s_tile.h = 16;
    else
      s_tile.h = s_rect.y + s_rect.h - s_tile.y;
  } else {
    fbupdate_rect_done();       /* No more tiles */
    return;
  }
  aio_setread(rf_host_hextile_subenc, NULL, 1);
}

