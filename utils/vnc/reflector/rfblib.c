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
 * $Id: rfblib.c 22251 2011-04-12 10:37:44Z gabriel $
 * RFB protocol helper functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include "rfblib.h"
#include "d3des.h"

void buf_get_pixfmt(void *buf, RFB_PIXEL_FORMAT *format)
{
  CARD8 *bbuf = buf;

  memcpy(format, buf, SZ_RFB_PIXEL_FORMAT);
  format->r_max = buf_get_CARD16(&bbuf[4]);
  format->g_max = buf_get_CARD16(&bbuf[6]);
  format->b_max = buf_get_CARD16(&bbuf[8]);
}

void buf_put_pixfmt(void *buf, RFB_PIXEL_FORMAT *format)
{
  CARD8 *bbuf = buf;

  memcpy(buf, format, SZ_RFB_PIXEL_FORMAT);
  buf_put_CARD16(&bbuf[4], format->r_max);
  buf_put_CARD16(&bbuf[6], format->g_max);
  buf_put_CARD16(&bbuf[8], format->b_max);
}

void rfb_gen_challenge(CARD8 *buf)
{
  int i;

  srandom((unsigned int)time(NULL));
  for (i = 0; i < 16; i++)
    *buf++ = (unsigned char)random();
}

void rfb_crypt(CARD8 *dst_buf, CARD8 *src_buf, unsigned char *password)
{
  unsigned char key[8];

  memset(key, 0, 8);
  strncpy((char *)key, (char *)password, 8);
  deskey(key, EN0);
  des(src_buf, dst_buf);
  des(src_buf + 8, dst_buf + 8);
}

