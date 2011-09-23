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
#ifndef __myTime_H__
#define __myTime_H__

#ifdef WIN32

#include <stdio.h>
#include <windows.h>
#include <sys/timeb.h> // Para _ftime
#include <time.h> // Para _ftime

#else

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

void myWait(long long microSecs);
void myWait(long long microSecs, const char *who);
void printTime(char *message);

#endif

#ifdef WIN32

int gettimeofday(struct timeval *tp, void *tz);

#endif

namespace IMMF {

class myTime {

#ifdef WIN32

  static int firstTime;
  static LARGE_INTEGER ticsPerSec;

#endif

 public:
  static int Wait(unsigned long int ms);
  static void Print(char *cadena);
};



class HeartBeat {
  int firstBeat;

#ifdef WIN32

  static int firstTime;
  static LARGE_INTEGER ticsPerSec;
  LARGE_INTEGER tics0;
  LARGE_INTEGER heartBeat;

#else

  unsigned long int heartBeat;
  struct timeval tv0;

#endif


 public:
  HeartBeat(unsigned long int ms = 100000);
  ~HeartBeat();
  void Set(unsigned long int ms);
  int Beat();
};

}

#ifdef WIN32

#define u64 __int64

#else

#define u64 long long

#endif

#endif










