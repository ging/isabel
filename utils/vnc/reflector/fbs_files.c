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
 * $Id: fbs_files.c 22251 2011-04-12 10:37:44Z gabriel $
 * Saving "framebuffer streams" in files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#include "rfblib.h"
#include "reflector.h"
#include "logging.h"

static char *s_fbs_prefix = NULL;
static int s_join_sessions;
static int s_fbs_idx = 0;
static FILE *s_fbs_fp = NULL;
static CARD8 *s_fbs_buffer, *s_fbs_buffer_ptr;
static struct timeval s_fbs_start_time, s_fbs_time;
static struct timezone s_fbs_timezone;
static CARD16 s_fbs_fb_width, s_fbs_fb_height;

void fbs_set_prefix(char *fbs_prefix, int join_sessions)
{
  s_fbs_prefix = fbs_prefix;
  s_join_sessions = join_sessions;
  s_fbs_idx = 0;
}

void fbs_open_file(CARD16 fb_width, CARD16 fb_height)
{
  int max_rect_size, w, h;
  CARD32 len;
  char fname[256];
  char fbs_header[256];
  char fbs_desktop_name[] = "RFB session archived by VNC Reflector";
  CARD8 fbs_newfbsize_msg[16] = {
    0, 0, 0, 1,                 /* msg header */
    0, 0, 0, 0, 0, 0, 0, 0,     /* x, y, w, h */
    0, 0, 0, 0                  /* encoding   */
  };

  if (s_fbs_prefix == NULL)
    return;

  /* Close the file if already opened */
  fbs_close_file();

  /* Increment session number */
  s_fbs_idx++;
  if (s_fbs_idx > 999)
    s_fbs_idx = 0;

  /* Prepare file name optionally suffixed with session number */
  len = strlen(s_fbs_prefix);
  if (len + 4 > 255) {
    log_write(LL_WARN, "FBS filename prefix too long");
    s_fbs_prefix = NULL;
    return;
  }
  if (!s_join_sessions) {
    sprintf(fname, "%s.%03d", s_fbs_prefix, s_fbs_idx);
  } else {
    strcpy(fname, s_fbs_prefix);
  }

  if (!s_join_sessions || s_fbs_idx == 1) {

    /* Open the file */
    s_fbs_fp = fopen(fname, "w");
    if (s_fbs_fp == NULL) {
      log_write(LL_WARN, "Could not open FBS file for writing");
      s_fbs_prefix = NULL;
      return;
    }
    log_write(LL_MSG, "Opened FBS file for writing: %s", fname);

    /* Remember current time */
    gettimeofday(&s_fbs_start_time, &s_fbs_timezone);

    /* Write file header */
    if (fwrite("FBS 001.000\n", 1, 12, s_fbs_fp) != 12) {
      log_write(LL_WARN, "Could not write FBS file header");
      fclose(s_fbs_fp);
      s_fbs_fp = NULL;
      return;
    }

    /* Prepare stream header data */
    memcpy(fbs_header, "RFB 003.003\n", 12);
    buf_put_CARD32(&fbs_header[12], 1);
    buf_put_CARD16(&fbs_header[16], fb_width);
    buf_put_CARD16(&fbs_header[18], fb_height);
    buf_put_pixfmt(&fbs_header[20], &g_screen_info.pixformat);
    if (!s_join_sessions) {
      len = g_screen_info.name_length;
      if (len > 192)
        len = 192;
      buf_put_CARD32(&fbs_header[36], len);
      memcpy(&fbs_header[40], g_screen_info.name, len);
    } else {
      len = sizeof(fbs_desktop_name) - 1;
      buf_put_CARD32(&fbs_header[36], len);
      memcpy(&fbs_header[40], fbs_desktop_name, len);
    }

    /* Write stream header data */
    fbs_write_data(fbs_header, 40 + len);

  } else {                      /* Next session in the same file */

    /* Open the file for append */
    s_fbs_fp = fopen(fname, "a");
    if (s_fbs_fp == NULL) {
      log_write(LL_WARN, "Could not re-open FBS file for writing");
      s_fbs_prefix = NULL;
      return;
    }
    log_write(LL_MSG, "Re-opened FBS file for writing: %s", fname);

    /* Write NewFBSize rect if framebuffer dimensions have changed */
    if (s_fbs_fb_width != fb_width || s_fbs_fb_height != fb_height) {
      buf_put_CARD16(&fbs_newfbsize_msg[8], fb_width);
      buf_put_CARD16(&fbs_newfbsize_msg[10], fb_height);
      buf_put_CARD32(&fbs_newfbsize_msg[12], RFB_ENCODING_NEWFBSIZE);
      fbs_write_data(fbs_newfbsize_msg, sizeof(fbs_newfbsize_msg));
    }

  }

  /* Allocate memory to hold one rectangle of maximum size */
  if (s_fbs_fp != NULL) {
    w = (int)fb_width;
    h = (int)fb_height;
    max_rect_size = 12 + (w * h * 4) + ((w + 15) / 16) * ((h + 15) / 16);
    s_fbs_buffer = malloc(max_rect_size);
    if (s_fbs_buffer == NULL) {
      log_write(LL_WARN, "Memory allocation error, closing FBS file");
      fclose(s_fbs_fp);
      s_fbs_fp = NULL;
    } else {
      log_write(LL_DETAIL, "Allocated buffer to cache FBS data, %d bytes",
                max_rect_size);
      s_fbs_buffer_ptr = s_fbs_buffer;
    }
  }

  /* Remember framebuffer dimensions */
  if (s_fbs_fp != NULL) {
    s_fbs_fb_width = fb_width;
    s_fbs_fb_height = fb_height;
  }
}

void fbs_write_data(void *buf, size_t len)
{
  CARD8 data_size_buf[4];
  CARD8 timestamp_buf[8];
  CARD32 timestamp;
  int padding;

  if (s_fbs_fp == NULL)
    return;

  /* Calculate current timestamp */
  gettimeofday(&s_fbs_time, &s_fbs_timezone);
  if (s_fbs_time.tv_sec < s_fbs_start_time.tv_sec) {
    /* FIXME: not sure if this is correct. */
    s_fbs_time.tv_sec += 60 * 60 * 24;
  }
  timestamp = (CARD32)((s_fbs_time.tv_sec - s_fbs_start_time.tv_sec) * 1000 +
                       (s_fbs_time.tv_usec - s_fbs_start_time.tv_usec) / 1000);

  padding = 3 - ((len - 1) & 0x03);
  buf_put_CARD32(data_size_buf, (CARD32)len);
  buf_put_CARD32(&timestamp_buf[padding], timestamp);

  if (fwrite(data_size_buf, 1, 4, s_fbs_fp) != 4 ||
      fwrite(buf, 1, len, s_fbs_fp) != len ||
      fwrite(timestamp_buf, 1, 4 + padding, s_fbs_fp) != 4 + padding) {
    log_write(LL_WARN, "Could not write FBS file data");
    fbs_close_file();
  }
}

void fbs_spool_byte(CARD8 b)
{
  if (s_fbs_fp != NULL)
    *s_fbs_buffer_ptr++ = b;
}

void fbs_spool_data(void *buf, size_t len)
{
  if (s_fbs_fp != NULL) {
    memcpy(s_fbs_buffer_ptr, buf, len);
    s_fbs_buffer_ptr += len;
  }
}

void fbs_flush_data(void)
{
  if (s_fbs_fp != NULL) {
    fbs_write_data(s_fbs_buffer, s_fbs_buffer_ptr - s_fbs_buffer);
    s_fbs_buffer_ptr = s_fbs_buffer;
  }
}

void fbs_close_file(void)
{
  if (s_fbs_fp != NULL) {
    if (fclose(s_fbs_fp) != 0)
      log_write(LL_WARN, "Could not close FBS file");
    s_fbs_fp = NULL;
    free(s_fbs_buffer);
  }
}

