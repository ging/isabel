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
#ifndef AVM_LOCKER_H
#define AVM_LOCKER_H

#include <icf2/general.h>
#ifdef __BUILD_FOR_WINXP
#include <windows.h>
#endif
#include "avm_default.h"


AVM_BEGIN_NAMESPACE;

// do not include pthread.h nor semaphore.h here
// hidden in the implementation

class PthreadCond;
class Locker;

/**
 * class used to hide usage of thread
 *
 * it might be implemented diferently for various platforms
 */
class PthreadMutex
{
    bool mutex_created;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
	void* m_pMutex;
#elif defined(__BUILD_FOR_WINXP)
    HANDLE m_pMutex;
#endif
friend class PthreadCond;
friend class Locker;
    
    PthreadMutex(const PthreadMutex& other);
    PthreadMutex& operator=(const PthreadMutex& other) ;

public:
    
	PthreadMutex( /* Attr = FAST */ );
    ~PthreadMutex();
    /// Lock mutex
    int Lock();
    /// TryLock mutex
    int TryLock();
    /// Unlock mutex
    int Unlock();
};

/**
 *
 */
class PthreadCond
{
    bool cond_created;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
	void* m_pCond;    
#elif defined(__BUILD_FOR_WINXP)
    HANDLE m_pCond;    
#endif
    PthreadCond(const PthreadCond& other);
    PthreadCond& operator=(const PthreadCond& other) ;

public:
    PthreadCond();
    ~PthreadCond();
    int Wait(PthreadMutex& m, float waitTime = -1.0);
    int Broadcast();
};

/**
 * Creates a new thread of control that executes concurrently with
 * the calling thread.
 */
class PthreadTask
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    void* m_pTask;    
#elif defined(__BUILD_FOR_WINXP)
    HANDLE m_pTask;
#endif
    /// \internal disabled
    PthreadTask(const PthreadTask&) :m_pTask(0) {}
    /// \internal disabled
    PthreadTask& operator=(const PthreadTask&) { return *this; }
public:
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    PthreadTask(void* attr, void* (*start_routine)(void *), void* arg);
#elif defined(__BUILD_FOR_WINXP)
    PthreadTask(void* attr, LPTHREAD_START_ROUTINE start_routine, void* arg);
#endif
    ~PthreadTask();
};

/**
 * Simple mutex locker which makes the mutex usage easier
 */
class Locker
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    PthreadMutex m_pMutex;    
    Locker(const Locker&) : m_pMutex(*(new PthreadMutex())) {}
    /// \internal disabled
    Locker& operator=(const Locker&) { return *this; }
#elif defined(__BUILD_FOR_WINXP)
    HANDLE m_pMutex;    
    /// \internal disabled
    //Locker(const Locker&) :m_pMutex(0) {}
    /// \internal disabled
    //Locker& operator=(const Locker&) { return *this; }
#endif
public:
    Locker(PthreadMutex& mutex);
    ~Locker();
};

AVM_END_NAMESPACE;

#ifdef AVM_COMPATIBLE
typedef avm::Locker Locker;
typedef avm::PthreadCond PthreadCond;
typedef avm::PthreadMutex PthreadMutex;
#endif

#endif // AVM_LOCKER_H
