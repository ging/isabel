/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////

#include "VumeterClient.h"

VumeterClient::VumeterClient() {
  s = NULL;
  remote = NULL;
  hostname[0] = '\0';
  port = 0;
  connected = 0;
  ts = 0;
  seq = 0;
  firstTime = 1;
}

VumeterClient::~VumeterClient() {
  if (connected) {
    delete s;
    delete remote;
  }
}

int VumeterClient::Connect(char *hostname, unsigned short int port) {
  Address *localAddr = new Address(PF);
  if ( localAddr->Resolve(NULL, 0) < 0 ) {
    perror("Resolve 2");
    return -1;
  }
  s = new Socket(localAddr->Getprotocol(), SOCK_DGRAM);
  if ( s -> Bind(localAddr) < 0 ) {
    perror("bind 2\n");
    return -1;
  }
  delete localAddr;

  int reuse = 1;
  setsockopt(s->GetFD(), SOL_SOCKET, SO_REUSEADDR, (int *)&reuse, sizeof(reuse));

  remote = new Address(PF);
  if (remote->Resolve(hostname, port) < 0 ) {
    perror("DataProtocolAduioBind::Resolve");
    return -1;
  }



  if(fcntl(s->GetFD(), F_SETOWN, getpid())< 0) {
    perror("VumeterClient::Connect : setown");
    return -1;
  }
  connected = 1;
  return 1;
}

#define MAX_BUFFER 64*1024

int VumeterClient::UpdateVumeter(unsigned int index, int power) {
  VumeterPacket vp;
  struct timeval now;

  if (!connected) return -1;

  gettimeofday(&now, NULL);

  if (firstTime) {
    firstTime = 0;
    memcpy(&lasttv, &now, sizeof(lasttv));
  }
  unsigned int diff = ((now.tv_sec*1000000+now.tv_usec)-(lasttv.tv_sec*1000000+lasttv.tv_usec))/1000;
  memcpy(&lasttv, &now, sizeof(lasttv));
  ts += diff;
  vp.index = htonl(index);
  vp.power = (char)power;

  unsigned char *data = (unsigned char *)&vp;
  size_t len = sizeof(vp);
  int mark = 0;
  unsigned char packet[MAX_BUFFER];
  size_t packetLen;
  SoundPacket sp;
  unsigned short codec = 150;

  if (len > MAX_BUFFER) {
    perror("VumeterClient::UpdateVumeter sendData::len>4096*4\n");
  }
  packetLen = sizeof(SoundPacket)+len;
  codec = 150;
  sp.word = 0x8000; //10 0 0 0000 0 0 0000000
  
  sp.word = sp.word | codec;
  
  if (mark) sp.word = sp.word | 0x0080; // 10 0 0 0000 1 0 0000000
  
  sp.word = htons(sp.word);
  
  memcpy(packet+sizeof(SoundPacket), data, len);
  sp.seq = htons(seq);
  sp.ts = htonl(ts);
  sp.ssrc = htonl(index);
  seq++;
  memcpy(packet, &sp, sizeof(SoundPacket));
  if (s->Sendto((void *)packet, packetLen, 0, remote) < 0 ) {				   
    perror("VumeterClient::UpdateVumeter : sendto(1)");
    return -1;
  }
  return 1;
}
