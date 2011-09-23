/* VNC Reflector
 * Copyright (C) 2001-2003 HorizonLive.com, Inc.  All rights reserved.
 * Copyright (C) 2000,2001 Constantin Kaplinsky.  All rights reserved.
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
 * $Id: decode_tight.c 22251 2011-04-12 10:37:44Z gabriel $
 * Decoding Tight-encoded rectangles.
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

/*
 * File-local data.
 */

static FB_RECT s_rect;
static z_stream s_zstream[4];
static int s_zstream_active[4] = { 0, 0, 0, 0 };
static CARD8 s_reset_streams = 0;

static int s_stream_id;
static int s_filter_id;
static int s_num_colors;
static CARD32 s_palette[256];
static int s_compressed_size, s_uncompressed_size;

static void rf_host_tight_compctl(void);
static void rf_host_tight_fill(void);
static void rf_host_tight_filter(void);
static void rf_host_tight_numcolors(void);
static void rf_host_tight_palette(void);
static void rf_host_tight_raw(void);
static void rf_host_tight_indexed(void);
static void rf_host_tight_len1(void);
static void rf_host_tight_len2(void);
static void rf_host_tight_len3(void);
static void rf_host_tight_compressed(void);

static void tight_draw_truecolor_data(CARD8 *src);
static void tight_draw_indexed_data(CARD8 *src);
static void tight_draw_gradient_data(CARD8 *src);

void reset_tight_streams(void)
{
  int stream_id;

  for (stream_id = 0; stream_id < 4; stream_id++) {
    if (s_zstream_active[stream_id]) {
      if (inflateEnd(&s_zstream[stream_id]) != Z_OK) {
        if (s_zstream[stream_id].msg != NULL) {
          log_write(LL_WARN, "inflateEnd() failed: %s",
                    s_zstream[stream_id].msg);
        } else {
          log_write(LL_WARN, "inflateEnd() failed");
        }
      }
      s_zstream_active[stream_id] = 0;
      s_reset_streams |= (1 << stream_id);
    }
  }
}

void setread_decode_tight(FB_RECT *r)
{
  s_rect = *r;
  aio_setread(rf_host_tight_compctl, NULL, sizeof(CARD8));
}

static void rf_host_tight_compctl(void)
{
  CARD8 comp_ctl;
  int stream_id;

  fbs_spool_byte(cur_slot->readbuf[0] | s_reset_streams);
  s_reset_streams = 0;

  /* Compression control byte */
  comp_ctl = cur_slot->readbuf[0];

  /* Flush zlib streams if we are told by the server to do so */
  for (stream_id = 0; stream_id < 4; stream_id++) {
    if ((comp_ctl & (1 < stream_id)) && s_zstream_active[stream_id]) {
      if (inflateEnd(&s_zstream[stream_id]) != Z_OK) {
        if (s_zstream[stream_id].msg != NULL) {
          log_write(LL_WARN, "inflateEnd() failed: %s",
                    s_zstream[stream_id].msg);
        } else {
          log_write(LL_WARN, "inflateEnd() failed");
        }
      }
      s_zstream_active[stream_id] = 0;
    }
  }
  comp_ctl &= 0xF0;             /* clear bits 3..0 */

  if (comp_ctl == RFB_TIGHT_FILL) {
    aio_setread(rf_host_tight_fill, NULL, 3);
  }
  else if (comp_ctl == RFB_TIGHT_JPEG) {
    log_write(LL_ERROR, "JPEG is not supported on host connections");
    aio_close(0);
    return;
  }
  else if (comp_ctl > RFB_TIGHT_MAX_SUBENCODING) {
    log_write(LL_ERROR, "Invalid sub-encoding in Tight-encoded data");
    aio_close(0);
    return;
  }
  else {                        /* "basic" compression */
    s_stream_id = (comp_ctl >> 4) & 0x03;
    if (comp_ctl & RFB_TIGHT_EXPLICIT_FILTER) {
      aio_setread(rf_host_tight_filter, NULL, 1);
    } else {
      s_filter_id = RFB_TIGHT_FILTER_COPY;
      s_uncompressed_size = s_rect.w * s_rect.h * 3;
      if (s_uncompressed_size < RFB_TIGHT_MIN_TO_COMPRESS) {
        aio_setread(rf_host_tight_raw, NULL, s_uncompressed_size);
      } else {
        aio_setread(rf_host_tight_len1, NULL, 1);
      }
    }
  }
}

static void rf_host_tight_fill(void)
{
  CARD32 color;

  fbs_spool_data(cur_slot->readbuf, 3);

  /* Note: cur_slot->readbuf is unsigned char[]. */
  color = (cur_slot->readbuf[0] << 16 |
           cur_slot->readbuf[1] << 8 |
           cur_slot->readbuf[2]);

  fill_fb_rect(&s_rect, color);
  fbupdate_rect_done();
}

static void rf_host_tight_filter(void)
{
  fbs_spool_byte(cur_slot->readbuf[0]);

  s_filter_id = cur_slot->readbuf[0];
  if (s_filter_id == RFB_TIGHT_FILTER_PALETTE) {
    aio_setread(rf_host_tight_numcolors, NULL, 1);
  } else {
    if (s_filter_id != RFB_TIGHT_FILTER_COPY &&
        s_filter_id != RFB_TIGHT_FILTER_GRADIENT) {
      log_write(LL_ERROR, "Unrecognized filter ID in the Tight decoder");
      aio_close(0);
      return;
    }
    s_uncompressed_size = s_rect.w * s_rect.h * 3;
    if (s_uncompressed_size < RFB_TIGHT_MIN_TO_COMPRESS) {
      aio_setread(rf_host_tight_raw, NULL, s_uncompressed_size);
    } else {
      aio_setread(rf_host_tight_len1, NULL, 1);
    }
  }
}

static void rf_host_tight_numcolors(void)
{
  fbs_spool_data(cur_slot->readbuf, 1);

  s_num_colors = cur_slot->readbuf[0] + 1;
  aio_setread(rf_host_tight_palette, NULL, s_num_colors * 3);
}

static void rf_host_tight_palette(void)
{
  int i, row_size;

  fbs_spool_data(cur_slot->readbuf, s_num_colors * 3);

  for (i = 0; i < s_num_colors; i++) {
    s_palette[i] = (cur_slot->readbuf[i*3] << 16 |
                    cur_slot->readbuf[i*3+1] << 8 |
                    cur_slot->readbuf[i*3+2]);
  }
  row_size = (s_num_colors <= 2) ? (s_rect.w + 7) / 8 : s_rect.w;
  s_uncompressed_size = s_rect.h * row_size;
  if (s_uncompressed_size < RFB_TIGHT_MIN_TO_COMPRESS) {
    aio_setread(rf_host_tight_indexed, NULL, s_uncompressed_size);
  } else {
    aio_setread(rf_host_tight_len1, NULL, 1);
  }
}

static void rf_host_tight_raw(void)
{
  fbs_spool_data(cur_slot->readbuf, s_uncompressed_size);

  tight_draw_truecolor_data(cur_slot->readbuf);
  fbupdate_rect_done();
}

static void rf_host_tight_indexed(void)
{
  fbs_spool_data(cur_slot->readbuf, s_uncompressed_size);

  tight_draw_indexed_data(cur_slot->readbuf);
  fbupdate_rect_done();
}

static void rf_host_tight_len1(void)
{
  fbs_spool_byte(cur_slot->readbuf[0]);

  s_compressed_size = cur_slot->readbuf[0] & 0x7F;
  if (cur_slot->readbuf[0] & 0x80) {
    aio_setread(rf_host_tight_len2, NULL, 1);
  } else {
    aio_setread(rf_host_tight_compressed, NULL, s_compressed_size);
  }
}

static void rf_host_tight_len2(void)
{
  fbs_spool_byte(cur_slot->readbuf[0]);

  s_compressed_size |= (cur_slot->readbuf[0] & 0x7F) << 7;
  if (cur_slot->readbuf[0] & 0x80) {
    aio_setread(rf_host_tight_len3, NULL, 1);
  } else {
    aio_setread(rf_host_tight_compressed, NULL, s_compressed_size);
  }
}

static void rf_host_tight_len3(void)
{
  fbs_spool_byte(cur_slot->readbuf[0]);

  s_compressed_size |= (cur_slot->readbuf[0] & 0x7F) << 14;
  aio_setread(rf_host_tight_compressed, NULL, s_compressed_size);
}

static void rf_host_tight_compressed(void)
{
  z_streamp zs;
  CARD8 *buf;
  int err;

  fbs_spool_data(cur_slot->readbuf, s_compressed_size);

  /* Initialize compression stream if needed */

  zs = &s_zstream[s_stream_id];
  if (!s_zstream_active[s_stream_id]) {
    zs->zalloc = Z_NULL;
    zs->zfree = Z_NULL;
    zs->opaque = Z_NULL;
    err = inflateInit(zs);
    if (err != Z_OK) {
      if (zs->msg != NULL) {
        log_write(LL_ERROR, "inflateInit() failed: %s", zs->msg);
      } else {
        log_write(LL_ERROR, "inflateInit() failed");
      }
      aio_close(0);
      return;
    }
    s_zstream_active[s_stream_id] = 1;
  }

  /* Allocate a buffer to put decompressed data into */

  buf = malloc(s_uncompressed_size);
  if (buf == NULL) {
    log_write(LL_ERROR, "Error allocating memory in Tight decoder");
    aio_close(0);
    return;
  }

  /* Decompress the data */

  zs->next_in = cur_slot->readbuf;
  zs->avail_in = s_compressed_size;
  zs->next_out = buf;
  zs->avail_out = s_uncompressed_size;

  err = inflate(zs, Z_SYNC_FLUSH);
  if (err != Z_OK && err != Z_STREAM_END) {
    if (zs->msg != NULL) {
      log_write(LL_ERROR, "inflate() failed: %s", zs->msg);
    } else {
      log_write(LL_ERROR, "inflate() failed: %d", err);
    }
    free(buf);
    aio_close(0);
    return;
  }

  if (zs->avail_out > 0)
    log_write(LL_WARN, "Decompressed data size is less than expected");

  /* Draw the data on the framebuffer */

  if (s_filter_id == RFB_TIGHT_FILTER_PALETTE) {
    tight_draw_indexed_data(buf);
  } else if (s_filter_id == RFB_TIGHT_FILTER_GRADIENT) {
    tight_draw_gradient_data(buf);
  } else {
    tight_draw_truecolor_data(buf);
  }

  free(buf);

  fbupdate_rect_done();
}

/*
 * Draw 24-bit truecolor pixel array on the framebuffer.
 */

static void tight_draw_truecolor_data(CARD8 *src)
{
  int x, y;
  CARD32 *fb_ptr;
  CARD8 *read_ptr;

  fb_ptr = &g_framebuffer[s_rect.y * (int)g_fb_width + s_rect.x];
  read_ptr = src;

  for (y = 0; y < s_rect.h; y++) {
    for (x = 0; x < s_rect.w; x++) {
      *fb_ptr++ = read_ptr[0] << 16 | read_ptr[1] << 8 | read_ptr[2];
      read_ptr += 3;
    }
    fb_ptr += g_fb_width - s_rect.w;
  }
}

/*
 * Draw indexed data on the framebuffer, each source pixel is either 1
 * bit (two colors) or 8 bits (up to 256 colors).
 */

static void tight_draw_indexed_data(CARD8 *src)
{
  int x, y, b, w;
  CARD32 *fb_ptr;
  CARD8 *read_ptr;

  fb_ptr = &g_framebuffer[s_rect.y * (int)g_fb_width + s_rect.x];
  read_ptr = src;

  if (s_num_colors <= 2) {
    w = (s_rect.w + 7) / 8;
    for (y = 0; y < s_rect.h; y++) {
      for (x = 0; x < s_rect.w / 8; x++) {
        for (b = 7; b >= 0; b--) {
          *fb_ptr++ = s_palette[*read_ptr >> b & 1];
        }
        read_ptr++;
      }
      for (b = 7; b >= 8 - s_rect.w % 8; b--) {
        *fb_ptr++ = s_palette[*read_ptr >> b & 1];
      }
      if (s_rect.w & 0x07)
        read_ptr++;
      fb_ptr += g_fb_width - s_rect.w;
    }
  } else {
    for (y = 0; y < s_rect.h; y++) {
      for (x = 0; x < s_rect.w; x++) {
        *fb_ptr++ = s_palette[*read_ptr++];
      }
      fb_ptr += g_fb_width - s_rect.w;
    }
  }
}

/*
 * Restore and draw the data processed with the "gradient" filter.
 */

static void tight_draw_gradient_data(CARD8 *src)
{
  int x, y, c;
  CARD32 *fb_ptr;
  CARD8 prev_row[2048*3];
  CARD8 this_row[2048*3];
  CARD8 pix[3];
  int est;

  fb_ptr = &g_framebuffer[s_rect.y * (int)g_fb_width + s_rect.x];

  memset(prev_row, 0, s_rect.w * 3);

  for (y = 0; y < s_rect.h; y++) {

    /* First pixel in a row */
    for (c = 0; c < 3; c++) {
      pix[c] = prev_row[c] + src[y*s_rect.w*3+c];
      this_row[c] = pix[c];
    }
    *fb_ptr++ = pix[0] << 16 | pix[1] << 8 | pix[2];

    /* Remaining pixels of a row */
    for (x = 1; x < s_rect.w; x++) {
      for (c = 0; c < 3; c++) {
	est = (int)prev_row[x*3+c] + (int)pix[c] - (int)prev_row[(x-1)*3+c];
	if (est > 0xFF) {
	  est = 0xFF;
	} else if (est < 0x00) {
	  est = 0x00;
	}
	pix[c] = (CARD8)est + src[(y*s_rect.w+x)*3+c];
	this_row[x*3+c] = pix[c];
      }
      *fb_ptr++ = pix[0] << 16 | pix[1] << 8 | pix[2];
    }

    fb_ptr += g_fb_width - s_rect.w;
    memcpy(prev_row, this_row, s_rect.w * 3);
  }
}

