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
/* ----------------------------------------------------------------------
 *
 * $Id: fqdn.c 20206 2010-04-08 10:55:00Z gabriel $
 *
 * (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
 * Dec 31, 1999 Transfered to Agora Systems S.A.
 * (C) Copyright 2000-2006. Agora Systems S.A.
 *
   ------------------------------------------------------------------- */

#include <stdio.h>
#include <sys/socket.h>    
#include <sys/types.h>    
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{

  if ( argc != 2 ) {
    printf("Usage: %s <hostname>\n",argv[0]);
    exit(1);
  }


  char *_h = argv[1];

  addrinfo hints, *res;
  int      gai_error;
  sockaddr_in6 sin6;
  sockaddr_in sin;
  char *addr;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = 0;

  if ((gai_error = getaddrinfo(_h, "domain", &hints, &res))) {
     printf("isabel_gethostbyname:: getaddrinfo error:: "
             "address failed. Host: [%s] Error: [%s]\n",
             _h, gai_strerror(gai_error));
     exit(1);
  }

  printf("%s\n",res->ai_canonname);

  freeaddrinfo(res);
  exit(0);
}


