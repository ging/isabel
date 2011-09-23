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
 * $Id: host_io.h 22251 2011-04-12 10:37:44Z gabriel $
 * Asynchronous interaction with VNC host.
 */

#ifndef _REFLIB_HOST_IO_H
#define _REFLIB_HOST_IO_H

#define TYPE_HOST_LISTENING_SLOT   2
#define TYPE_HOST_CONNECTING_SLOT  3
#define TYPE_HOST_ACTIVE_SLOT      4

/* Extension to AIO_SLOT structure to hold state for host connection */
typedef struct _HOST_SLOT {
  AIO_SLOT s;

  CARD32 temp_len;
  CARD16 fb_width;
  CARD16 fb_height;
} HOST_SLOT;

extern void host_activate(void);
extern void host_close_hook(void);

extern void pass_msg_to_host(CARD8 *msg, size_t len);
extern void pass_cuttext_to_host(CARD8 *text, size_t len);

extern void fill_fb_rect(FB_RECT *r, CARD32 color);
extern void fbupdate_rect_done(void);

/* decode_hextile.c */

extern void setread_decode_hextile(FB_RECT *r);

/* decode_tight.c */

extern void setread_decode_tight(FB_RECT *r);
extern void reset_tight_streams(void);

#endif /* _REFLIB_HOST_IO_H */
