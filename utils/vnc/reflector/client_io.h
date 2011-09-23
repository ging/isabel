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
 * $Id: client_io.h 22251 2011-04-12 10:37:44Z gabriel $
 * Asynchronous interaction with VNC clients.
 */

#ifndef _REFLIB_CLIENT_IO_H
#define _REFLIB_CLIENT_IO_H

#include "region.h"

#define TYPE_CL_SLOT    1

#define NUM_ENCODINGS  10

/* Extension to AIO_SLOT structure to hold client state */
typedef struct _CL_SLOT {
  AIO_SLOT s;

  CARD16 fb_width;
  CARD16 fb_height;

  RFB_PIXEL_FORMAT format;
  void *trans_table;
  TRANSFUNC_PTR trans_func;

  RegionRec pending_region;
  RegionRec copy_region;
  int copy_dx, copy_dy;

  CARD16 temp_count;
  unsigned char auth_challenge[16];
  unsigned char enc_prefer;
  unsigned char enc_enable[NUM_ENCODINGS];
  int compress_level;
  int jpeg_quality;
  z_stream zs_struct[4];
  int zs_active[4];
  int zs_level[4];
  size_t cut_len;
  BoxRec update_rect;
  unsigned int bgr233_f           :1;
  unsigned int readonly           :1;
  unsigned int connected          :1;
  unsigned int update_requested   :1;
  unsigned int update_in_progress :1;
  unsigned int enable_lastrect    :1;
  unsigned int enable_newfbsize   :1;
  unsigned int newfbsize_pending  :1;
} CL_SLOT;

void set_client_passwords(unsigned char *password, unsigned char *password_ro);
void af_client_accept(void);

/* Functions called from host_io.c */
void fn_client_add_rect(AIO_SLOT *slot, FB_RECT *rect);
void fn_client_send_rects(AIO_SLOT *slot);
void fn_client_send_cuttext(AIO_SLOT *slot, CARD8 *text, size_t len);

#endif /* _REFLIB_CLIENT_IO_H */
