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
#ifndef __Thread_H__
#define __Thread_H__

#ifdef WIN32

#include <windows.h>
#include <stdio.h>
#include <errno.h>

#include "myTime.h"

#else

#include <stdio.h>
#include <stdlib.h>
// Errores
#include <errno.h>
// memset
#include <string.h>
// Tiempo
#include <sys/time.h>
#include <sys/types.h>
// pthreads
#include <pthread.h>
#include <sched.h>
// Paginado y scheduling
#include <sys/mman.h>
#include <sched.h>
#include "myTime.h"

#endif

class Mutex {
#ifdef WIN32
  HANDLE lock;
#else
   pthread_mutex_t lock;   
#endif
 public:
  Mutex();
  virtual ~Mutex();
  void Wait();
  void Signal();
};

class Thread {
public:
  Thread();
  Thread(const char *name);
  virtual ~Thread();
  int Start();
  int Start(int priotiry);
  int Stop();
  // Este metodo hace el cancel por lo que no hay que usarla si dentro del metodo virtual Run se usan candados
  int Stop(int inmediatly);
  // Esta funcion se sobreescribe para poner el codigo que queremos que se ejecute en otro hilo dentro de un bucle indefinido.
  // Si solo se necesita una ejecucion separada y punto, al final de la funcion se debe llamar a terminate()
  inline virtual void Run(void) = 0;
  void TestCancel();
 private:
  char *name;
  int running;
  int doDelete;
#ifdef WIN32
  HANDLE handle;
  int stopped;
  friend  DWORD WINAPI _run(LPVOID args);
#else
  pthread_t thread;
  friend void *_run(void *args);
#endif
 protected:
   void Terminate(void);
};

#ifndef WIN32

void setscheduler(void);

#endif

#endif
















