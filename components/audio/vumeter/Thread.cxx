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

#include "Thread.h"

static int THR_DEBUG = 0;

Mutex::Mutex() {
#ifdef WIN32
  lock = CreateMutex(NULL, FALSE, NULL);
#else
  pthread_mutex_init(&lock, NULL);
#endif
}

Mutex::~Mutex() {
}

void Mutex::Wait() {
#ifdef WIN32
  WaitForSingleObject(lock, INFINITE);
#else
  pthread_mutex_lock(&lock);
#endif
}

void Mutex::Signal() {
#ifdef WIN32
  ReleaseMutex(lock);
#else
  pthread_mutex_unlock(&lock);
#endif
}

///////////////////////////////////////////////////////

#ifdef WIN32

DWORD WINAPI _run(LPVOID args) {
  Thread *t;
  t = (Thread *)args;
  while (t -> running) {
    t -> Run();
    t -> TestCancel();
  }
  t -> stopped = 1;
  if (t -> doDelete) {
    delete t;
  }
  return 0;
}

#else

void *_run(void *args) {
  Thread *t;

  t = (Thread *)args;
  while (t -> running) {
    t -> Run();
    t -> TestCancel();
  }
  if (t -> doDelete) {
    delete t;
  }
  return NULL;
}
#endif


// Falta añadirle una getion de candados para que al hacer el stop no se produzca un bloqueo en un candado no liberado por el bucle de run. O bien hacer de otra forma el stop, usando join en lugar de cancel
Thread::Thread() {
  doDelete = 0;
  running = 0;
#ifdef WIN32
  handle = NULL;
  stopped = 0;
#endif
  this -> name = strdup("name not assigned");
}

Thread::Thread(const char *name) {
  doDelete = 0;
  running = 0;
#ifdef WIN32
  handle = NULL;
  stopped = 0;
#endif
  this -> name = strdup(name);
}

Thread::~Thread() {
  if (THR_DEBUG) printf("Thread::~Thread:<%s> to stop\n", name);
  Stop();
  if (THR_DEBUG) printf("Thread::~Thread:<%s> stopped\n", name);
  free(name);
}

int Thread::Start() {
  return Start(0);
}

int Thread::Start(int priority) {

  if (running) return 1;
  running = 1;

#ifdef WIN32

  DWORD threadId;
  handle = CreateThread(NULL, 5000000, _run, this, 0, &threadId);
  if (priority) {
    BOOL res;
    res = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
    if (res != TRUE) {
      perror("Thread::Start : Failed to SetThreadPriority");
    }
  }
  return handle!=NULL;

#else

  pthread_attr_t attr, *theAttr;
  struct sched_param sched_param;
  
  theAttr = NULL;
  if (priority == 1) {
    theAttr = &attr;
    if (pthread_attr_init(&attr) != 0) {
      perror("Thread::pthread_attr_init");
      theAttr = NULL;
    } else {
      if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0) {
	perror("Thread::pthread_attr_setschedpolicy");
	theAttr = NULL;
      } else {
	if (sched_getparam(0, &sched_param) < 0) {
	  perror("Thread::sched_getparam");
	  theAttr = NULL;
	} else {
	  sched_param.sched_priority = sched_get_priority_max(SCHED_RR);  
	  if (pthread_attr_setschedparam(&attr, &sched_param) != 0) {
	    perror("Thread::pthread_attr_setschedparam");
	    theAttr = NULL;
	  }
	}
      }
    }
  }
  if (THR_DEBUG) {
    printf("Thread::Starting <%s>\n", name); 
    printTime("Thread starting");
    fflush(stdout);
  }
  if ( pthread_create(&(thread), theAttr, _run, this) != 0 ) {
    perror("Thread::pthread_create ( Posiblemente no se está ejecutando como root y se ha solicitado maxima prioridad");
    if ( pthread_create(&(thread), NULL, _run, this) != 0 ) {
      perror("Thread::pthread_create");
      return -1;
    }
  }
  if (THR_DEBUG) {
    printf("Thread::Started <%s>\n", name); 
    printTime("Thread Started");
    fflush(stdout);
  }
  return 1;

#endif

}

int Thread::Stop() {
  return Stop(0);
}

int Thread::Stop(int inmediatly) {
  if (!running) return 1;
  
  running = 0;
  if (THR_DEBUG) printf("Thread::Voy a Stop <%s>\n", name);

#ifdef WIN32
  int n = 0;
  while (!stopped && (n < 300)) {
    IMMF::Time::Wait(10000);
    n++;
  }
  if ( !stopped && (n == 300) ) inmediatly = 1;
  if (inmediatly) {
    CloseHandle(handle);
  }	
  return 1;

#else

  int val[1];
  
  if (inmediatly) {   
    pthread_cancel(thread);
  } else {
    if (pthread_join(thread, (void **)&val)!=0) {
      perror("Thread::Stop: pthread join");
    }
  }
  if (THR_DEBUG) printf("Thread::Stop <%s>\n", name); fflush(stdout);
  myWait(100000);
  
  return 1;

#endif
}

void Thread::TestCancel() {

#ifdef WIN32

#else

  pthread_testcancel();

#endif
}

void Thread::Terminate(void) {
  running = 0;
  doDelete = 1;
}

#ifndef WIN32

void setscheduler(void) {
  struct sched_param sched_param;
  
  //  printf("\nCambiando la política del scheduler a Round Robin\n");
  if (sched_getparam(0, &sched_param) < 0) {
    printf("Política no cambiada\n");
    
  }
  sched_param.sched_priority = sched_get_priority_max(SCHED_RR);
  if (!sched_setscheduler(0, SCHED_RR, &sched_param)) {
    fflush(stdout);
    return;
  }    
  //  printf("Política no cambiada\n");
}

#endif









