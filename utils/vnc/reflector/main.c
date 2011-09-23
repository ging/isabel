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
 * $Id: main.c 22251 2011-04-12 10:37:44Z gabriel $
 * Main module
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>

#include "rfblib.h"
#include "async_io.h"
#include "logging.h"
#include "reflector.h"
#include "host_connect.h"
#include "translate.h"
#include "host_io.h"
#include "client_io.h"
#include "encode.h"

/*
 * Configuration options
 */

static int   opt_no_banner;
static int   opt_cl_listen_port;
static char *opt_log_filename;
static char *opt_passwd_filename;
static char *opt_active_filename;
static char *opt_actions_filename;
static int   opt_foreground;
static int   opt_stderr_loglevel;
static int   opt_file_loglevel;
static char  opt_pid_file[256];
static char *opt_fbs_prefix;
static int   opt_join_sessions;
static char *opt_bind_ip;
static int   opt_request_tight;
static int   opt_tight_level;

static unsigned char opt_client_password[9];
static unsigned char opt_client_ro_password[9];

static char *opt_host_info_file;

/*
 * Global variables
 */

RFB_SCREEN_INFO g_screen_info;
CARD32 *g_framebuffer;
CARD16 g_fb_width, g_fb_height;

/*
 * Functions local to this file
 */

static void parse_args(int argc, char **argv);
static void report_usage(char *program_name);
static int read_password_file(void);
static int init_screen_info(void);
static int write_pid_file(void);
static int remove_pid_file(void);

/*
 * Implementation
 */

int main(int argc, char **argv)
{
  long cache_hits, cache_misses;

  /* Parse command line, exit on error */
  parse_args(argc, argv);

  if (!opt_no_banner) {
    fprintf(stderr,
"VNC Reflector %s.  Copyright (C) 2001-2003 HorizonLive.com, Inc.\n\n"
"HorizonLive provides e-Learning and collaborative synchronous presentation\n"
"solutions in a totally Web-based environment.  For more information about\n"
"HorizonLive, please see our website at http://www.horizonlive.com/\n\n",
            VERSION);
  }


   log_write(LL_MSG, "log_open");
  if (!log_open(opt_log_filename, opt_file_loglevel,
                (opt_foreground) ? opt_stderr_loglevel : -1)) {
    fprintf(stderr, "%s: error opening log file (ignoring this error)\n",
            argv[0]);
  }
  log_write(LL_MSG, "Starting VNC Reflector %s", VERSION);

  /* Fork the process to the background if necessary */
  if (!opt_foreground) {
    if (!opt_no_banner) {
      fprintf(stderr, "Starting in the background, "
              "see the log file for errors and other messages.\n");
    }

    if (getpid() != 1) {
      signal(SIGTTIN, SIG_IGN);
      signal(SIGTTOU, SIG_IGN);
      signal(SIGTSTP, SIG_IGN);
      if (fork ())
        return 0;
      setsid();
    }
    close(0);
    close(1);
    close(2);
    log_write(LL_INFO, "Switched to the background mode");
  }
  log_write(LL_MSG, "init_screen_info()");
  /* Initialization */
  if (init_screen_info()) {
    log_write(LL_MSG, "read_password_file()");
    read_password_file();
    set_host_encodings(opt_request_tight, opt_tight_level);
    set_client_passwords(opt_client_password, opt_client_ro_password);
    fbs_set_prefix(opt_fbs_prefix, opt_join_sessions);

    set_active_file(opt_active_filename);
    set_actions_file(opt_actions_filename);
    log_write(LL_INFO, "---------- haciendo bind -----------");
    aio_init();
    if (opt_bind_ip != NULL) {
      if (aio_set_bind_address(opt_bind_ip)) {
        log_write(LL_INFO, "Would bind listening sockets to address %s",
                  opt_bind_ip);
      } else {
        log_write(LL_WARN, "Illegal address to bind listening sockets to: %s",
                  opt_bind_ip);
      }
    }
    
    log_write(LL_INFO, "----------main.c-----------");
    /* Main work */
    if (connect_to_host(opt_host_info_file, opt_cl_listen_port)) {
      if (write_pid_file()) {
        set_control_signals();
        aio_mainloop();
        remove_pid_file();
      }
    }

    /* Cleanup */
    if (g_framebuffer != NULL) {
      log_write(LL_DETAIL, "Freeing framebuffer and associated structures");
      free(g_framebuffer);
      free_enc_cache();
    }
    if (g_screen_info.name != NULL)
      free(g_screen_info.name);

    get_hextile_caching_stats(&cache_hits, &cache_misses);
    if (cache_hits + cache_misses != 0) {
      log_write(LL_INFO, "Hextile BGR233 caching efficiency: %d%%",
                (int)((cache_hits * 100 + (cache_hits + cache_misses) / 2)
                      / (cache_hits + cache_misses)));
    }
  }

  log_write(LL_MSG, "Terminating");

  /* Close logs */
  if (!log_close() && opt_foreground) {
    fprintf(stderr, "%s: error closing log file (ignoring this error)\n",
            argv[0]);
  }

  /* Done */
  exit(1);
}

static void parse_args(int argc, char **argv)
{
  int err = 0;
  int c;
  char *temp_pid_file = NULL;
  char temp_buf[32];            /* 32 bytes should be more than enough */

  opt_no_banner = 0;
  opt_foreground = 0;
  opt_stderr_loglevel = -1;
  opt_file_loglevel = -1;
  opt_passwd_filename = NULL;
  opt_log_filename = NULL;
  opt_active_filename = NULL;
  opt_cl_listen_port = -1;
  opt_pid_file[0] = '\0';
  opt_fbs_prefix = NULL;
  opt_join_sessions = 0;
  opt_bind_ip = NULL;
  opt_request_tight = 0;
  opt_tight_level = -1;

  while (!err &&
         (c = getopt(argc, argv, "hqjv:f:p:a:c:g:l:i:s:b:tT:")) != -1) {
    switch (c) {
    case 'h':
      err = 1;
      break;
    case 'q':
      opt_no_banner = 1;
      break;
    case 'j':
      opt_join_sessions = 1;
      break;
    case 'v':
      if (opt_file_loglevel != -1)
        err = 1;
      else
        opt_file_loglevel = atoi(optarg);
      break;
    case 'f':
      opt_foreground = 1;
      if (opt_stderr_loglevel != -1)
        err = 1;
      else
        opt_stderr_loglevel = atoi(optarg);
      break;
    case 'p':
      if (opt_passwd_filename != NULL)
        err = 1;
      else
        opt_passwd_filename = optarg;
      break;
    case 'g':
      if (opt_log_filename != NULL)
        err = 1;
      else
        opt_log_filename = optarg;
      break;
    case 'a':
      if (opt_active_filename != NULL)
        err = 1;
      else
        opt_active_filename = optarg;
      break;
    case 'c':
      if (opt_actions_filename != NULL)
        err = 1;
      else
        opt_actions_filename = optarg;
      break;
    case 'l':
      if (opt_cl_listen_port != -1)
        err = 1;
      else {
        opt_cl_listen_port = atoi(optarg);
        if (opt_cl_listen_port <= 0)
          err = 1;
      }
      break;
    case 'i':
      if (temp_pid_file != NULL)
        err = 1;
      else
        temp_pid_file = optarg;
      break;
    case 's':
      if (opt_fbs_prefix != NULL)
        err = 1;
      else
        opt_fbs_prefix = optarg;
      break;
    case 'b':
      if (opt_bind_ip != NULL)
        err = 1;
      else
        opt_bind_ip = optarg;
      break;
    case 't':
      if (opt_request_tight)
        err = 1;
      else
        opt_request_tight = 1;
      break;
    case 'T':
      if (opt_request_tight) {
        err = 1;
      } else {
        opt_request_tight = 1;
        opt_tight_level = atoi(optarg);
        if (opt_tight_level <= 0 || opt_tight_level > 9)
          err = 1;
      }
      break;
    default:
      err = 1;
    }
  }

  /* Print usage help on error */
  if (err || optind != argc - 1) {
    report_usage(argv[0]);
    exit(1);
  }

  /* Provide reasonable defaults for some options */
  if (opt_file_loglevel == -1)
    opt_file_loglevel = LL_INFO;
  if (opt_passwd_filename == NULL)
    opt_passwd_filename = "passwd";
  if (opt_log_filename == NULL)
    opt_log_filename = "reflector.log";
  if (opt_cl_listen_port == -1)
    opt_cl_listen_port = 5999;

  /* Append listening port number to pid filename */
  if (temp_pid_file != NULL) {
    sprintf(temp_buf, "%d", opt_cl_listen_port);
    sprintf(opt_pid_file, "%.*s.%s", (int)(255 - strlen(temp_buf) - 1),
            temp_pid_file, temp_buf);
  }

  /* Save pointer to host info filename */
  opt_host_info_file = argv[optind];
}

static void report_usage(char *program_name)
{
  fprintf(stderr,
          "VNC Reflector %s.  Copyright (C) 2001-2003 HorizonLive.com, Inc."
          "\n\n",
          VERSION);

  fprintf(stderr, "Usage: %s [OPTIONS...] HOST_INFO_FILE\n\n",
          program_name);

  fprintf(stderr,
          "Options:\n"
          "  -i PID_FILE     - write pid file, appending listening port"
          " to the filename\n"
          "  -p PASSWD_FILE  - read a plaintext client password file"
          " [default: passwd]\n"
          "  -a ACTIVE_FILE  - create file during times when a host is"
          " connected\n"
          "  -c ACTIONS_FILE - on events, execute commands specified in"
          " a file\n"
          "  -l LISTEN_PORT  - port to listen for client connections"
          " [default: 5999]\n"
          "  -b IP_ADDRESS   - bind listening sockets to a specific IP"
          " [default: any]\n");
  fprintf(stderr,
          "  -s FBS_PREFIX   - save host sessions in rfbproxy-compatible"
          " files\n"
          "                    (optionally appending 3-digit session IDs"
          " to the\n"
          "                    filename prefix, only if used without the"
          " -j option)\n"
          "  -j              - join saved sessions (see -s option) in one"
          " session file\n"
          "  -t              - use Tight encoding for host communications"
          " if possible\n"
          "  -T COMPR_LEVEL  - like -t, but use the specified compression"
          " level (1..9)\n");
  fprintf(stderr,
          "  -g LOG_FILE     - write logs to the specified file"
          " [default: reflector.log]\n"
          "  -v LOG_LEVEL    - set verbosity level for the log file (0..%d)"
          " [default: %d]\n"
          "  -f LOG_LEVEL    - run in foreground, show logs on stderr"
          " at the specified\n"
          "                    verbosity level (0..%d) [note: use %d for"
          " normal output]\n"
          "  -q              - suppress printing copyright banner at startup\n"
          "  -h              - print this help message\n\n",
          LL_DEBUG, LL_INFO, LL_DEBUG, LL_MSG);

  fprintf(stderr,
          "Please refer to the README file for a description of the file"
          " formats for\n"
          "  HOST_INFO_FILE and PASSWD_FILE files mentioned above in this"
          " help text.\n\n");
}

static int read_password_file(void)
{
  FILE *passwd_fp;
  unsigned char *password_ptr = opt_client_password;
  int line = 0, len = 0;
  int c;

  /* Fill passwords with zeros */
  memset(opt_client_password, 0, 9);
  memset(opt_client_ro_password, 0, 9);

  log_write(LL_DETAIL, "Looking for passwords in the file \"%s\"",
            opt_passwd_filename);

  passwd_fp = fopen(opt_passwd_filename, "r");
  if (passwd_fp == NULL) {
    log_write(LL_WARN,
              "No client password file, assuming no authentication");
    return 1;
  }

  /* Read password file */
  while (line < 2) {
    c = getc(passwd_fp);
    if (c == '\r')
      c = getc(passwd_fp);      /* Handle MS-DOS-style end of line */
    if (c != '\n' && c != EOF && len < 8) {
      password_ptr[len++] = c;
    } else {
      password_ptr[len] = '\0';
      /* Truncate long passwords */
      if (len == 8 && c != '\n' && c != EOF) {
        log_write(LL_WARN, "Using only 8 first bytes of a longer password");
        do {
          c = getc(passwd_fp);
        } while (c != '\n' && c != EOF);
      }
      /* End of file */
      if (c == EOF)
        break;
      /* Empty password means no authentication */
      if (len == 0) {
         
         log_write(LL_WARN, "Got empty client password, hoping no auth is ok");
      }
      /* End of line */
      if (++line == 1) {
        password_ptr = opt_client_ro_password;
      }
      len = 0;
    }
  }
  if (len == 0) {
    if (line == 0) {
      log_write(LL_WARN, "Client password not specified, assuming no auth");
    } else {
      line--;
    }
  }

  log_write(LL_DEBUG, "Got %d password(s) from file, including empty ones",
            line + 1);

  /* Provide reasonable defaults if not all two passwords set */
  if (line == 0) {
    log_write(LL_DETAIL, "Read-only client password not specified");
    strcpy((char *)opt_client_ro_password, (char *)opt_client_password);
  }

  fclose(passwd_fp);
  return 1;
}

static int init_screen_info(void)
{
  union _LITTLE_ENDIAN {
    CARD32 value32;
    CARD8 test;
  } little_endian;

  /* Set the initial desktop name */
  g_screen_info.name_length = 1;
  g_screen_info.name = malloc(2);
  if (g_screen_info.name == NULL) {
    log_write(LL_ERROR, "Error allocating memory");
    return 0;
  }
  memcpy(g_screen_info.name, "?", 2);

  /* Fill in the PIXEL_FORMAT structure */
  g_screen_info.pixformat.bits_pixel = 32;
  g_screen_info.pixformat.color_depth = 24;
  g_screen_info.pixformat.true_color = 1;
  g_screen_info.pixformat.r_max = 255;
  g_screen_info.pixformat.g_max = 255;
  g_screen_info.pixformat.b_max = 255;
  g_screen_info.pixformat.r_shift = 16;
  g_screen_info.pixformat.g_shift = 8;
  g_screen_info.pixformat.b_shift = 0;

  /* Set correct endian flag in PIXEL_FORMAT */
  little_endian.value32 = 1;
  if (little_endian.test) {
    log_write(LL_DEBUG, "Our machine is little endian");
    g_screen_info.pixformat.big_endian = 0;
  } else {
    log_write(LL_DEBUG, "Our machine is big endian");
    g_screen_info.pixformat.big_endian = 1;
  }

  /* Make sure we would not try to free the framebuffer */
  g_framebuffer = NULL;

  return 1;
}

static int write_pid_file(void)
{
  int pid_fd, len;
  char buf[32];                 /* 32 bytes should be more than enough */

  if (opt_pid_file[0] == '\0')
    return 1;

  pid_fd = open(opt_pid_file, O_WRONLY | O_CREAT | O_EXCL, 0644);
  if (pid_fd == -1) {
    log_write(LL_ERROR, "Pid file exists, another instance may be running");
    return 0;
  }
  sprintf(buf, "%d\n", (int)getpid());
  len = strlen(buf);
  if (write(pid_fd, buf, len) != len) {
    close(pid_fd);
    log_write(LL_ERROR, "Error writing to pid file");
    return 0;
  }

  log_write(LL_DEBUG, "Wrote pid file: %s", opt_pid_file);

  close(pid_fd);
  return 1;
}

static int remove_pid_file(void)
{
  if (opt_pid_file[0] != '\0') {
    if (unlink(opt_pid_file) == 0) {
      log_write(LL_DEBUG, "Removed pid file", opt_pid_file);
    } else {
      log_write(LL_WARN, "Error removing pid file: %s", opt_pid_file);
      return 0;
    }
  }
  return 1;
}

