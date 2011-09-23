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
// $Id: conditionalVar.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/conditionalVar.hh>

#ifdef __THREADED_ICF

condVar_t::condVar_t(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond , NULL);
#elif defined(WIN32)
    mutex = CreateMutex(NULL, FALSE, NULL);
#else
#error "Please, define the mutex creation in your O.S."
#endif
}

condVar_t::~condVar_t(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy (&cond );
#elif defined(WIN32)
    CloseHandle(mutex);
#else
#error "Please, define the mutex destruction/closing/freeing in your O.S."
#endif
}

void
condVar_t::signal(void *arg)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_lock(&mutex);
    {
        doChange(arg);
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
#elif defined(WIN32)
    WaitForSingleObject(mutex,INFINITE);
    {
        doChange(arg);
    }
    ReleaseMutex(mutex);
    cond.Post();
#else
#error "Please, define the mutex signaling in your O.S."
#endif
}

void *
condVar_t::wait(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_lock(&mutex);

    while ( ! evalCondition())
    {
        pthread_cond_wait(&cond, &mutex);
    }

    void *arg= whenTrueLocked();

    pthread_mutex_unlock(&mutex);

    return whenTrueUnlocked(arg);
#elif defined(WIN32)
    WaitForSingleObject(mutex,INFINITE);
    while ( ! evalCondition())
    {
        cond.Wait(mutex);
    }
    void *arg= whenTrueLocked();

    ReleaseMutex(mutex);
    return whenTrueUnlocked(arg);
#else
#error "Please, define the mutex wait in your O.S."
#endif
}

#endif

