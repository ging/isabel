#ifndef __LOCKER_H
#define __LOCKER_H

#include "defines.h"

#ifdef __BUILD_FOR_LINUX
    #include <stdint.h>
    #include <inttypes.h>
#endif
#ifdef __BUILD_FOR_WINXP
#include <winsock2.h>
#include <windows.h>
#endif

namespace libvnc
{

class CPthreadCond;
class CLocker;

class CPthreadMutex
{
    bool mutex_created;
#ifdef __BUILD_FOR_LINUX
	void* m_pMutex;
#endif
#ifdef __BUILD_FOR_WINXP
    HANDLE m_pMutex;
#endif
friend class CPthreadCond;
friend class CLocker;
    
    CPthreadMutex(const CPthreadMutex& other);
    CPthreadMutex& operator=(const CPthreadMutex& other) ;

public:
    
	CPthreadMutex();
    ~CPthreadMutex();
    int Lock();
    int TryLock();
    int Unlock();
};

class CPthreadCond
{
    bool cond_created;
#ifdef __BUILD_FOR_LINUX
	void* m_pCond;    
#endif
#ifdef __BUILD_FOR_WINXP
    HANDLE m_pCond;    
#endif
    CPthreadCond(const CPthreadCond& other);
    CPthreadCond& operator=(const CPthreadCond& other) ;

public:
    CPthreadCond();
    ~CPthreadCond();
    int Wait(CPthreadMutex& m, float waitTime = -1.0);
    int Broadcast();
};

class CLocker
{
#ifdef __BUILD_FOR_LINUX
    CPthreadMutex m_pMutex;    
    CLocker(const CLocker&) : m_pMutex(*(new CPthreadMutex())) {}
    CLocker& operator=(const CLocker&) { return *this; }
#endif
#ifdef __BUILD_FOR_WINXP
    HANDLE m_pMutex;        
#endif
public:
    CLocker(CPthreadMutex& mutex);
    ~CLocker();
};

}

#endif 
