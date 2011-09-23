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
 * $Id: host_io.c 22251 2011-04-12 10:37:44Z gabriel $
 * Asynchronous interaction with VNC host.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>

#include "rfblib.h"
#include "reflector.h"
#include "async_io.h"
#include "logging.h"
#include "translate.h"
#include "client_io.h"
#include "host_connect.h"
#include "host_io.h"
#include "encode.h"

static void host_really_activate(AIO_SLOT *slot);
static void fn_host_pass_newfbsize(AIO_SLOT *slot);

static void rf_host_msg(void);

static void rf_host_fbupdate_hdr(void);
static void rf_host_fbupdate_recthdr(void);
static void rf_host_fbupdate_raw(void);
static void rf_host_copyrect(void);

static void fn_host_add_client_rect(AIO_SLOT *slot);

static void rf_host_colormap_hdr(void);
static void rf_host_colormap_data(void);

static void rf_host_cuttext_hdr(void);
static void rf_host_cuttext_data(void);
static void fn_host_pass_cuttext(AIO_SLOT *slot);

static void reset_framebuffer(void);
static void request_update(int incr);

/*
 * Implementation
 */

static AIO_SLOT *s_host_slot = NULL;
static AIO_SLOT *s_new_slot = NULL;

/* Prepare host I/O slot for operating in main protocol phase */
void host_activate(void)
{
  if (s_host_slot == NULL) {
    /* Just activate */
    host_really_activate(cur_slot);
  } else {
    /* Let close hook do the work */
    s_new_slot = cur_slot;
    aio_close_other(s_host_slot, 0);
  }
}

/* On-close hook */

void host_close_hook(void)
{

  if (cur_slot->type == TYPE_HOST_ACTIVE_SLOT) {
    /* Close session file if open  */
    fbs_close_file();

    /* Erase framebuffer contents, invalidate cache */
    /* FIXME: Don't reset if there is a new connection, so the
       framebuffer (of its new size) would be changed anyway? */
    reset_framebuffer();

    /* No active slot exist */
    s_host_slot = NULL;
  }

  if (cur_slot->errread_f) {
    if (cur_slot->io_errno) {
      log_write(LL_ERROR, "Host I/O error, read: %s",
                strerror(cur_slot->io_errno));
    } else {
      log_write(LL_ERROR, "Host I/O error, read");
    }
  } else if (cur_slot->errwrite_f) {
    if (cur_slot->io_errno) {
      log_write(LL_ERROR, "Host I/O error, write: %s",
                strerror(cur_slot->io_errno));
    } else {
      log_write(LL_ERROR, "Host I/O error, write");
    }
  } else if (cur_slot->errio_f) {
    log_write(LL_ERROR, "Host I/O error");
  }

  if (s_new_slot == NULL) {
    log_write(LL_WARN, "Closing connection to host");
    /* Exit event loop if framebuffer does not exist yet. */
    if (g_framebuffer == NULL)
    remove_active_file();
  } else {
    log_write(LL_INFO, "Closing previous connection to host");
    host_really_activate(s_new_slot);
    s_new_slot = NULL;
  }
  log_write(LL_MSG, "Exiting the program");
  exit(1);
}

static void host_really_activate(AIO_SLOT *slot)
{
  AIO_SLOT *saved_slot = cur_slot;
  HOST_SLOT *hs = (HOST_SLOT *)slot;

  log_write(LL_MSG, "Activating new host connection");
  slot->type = TYPE_HOST_ACTIVE_SLOT;
  s_host_slot = slot;

  write_active_file();
  perform_action("host_activate");

  /* Allocate the framebuffer or extend its dimensions if necessary */
  if (!alloc_framebuffer(hs->fb_width, hs->fb_height)) {
    aio_close(1);
    return;
  }

  /* Set default desktop geometry for new client connections */
  g_screen_info.width = hs->fb_width;
  g_screen_info.height = hs->fb_height;

  /* If requested, open file to save this session and write the header */
  fbs_open_file(hs->fb_width, hs->fb_height);

  cur_slot = slot;

  /* Reset zlib streams in the Tight decoder */
  reset_tight_streams();

  /* Request initial screen contents */
  log_write(LL_DETAIL, "Requesting full framebuffer update");
  request_update(0);
  aio_setread(rf_host_msg, NULL, 1);

  /* Notify clients about desktop geometry change */
  aio_walk_slots(fn_host_pass_newfbsize, TYPE_CL_SLOT);

  cur_slot = saved_slot;
}

/*
 * Inform a client about new desktop geometry.
 */

static void fn_host_pass_newfbsize(AIO_SLOT *slot)
{
  HOST_SLOT *hs = (HOST_SLOT *)cur_slot;
  FB_RECT r;

  r.enc = RFB_ENCODING_NEWFBSIZE;
  r.x = r.y = 0;
  r.w = hs->fb_width;
  r.h = hs->fb_height;
  fn_client_add_rect(slot, &r);
}

/***************************/
/* Processing RFB messages */
/***************************/

static void rf_host_msg(void)
{
  int msg_id;

  msg_id = (int)cur_slot->readbuf[0] & 0xFF;
  switch(msg_id) {
  case 0:                       /* FramebufferUpdate */
    aio_setread(rf_host_fbupdate_hdr, NULL, 3);
    break;
  case 1:                       /* SetColourMapEntries */
    aio_setread(rf_host_colormap_hdr, NULL, 5);
    break;
  case 2:                       /* Bell */
    log_write(LL_DETAIL, "Received Bell message from host");
    fbs_write_data(cur_slot->readbuf, 1);
    aio_setread(rf_host_msg, NULL, 1);
    break;
  case 3:                       /* ServerCutText */
    aio_setread(rf_host_cuttext_hdr, NULL, 7);
    break;
  default:
    log_write(LL_ERROR, "Unknown server message type: %d", msg_id);
    aio_close(0);
  }
}

/********************************/
/* Handling framebuffer updates */
/********************************/

/* FIXME: Add state variables to the AIO_SLOT structure clone. */

static CARD16 rect_count;
static FB_RECT cur_rect;
static CARD16 rect_cur_row;

static void rf_host_fbupdate_hdr(void)
{
  CARD8 hdr_buf[4];

  rect_count = buf_get_CARD16(&cur_slot->readbuf[1]);

  if (rect_count == 0xFFFF) {
    log_write(LL_DETAIL, "Receiving framebuffer update");
  } else {
    log_write(LL_DETAIL, "Receiving framebuffer update, %d rectangle(s)",
              rect_count);
  }

  hdr_buf[0] = 0;
  memcpy(&hdr_buf[1], cur_slot->readbuf, 3);
  fbs_spool_data(hdr_buf, 4);

  if (rect_count) {
    aio_setread(rf_host_fbupdate_recthdr, NULL, 12);
  } else {
    log_write(LL_DEBUG, "Requesting incremental framebuffer update");
    request_update(1);
    aio_setread(rf_host_msg, NULL, 1);
  }
}

static void rf_host_fbupdate_recthdr(void)
{
  HOST_SLOT *hs = (HOST_SLOT *)cur_slot;

  cur_rect.x = buf_get_CARD16(cur_slot->readbuf);
  cur_rect.y = buf_get_CARD16(&cur_slot->readbuf[2]);
  cur_rect.w = buf_get_CARD16(&cur_slot->readbuf[4]);
  cur_rect.h = buf_get_CARD16(&cur_slot->readbuf[6]);
  cur_rect.enc = buf_get_CARD32(&cur_slot->readbuf[8]);

  fbs_spool_data(cur_slot->readbuf, 12);

  /* Handle LastRect "encoding" first */
  if (cur_rect.enc == RFB_ENCODING_LASTRECT) {
    log_write(LL_DEBUG, "LastRect marker received from the host");
    cur_rect.x = cur_rect.y = 0;
    rect_count = 1;
    fbupdate_rect_done();
    return;
  }

  /* Ignore zero-size rectangles */
  if (cur_rect.h == 0 || cur_rect.w == 0) {
    log_write(LL_WARN, "Zero-size rectangle %dx%d at %d,%d (ignoring)",
              (int)cur_rect.w, (int)cur_rect.h,
              (int)cur_rect.x, (int)cur_rect.y);
    fbupdate_rect_done();
    return;
  }

  /* Handle NewFBSize "encoding", as a special case */
  if (cur_rect.enc == RFB_ENCODING_NEWFBSIZE) {
    log_write(LL_INFO, "New host desktop geometry: %dx%d",
              (int)cur_rect.w, (int)cur_rect.h);
    g_screen_info.width = hs->fb_width = cur_rect.w;
    g_screen_info.height = hs->fb_height = cur_rect.h;

    /* Reallocate the framebuffer if necessary */
    if (!alloc_framebuffer(hs->fb_width, hs->fb_height)) {
      aio_close(1);
      return;
    }

    cur_rect.x = cur_rect.y = 0;

    /* NewFBSize is always the last rectangle regardless of rect_count */
    rect_count = 1;
    fbupdate_rect_done();
    return;
  }

  /* Prevent overflow of the framebuffer */
  if (cur_rect.x >= g_fb_width || cur_rect.x + cur_rect.w > g_fb_width ||
      cur_rect.y >= g_fb_height || cur_rect.y + cur_rect.h > g_fb_height) {
    log_write(LL_ERROR, "Rectangle out of framebuffer bounds: %dx%d at %d,%d",
              (int)cur_rect.w, (int)cur_rect.h,
              (int)cur_rect.x, (int)cur_rect.y);
    aio_close(0);
    return;
  }

  /* Ok, now the rectangle seems correct */
  log_write(LL_DEBUG, "Receiving rectangle %dx%d at %d,%d",
            (int)cur_rect.w, (int)cur_rect.h,
            (int)cur_rect.x, (int)cur_rect.y);

  switch(cur_rect.enc) {
  case RFB_ENCODING_RAW:
    log_write(LL_DEBUG, "Receiving raw data, expecting %d byte(s)",
              cur_rect.w * cur_rect.h * sizeof(CARD32));
    rect_cur_row = 0;
    aio_setread(rf_host_fbupdate_raw,
                &g_framebuffer[cur_rect.y * (int)g_fb_width +
                               cur_rect.x],
                cur_rect.w * sizeof(CARD32));
    break;
  case RFB_ENCODING_COPYRECT:
    log_write(LL_DEBUG, "Receiving CopyRect instruction");
    aio_setread(rf_host_copyrect, NULL, 4);
    break;
  case RFB_ENCODING_HEXTILE:
    log_write(LL_DEBUG, "Receiving Hextile-encoded data");
    setread_decode_hextile(&cur_rect);
    break;
  case RFB_ENCODING_TIGHT:
    log_write(LL_DEBUG, "Receiving Tight-encoded data");
    setread_decode_tight(&cur_rect);
    break;
  default:
    log_write(LL_ERROR, "Unknown encoding: 0x%08lX",
              (unsigned long)cur_rect.enc);
    aio_close(0);
  }
}

static void rf_host_fbupdate_raw(void)
{
  int idx;

  fbs_spool_data(cur_slot->readbuf, cur_rect.w * sizeof(CARD32));

  if (++rect_cur_row < cur_rect.h) {
    /* Read next row */
    idx = (cur_rect.y + rect_cur_row) * (int)g_fb_width + cur_rect.x;
    aio_setread(rf_host_fbupdate_raw, &g_framebuffer[idx],
                cur_rect.w * sizeof(CARD32));
  } else {
    /* Done with this rectangle */
    fbupdate_rect_done();
  }
}

static void rf_host_copyrect(void)
{
  CARD32 *src_ptr;
  CARD32 *dst_ptr;
  int width = (int)g_fb_width;
  int row;

  fbs_spool_data(cur_slot->readbuf, 4);

  cur_rect.src_x = buf_get_CARD16(cur_slot->readbuf);
  cur_rect.src_y = buf_get_CARD16(&cur_slot->readbuf[2]);

  if ( cur_rect.src_x >= g_fb_width ||
       cur_rect.src_x + cur_rect.w > g_fb_width ||
       cur_rect.src_y >= g_fb_height ||
       cur_rect.src_y + cur_rect.h > g_fb_height ) {
    log_write(LL_ERROR,
              "CopyRect from outside of the framebuffer: %dx%d from %d,%d",
              (int)cur_rect.w, (int)cur_rect.h,
              (int)cur_rect.src_x, (int)cur_rect.src_y);
    aio_close(0);
    return;
  }

  if (cur_rect.src_y > cur_rect.y) {
    /* Copy rows starting from top */
    src_ptr = &g_framebuffer[cur_rect.src_y * width + cur_rect.src_x];
    dst_ptr = &g_framebuffer[cur_rect.y * width + cur_rect.x];
    for (row = 0; row < cur_rect.h; row++) {
      memmove(dst_ptr, src_ptr, cur_rect.w * sizeof(CARD32));
      src_ptr += width;
      dst_ptr += width;
    }
  } else {
    /* Copy rows starting from bottom */
    src_ptr = &g_framebuffer[(cur_rect.src_y + cur_rect.h - 1) * width +
                             cur_rect.src_x];
    dst_ptr = &g_framebuffer[(cur_rect.y + cur_rect.h - 1) * width +
                             cur_rect.x];
    for (row = 0; row < cur_rect.h; row++) {
      memmove(dst_ptr, src_ptr, cur_rect.w * sizeof(CARD32));
      src_ptr -= width;
      dst_ptr -= width;
    }
  }

  fbupdate_rect_done();
}

/********************************/
/* Functions called by decoders */
/********************************/

/*
 * In the framebuffer, fill a rectangle with a specified color.
 */

void fill_fb_rect(FB_RECT *r, CARD32 color)
{
  int x, y;
  CARD32 *fb_ptr;

  fb_ptr = &g_framebuffer[r->y * (int)g_fb_width + r->x];

  /* Fill the first row */
  for (x = 0; x < r->w; x++)
    fb_ptr[x] = color;

  /* Copy the first row into all other rows */
  for (y = 1; y < r->h; y++)
    memcpy(&fb_ptr[y * g_fb_width], fb_ptr, r->w * sizeof(CARD32));
}

/*
 * This function is called by decoders after the whole rectangle
 * has been successfully decoded.
 */

void fbupdate_rect_done(void)
{
  if (cur_rect.w != 0 && cur_rect.h != 0) {
    log_write(LL_DEBUG, "Received rectangle ok");

    /* Cached data for this rectangle is not valid any more */
    invalidate_enc_cache(&cur_rect);

    /* Save data in a file if necessary */
    fbs_flush_data();

    /* Queue this rectangle for each client */
    aio_walk_slots(fn_host_add_client_rect, TYPE_CL_SLOT);
  }

  if (--rect_count) {
    aio_setread(rf_host_fbupdate_recthdr, NULL, 12);
  } else {
    /* Done with the whole update */
    aio_walk_slots(fn_client_send_rects, TYPE_CL_SLOT);
    log_write(LL_DEBUG, "Requesting incremental framebuffer update");
    request_update(1);
    aio_setread(rf_host_msg, NULL, 1);
  }
}

static void fn_host_add_client_rect(AIO_SLOT *slot)
{
  fn_client_add_rect(slot, &cur_rect);
}

/*****************************************/
/* Handling SetColourMapEntries messages */
/*****************************************/

static void rf_host_colormap_hdr(void)
{
  CARD16 num_colors;

  log_write(LL_WARN, "Ignoring SetColourMapEntries message");

  num_colors = buf_get_CARD16(&cur_slot->readbuf[3]);
  if (num_colors > 0)
    aio_setread(rf_host_colormap_data, NULL, num_colors * 6);
  else
    aio_setread(rf_host_msg, NULL, 1);
}

static void rf_host_colormap_data(void)
{
  /* Nothing to do with colormap */
  aio_setread(rf_host_msg, NULL, 1);
}

/***********************************/
/* Handling ServerCutText messages */
/***********************************/

/* FIXME: Add state variables to the AIO_SLOT structure clone. */
static size_t cut_len;
static CARD8 *cut_text;

static void rf_host_cuttext_hdr(void)
{
  log_write(LL_DETAIL,
            "Receiving ServerCutText message from host, %lu byte(s)",
            (unsigned long)cut_len);

  cut_len = (size_t)buf_get_CARD32(&cur_slot->readbuf[3]);
  if (cut_len > 0)
    aio_setread(rf_host_cuttext_data, NULL, cut_len);
  else
    rf_host_cuttext_data();
}

static void rf_host_cuttext_data(void)
{
  cut_text = cur_slot->readbuf;
  aio_walk_slots(fn_host_pass_cuttext, TYPE_CL_SLOT);
  aio_setread(rf_host_msg, NULL, 1);
}

static void fn_host_pass_cuttext(AIO_SLOT *slot)
{
  fn_client_send_cuttext(slot, cut_text, cut_len);
}

/*************************************/
/* Functions called from client_io.c */
/*************************************/

/* FIXME: Function naming. Have to invent consistent naming rules. */

void pass_msg_to_host(CARD8 *msg, size_t len)
{
  AIO_SLOT *saved_slot = cur_slot;

  if (s_host_slot != NULL) {
    cur_slot = s_host_slot;
    aio_write(NULL, msg, len);
    cur_slot = saved_slot;
  }
}

void pass_cuttext_to_host(CARD8 *text, size_t len)
{
  AIO_SLOT *saved_slot = cur_slot;
  CARD8 client_cuttext_hdr[8] = {
    6, 0, 0, 0, 0, 0, 0, 0
  };

  if (s_host_slot != NULL) {
    buf_put_CARD32(&client_cuttext_hdr[4], (CARD32)len);

    cur_slot = s_host_slot;
    aio_write(NULL, client_cuttext_hdr, sizeof(client_cuttext_hdr));
    aio_write(NULL, text, len);
    cur_slot = saved_slot;
  }
}

/********************/
/* Helper functions */
/********************/

/*
 * Clear the framebuffer, invalidate hextile cache.
 */

static void reset_framebuffer(void)
{
  HOST_SLOT *hs = (HOST_SLOT *)cur_slot;
  FB_RECT r;

  log_write(LL_DETAIL, "Clearing framebuffer and cache");
  memset(g_framebuffer, 0, g_fb_width * g_fb_height * sizeof(CARD32));

  r.x = r.y = 0;
  r.w = g_fb_width;
  r.h = g_fb_height;

  invalidate_enc_cache(&r);

  /* Queue changed rectangle (the whole host screen) for each client */
  r.w = hs->fb_width;
  r.h = hs->fb_height;
  aio_walk_slots(fn_host_add_client_rect, TYPE_CL_SLOT);
}

/*
 * Send a FramebufferUpdateRequest for the whole screen
 */

static void request_update(int incr)
{
  HOST_SLOT *hs = (HOST_SLOT *)cur_slot;
  unsigned char fbupdatereq_msg[] = {
    3,                          /* Message id */
    0,                          /* Incremental if 1 */
    0, 0, 0, 0,                 /* X position, Y position */
    0, 0, 0, 0                  /* Width, height */
  };

  fbupdatereq_msg[1] = (incr) ? 1 : 0;
  buf_put_CARD16(&fbupdatereq_msg[6], hs->fb_width);
  buf_put_CARD16(&fbupdatereq_msg[8], hs->fb_height);

  log_write(LL_DEBUG, "Sending FramebufferUpdateRequest message");
  aio_write(NULL, fbupdatereq_msg, sizeof(fbupdatereq_msg));
}

