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
 * $Id: gethostbyname.c 20206 2010-04-08 10:55:00Z gabriel $
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

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
get_addr4(char *node)
{
    addrinfo hints, *res;
    sockaddr_in sin;
    char *addr;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    if ( getaddrinfo(node, NULL, &hints, &res) ) {
        return 1;
    }

    char *addrStr = (char *)malloc(256);
    memset(addrStr, 0, 256);

    memset(&sin, 0, sizeof(sin));
    memcpy(&sin, res->ai_addr, res->ai_addrlen);
    addr = (char *)&sin.sin_addr;

    if (inet_ntop(res->ai_family, addr, addrStr, 256 ) != NULL) {
        printf("%s\n",addrStr);

        free(addrStr);
        freeaddrinfo(res);

        exit(0);
    }

    return 1;
}

int
get_addr6(char *node)
{
    addrinfo hints, *res;
    sockaddr_in6 sin6;
    char *addr;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET6;

    if ( getaddrinfo(node, NULL, &hints, &res) ) {
        return 1;
    }

    char *addrStr = (char *)malloc(256);
    memset(addrStr, 0, 256);

    memset(&sin6, 0, sizeof(sin6));
    memcpy(&sin6, res->ai_addr, res->ai_addrlen);
    addr = (char *)&sin6.sin6_addr;

    if (inet_ntop(res->ai_family, addr, addrStr, 256 ) != NULL) {
        printf("%s\n",addrStr);

        free(addrStr);
        freeaddrinfo(res);

        exit(0);
    }

    return 1;
}

int
main(int argc, char **argv)
{
    if ( argc != 2 ) {
      printf("Usage: %s <hostname>\n",argv[0]);
      exit(1);
    }

    char *_h = argv[1];

    get_addr4(_h);

    get_addr6(_h);

    printf("isabel_gethostbyname:: address failed. Host: [%s]\n", _h);

    exit(1);
}

