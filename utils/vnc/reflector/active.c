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
 * $Id: active.c 22251 2011-04-12 10:37:44Z gabriel $
 * Active file marker implementation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "rfblib.h"
#include "reflector.h"
#include "logging.h"

static char *active_file = NULL;

int set_active_file(char *file_path)
{
  if (file_path == NULL)
    return 0;

  active_file = malloc(strlen(file_path) + 1);
  if (active_file == NULL)
    return 0;

  strcpy(active_file, file_path);

  return 1;
}

int write_active_file(void)
{
  FILE *active_fp;

  if (active_file == NULL)
    return 1;

  active_fp = fopen(active_file, "w");
  if (active_fp == NULL) {
    log_write(LL_WARN, "Error creating active file marker: %s", active_file);
    return 0;
  }

  fprintf(active_fp, "quirk\n");

  fflush(active_fp);
  fclose(active_fp);

  return 1;
}

int remove_active_file(void)
{
  if (active_file != NULL) {
    if (unlink(active_file) != 0) {
      log_write(LL_WARN, "Error removing active file marker: %s", active_file);
      return 0;
    }
    log_write(LL_DEBUG, "Removed active file marker", active_file);
  }

  return 1;
}

