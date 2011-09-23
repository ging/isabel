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
#ifndef __JitterBuffer_H__
#define __JitterBuffer_H__

// Errores
#include <errno.h>
#include "myTime.h"
#include "Thread.h"

#include <unistd.h>
#include <netinet/in.h>

typedef int ChannelId;

typedef struct {
   unsigned short word;
   unsigned short seq;
   unsigned int ts;
   ChannelId ssrc;
} SoundPacket;

typedef struct {
  unsigned char *data;
  size_t len;
  int pt;
  struct timeval tv;
  unsigned char *crcs;
  int crcsSize;
  unsigned char *eh;
  int ehSize;
  int mark;
  unsigned int ts;
  unsigned short seq;
  unsigned int ssrc;
  int usable;
} RTPPacket;

typedef struct Nodo {
   struct Nodo *next; // Nodo que tiene un ts posterior
   struct Nodo *ant;
   RTPPacket *frame;
} Nodo;

// Con este Jitter buffer es necesario que aunque el audio no se envie se incremente el ts
class JitterBuffer {
  Nodo *first; // Primero que hay que consumir
  char *name;
  unsigned int msBuffer; // tamaño maximo del buffer en ms
  unsigned int msBuffered; // ms de tiempo guardados (independientemente del numero de paquetes)
  unsigned int delay; // ms de tiempo minimo antes de permitir el primer pop (delay)
  unsigned int threshold;
  int enough;

  Mutex lock;
  struct timeval tvFirstPop;
  struct timeval tvFirstPush;
  struct timeval tvLastEnough;
  struct timeval tvLastNotEnough;
  struct timeval tvLastFrame;
  unsigned int firstTSPopped;
  unsigned int lastSeqPopped;
  int firstPop;
  unsigned int lastTSPushed;
  unsigned int firstTSPushed;
  int firstPush;
  int nFrames;

  unsigned int lastTSPopped;

  unsigned int framesLostLleno;
  unsigned int framesLostViejo;
  unsigned int framesLostFirst;
  unsigned int framesLostInusable;
  unsigned int framesLostReset;
  unsigned int framesInexistentes;
  unsigned int vecesRetrasado;
  unsigned int framesDuplicates;
  int diferencial;
  float maxjitter;
  float maxvar;

  unsigned int lastSeqPushed; // Ultimo metido en el jb
  unsigned int lastLastSeqPushed; // ultimo usado para la cuenta de los perdidos
  unsigned int lastFramesInexistentes; // ultimo usado para la cuenta de los perdidos
  struct timeval tvLastLost;
 private:
  int Enough();
 public:
  JitterBuffer(char *name, unsigned int ms); // Pone como delay por defecto, un 10% del maximo
  ~JitterBuffer();
  RTPPacket *Pop();
  int Push(RTPPacket *frame);
  int SetDelay(unsigned int ms);
  unsigned int GetDelay();
  int SetSize(unsigned int ms);
  unsigned int GetSize();
  int Reset();
  void Imprime();
  unsigned int GetMSBuffered();
  unsigned int GetThreshold();
  unsigned int GetFramesLostLleno();
  unsigned int GetFramesLostViejo();
  unsigned int GetFramesLostFirst();
  unsigned int GetFramesLostInusable();
  unsigned int GetFramesLostReset();
  unsigned int GetFramesInexistentes();
  unsigned int GetFramesDuplicates();
  int GetDifferential();
  char *GetInfo();
  float GetMaxJitter();
  float GetLostPercent();
};

RTPPacket *RTPPacketCopy(RTPPacket *rtpo);
RTPPacket *RTPPacketNew(unsigned char *data, size_t len);
int RTPPacketDelete(RTPPacket *rtp);

#endif

















