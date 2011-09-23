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
/////////////////////////////////////////////////////////////////////////
//
// $Id: localaddress.c 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2006. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) 
{
    addrinfo hints, *res, *ressave;
    int      gai_error, sockfd;
    sockaddr_storage saddr;
    char clienthost[NI_MAXHOST];
    char clientservice[NI_MAXSERV];

    memset(&hints, 0, sizeof(struct addrinfo));

    if (argc != 2) {
       printf("Usage error: isabel_localaddress <hostname>\n");
       exit(4);
    }

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_flags    = AI_PASSIVE;

    if ((gai_error = getaddrinfo(argv[1], 0, &hints, &res))) {
        printf("  getaddrinfo error:: address failed. Host: [%s] Error: [%s]\n",
                argv[1],  gai_strerror(gai_error));
        exit(2);
    }

    ressave=res;

    /*
       Try open socket with each address getaddrinfo returned,
       until we get a valid listening socket.
    */
    sockfd=-1;
    while (res) {
        sockfd = socket(res->ai_family,
                        SOCK_STREAM,
                        res->ai_protocol);

        
        memset(clienthost, 0, sizeof(clienthost));
        memset(clientservice, 0, sizeof(clientservice));

        getnameinfo(res->ai_addr, res->ai_addrlen,
                    clienthost, sizeof(clienthost),
                    clientservice, sizeof(clientservice),
                    NI_NUMERICHOST);

        if (!(sockfd < 0)) {
            if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
                 printf("%s (resolved to %s) detected as a local network interface.\n", 
                        argv[1], clienthost);
                 break;
            } else {
                 printf("%s (resolved to %s) detected as a remote network interface.\n", 
                        argv[1], clienthost);
            }

            close(sockfd);
            sockfd=-1;
        } else {
            printf("SOCKET=%d STREAM=%d DGRAM=%d ERROR=%m\n",
                   res->ai_socktype, SOCK_STREAM, SOCK_DGRAM);
        }
        res = res->ai_next;
    }

    if (sockfd < 0) {
        freeaddrinfo(ressave);
        exit(1);
    }

    close(sockfd);

    freeaddrinfo(ressave);

    exit(0);
}





