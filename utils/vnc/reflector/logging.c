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
 * $Id: logging.c 22251 2011-04-12 10:37:44Z gabriel $
 * Logging implementation
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "logging.h"

static FILE *log_fp = NULL;
static char *log_fname = NULL;
static int log_file_level = -1;
static int log_stderr_level = -1;

static char log_lchar[] = "@!*+-: ";

/*******************************************************************
 *
 *  Open log file.
 *    filename: name for a file to append logs to.
 *    level: maximum log level to write messages at,
 *      -1 turns logging off.
 *  Returns 1 on success, 0 on failure.
 *
 *******************************************************************/

int log_open(char *filename, int file_level, int stderr_level)
{
  if (log_fname != NULL || log_fp != NULL) {
    log_write(LL_INTERR, "Trying to open already opened log");
    return 0;
  }

  log_stderr_level = stderr_level;

  log_fp = fopen(filename, "a");
  if (log_fp == NULL)
    return 0;

  log_fname = malloc(strlen(filename) + 1);
  if (log_fname == NULL) {
    fclose(log_fp);
    log_fp = NULL;
    return 0;
  }

  strcpy(log_fname, filename);
  log_file_level = file_level;

  fprintf(log_fp, "\n");
  fflush(log_fp);

  return 1;
}

/*******************************************************************
 *
 *  Reopen log file.
 *  Returns 1 on success, 0 on failure.
 *
 *******************************************************************/

int log_reopen(void)
{
  if (log_fname == NULL || log_fp == NULL)
    return 0;

  if (fclose(log_fp) == EOF) {
    free(log_fname);
    log_fname = NULL;
    log_fp = NULL;
    return 0;
  }

  log_fp = fopen(log_fname, "a");
  if (log_fp == NULL) {
    free(log_fname);
    log_fname = NULL;
    return 0;
  }

  return 1;
}

/*******************************************************************
 *
 *  Close log file.
 *  Returns 1 on success, 0 on failure.
 *
 *******************************************************************/

int log_close(void)
{
  int success = 1;

  if (log_fname != NULL) {
    free(log_fname);
    log_fname = NULL;
  }

  if (log_fp != NULL) {
    success = (fclose(log_fp) != EOF);
    log_fp = NULL;
  }

  log_file_level = -1;
  log_stderr_level = -1;
  return success;
}

/*******************************************************************
 *
 *  Write to the log file.
 *    level: log level, see LL_* constants;
 *    msg: the string to write to the log.
 *
 *******************************************************************/

void log_write(int level, char *format, ...)
{
  va_list arg_list;
  time_t now;
  char time_buf[32];
  char level_char = ' ';

  va_start(arg_list, format);

  if ( (log_fp != NULL && level <= log_file_level) ||
       level <= log_stderr_level ) {
    now = time(NULL);
    strftime(time_buf, 31, "%d/%m/%y %H:%M:%S", localtime(&now));

    if (level >= 0 && level < sizeof(log_lchar) - 1)
      level_char = log_lchar[level];

    if (level <= log_file_level) {
      fprintf(log_fp, "%s %c ", time_buf, (int)level_char);
      vfprintf(log_fp, format, arg_list);
      fprintf(log_fp, "\n");
      fflush(log_fp);
    }
    if (level <= log_stderr_level) {
      fprintf(stderr, "%s %c ", time_buf, (int)level_char);
      vfprintf(stderr, format, arg_list);
      fprintf(stderr, "\n");
      fflush(stderr);
    }
  }

  va_end(arg_list);
}

