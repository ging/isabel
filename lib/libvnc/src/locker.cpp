#include "defines.h"

#ifdef __BUILD_FOR_LINUX
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#endif

#include "locker.h"

using namespace libvnc;

CPthreadMutex::CPthreadMutex(const CPthreadMutex& other)
{  
	m_pMutex = other.m_pMutex;  
	mutex_created = false;
}

CPthreadMutex& 
CPthreadMutex::operator=(const CPthreadMutex& other) 
{ 
	if (mutex_created)
	{
#ifdef __BUILD_FOR_LINUX
		pthread_mutex_destroy((pthread_mutex_t*) m_pMutex);
		delete (pthread_mutex_t*)m_pMutex;
#endif
#ifdef __BUILD_FOR_WINXP
		CloseHandle(m_pMutex);		
#endif
	}
	m_pMutex = other.m_pMutex; 
	mutex_created = false;
	return *this; 
}

CPthreadCond::CPthreadCond(const CPthreadCond& other)
{ 
	m_pCond = other.m_pCond; cond_created = false;
}

CPthreadCond& 
CPthreadCond::operator=(const CPthreadCond& other) 
{ 
	if (cond_created)
	{
#ifdef __BUILD_FOR_LINUX
		pthread_cond_destroy((pthread_cond_t*) m_pCond);
		delete (pthread_cond_t*) m_pCond;
#endif
#ifdef __BUILD_FOR_WINXP
		CloseHandle(m_pCond);		
#endif
	}
	m_pCond = other.m_pCond;
	cond_created = false;  
	return *this; 
}

#ifdef __BUILD_FOR_LINUX
CPthreadMutex::CPthreadMutex()
{
	m_pMutex = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*) m_pMutex, NULL);
	mutex_created = true;
}

CPthreadMutex::~CPthreadMutex()
{
	if (mutex_created)
	{
		pthread_mutex_destroy((pthread_mutex_t*) m_pMutex);
		delete (pthread_mutex_t*)m_pMutex;
		mutex_created = false;
	}
}

int CPthreadMutex::Lock()
{
	return pthread_mutex_lock((pthread_mutex_t*) m_pMutex);
}

int CPthreadMutex::TryLock()
{
	int r = pthread_mutex_trylock((pthread_mutex_t*) m_pMutex);
	return (r == EBUSY) ? -1 : 0;
}

int CPthreadMutex::Unlock()
{
	return pthread_mutex_unlock((pthread_mutex_t*) m_pMutex);
}

CPthreadCond::CPthreadCond()
{
	m_pCond = new pthread_cond_t;
	pthread_cond_init((pthread_cond_t*) m_pCond, NULL);
	cond_created = true;
}

CPthreadCond::~CPthreadCond()
{
	if (cond_created)
	{
		pthread_cond_destroy((pthread_cond_t*) m_pCond);
		delete (pthread_cond_t*) m_pCond;
		cond_created = false;
	}
}

int CPthreadCond::Wait(CPthreadMutex& m, float waitTime)
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
			perror("CPthreadCond::Wait()");
		return r;
	}

	return pthread_cond_wait((pthread_cond_t*) m_pCond,
		(pthread_mutex_t*) m.m_pMutex);
}

int CPthreadCond::Broadcast()
{
	return pthread_cond_broadcast((pthread_cond_t*) m_pCond);
}

CLocker::CLocker(CPthreadMutex& mutex) : m_pMutex(mutex)
{
	m_pMutex.Lock(); 
}

CLocker::~CLocker()
{
	m_pMutex.Unlock(); 
}
#endif

#ifdef __BUILD_FOR_WINXP
CPthreadMutex::CPthreadMutex()
{
	m_pMutex = CreateMutex(NULL,FALSE,NULL);
}


CPthreadMutex::~CPthreadMutex()
{
	CloseHandle(m_pMutex);
}

int CPthreadMutex::Lock()
{	
	int ret = WaitForSingleObject(m_pMutex,INFINITE);
	return ret;
}

int CPthreadMutex::TryLock()
{
	int r = WaitForSingleObject(m_pMutex,0);
	return (r == WAIT_TIMEOUT) ? -1 : 0;
}

int CPthreadMutex::Unlock()
{
	int ret = ReleaseMutex(m_pMutex);
	return ret;
}

CPthreadCond::CPthreadCond()
{
	m_pCond = CreateEvent(NULL,TRUE,FALSE,NULL);
}

CPthreadCond::~CPthreadCond()
{
	CloseHandle(m_pCond);
}

int CPthreadCond::Wait(CPthreadMutex& m, float waitTime)
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
	return -1;
}

int CPthreadCond::Broadcast()
{
	SetEvent(m_pCond);
	return 0;
}

CLocker::CLocker(CPthreadMutex& mutex) : m_pMutex(mutex.m_pMutex)
{
	WaitForSingleObject(m_pMutex,INFINITE);
}

CLocker::~CLocker()
{
	ReleaseMutex(m_pMutex);
}

#endif
