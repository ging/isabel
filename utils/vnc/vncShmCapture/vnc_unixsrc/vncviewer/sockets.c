/*
 *  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */

/*
 * sockets.c - functions to deal with sockets.
 */

#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <assert.h>
#include <vncviewer.h>

void PrintInHex(char *buf, int len);

Bool errorMessageOnReadFailure = True;

#define BUF_SIZE 8192
static char buf[BUF_SIZE];
static char *bufoutptr = buf;
static int buffered = 0;
/*
 *     BEGIN: Changes to support IPv6 
 */
void GetPeerName(int sock,char *name);
void GetSockName(int sock,char *name);
/*
 *     END: Changes to support IPv6 
 */

/*
 * ReadFromRFBServer is called whenever we want to read some data from the RFB
 * server.  It is non-trivial for two reasons:
 *
 * 1. For efficiency it performs some intelligent buffering, avoiding invoking
 *    the read() system call too often.  For small chunks of data, it simply
 *    copies the data out of an internal buffer.  For large amounts of data it
 *    reads directly into the buffer provided by the caller.
 *
 * 2. Whenever read() would block, it invokes the Xt event dispatching
 *    mechanism to process X events.  In fact, this is the only place these
 *    events are processed, as there is no XtAppMainLoop in the program.
 */

static Bool rfbsockReady = False;
static void
rfbsockReadyCallback(XtPointer clientData, int *fd, XtInputId *id)
{
  rfbsockReady = True;
  XtRemoveInput(*id);
}

static void
ProcessXtEvents()
{
  rfbsockReady = False;
  XtAppAddInput(appContext, rfbsock, (XtPointer)XtInputReadMask,
		rfbsockReadyCallback, NULL);
  while (!rfbsockReady) {
    XtAppProcessEvent(appContext, XtIMAll);
  }
}

Bool
ReadFromRFBServer(char *out, unsigned int n)
{
  if (n <= buffered) {
    memcpy(out, bufoutptr, n);
    bufoutptr += n;
    buffered -= n;
    return True;
  }

  memcpy(out, bufoutptr, buffered);

  out += buffered;
  n -= buffered;

  bufoutptr = buf;
  buffered = 0;

  if (n <= BUF_SIZE) {

    while (buffered < n) {
      int i = read(rfbsock, buf + buffered, BUF_SIZE - buffered);
      if (i <= 0) {
	if (i < 0) {
	  if (errno == EWOULDBLOCK || errno == EAGAIN) {
	    ProcessXtEvents();
	    i = 0;
	  } else {
	    fprintf(stderr,programName);
	    perror(": read");
	    return False;
	  }
	} else {
	  if (errorMessageOnReadFailure) {
	    fprintf(stderr,"%s: VNC server closed connection\n",programName);
	  }
	  return False;
	}
      }
      buffered += i;
    }

    memcpy(out, bufoutptr, n);
    bufoutptr += n;
    buffered -= n;
    return True;

  } else {

    while (n > 0) {
      int i = read(rfbsock, out, n);
      if (i <= 0) {
	if (i < 0) {
	  if (errno == EWOULDBLOCK || errno == EAGAIN) {
	    ProcessXtEvents();
	    i = 0;
	  } else {
	    fprintf(stderr,programName);
	    perror(": read");
	    return False;
	  }
	} else {
	  if (errorMessageOnReadFailure) {
	    fprintf(stderr,"%s: VNC server closed connection\n",programName);
	  }
	  return False;
	}
      }
      out += i;
      n -= i;
    }

    return True;
  }
}


/*
 * Write an exact number of bytes, and don't return until you've sent them.
 */

Bool
WriteExact(int sock, char *buf, int n)
{
  fd_set fds;
  int i = 0;
  int j;

  while (i < n) {
    j = write(sock, buf + i, (n - i));
    if (j <= 0) {
      if (j < 0) {
	if (errno == EWOULDBLOCK || errno == EAGAIN) {
	  FD_ZERO(&fds);
	  FD_SET(rfbsock,&fds);

	  if (select(rfbsock+1, NULL, &fds, NULL, NULL) <= 0) {
	    fprintf(stderr,programName);
	    perror(": select");
	    return False;
	  }
	  j = 0;
	} else {
	  fprintf(stderr,programName);
	  perror(": write");
	  return False;
	}
      } else {
	fprintf(stderr,"%s: write failed\n",programName);
	return False;
      }
    }
    i += j;
  }
  return True;
}


/*
 * ConnectToTcpAddr connects to the given TCP port.
 */

int
ConnectToTcpAddr(const char *host, int port)
{
/* ----------------------------------------------------------------
    -----------------------------------------------------------------
     BEGIN: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */

  int sock;
  int error;
  struct addrinfo hints, *res, *aux;
  int one = 1;

  char service[32];
  memset(service, 0, 32);
  sprintf(service, "%d", port);

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  fprintf(stderr,"Trying to connect to host=%s on port=%s\n", host, service);
  error = getaddrinfo(host, service, &hints, &res);
  if (error) {
    fprintf(stderr,programName);
    perror(": ConnectToTcpAddr: getaddrinfo");
    return -1;
  }

  for (aux = res; aux ;aux = aux->ai_next) {
    sock = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
    if (sock < 0) {
      fprintf(stderr,programName);
      perror(": ConnectToTcpAddr: socket");
      continue;
    }

  if (connect(sock, aux->ai_addr, aux->ai_addrlen) < 0) {
      fprintf(stderr,programName);
      perror(": ConnectToTcpAddr: connect");
      close(sock);
      continue;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one)) < 0) {
      fprintf(stderr,programName);
      perror(": ConnectToTcpAddr: setsockopt");
      close(sock);
      continue;
    }

    freeaddrinfo(res);
    return sock;
  }
  
  freeaddrinfo(res);
  return -1;

  /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     END: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
}


/*
 * FindFreeTcpPort tries to find unused TCP port in the range
 * (TUNNEL_PORT_OFFSET, TUNNEL_PORT_OFFSET + 99]. Returns 0 on failure.
 */

int
FindFreeTcpPort(void)
{
/* ----------------------------------------------------------------
    -----------------------------------------------------------------
     BEGIN: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */

  int sock, port;
  struct sockaddr_storage addr;


  memset(&addr, 0, sizeof(addr));
  addr.ss_family = AF_INET6;

  sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    addr.ss_family = AF_INET;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr,programName);
        perror(": FindFreeTcpPort: socket");
        return 0;
    }
  }

  for (port = TUNNEL_PORT_OFFSET + 99; port > TUNNEL_PORT_OFFSET; port--) {
    if (addr.ss_family==AF_INET6)
      ((struct sockaddr_in *)&addr)->sin_port = htons((unsigned short)port);
    else 
      ((struct sockaddr_in6 *)&addr)->sin6_port = htons((unsigned short)port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
      close(sock);
      return port;
    }
  }

  close(sock);
  return 0;

  /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     END: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
}


/*
 * ListenAtTcpPort starts listening at the given TCP port.
 */

int
ListenAtTcpPort(int port)
{
   /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     BEGIN: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
  int sock, error;
  struct addrinfo hints, *res, *aux;
  int one = 1;

  char service[32];
  memset(service, 0, 32);
  sprintf(service, "%d", port);

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;

  if (error = getaddrinfo(NULL, service, &hints, &res)) {
    fprintf(stderr,programName);
    perror(": ListenAtTcpPort: getaddrinfo");
    return -1;
  }

  for (aux = res; aux; aux = aux->ai_next) {
    sock = socket(aux->ai_family, aux->ai_socktype, aux->ai_protocol);
    if (sock < 0) {
      fprintf(stderr,programName);
      perror(": ListenAtTcpPort: socket");
      continue;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
                   (const char *)&one, sizeof(one)) < 0) 
    {
      fprintf(stderr,programName);
      perror(": ListenAtTcpPort: setsockopt");
      close(sock);
      continue;
    }

    if (bind(sock, aux->ai_addr, aux->ai_addrlen) < 0) {
      fprintf(stderr,programName);
      perror(": ListenAtTcpPort: bind");
      close(sock);
      continue;
    }

    if (listen(sock, 5) < 0) {
      fprintf(stderr,programName);
      perror(": ListenAtTcpPort: listen");
      close(sock);
      continue;
    }
  
    freeaddrinfo(res);
    return sock;
  }

  freeaddrinfo(res);
  return -1;
  /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     END: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
}


/*
 * AcceptTcpConnection accepts a TCP connection.
 */

int
AcceptTcpConnection(int listenSock)
{
  int sock;
 /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     BEGIN: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */

  struct sockaddr_storage addr;

  /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     END: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
  int addrlen = sizeof(addr);
  int one = 1;

  sock = accept(listenSock, (struct sockaddr *) &addr, &addrlen);
  if (sock < 0) {
    fprintf(stderr,programName);
    perror(": AcceptTcpConnection: accept");
    return -1;
  }

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		 (char *)&one, sizeof(one)) < 0) {
    fprintf(stderr,programName);
    perror(": AcceptTcpConnection: setsockopt");
    close(sock);
    return -1;
  }

  return sock;
}


/*
 * SetNonBlocking sets a socket into non-blocking mode.
 */

Bool
SetNonBlocking(int sock)
{
  if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
    fprintf(stderr,programName);
    perror(": AcceptTcpConnection: fcntl");
    return False;
  }
  return True;
}


/*
 * StringToIPAddr - convert a host string to an IP address.
 */

Bool
StringToIPAddr(const char *str, unsigned int *addr)
{
  struct hostent *hp;

  if (strcmp(str,"") == 0) {
    *addr = 0; /* local */
    return True;
  }

  *addr = inet_addr(str);

  if (*addr != -1)
    return True;

//Looking for IPv6
hp = gethostbyname2(str, AF_INET6);

if (( hp== NULL))
{
    perror("I Can't resolve IPv6\n"); 
	hp = gethostbyname2(str, AF_INET);
	if (( hp== NULL)){
		perror("Can't resolve IPv4\n");
		return False;
	}
	else{
	*addr = *(unsigned int *)hp->h_addr;
    return True;
	}
}
else{
 *addr = *(unsigned int *)hp->h_addr;
    return True;
 }
  return False;
}


/*
 * Test if the other end of a socket is on the same machine.
 */

Bool
SameMachine(int sock)
{
  /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     BEGIN: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
  struct sockaddr_storage peeraddr, myaddr;    /* input */
  int result;
  char *peername;
  char *localname;
  result=0;
  peername=(char *) malloc(50);
  localname=(char *) malloc(50);
  GetPeerName(sock,peername);
  GetSockName(sock,localname);

  if(appData.shmCapture){
     
  fprintf(stderr,"Hay que usar memoria compartida idMemoria=%d, Tama=%d\n",appData.idShMemory,appData.sizeShMemory);
  }
  fprintf(stderr,"remote host=%s ,local host=%s",peername,localname);
  if(strcmp(peername,localname)){
    result =0;
  }
  else{
     result=1;
}
  free(peername);
  free(localname);
  return result;

  /* ----------------------------------------------------------------
    -----------------------------------------------------------------
     END: Changes to support IPv6 
    -----------------------------------------------------------------
    -----------------------------------------------------------------
  */
}


/*
 * Print out the contents of a packet for debugging.
 */

/*
Helper functions
*/
void GetPeerName(int sock,char *name){
 struct sockaddr_storage sockinfo;
        int sockinfosize = sizeof(sockinfo);
        int ret;
        // Get the peer address for the client socket
        getpeername(sock, (struct sockaddr *)&sockinfo, &sockinfosize);
        ret=getnameinfo((struct sockaddr *)&sockinfo,sockinfosize,name,40,NULL,0,NI_NUMERICHOST);
}
////////////////////////////


void GetSockName(int sock,char *name)
{
	struct sockaddr_storage	sockinfo;
	int					sockinfosize = sizeof(sockinfo);
	// Get the peer address for the client socket
	getsockname(sock, (struct sockaddr *)&sockinfo, &sockinfosize);
	getnameinfo((struct sockaddr *)&sockinfo,sockinfosize,name,40,NULL,0,NI_NUMERICHOST);
}

/*
/Helper Functions
 */

void
PrintInHex(char *buf, int len)
{
  int i, j;
  char c, str[17];

  str[16] = 0;

  fprintf(stderr,"ReadExact: ");

  for (i = 0; i < len; i++)
    {
      if ((i % 16 == 0) && (i != 0)) {
	fprintf(stderr,"           ");
      }
      c = buf[i];
      str[i % 16] = (((c > 31) && (c < 127)) ? c : '.');
      fprintf(stderr,"%02x ",(unsigned char)c);
      if ((i % 4) == 3)
	fprintf(stderr," ");
      if ((i % 16) == 15)
	{
	  fprintf(stderr,"%s\n",str);
	}
    }
  if ((i % 16) != 0)
    {
      for (j = i % 16; j < 16; j++)
	{
	  fprintf(stderr,"   ");
	  if ((j % 4) == 3) fprintf(stderr," ");
	}
      str[i % 16] = 0;
      fprintf(stderr,"%s\n",str);
    }

  fflush(stderr);
}
