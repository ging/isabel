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

#include "JitterBuffer.h"

static int DEBUG = 0;

void JitterBuffer::Imprime() {
  Nodo *viejo;

  viejo = first;
  printf("JB <%s> msBuffer = %u, msBuffered = %u, delay = %u, threshold = %u, firstTSPushed = %u, firstTSPopped = %u:", 
	 name, msBuffer, msBuffered, delay, threshold, firstTSPushed, firstTSPopped);
  while (viejo) {
    printf(" TS = %u", viejo -> frame -> ts);
    viejo = viejo -> next;
  }
  printf("\n");
}

char *JitterBuffer::GetInfo() {
  char cadena[1024*8] = "";
  char aux[1024];
  Nodo *viejo;

  lock.Wait();
  viejo = first;
  sprintf(cadena, "JB <%s> msBuffer = %u, msBuffered = %u, delay = %u, threshold = %u, firstTSPushed = %u, firstTSPopped = %u:\n", 
	 name, msBuffer, msBuffered, delay, threshold, firstTSPushed, firstTSPopped);
  while (viejo) {
    sprintf(aux, " TS = %u", viejo -> frame -> ts);
    strcat(cadena, aux);
    viejo = viejo -> next;
  }
  strcat(cadena, "\n");
  lock.Signal();
  return strdup(cadena);
}

JitterBuffer::JitterBuffer(char *name, unsigned int ms) {

  this -> first = NULL;
  this -> name = strdup(name);
  this -> msBuffer = ms;
  this -> msBuffered =0;
  this -> delay = (int)(ms*25.0/100.0);

  gettimeofday(&tvFirstPop, NULL);
  gettimeofday(&tvFirstPush, NULL);
  gettimeofday(&tvLastEnough, NULL);
  gettimeofday(&tvLastNotEnough, NULL);

  firstTSPopped = 0;
  firstPop = 1;

  lastTSPushed = 0;
  firstTSPushed = 0;
  firstPush = 1;

  threshold = delay;
  enough = 0;
  framesLostLleno = 0;
  framesLostViejo = 0;
  framesLostInusable = 0;
  framesLostReset = 0;
  framesLostFirst = 0;
  framesInexistentes = 0;
  nFrames = 0;
  lastSeqPopped = 0;
  vecesRetrasado = 0;
  framesDuplicates = 0;
  diferencial = 0;
  maxjitter = 0.0;
  maxvar = 0.0;
  lastSeqPushed = 0;
  lastLastSeqPushed = 0;
  lastTSPopped = 0;
}

JitterBuffer::~JitterBuffer() {
  Nodo *viejo;
  lock.Wait();
  while (first) {
    viejo = first;
    first = first -> next;
    delete viejo -> frame;
    free(viejo);
  }
  if (DEBUG) printf("JB %s has lost %u frames por estar lleno y %d por hacerse viejos\n", name, framesLostLleno, framesLostViejo);
  free(name);
  lock.Signal();
}

// Inserta un nuevo Frame. El usuario del JitterBuffer no debe borrar el frame creado para insertarlo
int JitterBuffer::Push(RTPPacket *frame) {
  Nodo *nuevo;
  Nodo *ant, *sig;
  unsigned int ts;

  nuevo = (Nodo *)malloc(sizeof(Nodo));
  if (!nuevo) {
    perror("JitterBuffer::Push:malloc");
    return -1;
  }
  nuevo -> frame = frame; 
  nuevo -> ant = nuevo -> next = NULL;
  ts = frame -> ts;

  lock.Wait();

  
  if (lastTSPopped > frame -> ts) {
    // printf("Llega %u y el last es %u\n", frame -> ts, lastTSPopped);
    //  lock.Signal();
    // return -1;
  }
  

  if  ((msBuffered > threshold) && (firstPop)) {
    if (first) {
      Nodo *viejo;
      viejo = first;
      first = first -> next;
      if (first) {
	first -> ant = NULL;
	msBuffered -= (first -> frame -> ts - viejo -> frame -> ts);
	firstTSPushed = first -> frame -> ts;
      }
      delete viejo -> frame;
      free(viejo);
      framesLostFirst++;
      nFrames--;
    }
  }
  sig = first;
  ant = first;
  while (sig) {
    // Si encontramos un frame posterior al nuevo colocamos el nuevo delante de el
    if (sig -> frame -> ts > ts) {
      ant = sig -> ant;
      break;
    }
    ant = sig;
    sig = sig -> next;
  }
  nuevo -> next = sig;
  nuevo -> ant = ant;
  // Cuando metemos el primer paquete cogemos el last
  if (firstPush) {

    if (DEBUG) printf("Push: Primer TS = %u\n", ts);
    gettimeofday(&tvFirstPush, NULL);
    lastTSPushed = ts;
    firstTSPushed = ts;
    firstPush = 0;
  }
  if (ant) {
    // Comprobar duplicados
    if (ant -> frame -> ts == ts) {
      if (DEBUG) printf("[%s]Push TS=%d Duplicado\n", name, ts);
      framesDuplicates++;
      free(nuevo);
      lock.Signal();
      return -1;
    }
    // Si ya tiene alguien detras (antes por sacar) podriamos estar metiendo el último por sacar. 
    // Aun no lo sabemos hasta comprobar sig
    ant -> next = nuevo;
  } else {
    // Si no hay nadie antes que el podriamos estar metiendo el primer frame por lo que aun no tendriamos nada buffered
    first = nuevo;
  }
  if (sig) {
    // No es el ultimo hasta el momento por lo que no estamos añadiendo mas ms al buffer ya que estamos rellenando un hueco
    sig -> ant = nuevo;
  } else {
    // Es el ultimo hasta el momento por lo que el ultimo ts pushed sera el del anterior si lo había
    msBuffered = msBuffered + (ts-lastTSPushed);
    lastTSPushed = ts;
    lastSeqPushed = frame -> seq;
  }
  nFrames++;
  if (DEBUG > 2) printf("JB::Push <%s> buffered %05ums (%05u-%05u). TS=%u  Min=%u Max=%uThreshold=%u\n", 
		    name, 
		    msBuffered,
		    firstTSPushed,
		    lastTSPushed,
		    ts, 
		    delay,
		    msBuffer,
		    threshold);
  //  Imprime();
  lock.Signal();
  return 1;
}

// Suprime el paquete mas viejo de la lista y lo devuelve. Si no hay devuelve NULL.
RTPPacket *JitterBuffer::Pop() {
  Nodo *popped;
  RTPPacket *frame = NULL;
  struct timeval tvNew;
  u64 t, firstTS; // ms transcurridos desde primer pop
  int ft = 0;
  
  lock.Wait();
  // Si no hay salgo
  if (!first) {
    lock.Signal();
    return NULL;
  }
  // SI hay calculo el tiempo objetivo
  gettimeofday(&tvNew, NULL);
  t = (tvNew.tv_sec*1000000+tvNew.tv_usec)-(tvFirstPop.tv_sec*1000000+tvFirstPop.tv_usec);
  t /= 1000;
  if (t < 0) t = 0;
  // Si ya he sacado antes
  if (!firstPop) {    
    // Este bucle debe dejar en first el siguiente para coger
    while (first) {
      firstTS = (first -> frame -> ts - firstTSPopped);
           
      //      if (t < firstTS) printf("No toca todavía\n");

      diferencial = (diferencial*25+(t-((first -> frame -> ts - firstTSPopped)))*75)/100;
      break;
    }
    if (!first) {
      msBuffered = 0;
      firstPush = 1;
      firstPop = 1;
      lock.Signal();
      return NULL;
    }
  } else {
    ft = 1;
    
    // Delay minimo
    if (msBuffered < delay) {
      lock.Signal();
      return NULL;
    }
  }
  
  if (!first -> frame -> usable) {
    if (first -> next) {
      if (!first -> next -> frame -> usable) {
	lock.Signal();
	return NULL;
      } else {
	frame = first -> frame;
	popped = first;
	first = first -> next;
	if (first) {
	  first -> ant = NULL;
	}
	nFrames--;
	framesLostInusable++;
	free(popped);
	delete frame;
	printf("no usable2\n");
      }
    } else {
      lock.Signal();
      return NULL;
    }
  }

  if (firstPop) {
    //printf("Ahora ajustaría el delay a %f %d %d\n", 50+maxjitter+maxvar*4.0, msBuffered, threshold);
    // printf("delay = %f\n", 50+maxjitter+maxvar*4.0);
    // Si es el primer pop se da lo que haya sin comprobar tiempos ni restar tiempo al buffer
    firstPop = 0;
    firstTSPopped = first -> frame -> ts;
    memcpy(&tvFirstPop, &tvNew, sizeof(tvFirstPop));
    lastSeqPopped = first -> frame -> seq;
    if (DEBUG) printf("First Pop at %08lu:%08lu\nFirst Push at %08lu:%08lu\n", 
		      tvFirstPop.tv_sec, tvFirstPop.tv_usec, 
		      tvFirstPush.tv_sec, tvFirstPush.tv_usec);
  }

  frame = first -> frame;

  popped = first;
  first = first -> next;
  if (first) {
    first -> ant = NULL;
  }
  nFrames--;
  free(popped);

  if (first) {
    msBuffered -= (first -> frame -> ts - frame -> ts);
    firstTSPushed = first -> frame -> ts;
  } else {
    msBuffered = 0;
    firstPush = 1;
  }
    // lo que pasa es que entran en el jb viejos
  if (!ft && (frame -> seq != ((lastSeqPopped+1)%65536))) {
    int faltan;
    if (frame -> seq > lastSeqPopped+1) {
      faltan = (frame -> seq - (lastSeqPopped+1));
    } else {
      if (frame -> seq < 50) faltan = ((frame -> seq+65535) - (lastSeqPopped+1));
      else faltan = 0;
    }
    framesInexistentes += faltan;
  }
  { // Medida del max jitter;
    float diff;
    diff = ((frame -> tv.tv_sec*1000000+frame -> tv.tv_usec)-(tvLastFrame.tv_sec*1000000+tvLastFrame.tv_usec));
    diff /= 1000;
    maxjitter = maxjitter*0.9+diff*0.1;
    maxvar = maxvar*0.9+0.1*(maxjitter-maxvar);
    memcpy(&tvLastFrame, &(frame -> tv), sizeof(tvLastFrame));
  }
  lastSeqPopped = frame -> seq;
  lastTSPopped = frame -> ts;
  if (DEBUG == 2) printf("JB::Pop  <%s> buffered %05ums T = %llums TS=%ums(%u) Seq = %u\n", 
		    name, 
		    msBuffered,
		    t, 
		    frame -> ts-firstTSPopped, 
			 frame -> ts,
			 frame -> seq
		    );
  //  Imprime();
  lock.Signal();
  return frame;
}

int JitterBuffer::Enough() {
  struct timeval tvNew;
  u64 t; // ms transcurridos desde primer pop

  //  printf("msBuffered = %04u threshold = %04u enough = %d nframes = %d firstPop = %d\n", msBuffered, threshold, enough, nFrames, firstPop);
  //    Imprime();
  if (msBuffered >= threshold) {
    if (!enough) {
      gettimeofday(&tvLastEnough, NULL);
    }
    enough = 1;
    gettimeofday(&tvNew, NULL);
    t = (tvNew.tv_sec*1000000+tvNew.tv_usec)-(tvLastEnough.tv_sec*1000000+tvLastEnough.tv_usec);
    t /= 1000;
    if (t > 500) { // Cuantos ms funcionando bien 500
      threshold -= 10;//(unsigned int)(threshold*75.0/100.0);
      if (threshold < delay) {
	threshold = delay;
      } else {
	if (DEBUG) 
	  printf("Reduciendo buffer del canal %s a %ums(delay=%u, size=%u)\n", name, threshold, delay, msBuffer);
      }
      memcpy(&tvLastEnough, &tvNew, sizeof(tvLastEnough));
    }
  } else {
    if ((!first) && (enough)) { // Si antes había y ahora no
      enough = 0;
      threshold += 10;
      
      if (threshold > (msBuffer*90/100)) {
	threshold = msBuffer*90/100;
      } else {
	if (DEBUG) 
	  printf("Aumentado buffer del canal %s a %ums(delay=%u, size=%u)\n", name, threshold, delay, msBuffer);      
      }
    }
  }
  return enough;
}

int JitterBuffer::SetDelay(unsigned int ms) {
  Reset();
  lock.Wait();
  if (ms < msBuffered) {
    if (DEBUG) {
      perror("JitterBuffer::SetDelay : reduciendo el tamaño a una cantidad menor que la que tenemos guardada");
    }
    return -1;
  }
  delay = ms;
  threshold = delay;
  lock.Signal();
  return 1;
}

unsigned int JitterBuffer::GetDelay() {
  return delay;
}

int JitterBuffer::SetSize(unsigned int ms) {
  Reset();
  lock.Wait();
  if (ms < msBuffered) {
    if (DEBUG) {
      perror("JitterBuffer::SetSize : reduciendo el tamaño a una cantidad menor que la que tenemos guardada");
      return -1;
    }
  }
  msBuffer = ms;
  lock.Signal();
  return 1;
}

unsigned int JitterBuffer::GetSize() {
  return msBuffer;
}

int JitterBuffer::Reset() {
  Nodo *viejo;

  lock.Wait();
  while (first) {
    viejo = first;
    first = first -> next;
    delete viejo -> frame;
    free(viejo);
    framesLostReset++;
  }
  msBuffered = 0;
  firstTSPopped = 0;
  firstPop = 1;
  lastTSPushed = 0;
  firstTSPushed = 0;
  firstPush = 1;
  threshold = delay;
  enough = 0;
  nFrames = 0;
  lock.Signal();
  return 1;
}

unsigned int JitterBuffer::GetMSBuffered() {
  return msBuffered;
}

unsigned int JitterBuffer::GetThreshold() {
  return threshold;
}

unsigned int JitterBuffer::GetFramesLostLleno() {
  return framesLostLleno;
}

unsigned int JitterBuffer::GetFramesLostViejo() {
  return framesLostViejo;
}

unsigned int JitterBuffer::GetFramesLostReset() {
  return framesLostReset;
}

unsigned int JitterBuffer::GetFramesLostFirst() {
  return framesLostFirst;
}

unsigned int JitterBuffer::GetFramesLostInusable() {
  return framesLostInusable;
}

unsigned int JitterBuffer::GetFramesInexistentes() {
  return framesInexistentes;
}

unsigned int JitterBuffer::GetFramesDuplicates() {
  return framesDuplicates;
}

int JitterBuffer::GetDifferential() {
  return diferencial;
}

float JitterBuffer::GetMaxJitter() {
  return maxjitter;
}

float JitterBuffer::GetLostPercent() {
  struct timeval tv;
  float diff;
  float retval;

  gettimeofday(&tv, NULL);
  diff = (tv.tv_sec*1000000+tv.tv_usec) - (tvLastLost.tv_sec*1000000 + tvLastLost.tv_usec);
  diff = diff / 1000;
  memcpy(&tvLastLost, &tv, sizeof(tv));
  retval = (100.0*(framesInexistentes-lastFramesInexistentes)/(lastSeqPushed-lastLastSeqPushed+1));
  lastLastSeqPushed = lastSeqPushed;
  lastFramesInexistentes = framesInexistentes;
  return retval;
}

/////////////////////////////////////////

RTPPacket *RTPPacketNew(unsigned char *buffer, size_t bufferSize) {
  RTPPacket *rtp = NULL;
  SoundPacket sp;
  unsigned char *data = NULL;
  int datalen = 0;
  int codec;
  int mark;
  int ehlen = 0, crcslen = 0;
  unsigned char *crcs = NULL;
  unsigned char *eh = NULL;
  struct timeval tv;
   
  gettimeofday(&tv, NULL);
  if (bufferSize < sizeof(SoundPacket)) {
    perror("RTPPacketNew:: tamaño menor del paquete");
    return NULL;
  }
  memcpy(&sp, buffer, sizeof(SoundPacket));
  sp.word = ntohs(sp.word);
  sp.seq = ntohs(sp.seq);
  sp.ts = ntohl(sp.ts);
  sp.ssrc = ntohl(sp.ssrc);
  codec = (sp.word&0x007F);
  mark = (sp.word&0x0080);

  data = buffer + sizeof(SoundPacket);
  // CRC List
  if (sp.word & 0x0F00) {
    crcs = data;
    data += 4*((sp.word&0x0F00)>>8);
    crcslen = 4*((sp.word&0x0F00)>>8);
    printf("Hay %dcssrc's\n", ((sp.word&0x0F00)>>8));
  }
  // EH Header
  if (sp.word&0x1000) {
    int pint;
    
    eh = data;
    pint = *(int *)data;
    pint = ntohl(pint);
    pint = pint & 0xffff;
    pint *= 4; // La longitud está expresada en palabras de 32bits
    pint += 4; // La propia cabecera inicial.
    data += pint;  // Sumamos la longitud de la cabecera de extension
    ehlen = pint;
    //		printf("añadidos %d\n", pint &0xffff);
  }
  datalen = bufferSize-sizeof(SoundPacket)-ehlen-crcslen;

  if ( !(rtp = (RTPPacket *)malloc(sizeof(RTPPacket))) ) {
    perror("RTPPacketNew::malloc");
    return NULL;
  }
  rtp -> data = NULL;
  if (datalen) {
    if ( !(rtp -> data  = (unsigned char *)malloc(datalen)) ) {
      perror("RTPPacketNew::malloc(2)");
      return NULL;
    }
    memcpy(rtp -> data, data, datalen);
  }
  rtp -> crcs = NULL;
  if (crcslen) {
    if (!(rtp -> crcs  = (unsigned char *)malloc(crcslen)) ) {
      perror("RTPPacketNew::malloc(3)");
      return NULL;
    }
    memcpy(rtp -> crcs, crcs, crcslen);
  }
  rtp -> eh = NULL;
  if (ehlen) {
    if (!(rtp -> eh  = (unsigned char *)malloc(ehlen)) ) {
      perror("RTPPacketNew::malloc(4)");
      return NULL;
    }
    memcpy(rtp -> eh, eh, ehlen);
  }
  rtp -> len = datalen;
  rtp -> ehSize = ehlen;
  rtp -> crcsSize = crcslen;
  rtp -> pt = codec;
  memcpy(&(rtp -> tv), &tv, sizeof(rtp -> tv));
  rtp -> mark = mark;
  rtp -> ts = sp.ts;
  rtp -> seq = sp.seq;
  rtp -> ssrc = sp.ssrc;
  rtp -> usable = 1;
  return(rtp);
}

RTPPacket *RTPPacketCopy(RTPPacket *rtpo) {
  RTPPacket *rtp = NULL;

  if (!rtpo) {
    printf("RTPPacketCopy = NULL\n");
    return NULL;
  }
  if ( !(rtp = (RTPPacket *)malloc(sizeof(RTPPacket))) ) {
    perror("RTPPacketNew::malloc");
    return NULL;
  }
  /*
  if ( !(rtp -> data  = (unsigned char *)malloc(rtpo -> len)) ) {
    perror("RTPPacketNew::malloc(2)");
    return NULL;
  }
  */
  rtp -> data = NULL;
  if (!(rtp -> crcs  = (unsigned char *)malloc(rtpo -> crcsSize)) ) {
    perror("RTPPacketNew::malloc(3)");
    return NULL;
  }
  if (!(rtp -> eh  = (unsigned char *)malloc(rtpo -> ehSize)) ) {
    perror("RTPPacketNew::malloc(4)");
    return NULL;
  }
  rtp -> len = rtpo -> len;
  rtp -> crcsSize = rtpo -> crcsSize;
  rtp -> ehSize = rtpo -> ehSize;
  //memcpy(rtp -> data, rtpo -> data, rtp -> len);
  memcpy(&(rtp -> tv), &rtpo -> tv, sizeof(rtp -> tv));
  memcpy(rtp -> crcs, rtpo -> crcs, rtp -> ehSize);
  rtp -> pt = rtpo -> pt;
  rtp -> mark = rtpo -> mark;
  rtp -> ts = rtpo -> ts;
  rtp -> seq = rtpo -> seq;
  rtp -> ssrc = rtpo -> ssrc;
  rtp -> usable = rtpo -> usable;
  return rtp;
}

int RTPPacketDelete(RTPPacket *rtp) {
  free(rtp -> data);
  free(rtp -> crcs);
  free(rtp -> eh);
  free(rtp);
  return 1;
}


