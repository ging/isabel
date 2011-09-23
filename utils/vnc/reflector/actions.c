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
 * $Id: actions.c 22251 2011-04-12 10:37:44Z gabriel $
 * Performing actions on events
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "rfblib.h"
#include "reflector.h"
#include "logging.h"

#define MAX_CMD_SIZE  1024

static char *actions_file = NULL;

static int read_action(char *action_str, char *buf, int bufsize);

int set_actions_file(char *file_path)
{
  if (file_path == NULL)
    return 0;

  actions_file = malloc(strlen(file_path) + 1);
  if (actions_file == NULL)
    return 0;

  strcpy(actions_file, file_path);

  return 1;
}

int perform_action(char *action_str)
{
  char cmd[MAX_CMD_SIZE];
  int status;

  if (actions_file == NULL)
    return 1;

  if (!read_action(action_str, cmd, MAX_CMD_SIZE)) {
    log_write(LL_WARN, "Failed to read actions file: %s", actions_file);
    return 0;
  }

  if (cmd[0] == '\0') {
    log_write(LL_DETAIL, "Missing action for `%s'", action_str);
    return 0;
  }

  status = system(cmd);
  if (status == -1 || status == 0x7F00) {
    log_write(LL_WARN, "Failed to execute action command");
    return 0;
  }

  status = status >> 8 & 0xFF;
  if (status == 0) {
    log_write(LL_DETAIL, "Action command done");
  } else {
    log_write(LL_INFO, "Action command exited with status %d", status);
  }

  return 1;
}

/*
 * Parse a file with actions searcing for the keyword provided in the
 * action_str. The keyword should be found in the beginning of line,
 * before a colon and optional spaces. The rest of the string is
 * copied to the buf[] of size not less than bufsize.
 */

static int read_action(char *action_str, char *buf, int bufsize)
{
  FILE *actions_fp;
  int len, i, c = 0;

  if (actions_file == NULL)
    return 0;

  actions_fp = fopen(actions_file, "r");
  if (actions_fp == NULL)
    return 0;

  len = strlen(action_str);
  while (c != EOF) {
    /* Compare beginning of line with the keyword in action_str. */
    for (i = 0; i < len; i++) {
      c = getc(actions_fp);
      if (c != action_str[i])
        break;
    }
    /* If found, skip spaces after the colon, and copy the command. */
    if (i == len && getc(actions_fp) == ':') {
      do {
        c = getc(actions_fp);
      } while (c == ' ' || c == '\t');
      for (i = 0; c != '\n' && c != EOF; i++) {
        if (i >= bufsize - 1) {
          log_write(LL_WARN, "Action command too long");
          fclose(actions_fp);
          return 0;
        }
        buf[i] = c;
        c = getc(actions_fp);
      }
      c = buf[i] = '\0';        /* Make sure (c != EOF). */
      break;
    }
    /* Otherwise, skip to the end of this line. */
    while (c != '\n' && c != EOF) {
      c = getc(actions_fp);
    }
  }

  if (c == EOF)
    buf[0] = '\0';

  fclose(actions_fp);

  return 1;
}

