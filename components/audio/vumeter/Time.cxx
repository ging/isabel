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

#include "myTime.h"

#ifndef WIN32
// microSecs son milisegundos
void myWait(long long microSecs, const char *who) {
  struct timespec req, rem;
  if (microSecs < 10000) return;
  microSecs -= 10000;
  req.tv_sec=microSecs / 1000000;
  if (microSecs < 1000000) {
    req.tv_nsec = microSecs*1000;
  } else {
    req.tv_nsec = (microSecs%1000)*1000;
  }
  nanosleep(&req, &rem);
  return ;
}

void myWait(long long microSecs) {
  myWait(microSecs, "who knows who is using this wait?");
}

void printTime(char *message) {
  struct timeval tv0;
  struct timezone tz;
  gettimeofday(&tv0, &tz);
  printf("%s at %010ld:%06ld\n",message, tv0.tv_sec, tv0.tv_usec);
}

#endif

namespace IMMF {

#ifdef WIN32

int myTime::firstTime = 1;
LARGE_INTEGER myTime::ticsPerSec;

#endif

int myTime::Wait(unsigned long int ms) {
#ifdef WIN32

  LARGE_INTEGER tics0, tics1;

  if (firstTime) {
    LARGE_INTEGER freq;

    firstTime = 0;
    QueryPerformanceFrequency(&freq);
    ticsPerSec.QuadPart = freq.QuadPart/1000;
  }
  QueryPerformanceCounter(&tics0);
  Sleep(ms/1000);
  QueryPerformanceCounter(&tics1);
  return (tics1.QuadPart-tics0.QuadPart)/ticsPerSec.QuadPart;


#else

  struct timespec req, rem;
  if (ms < 10000) return 0;
  ms -= 10000;
  req.tv_sec=ms / 1000000;
  if (ms < 1000000) {
    req.tv_nsec = ms*1000;
  } else {
    req.tv_nsec = (ms % 1000)*1000;
  }
  int res = nanosleep(&req, &rem);
  return res;

#endif
}

void myTime::Print(char *cadena) {
#ifdef WIN32

  LARGE_INTEGER tics0;

  if (firstTime) {
    LARGE_INTEGER freq;

    firstTime = 0;
    QueryPerformanceFrequency(&freq);
    ticsPerSec.QuadPart = freq.QuadPart/1000;
  }
  QueryPerformanceCounter(&tics0);
  sprintf(cadena, "%f", (float)tics0.QuadPart/ticsPerSec.QuadPart); 

#else

  struct timeval tv0;
  struct timezone tz;

  gettimeofday(&tv0, &tz);
  sprintf(cadena, "%08ld:%06ld", tv0.tv_sec, tv0.tv_usec);

#endif
}

////////////////////////////////////////////////

#ifdef WIN32

int HeartBeat::firstTime = 1;
LARGE_INTEGER HeartBeat::ticsPerSec;

#endif

HeartBeat::HeartBeat(unsigned long int ms) {
  firstBeat = 0;
#ifdef WIN32
  
  heartBeat.QuadPart = ms;

#else

  heartBeat = ms;

#endif
}

HeartBeat::~HeartBeat() {
}

int HeartBeat::Beat() {
#ifdef WIN32

  LARGE_INTEGER tics1, diff;

  if (firstTime) {
    LARGE_INTEGER freq;

    firstBeat = 0;
    QueryPerformanceFrequency(&freq);
    ticsPerSec.QuadPart = freq.QuadPart/1000;
  }
  QueryPerformanceCounter(&tics1);
  diff.QuadPart = ((tics1.QuadPart-tics0.QuadPart)/ticsPerSec.QuadPart);
  if (!firstBeat) {
    if (diff.QuadPart < heartBeat.QuadPart) {
      myTime::Wait(heartBeat.QuadPart-diff.QuadPart);
    }
  } else {
    firstTime = 0;
  }
  QueryPerformanceCounter(&tics0);
  return diff.QuadPart > heartBeat.QuadPart;

#else

  struct timeval tv1;
  unsigned long long int diff;

  gettimeofday(&tv1, NULL);

  diff = (tv1.tv_sec*1000000 + tv1.tv_usec) - (tv0.tv_sec*1000000 + tv0.tv_usec);
  if (diff < 0) diff = 0;
  //  diff /= 1000;
  if (!firstBeat) {
    if (diff < heartBeat) {
      myTime::Wait(heartBeat-diff);
    }
  } else {
    firstBeat = 0;
  }
  gettimeofday(&tv0, NULL);
  return (diff > heartBeat);
#endif
}

void HeartBeat::Set(unsigned long int ms) {
#ifdef WIN32

  heartBeat.QuadPart = ms;

#else

  heartBeat = ms;

#endif
}

#ifdef WIN32

int gettimeofday(struct timeval *tp, void *tz)
{
	struct _timeb timebuffer;   

	_ftime( &timebuffer );
	tp->tv_sec  = timebuffer.time;
	tp->tv_usec = timebuffer.millitm * 1000;
	return 0;
}

#endif

}

