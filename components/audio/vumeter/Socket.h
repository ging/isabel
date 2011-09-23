/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __Socket_h__
#define __Socket_h__

#include "inet_family.h"

#ifdef WIN32
// Red
#include "myTime.h"
#include <io.h>
#include <Ws2tcpip.h>

// Ficheros
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>


#define open	_open
#define close	_close

#define IP6_SCOPE_GLOBAL 1

#else

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define IP6_SCOPE_LINK 128
#define IP6_SCOPE_SITE 64
#define IP6_SCOPE_GLOBAL 0

#endif

class Address {
 private:
  int protocol;
  struct sockaddr_storage ss;
 public:
  Address(int protocol);
  Address(Address *address);
  Address(struct sockaddr *addr, int protocol);
  ~Address();
  void Gethostname(char *name, size_t len, bool byName = 1);
  int Getport();
  int Setport(int port);
  int Resolve(const char *hostname, int port);
  socklen_t Getsize();
  int Setsockaddr(struct sockaddr *addr, int protocol);
  struct sockaddr *Getsockaddr();
  int Getprotocol();
  int Equals(Address *addr2);
  int Hostequals(Address *addr2);
  int IsMulticast();
  void Print();

  static int Getownaddress(char *interface, char *dstbuffer, int buffersize);
  static int Getanyownaddress6(char *interface, char *dstbuffer, int buffersize);
  static int Getownaddress6(char *interface, char *dstbuffer, int buffersize, int scope = IP6_SCOPE_GLOBAL);
  static int Getownbcastaddress(char *interface, char *dstbuffer, int buffersize);

};

class Socket {
 private:
  int sockfd;
 public:
  static Socket *Create(int domain, int type);
  Socket(int sockfd);
  Socket(int domain, int type);
  Socket(int domain, int type, int protocol);
  ~Socket();
  int Getfd();
  inline int GetFD() { return Getfd(); };
  int Bind(Address *address);
  int Getport();
  int Send(unsigned char *buffer, unsigned int buffersize);
  int Read(unsigned char *buffer, unsigned int buffersize);

#ifndef WIN32

  int EnableMulticast(Address *address);

#endif

  int Sendto(const void *msg, size_t len, int flags, Address *dest_addr);
  int Recvfrom(void *buf, size_t len, int flags, Address *from_addr);
  int Listen(int backlog);
  int Connect(Address *serv_addr);
  Socket *Accept(Address *from_addr);
  void SetSocketMaxBuffers();
  int Close();
  static int Isfree(int port, int type);
};

#ifdef WIN32

void InitiateLibrary();

#endif

#endif

