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
 * $Id: logging.h 22251 2011-04-12 10:37:44Z gabriel $
 * Logging functionality
 */

#ifndef _REFLIB_LOGGING_H
#define _REFLIB_LOGGING_H

/* Log levels */
#define LL_INTERR  0
#define LL_ERROR   1
#define LL_WARN    2
#define LL_MSG     3
#define LL_INFO    4
#define LL_DETAIL  5
#define LL_DEBUG   6

/* Functions */
int log_open(char *filename, int file_level, int stderr_level);
int log_reopen(void);
int log_close(void);
void log_write(int level, char *fmt, ...);

#endif /* _REFLIB_LOGGING_H */
