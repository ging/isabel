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
/////////////////////////////////////////////////////////////////////////
//
// $Id: lockSupport.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/lockSupport.hh>

lockedItem_t::lockedItem_t(void)
{
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_init(&mutex, NULL);
#elif defined(WIN32)
    mutex = CreateMutex(NULL, FALSE, NULL);
#else
#error "Please, call mutex init in your O.S."
#endif
#endif
}

lockedItem_t::~lockedItem_t(void)
{
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_destroy(&mutex);
#elif defined(WIN32)
    CloseHandle(mutex);
#else
#error "Please, dispose mutex in your O.S."
#endif
#endif
}

#ifdef __THREADED_ICF
void
lockedItem_t::__lock(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_lock(&mutex);
#elif defined(WIN32)
    WaitForSingleObject(mutex,INFINITE);
#else
#error "Please, lock mutex in your O.S."
#endif
}

void
lockedItem_t::__unlock(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_unlock(&mutex);
#elif defined(WIN32)
    ReleaseMutex(mutex);
#else
#error "Please, unlock mutex in your O.S."
#endif
}
#endif


lockedItem_t::locker_t
lockedItem_t::lock(void)
{
    return locker_t(new _locker_t(this));
}





rwLockedItem_t::rwLockedItem_t(void)
{
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_init(&__wrLock     , NULL);
    pthread_mutex_init(&__rdLock     , NULL);
    pthread_cond_init (&__rdCountCond, NULL);
#elif defined(WIN32)
    __wrLock = CreateMutex(NULL, FALSE, NULL);
    __rdLock = CreateMutex(NULL, FALSE, NULL);
#else
#error "Please, create mutex in your O.S."
#endif

    __rdCount= 0;
#endif
}

rwLockedItem_t::~rwLockedItem_t(void)
{
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_destroy(&__wrLock     );
    pthread_mutex_destroy(&__rdLock     );
    pthread_cond_destroy (&__rdCountCond);
#elif defined(WIN32)
    CloseHandle(__wrLock     );
    CloseHandle(__rdLock     );
#else
#error "Please, release mutex in your O.S."
#endif

    __rdCount= 0;
#endif
}

#ifdef __THREADED_ICF
void
rwLockedItem_t::__rd_lock(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_lock(&__wrLock); {

        pthread_mutex_lock(&__rdLock); {
            __rdCount++;
        } pthread_mutex_unlock(&__rdLock);

    } pthread_mutex_unlock(&__wrLock);
#elif defined(WIN32)
    WaitForSingleObject(__wrLock,INFINITE); {

        WaitForSingleObject(__rdLock,INFINITE); {
            __rdCount++;
        } ReleaseMutex(__rdLock);

    } ReleaseMutex(__wrLock);
#else
#error "Please, lock read mutex in your O.S."
#endif
}

void
rwLockedItem_t::__wr_lock(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_lock(&__wrLock);  // released at __wr_unlock

    pthread_mutex_lock(&__rdLock);
    {
        while(__rdCount> 0)
            pthread_cond_wait(&__rdCountCond, &__rdLock);
    }
    pthread_mutex_unlock(&__rdLock);
#elif defined(WIN32)
    WaitForSingleObject(__wrLock,INFINITE);  // released at __wr_unlock
    WaitForSingleObject(__rdLock,INFINITE);
    {
        while(__rdCount> 0)
        {
            __rdCountCond.Wait(__rdLock);
        }
    }
    ReleaseMutex(__rdLock);
#else
#error "Please, lock write mutex in your O.S."
#endif
}

void
rwLockedItem_t::__rd_unlock(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_lock(&__rdLock); {
        __rdCount--;
    } pthread_mutex_unlock(&__rdLock);

    pthread_cond_signal(&__rdCountCond);
#elif defined(WIN32)
    WaitForSingleObject(__rdLock,INFINITE); {
        __rdCount--;
    } ReleaseMutex(__rdLock);

    __rdCountCond.Post();
#else
#error "Please, unlock read mutex in your O.S."
#endif
}

void
rwLockedItem_t::__wr_unlock(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_unlock(&__wrLock);
#elif defined(WIN32)
    ReleaseMutex(__wrLock);
#else
#error "Please, unlock write mutex in your O.S."
#endif
}
#endif

rwLockedItem_t::locker_t
rwLockedItem_t::rdLock(void)
{
    return locker_t(new _locker_t(this, _locker_t::RD));
}

rwLockedItem_t::locker_t
rwLockedItem_t::wrLock(void)
{
    return locker_t(new _locker_t(this, _locker_t::WR));
}





//
// force template expansion
//
//#pragma instantiate smartReference_t<lockedItem_t::_locker_t>
//#pragma instantiate smartReference_t<rwLockedItem_t::_locker_t>


