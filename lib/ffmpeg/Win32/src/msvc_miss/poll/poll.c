/* Emulation for poll(2)
   Contributed by Paolo Bonzini.

   Copyright 2001-2003, 2006-2011 Free Software Foundation, Inc.

   This file is part of gnulib.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Tell gcc not to warn about the (nfd &lt; 0) tests, below.  */
#if (__GNUC__ == 4 &amp;&amp; 3 &lt;= __GNUC_MINOR__) || 4 &lt; __GNUC__
# pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#include &lt;config.h&gt;
#include &lt;alloca.h&gt;

#include &lt;sys/types.h&gt;

/* Specification.  */
#include &lt;poll.h&gt;

#include &lt;errno.h&gt;
#include &lt;limits.h&gt;
#include &lt;assert.h&gt;

#if (defined _WIN32 || defined __WIN32__) &amp;&amp; ! defined __CYGWIN__
# define WIN32_NATIVE
# include &lt;winsock2.h&gt;
# include &lt;windows.h&gt;
# include &lt;io.h&gt;
# include &lt;stdio.h&gt;
# include &lt;conio.h&gt;
#else
# include &lt;sys/time.h&gt;
# include &lt;sys/socket.h&gt;
# include &lt;sys/select.h&gt;
# include &lt;unistd.h&gt;
#endif

#ifdef HAVE_SYS_IOCTL_H
# include &lt;sys/ioctl.h&gt;
#endif
#ifdef HAVE_SYS_FILIO_H
# include &lt;sys/filio.h&gt;
#endif

#include &lt;time.h&gt;

#ifndef INFTIM
# define INFTIM (-1)
#endif

/* BeOS does not have MSG_PEEK.  */
#ifndef MSG_PEEK
# define MSG_PEEK 0
#endif

#ifdef WIN32_NATIVE

#define IsConsoleHandle(h) (((long) (h) &amp; 3) == 3)

static BOOL
IsSocketHandle (HANDLE h)
{
  WSANETWORKEVENTS ev;

  if (IsConsoleHandle (h))
    return FALSE;

  /* Under Wine, it seems that getsockopt returns 0 for pipes too.
     WSAEnumNetworkEvents instead distinguishes the two correctly.  */
  ev.lNetworkEvents = 0xDEADBEEF;
  WSAEnumNetworkEvents ((SOCKET) h, NULL, &amp;ev);
  return ev.lNetworkEvents != 0xDEADBEEF;
}

/* Declare data structures for ntdll functions.  */
typedef struct _FILE_PIPE_LOCAL_INFORMATION {
  ULONG NamedPipeType;
  ULONG NamedPipeConfiguration;
  ULONG MaximumInstances;
  ULONG CurrentInstances;
  ULONG InboundQuota;
  ULONG ReadDataAvailable;
  ULONG OutboundQuota;
  ULONG WriteQuotaAvailable;
  ULONG NamedPipeState;
  ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef struct _IO_STATUS_BLOCK
{
  union {
    DWORD Status;
    PVOID Pointer;
  } u;
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _FILE_INFORMATION_CLASS {
  FilePipeLocalInformation = 24
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef DWORD (WINAPI *PNtQueryInformationFile)
         (HANDLE, IO_STATUS_BLOCK *, VOID *, ULONG, FILE_INFORMATION_CLASS);

# ifndef PIPE_BUF
#  define PIPE_BUF      512
# endif

/* Compute revents values for file handle H.  If some events cannot happen
   for the handle, eliminate them from *P_SOUGHT.  */

static int
win32_compute_revents (HANDLE h, int *p_sought)
{
  int i, ret, happened;
  INPUT_RECORD *irbuffer;
  DWORD avail, nbuffer;
  BOOL bRet;
  IO_STATUS_BLOCK iosb;
  FILE_PIPE_LOCAL_INFORMATION fpli;
  static PNtQueryInformationFile NtQueryInformationFile;
  static BOOL once_only;

  switch (GetFileType (h))
    {
    case FILE_TYPE_PIPE:
      if (!once_only)
        {
          NtQueryInformationFile = (PNtQueryInformationFile)
            GetProcAddress (GetModuleHandle ("ntdll.dll"),
                            "NtQueryInformationFile");
          once_only = TRUE;
        }

      happened = 0;
      if (PeekNamedPipe (h, NULL, 0, NULL, &amp;avail, NULL) != 0)
        {
          if (avail)
            happened |= *p_sought &amp; (POLLIN | POLLRDNORM);
        }
      else if (GetLastError () == ERROR_BROKEN_PIPE)
        happened |= POLLHUP;

      else
        {
          /* It was the write-end of the pipe.  Check if it is writable.
             If NtQueryInformationFile fails, optimistically assume the pipe is
             writable.  This could happen on Win9x, where NtQueryInformationFile
             is not available, or if we inherit a pipe that doesn't permit
             FILE_READ_ATTRIBUTES access on the write end (I think this should
             not happen since WinXP SP2; WINE seems fine too).  Otherwise,
             ensure that enough space is available for atomic writes.  */
          memset (&amp;iosb, 0, sizeof (iosb));
          memset (&amp;fpli, 0, sizeof (fpli));

          if (!NtQueryInformationFile
              || NtQueryInformationFile (h, &amp;iosb, &amp;fpli, sizeof (fpli),
                                         FilePipeLocalInformation)
              || fpli.WriteQuotaAvailable &gt;= PIPE_BUF
              || (fpli.OutboundQuota &lt; PIPE_BUF &amp;&amp;
                  fpli.WriteQuotaAvailable == fpli.OutboundQuota))
            happened |= *p_sought &amp; (POLLOUT | POLLWRNORM | POLLWRBAND);
        }
      return happened;

    case FILE_TYPE_CHAR:
      ret = WaitForSingleObject (h, 0);
      if (!IsConsoleHandle (h))
        return ret == WAIT_OBJECT_0 ? *p_sought &amp; ~(POLLPRI | POLLRDBAND) : 0;

      nbuffer = avail = 0;
      bRet = GetNumberOfConsoleInputEvents (h, &amp;nbuffer);
      if (bRet)
        {
          /* Input buffer.  */
          *p_sought &amp;= POLLIN | POLLRDNORM;
          if (nbuffer == 0)
            return POLLHUP;
          if (!*p_sought)
            return 0;

          irbuffer = (INPUT_RECORD *) alloca (nbuffer * sizeof (INPUT_RECORD));
          bRet = PeekConsoleInput (h, irbuffer, nbuffer, &amp;avail);
          if (!bRet || avail == 0)
            return POLLHUP;

          for (i = 0; i &lt; avail; i++)
            if (irbuffer[i].EventType == KEY_EVENT)
              return *p_sought;
          return 0;
        }
      else
        {
          /* Screen buffer.  */
          *p_sought &amp;= POLLOUT | POLLWRNORM | POLLWRBAND;
          return *p_sought;
        }

    default:
      ret = WaitForSingleObject (h, 0);
      if (ret == WAIT_OBJECT_0)
        return *p_sought &amp; ~(POLLPRI | POLLRDBAND);

      return *p_sought &amp; (POLLOUT | POLLWRNORM | POLLWRBAND);
    }
}

/* Convert fd_sets returned by select into revents values.  */

static int
win32_compute_revents_socket (SOCKET h, int sought, long lNetworkEvents)
{
  int happened = 0;

  if ((lNetworkEvents &amp; (FD_READ | FD_ACCEPT | FD_CLOSE)) == FD_ACCEPT)
    happened |= (POLLIN | POLLRDNORM) &amp; sought;

  else if (lNetworkEvents &amp; (FD_READ | FD_ACCEPT | FD_CLOSE))
    {
      int r, error;

      char data[64];
      WSASetLastError (0);
      r = recv (h, data, sizeof (data), MSG_PEEK);
      error = WSAGetLastError ();
      WSASetLastError (0);

      if (r &gt; 0 || error == WSAENOTCONN)
        happened |= (POLLIN | POLLRDNORM) &amp; sought;

      /* Distinguish hung-up sockets from other errors.  */
      else if (r == 0 || error == WSAESHUTDOWN || error == WSAECONNRESET
               || error == WSAECONNABORTED || error == WSAENETRESET)
        happened |= POLLHUP;

      else
        happened |= POLLERR;
    }

  if (lNetworkEvents &amp; (FD_WRITE | FD_CONNECT))
    happened |= (POLLOUT | POLLWRNORM | POLLWRBAND) &amp; sought;

  if (lNetworkEvents &amp; FD_OOB)
    happened |= (POLLPRI | POLLRDBAND) &amp; sought;

  return happened;
}

#else /* !MinGW */

/* Convert select(2) returned fd_sets into poll(2) revents values.  */
static int
compute_revents (int fd, int sought, fd_set *rfds, fd_set *wfds, fd_set *efds)
{
  int happened = 0;
  if (FD_ISSET (fd, rfds))
    {
      int r;
      int socket_errno;

# if defined __MACH__ &amp;&amp; defined __APPLE__
      /* There is a bug in Mac OS X that causes it to ignore MSG_PEEK
         for some kinds of descriptors.  Detect if this descriptor is a
         connected socket, a server socket, or something else using a
         0-byte recv, and use ioctl(2) to detect POLLHUP.  */
      r = recv (fd, NULL, 0, MSG_PEEK);
      socket_errno = (r &lt; 0) ? errno : 0;
      if (r == 0 || socket_errno == ENOTSOCK)
        ioctl (fd, FIONREAD, &amp;r);
# else
      char data[64];
      r = recv (fd, data, sizeof (data), MSG_PEEK);
      socket_errno = (r &lt; 0) ? errno : 0;
# endif
      if (r == 0)
        happened |= POLLHUP;

      /* If the event happened on an unconnected server socket,
         that's fine. */
      else if (r &gt; 0 || ( /* (r == -1) &amp;&amp; */ socket_errno == ENOTCONN))
        happened |= (POLLIN | POLLRDNORM) &amp; sought;

      /* Distinguish hung-up sockets from other errors.  */
      else if (socket_errno == ESHUTDOWN || socket_errno == ECONNRESET
               || socket_errno == ECONNABORTED || socket_errno == ENETRESET)
        happened |= POLLHUP;

      else
        happened |= POLLERR;
    }

  if (FD_ISSET (fd, wfds))
    happened |= (POLLOUT | POLLWRNORM | POLLWRBAND) &amp; sought;

  if (FD_ISSET (fd, efds))
    happened |= (POLLPRI | POLLRDBAND) &amp; sought;

  return happened;
}
#endif /* !MinGW */

int
poll (struct pollfd *pfd, nfds_t nfd, int timeout)
{
#ifndef WIN32_NATIVE
  fd_set rfds, wfds, efds;
  struct timeval tv;
  struct timeval *ptv;
  int maxfd, rc;
  nfds_t i;

# ifdef _SC_OPEN_MAX
  static int sc_open_max = -1;

  if (nfd &lt; 0
      || (nfd &gt; sc_open_max
          &amp;&amp; (sc_open_max != -1
              || nfd &gt; (sc_open_max = sysconf (_SC_OPEN_MAX)))))
    {
      errno = EINVAL;
      return -1;
    }
# else /* !_SC_OPEN_MAX */
#  ifdef OPEN_MAX
  if (nfd &lt; 0 || nfd &gt; OPEN_MAX)
    {
      errno = EINVAL;
      return -1;
    }
#  endif /* OPEN_MAX -- else, no check is needed */
# endif /* !_SC_OPEN_MAX */

  /* EFAULT is not necessary to implement, but let's do it in the
     simplest case. */
  if (!pfd)
    {
      errno = EFAULT;
      return -1;
    }

  /* convert timeout number into a timeval structure */
  if (timeout == 0)
    {
      ptv = &amp;tv;
      ptv-&gt;tv_sec = 0;
      ptv-&gt;tv_usec = 0;
    }
  else if (timeout &gt; 0)
    {
      ptv = &amp;tv;
      ptv-&gt;tv_sec = timeout / 1000;
      ptv-&gt;tv_usec = (timeout % 1000) * 1000;
    }
  else if (timeout == INFTIM)
    /* wait forever */
    ptv = NULL;
  else
    {
      errno = EINVAL;
      return -1;
    }

  /* create fd sets and determine max fd */
  maxfd = -1;
  FD_ZERO (&amp;rfds);
  FD_ZERO (&amp;wfds);
  FD_ZERO (&amp;efds);
  for (i = 0; i &lt; nfd; i++)
    {
      if (pfd[i].fd &lt; 0)
        continue;

      if (pfd[i].events &amp; (POLLIN | POLLRDNORM))
        FD_SET (pfd[i].fd, &amp;rfds);

      /* see select(2): "the only exceptional condition detectable
         is out-of-band data received on a socket", hence we push
         POLLWRBAND events onto wfds instead of efds. */
      if (pfd[i].events &amp; (POLLOUT | POLLWRNORM | POLLWRBAND))
        FD_SET (pfd[i].fd, &amp;wfds);
      if (pfd[i].events &amp; (POLLPRI | POLLRDBAND))
        FD_SET (pfd[i].fd, &amp;efds);
      if (pfd[i].fd &gt;= maxfd
          &amp;&amp; (pfd[i].events &amp; (POLLIN | POLLOUT | POLLPRI
                               | POLLRDNORM | POLLRDBAND
                               | POLLWRNORM | POLLWRBAND)))
        {
          maxfd = pfd[i].fd;
          if (maxfd &gt; FD_SETSIZE)
            {
              errno = EOVERFLOW;
              return -1;
            }
        }
    }

  /* examine fd sets */
  rc = select (maxfd + 1, &amp;rfds, &amp;wfds, &amp;efds, ptv);
  if (rc &lt; 0)
    return rc;

  /* establish results */
  rc = 0;
  for (i = 0; i &lt; nfd; i++)
    if (pfd[i].fd &lt; 0)
      pfd[i].revents = 0;
    else
      {
        int happened = compute_revents (pfd[i].fd, pfd[i].events,
                                        &amp;rfds, &amp;wfds, &amp;efds);
        if (happened)
          {
            pfd[i].revents = happened;
            rc++;
          }
      }

  return rc;
#else
  static struct timeval tv0;
  static HANDLE hEvent;
  WSANETWORKEVENTS ev;
  HANDLE h, handle_array[FD_SETSIZE + 2];
  DWORD ret, wait_timeout, nhandles;
  fd_set rfds, wfds, xfds;
  BOOL poll_again;
  MSG msg;
  int rc = 0;
  nfds_t i;

  if (nfd &lt; 0 || timeout &lt; -1)
    {
      errno = EINVAL;
      return -1;
    }

  if (!hEvent)
    hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);

  handle_array[0] = hEvent;
  nhandles = 1;
  FD_ZERO (&amp;rfds);
  FD_ZERO (&amp;wfds);
  FD_ZERO (&amp;xfds);

  /* Classify socket handles and create fd sets. */
  for (i = 0; i &lt; nfd; i++)
    {
      int sought = pfd[i].events;
      pfd[i].revents = 0;
      if (pfd[i].fd &lt; 0)
        continue;
      if (!(sought &amp; (POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM | POLLWRBAND
                      | POLLPRI | POLLRDBAND)))
        continue;

      h = (HANDLE) _get_osfhandle (pfd[i].fd);
      assert (h != NULL);
      if (IsSocketHandle (h))
        {
          int requested = FD_CLOSE;

          /* see above; socket handles are mapped onto select.  */
          if (sought &amp; (POLLIN | POLLRDNORM))
            {
              requested |= FD_READ | FD_ACCEPT;
              FD_SET ((SOCKET) h, &amp;rfds);
            }
          if (sought &amp; (POLLOUT | POLLWRNORM | POLLWRBAND))
            {
              requested |= FD_WRITE | FD_CONNECT;
              FD_SET ((SOCKET) h, &amp;wfds);
            }
          if (sought &amp; (POLLPRI | POLLRDBAND))
            {
              requested |= FD_OOB;
              FD_SET ((SOCKET) h, &amp;xfds);
            }

          if (requested)
            WSAEventSelect ((SOCKET) h, hEvent, requested);
        }
      else
        {
          /* Poll now.  If we get an event, do not poll again.  Also,
             screen buffer handles are waitable, and they'll block until
             a character is available.  win32_compute_revents eliminates
             bits for the "wrong" direction. */
          pfd[i].revents = win32_compute_revents (h, &amp;sought);
          if (sought)
            handle_array[nhandles++] = h;
          if (pfd[i].revents)
            timeout = 0;
        }
    }

  if (select (0, &amp;rfds, &amp;wfds, &amp;xfds, &amp;tv0) &gt; 0)
    {
      /* Do MsgWaitForMultipleObjects anyway to dispatch messages, but
         no need to call select again.  */
      poll_again = FALSE;
      wait_timeout = 0;
    }
  else
    {
      poll_again = TRUE;
      if (timeout == INFTIM)
        wait_timeout = INFINITE;
      else
        wait_timeout = timeout;
    }

  for (;;)
    {
      ret = MsgWaitForMultipleObjects (nhandles, handle_array, FALSE,
                                       wait_timeout, QS_ALLINPUT);

      if (ret == WAIT_OBJECT_0 + nhandles)
        {
          /* new input of some other kind */
          BOOL bRet;
          while ((bRet = PeekMessage (&amp;msg, NULL, 0, 0, PM_REMOVE)) != 0)
            {
              TranslateMessage (&amp;msg);
              DispatchMessage (&amp;msg);
            }
        }
      else
        break;
    }

  if (poll_again)
    select (0, &amp;rfds, &amp;wfds, &amp;xfds, &amp;tv0);

  /* Place a sentinel at the end of the array.  */
  handle_array[nhandles] = NULL;
  nhandles = 1;
  for (i = 0; i &lt; nfd; i++)
    {
      int happened;

      if (pfd[i].fd &lt; 0)
        continue;
      if (!(pfd[i].events &amp; (POLLIN | POLLRDNORM |
                             POLLOUT | POLLWRNORM | POLLWRBAND)))
        continue;

      h = (HANDLE) _get_osfhandle (pfd[i].fd);
      if (h != handle_array[nhandles])
        {
          /* It's a socket.  */
          WSAEnumNetworkEvents ((SOCKET) h, NULL, &amp;ev);
          WSAEventSelect ((SOCKET) h, 0, 0);

          /* If we're lucky, WSAEnumNetworkEvents already provided a way
             to distinguish FD_READ and FD_ACCEPT; this saves a recv later.  */
          if (FD_ISSET ((SOCKET) h, &amp;rfds)
              &amp;&amp; !(ev.lNetworkEvents &amp; (FD_READ | FD_ACCEPT)))
            ev.lNetworkEvents |= FD_READ | FD_ACCEPT;
          if (FD_ISSET ((SOCKET) h, &amp;wfds))
            ev.lNetworkEvents |= FD_WRITE | FD_CONNECT;
          if (FD_ISSET ((SOCKET) h, &amp;xfds))
            ev.lNetworkEvents |= FD_OOB;

          happened = win32_compute_revents_socket ((SOCKET) h, pfd[i].events,
                                                   ev.lNetworkEvents);
        }
      else
        {
          /* Not a socket.  */
          int sought = pfd[i].events;
          happened = win32_compute_revents (h, &amp;sought);
          nhandles++;
        }

       if ((pfd[i].revents |= happened) != 0)
        rc++;
    }

  return rc;
#endif
}

