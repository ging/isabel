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

#include <icf2/general.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#elif defined(__BUILD_FOR_WINXP)
#include <stdio.h>
#include <windows.h>
#endif

#include "avm_locker.h"
#include "avm_output.h"

#define __MODULE__ "PThreadCond"

AVM_BEGIN_NAMESPACE;

PthreadMutex::PthreadMutex(const PthreadMutex& other)
{  
	m_pMutex = other.m_pMutex;  
	mutex_created = false;
}

PthreadMutex& 
PthreadMutex::operator=(const PthreadMutex& other) 
{ 
	if (mutex_created)
	{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
		pthread_mutex_destroy((pthread_mutex_t*) m_pMutex);
		delete (pthread_mutex_t*)m_pMutex;
#elif defined(__BUILD_FOR_WINXP)
		CloseHandle(m_pMutex);		
#endif
	}
	m_pMutex = other.m_pMutex; 
	mutex_created = false;
	return *this; 
}

PthreadCond::PthreadCond(const PthreadCond& other)
{ 
	m_pCond = other.m_pCond; cond_created = false;
}

PthreadCond& 
PthreadCond::operator=(const PthreadCond& other) 
{ 
	if (cond_created)
	{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
		pthread_cond_destroy((pthread_cond_t*) m_pCond);
		delete (pthread_cond_t*) m_pCond;
#elif defined(__BUILD_FOR_WINXP)
		CloseHandle(m_pCond);		
#endif
	}
	m_pCond = other.m_pCond;
	cond_created = false;  
	return *this; 
}

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)

PthreadMutex::PthreadMutex( /*Attr mattr */ )
{
	m_pMutex = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*) m_pMutex, NULL);
	mutex_created = true;
}

PthreadMutex::~PthreadMutex()
{
	if (mutex_created)
	{
		pthread_mutex_destroy((pthread_mutex_t*) m_pMutex);
		delete (pthread_mutex_t*)m_pMutex;
		mutex_created = false;
	}
}

int PthreadMutex::Lock()
{
	return pthread_mutex_lock((pthread_mutex_t*) m_pMutex);
}

int PthreadMutex::TryLock()
{
	int r = pthread_mutex_trylock((pthread_mutex_t*) m_pMutex);
	return (r == EBUSY) ? -1 : 0;
}

int PthreadMutex::Unlock()
{
	return pthread_mutex_unlock((pthread_mutex_t*) m_pMutex);
}

PthreadCond::PthreadCond()
{
	m_pCond = new pthread_cond_t;
	pthread_cond_init((pthread_cond_t*) m_pCond, NULL);
	cond_created = true;
}

PthreadCond::~PthreadCond()
{
	if (cond_created)
	{
		pthread_cond_destroy((pthread_cond_t*) m_pCond);
		delete (pthread_cond_t*) m_pCond;
		cond_created = false;
	}
}

int PthreadCond::Wait(PthreadMutex& m, float waitTime)
{
	if (waitTime >= 0.0)
	{
		struct timespec timeout;
		struct timeval now;
		gettimeofday(&now, 0);


		timeout.tv_sec = now.tv_sec + (int)waitTime;
		waitTime -= (int)waitTime;
		timeout.tv_nsec = (now.tv_usec + (int)(waitTime * 1000000)) * 1000;

		if (timeout.tv_nsec >= 1000000000)
		{
			timeout.tv_nsec -= 1000000000;
			timeout.tv_sec++;
		}
		// time limited conditional waiting here!
		// sometimes decoder thread could be stoped
		// and we would have wait here forever
		int r = pthread_cond_timedwait((pthread_cond_t*) m_pCond,
			(pthread_mutex_t*) m.m_pMutex, &timeout);
		if (r < 0)
			perror("PthreadCond::Wait()");
		return r;
	}

	return pthread_cond_wait((pthread_cond_t*) m_pCond,
		(pthread_mutex_t*) m.m_pMutex);
}

int PthreadCond::Broadcast()
{
	return pthread_cond_broadcast((pthread_cond_t*) m_pCond);
}


PthreadTask::PthreadTask(void* attr, void* (*start_routine)(void *), void* arg)
{
	m_pTask = new pthread_t;
	if (pthread_create((pthread_t*) m_pTask,
		(pthread_attr_t*) attr, start_routine, arg))
		perror("PthreadTask()");
}

PthreadTask::~PthreadTask()
{
	int result = pthread_join(* ((pthread_t*)m_pTask), 0);
	delete (pthread_t*)m_pTask;
	if (result == EDEADLK)
	{
		perror("~PthreadTask()");
		pthread_exit(NULL);
	}
}


Locker::Locker(PthreadMutex& mutex) : m_pMutex(mutex)
{
	m_pMutex.Lock(); 
}

Locker::~Locker()
{
	m_pMutex.Unlock(); 
}

#elif defined(__BUILD_FOR_WINXP)

PthreadMutex::PthreadMutex()
{
	m_pMutex = CreateMutex(NULL,FALSE,NULL);
}


PthreadMutex::~PthreadMutex()
{
	CloseHandle(m_pMutex);
}

int PthreadMutex::Lock()
{	
	int ret = WaitForSingleObject(m_pMutex,INFINITE);
	return ret;
}

int PthreadMutex::TryLock()
{
	int r = WaitForSingleObject(m_pMutex,0);
	return (r == WAIT_TIMEOUT) ? -1 : 0;
}

int PthreadMutex::Unlock()
{
	int ret = ReleaseMutex(m_pMutex);
	return ret;
}

PthreadCond::PthreadCond()
{
	m_pCond = CreateEvent(NULL,TRUE,FALSE,NULL);
}

PthreadCond::~PthreadCond()
{
	CloseHandle(m_pCond);
}

int PthreadCond::Wait(PthreadMutex& m, float waitTime)
{
	int timeout;
	if (waitTime < 0) 
		timeout = INFINITE;
	else
		timeout = (int)waitTime*1000;
	m.Unlock();
	if (WaitForSingleObject(m_pCond,timeout)==WAIT_OBJECT_0)
	{
		ResetEvent(m_pCond);
		m.Lock();
		return 0;
	}	
	m.Lock();
	avm_printf(__MODULE__, "Wait timed out!\n");
	return -1;
}

int PthreadCond::Broadcast()
{
	SetEvent(m_pCond);
	return 0;
}


PthreadTask::PthreadTask(void* attr, LPTHREAD_START_ROUTINE start_routine, void* arg)
{    

	DWORD ID;
	m_pTask = CreateThread(NULL,NULL,start_routine,arg,0,&ID);    
}

PthreadTask::~PthreadTask()
{
	WaitForSingleObject(m_pTask,INFINITE);
	CloseHandle(m_pTask);
}

Locker::Locker(PthreadMutex& mutex) : m_pMutex(mutex.m_pMutex)
{
	WaitForSingleObject(m_pMutex,INFINITE);
}

Locker::~Locker()
{
	ReleaseMutex(m_pMutex);
}

#endif
AVM_END_NAMESPACE;
