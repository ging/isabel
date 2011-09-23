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
 * $Id: client_io.c 22251 2011-04-12 10:37:44Z gabriel $
 * Asynchronous interaction with VNC clients.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>

#include "rfblib.h"
#include "logging.h"
#include "async_io.h"
#include "reflector.h"
#include "host_io.h"
#include "translate.h"
#include "client_io.h"
#include "encode.h"

static unsigned char *s_password;
static unsigned char *s_password_ro;

/*
 * Prototypes for static functions
 */

static void cf_client(void);
static void rf_client_ver(void);
static void rf_client_auth(void);
static void wf_client_auth_failed(void);
static void rf_client_initmsg(void);
static void rf_client_msg(void);
static void rf_client_pixfmt(void);
static void rf_client_colormap_hdr(void);
static void rf_client_colormap_data(void);
static void rf_client_encodings_hdr(void);
static void rf_client_encodings_data(void);
static void rf_client_updatereq(void);
static void wf_client_update_finished(void);
static void rf_client_keyevent(void);
static void rf_client_ptrevent(void);
static void rf_client_cuttext_hdr(void);
static void rf_client_cuttext_data(void);

static void set_trans_func(CL_SLOT *cl);
static void send_newfbsize(void);
static void send_update(void);

/*
 * Implementation
 */

void set_client_passwords(unsigned char *password, unsigned char *password_ro)
{
  s_password = password;
  s_password_ro = password_ro;
}

void af_client_accept(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  int i;

  /* FIXME: Function naming is bad (client_accept_hook?). */

  cur_slot->type = TYPE_CL_SLOT;
  cl->connected = 0;
  cl->trans_table = NULL;
  aio_setclose(cf_client);

  for (i = 0; i < 4; i++)
    cl->zs_active[i] = 0;

  log_write(LL_MSG, "af_client_accept(): accepted connection from %s", cur_slot->name);

  aio_write(NULL, "RFB 003.003\n", 12);
  aio_setread(rf_client_ver, NULL, 12);
}

static void cf_client(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  int i;

  if (cur_slot->errread_f) {
    if (cur_slot->io_errno) {
      log_write(LL_WARN, "Error reading from %s: %s",
                cur_slot->name, strerror(cur_slot->io_errno));
    } else {
      log_write(LL_WARN, "Error reading from %s", cur_slot->name);
    }
  } else if (cur_slot->errwrite_f) {
    if (cur_slot->io_errno) {
      log_write(LL_WARN, "Error sending to %s: %s",
                cur_slot->name, strerror(cur_slot->io_errno));
    } else {
      log_write(LL_WARN, "Error sending to %s", cur_slot->name);
    }
  } else if (cur_slot->errio_f) {
    log_write(LL_WARN, "I/O error, client %s", cur_slot->name);
  }
  log_write(LL_MSG, "Closing client connection %s", cur_slot->name);

  /* Free region structures. */
  REGION_UNINIT(&cl->pending_region);
  REGION_UNINIT(&cl->copy_region);

  /* Free zlib streams.
     FIXME: Maybe put cleanup function in encoder. */
  for (i = 0; i < 4; i++) {
    if (cl->zs_active[i])
      deflateEnd(&cl->zs_struct[i]);
  }

  /* Free dynamically allocated memory. */
  if (cl->trans_table != NULL)
    free(cl->trans_table);
}

static void rf_client_ver(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  CARD8 msg[20];

  /* FIXME: Check protocol version. */

  /* FIXME: Functions like authentication should be available in
     separate modules, not in I/O part of the code. */
  /* FIXME: Higher level I/O functions should be implemented
     instead of things like buf_put_CARD32 + aio_write. */

  log_write(LL_DETAIL, "Client supports %.11s", cur_slot->readbuf);

  if (s_password[0]) {
    /* Request VNC authentication */
    buf_put_CARD32(msg, 2);

    /* Prepare "random" challenge */
    rfb_gen_challenge(cl->auth_challenge);
    memcpy(&msg[4], cl->auth_challenge, 16);

    /* Send both auth ID and challenge */
    aio_write(NULL, msg, 20);
    aio_setread(rf_client_auth, NULL, 16);
  } else {
    log_write(LL_WARN, "Not requesting authentication from %s",
              cur_slot->name);
    buf_put_CARD32(msg, 1);
    aio_write(NULL, msg, 4);
    aio_setread(rf_client_initmsg, NULL, 1);
  }
}

static void rf_client_auth(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  unsigned char resp_rw[16];
  unsigned char resp_ro[16];
  unsigned char msg[4];

  /* Place correct crypted responses to resp_rw, resp_ro */
  rfb_crypt(resp_rw, cl->auth_challenge, s_password);
  rfb_crypt(resp_ro, cl->auth_challenge, s_password_ro);

  /* Compare client response with correct ones */
  /* FIXME: Implement "too many tries" functionality some day. */
  if (memcmp(cur_slot->readbuf, resp_rw, 16) == 0) {
    cl->readonly = 0;
    log_write(LL_MSG, "Full-control authentication passed by %s",
              cur_slot->name);
  } else if (memcmp(cur_slot->readbuf, resp_ro, 16) == 0) {
    cl->readonly = 1;
    log_write(LL_MSG, "Read-only authentication passed by %s",
              cur_slot->name);
  } else {
    log_write(LL_WARN, "Authentication failed for %s", cur_slot->name);
    buf_put_CARD32(msg, 1);
    aio_write(wf_client_auth_failed, msg, 4);
    return;
  }

  buf_put_CARD32(msg, 0);
  aio_write(NULL, msg, 4);
  aio_setread(rf_client_initmsg, NULL, 1);
}

static void wf_client_auth_failed(void)
{
  aio_close(0);
}

static void rf_client_initmsg(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  unsigned char msg_server_init[24];

  if (cur_slot->readbuf[0] == 0) {
    log_write(LL_WARN, "Non-shared session requested by %s", cur_slot->name);
    aio_close(0);
  }

  /* Save initial desktop geometry for this client */
  cl->fb_width = g_screen_info.width;
  cl->fb_height = g_screen_info.height;
  cl->enable_newfbsize = 0;

  /* Send ServerInitialisation message */
  buf_put_CARD16(msg_server_init, cl->fb_width);
  buf_put_CARD16(msg_server_init + 2, cl->fb_height);
  buf_put_pixfmt(msg_server_init + 4, &g_screen_info.pixformat);
  buf_put_CARD32(msg_server_init + 20, g_screen_info.name_length);
  aio_write(NULL, msg_server_init, 24);
  aio_write(NULL, g_screen_info.name, g_screen_info.name_length);
  aio_setread(rf_client_msg, NULL, 1);

  /* Set up initial pixel format and encoders' parameters */
  memcpy(&cl->format, &g_screen_info.pixformat, sizeof(RFB_PIXEL_FORMAT));
  cl->trans_func = transfunc_null;
  cl->bgr233_f = 0;
  cl->compress_level = 6;       /* default compression level */
  cl->jpeg_quality = -1;        /* disable JPEG by default */

  /* The client did not requested framebuffer updates yet */
  cl->update_requested = 0;
  cl->update_in_progress = 0;
  REGION_INIT(&cl->pending_region, NullBox, 16);
  REGION_INIT(&cl->copy_region, NullBox, 8);
  cl->newfbsize_pending = 0;

  /* We are connected. */
  cl->connected = 1;
}

static void rf_client_msg(void)
{
  int msg_id;

  msg_id = (int)cur_slot->readbuf[0] & 0xFF;
  switch(msg_id) {
  case 0:                       /* SetPixelFormat */
    aio_setread(rf_client_pixfmt, NULL, 3 + sizeof(RFB_PIXEL_FORMAT));
    break;
  case 1:                       /* FixColourMapEntries */
    aio_setread(rf_client_colormap_hdr, NULL, 5);
    break;
  case 2:                       /* SetEncodings */
    aio_setread(rf_client_encodings_hdr, NULL, 3);
    break;
  case 3:                       /* FramebufferUpdateRequest */
    aio_setread(rf_client_updatereq, NULL, 9);
    break;
  case 4:                       /* KeyEvent */
    aio_setread(rf_client_keyevent, NULL, 7);
    break;
  case 5:                       /* PointerEvent */
    aio_setread(rf_client_ptrevent, NULL, 5);
    break;
  case 6:                       /* ClientCutText */
    aio_setread(rf_client_cuttext_hdr, NULL, 7);
    break;
  default:
    log_write(LL_ERROR, "Unknown client message type %d from %s",
              msg_id, cur_slot->name);
    aio_close(0);
  }
}

static void rf_client_pixfmt(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;

  buf_get_pixfmt(&cur_slot->readbuf[3], &cl->format);

  log_write(LL_DETAIL, "Pixel format (%d bpp) set by %s",
            cl->format.bits_pixel, cur_slot->name);

  set_trans_func(cl);

  aio_setread(rf_client_msg, NULL, 1);
}

static void rf_client_colormap_hdr(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;

  log_write(LL_WARN, "Ignoring FixColourMapEntries message from %s",
            cur_slot->name);

  cl->temp_count = buf_get_CARD16(&cur_slot->readbuf[3]);
  aio_setread(rf_client_colormap_data, NULL, cl->temp_count * 6);
}

static void rf_client_colormap_data(void)
{
  /* Nothing to do with FixColourMapEntries */
  aio_setread(rf_client_msg, NULL, 1);
}

static void rf_client_encodings_hdr(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;

  cl->temp_count = buf_get_CARD16(&cur_slot->readbuf[1]);
  aio_setread(rf_client_encodings_data, NULL, cl->temp_count * sizeof(CARD32));
}

static void rf_client_encodings_data(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  int i;
  int preferred_enc_set = 0;
  CARD32 enc;

  /* Reset encoding list (always enable raw encoding) */
  cl->enc_enable[RFB_ENCODING_RAW] = 1;
  cl->enc_prefer = RFB_ENCODING_RAW;
  cl->compress_level = -1;
  cl->jpeg_quality = -1;
  cl->enable_lastrect = 0;
  cl->enable_newfbsize = 0;
  for (i = 1; i < NUM_ENCODINGS; i++)
    cl->enc_enable[i] = 0;

  /* Read and store encoding list supplied by the client */
  for (i = 0; i < (int)cl->temp_count; i++) {
    enc = buf_get_CARD32(&cur_slot->readbuf[i * sizeof(CARD32)]);
    if (!preferred_enc_set) {
      if ( enc == RFB_ENCODING_RAW ||
           enc == RFB_ENCODING_HEXTILE ||
           enc == RFB_ENCODING_TIGHT ) {
        cl->enc_prefer = enc;
        preferred_enc_set = 1;
      }
    }
    if (enc >= 0 && enc < NUM_ENCODINGS) {
      cl->enc_enable[enc] = 1;
    } else if (enc >= RFB_ENCODING_COMPESSLEVEL0 &&
               enc <= RFB_ENCODING_COMPESSLEVEL9 &&
               cl->compress_level == -1) {
      cl->compress_level = (int)(enc - RFB_ENCODING_COMPESSLEVEL0);
      log_write(LL_DETAIL, "Compression level %d requested by client %s",
                cl->compress_level, cur_slot->name);
    } else if (enc >= RFB_ENCODING_QUALITYLEVEL0 &&
               enc <= RFB_ENCODING_QUALITYLEVEL9 &&
               cl->jpeg_quality == -1) {
      cl->jpeg_quality = (int)(enc - RFB_ENCODING_QUALITYLEVEL0);
      log_write(LL_DETAIL, "JPEG quality level %d requested by client %s",
                cl->jpeg_quality, cur_slot->name);
    } else if (enc == RFB_ENCODING_LASTRECT) {
      log_write(LL_DETAIL, "Client %s supports LastRect markers",
                cur_slot->name);
      cl->enable_lastrect = 1;
    } else if (enc == RFB_ENCODING_NEWFBSIZE) {
      cl->enable_newfbsize = 1;
      log_write(LL_DETAIL, "Client %s supports desktop geometry changes",
                cur_slot->name);
    }
  }
  if (cl->compress_level < 0)
    cl->compress_level = 6;     /* default compression level */

  /* CopyRect was pending but the client does not want it any more. */
  if (!cl->enc_enable[RFB_ENCODING_COPYRECT] &&
      REGION_NOTEMPTY(&cl->copy_region)) {
    REGION_UNION(&cl->pending_region, &cl->pending_region, &cl->copy_region);
    REGION_EMPTY(&cl->copy_region);
  }

  log_write(LL_DEBUG, "Encoding list set by %s", cur_slot->name);
  if (cl->enc_prefer == RFB_ENCODING_RAW) {
    log_write(LL_WARN, "Using raw encoding for client %s",
              cur_slot->name);
  } else if (cl->enc_prefer == RFB_ENCODING_TIGHT) {
    log_write(LL_DETAIL, "Using Tight encoding for client %s",
              cur_slot->name);
  } else if (cl->enc_prefer == RFB_ENCODING_HEXTILE) {
    log_write(LL_DETAIL, "Using Hextile encoding for client %s",
              cur_slot->name);
  }
  aio_setread(rf_client_msg, NULL, 1);
}

static void rf_client_updatereq(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  RegionRec tmp_region;
  BoxRec rect;

  rect.x1 = buf_get_CARD16(&cur_slot->readbuf[1]);
  rect.y1 = buf_get_CARD16(&cur_slot->readbuf[3]);
  rect.x2 = rect.x1 + buf_get_CARD16(&cur_slot->readbuf[5]);
  rect.y2 = rect.y1 + buf_get_CARD16(&cur_slot->readbuf[7]);

  /* Make sure the rectangle bounds fit the framebuffer. */
  if (rect.x1 > cl->fb_width)
    rect.x1 = cl->fb_width;
  if (rect.y1 > cl->fb_height)
    rect.y1 = cl->fb_height;
  if (rect.x2 > cl->fb_width)
    rect.x2 = cl->fb_width;
  if (rect.y2 > cl->fb_height)
    rect.y2 = cl->fb_height;

  cl->update_rect = rect;
  cl->update_requested = 1;

  if (!cur_slot->readbuf[0]) {
    log_write(LL_DEBUG, "Received framebuffer update request (full) from %s",
              cur_slot->name);
    if (!cl->newfbsize_pending) {
      REGION_INIT(&tmp_region, &rect, 1);
      REGION_UNION(&cl->pending_region, &cl->pending_region, &tmp_region);
      REGION_UNION(&cl->pending_region, &cl->pending_region, &cl->copy_region);
      REGION_EMPTY(&cl->copy_region);
      REGION_UNINIT(&tmp_region);
    }
  } else {
    log_write(LL_DEBUG, "Received framebuffer update request from %s",
              cur_slot->name);
  }

  if (!cl->update_in_progress &&
      (cl->newfbsize_pending ||
       REGION_NOTEMPTY(&cl->pending_region) ||
       REGION_NOTEMPTY(&cl->copy_region))) {
    send_update();
  }

  aio_setread(rf_client_msg, NULL, 1);
}

static void wf_client_update_finished(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;

  log_write(LL_DEBUG, "Finished sending framebuffer update to %s",
            cur_slot->name);

  cl->update_in_progress = 0;
  if (cl->update_requested &&
      (cl->newfbsize_pending ||
       REGION_NOTEMPTY(&cl->pending_region) ||
       REGION_NOTEMPTY(&cl->copy_region))) {
    send_update();
  }
}

static void rf_client_keyevent(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  CARD8 msg[8];

  if (!cl->readonly) {
    msg[0] = 4;                 /* KeyEvent */
    memcpy(&msg[1], cur_slot->readbuf, 7);
    pass_msg_to_host(msg, sizeof(msg));
  }

  aio_setread(rf_client_msg, NULL, 1);
}

static void rf_client_ptrevent(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  CARD16 x, y;
  CARD8 msg[6];

  if (!cl->readonly) {
    msg[0] = 5;                 /* PointerEvent */
    msg[1] = cur_slot->readbuf[0];
    x = buf_get_CARD16(&cur_slot->readbuf[1]);
    y = buf_get_CARD16(&cur_slot->readbuf[3]);

    /* Pointer position should fit in the host screen */
    if (x >= g_screen_info.width)
      x = g_screen_info.width - 1;
    if (y >= g_screen_info.height)
      y = g_screen_info.height - 1;

    buf_put_CARD16(&msg[2], x);
    buf_put_CARD16(&msg[4], y);
    pass_msg_to_host(msg, sizeof(msg));
  }

  aio_setread(rf_client_msg, NULL, 1);
}

static void rf_client_cuttext_hdr(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;

  log_write(LL_DEBUG, "Receiving ClientCutText message from %s",
            cur_slot->name);

  cl->cut_len = (int)buf_get_CARD32(&cur_slot->readbuf[3]);
  aio_setread(rf_client_cuttext_data, NULL, cl->cut_len);
}

static void rf_client_cuttext_data(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;

  if (!cl->readonly)
    pass_cuttext_to_host(cur_slot->readbuf, cl->cut_len);

  aio_setread(rf_client_msg, NULL, 1);
}

/*
 * Functions called from host_io.c
 */

void fn_client_add_rect(AIO_SLOT *slot, FB_RECT *rect)
{
  CL_SLOT *cl = (CL_SLOT *)slot;
  RegionRec add_region;
  BoxRec add_rect;
  int stored;
  int dx, dy;

  if (!cl->connected || cl->newfbsize_pending)
    return;

  /* If the framebuffer geometry has been changed, then we don't care
     about pending pixel updates any more, because all clients will
     want to update the whole framebuffer. */

  if (g_screen_info.width != cl->fb_width ||
      g_screen_info.height != cl->fb_height) {
    cl->newfbsize_pending = 1;
    REGION_EMPTY(&cl->pending_region);
    REGION_EMPTY(&cl->copy_region);
    return;
  }

  add_rect.x1 = rect->x;
  add_rect.y1 = rect->y;
  add_rect.x2 = add_rect.x1 + rect->w;
  add_rect.y2 = add_rect.y1 + rect->h;
  REGION_INIT(&add_region, &add_rect, 4);

  /* FIXME: Currently, CopyRect is stored in copy_region only if there
     were no other non-CopyRect updates pending for this client.
     Normally, that's ok, because VNC servers send CopyRect rectangles
     before non-CopyRect ones, but of course more elegant and
     efficient handling could be possible to implement here. */
  stored = 0;
  if (rect->enc == RFB_ENCODING_COPYRECT &&
      cl->enc_enable[RFB_ENCODING_COPYRECT] &&
      !REGION_NOTEMPTY(&cl->pending_region)) {
    dx = rect->x - rect->src_x;
    dy = rect->y - rect->src_y;
    if (!REGION_NOTEMPTY(&cl->copy_region) ||
        (dx == cl->copy_dx && dy == cl->copy_dy)) {
      REGION_UNION(&cl->copy_region, &cl->copy_region, &add_region);
      cl->copy_dx = dx;
      cl->copy_dy = dy;
      stored = 1;
    }
  }
  if (!stored)
    REGION_UNION(&cl->pending_region, &cl->pending_region, &add_region);

  REGION_UNINIT(&add_region);
}

void fn_client_send_rects(AIO_SLOT *slot)
{
  CL_SLOT *cl = (CL_SLOT *)slot;
  AIO_SLOT *saved_slot = cur_slot;

  if (!cl->update_in_progress && cl->update_requested &&
      (cl->newfbsize_pending ||
       REGION_NOTEMPTY(&cl->pending_region) ||
       REGION_NOTEMPTY(&cl->copy_region))) {
    cur_slot = slot;
    send_update();
    cur_slot = saved_slot;
  }
}

void fn_client_send_cuttext(AIO_SLOT *slot, CARD8 *text, size_t len)
{
  CL_SLOT *cl = (CL_SLOT *)slot;
  AIO_SLOT *saved_slot = cur_slot;
  CARD8 svr_cuttext_hdr[8] = {
    3, 0, 0, 0, 0, 0, 0, 0
  };

  if (cl->connected) {
    cur_slot = slot;

    log_write(LL_DEBUG, "Sending ServerCutText message to %s", cur_slot->name);
    buf_put_CARD32(&svr_cuttext_hdr[4], (CARD32)len);
    aio_write(NULL, svr_cuttext_hdr, 8);
    if (len)
      aio_write(NULL, text, len);

    cur_slot = saved_slot;
  }
}

/*
 * Non-callback functions
 */

static void set_trans_func(CL_SLOT *cl)
{
  if (cl->trans_table != NULL) {
    free(cl->trans_table);
    cl->trans_table = NULL;
    cl->trans_func = transfunc_null;
  }

  cl->bgr233_f = 0;

  if ( cl->format.bits_pixel != g_screen_info.pixformat.bits_pixel ||
       cl->format.color_depth != g_screen_info.pixformat.color_depth ||
       cl->format.big_endian != g_screen_info.pixformat.big_endian ||
       ((cl->format.true_color != 0) !=
        (g_screen_info.pixformat.true_color != 0)) ||
       cl->format.r_max != g_screen_info.pixformat.r_max ||
       cl->format.g_max != g_screen_info.pixformat.g_max ||
       cl->format.b_max != g_screen_info.pixformat.b_max ||
       cl->format.r_shift != g_screen_info.pixformat.r_shift ||
       cl->format.g_shift != g_screen_info.pixformat.g_shift ||
       cl->format.b_shift != g_screen_info.pixformat.b_shift ) {

    cl->trans_table = gen_trans_table(&cl->format);
    switch(cl->format.bits_pixel) {
    case 8:
      cl->trans_func = transfunc8;
      if ( cl->format.r_max == 7 && cl->format.g_max == 7 &&
           cl->format.b_max == 3 && cl->format.r_shift == 0 &&
           cl->format.g_shift == 3 && cl->format.b_shift == 6 &&
           cl->format.true_color != 0 ) {
        cl->bgr233_f = 1;
      }
      break;
    case 16:
      cl->trans_func = transfunc16;
      break;
    case 32:
      cl->trans_func = transfunc32;
      break;
    }
    log_write(LL_DEBUG, "Pixel format translation tables prepared");

  } else {
    log_write(LL_DETAIL, "No pixel format translation needed");
  }
}

static void send_newfbsize(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  CARD8 msg_hdr[4] = {
    0, 0, 0, 1
  };
  CARD8 rect_hdr[12];
  FB_RECT rect;

  log_write(LL_DEBUG, "Sending NewFBSize update (%dx%d) to %s",
            (int)cl->fb_width, (int)cl->fb_height, cur_slot->name);

  buf_put_CARD16(&msg_hdr[2], 1);
  aio_write(NULL, msg_hdr, 4);

  rect.x = 0;
  rect.y = 0;
  rect.w = cl->fb_width;
  rect.h = cl->fb_height;
  rect.enc = RFB_ENCODING_NEWFBSIZE;

  put_rect_header(rect_hdr, &rect);
  aio_write(wf_client_update_finished, rect_hdr, 12);

  /* Something has been queued for sending. */
  cl->update_in_progress = 1;
  cl->update_requested = 0;
}

/*
 * Send pending framebuffer update.
 * FIXME: Function too big.
 */

static void send_update(void)
{
  CL_SLOT *cl = (CL_SLOT *)cur_slot;
  BoxRec fb_rect;
  RegionRec fb_region, clip_region, outer_region;
  CARD8 msg_hdr[4] = {
    0, 0, 0, 1
  };
  CARD8 rect_hdr[12];
  FB_RECT rect;
  AIO_BLOCK *block;
  AIO_FUNCPTR fn = NULL;
  int num_copy_rects, num_penging_rects, num_all_rects;
  int raw_bytes = 0, hextile_bytes = 0;
  int i, idx, rev_order;

  /* Process framebuffer size change. */
  if (cl->newfbsize_pending) {
    /* Update framebuffer size, clear newfbsize_pending flag. */
    cl->fb_width = g_screen_info.width;
    cl->fb_height = g_screen_info.height;
    cl->newfbsize_pending = 0;
    log_write(LL_DEBUG, "Applying new framebuffer size (%dx%d) to %s",
              (int)cl->fb_width, (int)cl->fb_height, cur_slot->name);
    /* In any case, mark all the framebuffer contents as changed. */
    fb_rect.x1 = 0;
    fb_rect.y1 = 0;
    fb_rect.x2 = cl->fb_width;
    fb_rect.y2 = cl->fb_height;
    REGION_INIT(&fb_region, &fb_rect, 1);
    REGION_COPY(&cl->pending_region, &fb_region);
    REGION_UNINIT(&fb_region);
    REGION_EMPTY(&cl->copy_region);
    /* If NewFBSize is supported by the client, send only NewFBSize
       pseudo-rectangle, pixel data will be sent in the next update. */
    if (cl->enable_newfbsize) {
      send_newfbsize();
      return;
    }
  } else {
    /* Exclude CopyRect areas covered by pending_region. */
    REGION_SUBTRACT(&cl->copy_region, &cl->copy_region, &cl->pending_region);
  }

  /* Clip regions to the rectangle requested by the client. */
  REGION_INIT(&clip_region, &cl->update_rect, 1);
  REGION_INTERSECT(&cl->pending_region, &cl->pending_region, &clip_region);
  if (REGION_NOTEMPTY(&cl->copy_region)) {
    REGION_INTERSECT(&cl->copy_region, &cl->copy_region, &clip_region);

    REGION_INIT(&outer_region, NullBox, 8);
    REGION_COPY(&outer_region, &cl->copy_region);
    REGION_TRANSLATE(&clip_region, cl->copy_dx, cl->copy_dy);
    REGION_INTERSECT(&cl->copy_region, &cl->copy_region, &clip_region);
    REGION_SUBTRACT(&outer_region, &outer_region, &cl->copy_region);
    REGION_UNION(&cl->pending_region, &cl->pending_region, &outer_region);
    REGION_UNINIT(&outer_region);
  }
  REGION_UNINIT(&clip_region);

  /* Reduce the number of rectangles if possible. */
  if (cl->enc_prefer == RFB_ENCODING_TIGHT && cl->enable_lastrect) {
    region_pack(&cl->pending_region, 32);
  } else {
    region_pack(&cl->pending_region, 12);
  }

  /* Compute the number of rectangles in regions. */
  num_penging_rects = REGION_NUM_RECTS(&cl->pending_region);
  num_copy_rects = REGION_NUM_RECTS(&cl->copy_region);
  num_all_rects = num_penging_rects + num_copy_rects;
  if (num_all_rects == 0)
    return;

  log_write(LL_DEBUG, "Sending framebuffer update (min %d rects) to %s",
            num_all_rects, cur_slot->name);

  /* Prepare and send FramebufferUpdate message header. */
  /* FIXME: Enable Tight encoding even if LastRect is not supported. */
  /* FIXME: Do not send LastRect if all the rectangles are CopyRect. */
  if (cl->enc_prefer == RFB_ENCODING_TIGHT && cl->enable_lastrect) {
    buf_put_CARD16(&msg_hdr[2], 0xFFFF);
  } else {
    buf_put_CARD16(&msg_hdr[2], num_all_rects);
  }
  aio_write(NULL, msg_hdr, 4);

  /* Determine the order in which CopyRect rectangles should be sent. */
  rev_order = (cl->copy_dy > 0 || (cl->copy_dy == 0 && cl->copy_dx > 0));

  /* For each CopyRect rectangle: */
  for (i = 0; i < num_copy_rects; i++) {
    idx = (rev_order) ? num_copy_rects - i - 1 : i;
    rect.x = REGION_RECTS(&cl->copy_region)[idx].x1;
    rect.y = REGION_RECTS(&cl->copy_region)[idx].y1;
    rect.w = REGION_RECTS(&cl->copy_region)[idx].x2 - rect.x;
    rect.h = REGION_RECTS(&cl->copy_region)[idx].y2 - rect.y;
    rect.src_x = rect.x - cl->copy_dx;
    rect.src_y = rect.y - cl->copy_dy;
    rect.enc = RFB_ENCODING_COPYRECT;
    log_write(LL_DEBUG, "Sending CopyRect rectangle %dx%d at %d,%d to %s",
              (int)rect.w, (int)rect.h, (int)rect.x, (int)rect.y,
              cur_slot->name);

    /* Prepare the CopyRect rectangle. */
    block = rfb_encode_copyrect_block(cl, &rect);

    /* If it's the last rectangle, install hook function which would
       be called after all data has been sent. But do not do that if
       we use Tight encoding since there would be one more rectangle
       (LastRect marker) */
    if (i == num_all_rects - 1) {
      if (cl->enc_prefer != RFB_ENCODING_TIGHT || !cl->enable_lastrect)
        fn = wf_client_update_finished;
    }

    /* Send the rectangle.
       FIXME: Check for block == NULL? */
    aio_write_nocopy(fn, block);
  }

  /* For each of the usual pending rectangles: */
  for (i = 0; i < num_penging_rects; i++) {
    rect.x = REGION_RECTS(&cl->pending_region)[i].x1;
    rect.y = REGION_RECTS(&cl->pending_region)[i].y1;
    rect.w = REGION_RECTS(&cl->pending_region)[i].x2 - rect.x;
    rect.h = REGION_RECTS(&cl->pending_region)[i].y2 - rect.y;
    log_write(LL_DEBUG, "Sending rectangle %dx%d at %d,%d to %s",
              (int)rect.w, (int)rect.h, (int)rect.x, (int)rect.y,
              cur_slot->name);

    if (cl->enc_prefer == RFB_ENCODING_TIGHT && cl->enable_lastrect) {
      /* Use Tight encoding */
      rect.enc = RFB_ENCODING_TIGHT;
      rfb_encode_tight(cl, &rect);
      continue;                 /* Important! */
    } else if ( cl->enc_prefer != RFB_ENCODING_RAW &&
                cl->enc_enable[RFB_ENCODING_HEXTILE] ) {
      /* Use Hextile encoding */
      rect.enc = RFB_ENCODING_HEXTILE;
      block = rfb_encode_hextile_block(cl, &rect);
      if (block != NULL) {
        hextile_bytes += block->data_size;
        raw_bytes += rect.w * rect.h * (cl->format.bits_pixel / 8);
      }
    } else {
      /* Use Raw encoding */
      rect.enc = RFB_ENCODING_RAW;
      block = rfb_encode_raw_block(cl, &rect);
    }

    /* If it's the last rectangle, install hook function which would
       be called after all data has been sent. But do not do that if
       we use Tight encoding since there would be one more rectangle
       (LastRect marker) */
    if (i == num_penging_rects - 1)
      fn = wf_client_update_finished;

    /* Send the rectangle.
       FIXME: Check for block == NULL? */
    aio_write_nocopy(fn, block);
  }

  REGION_EMPTY(&cl->pending_region);
  REGION_EMPTY(&cl->copy_region);

  /* Send LastRect marker. */
  if (cl->enc_prefer == RFB_ENCODING_TIGHT && cl->enable_lastrect) {
    rect.x = rect.y = rect.w = rect.h = 0;
    rect.enc = RFB_ENCODING_LASTRECT;
    put_rect_header(rect_hdr, &rect);
    aio_write(wf_client_update_finished, rect_hdr, 12);
  }

  /* Something has been queued for sending. */
  cl->update_in_progress = 1;
  cl->update_requested = 0;
}

