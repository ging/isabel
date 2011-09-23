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
#ifndef __VumeterClient_H__
#define __VumeterClient_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "Socket.h"
#include "JitterBuffer.h"

typedef struct {
  unsigned int index;
  char power;
} VumeterPacket;

class VumeterClient {
  Socket *s;
  char hostname[1024];
  unsigned short int port;
  int connected;
  Address *remote;

  unsigned int ts;
  unsigned short int seq;
  struct timeval lasttv;
  int firstTime;
 public:
  VumeterClient();
  ~VumeterClient();
  int Connect(char *hostname, unsigned short int port);
  int UpdateVumeter(unsigned int index, int power);
};


#endif
