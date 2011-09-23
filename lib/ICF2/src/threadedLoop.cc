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
// $Id: threadedLoop.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include <icf2/general.h>
#include <icf2/threadedLoop.hh>


#ifdef __THREADED_ICF


//
// tlmQueue_t -- message queue for threaded loops
//
int
tlmQueue_t::evalCondition(void)
{
    return (__head!= NULL);
}

void *
tlmQueue_t::whenTrueLocked(void)
{
    assert(__head);

    void *retVal= __head;
    
    __head= __head->next;
    if(!__head)
        __tail= NULL;

    tlMsg_t *m= reinterpret_cast<tlMsg_t*>(retVal);

    __thrLoop->__tlDequeueHook(m);

    return retVal;
}

void *
tlmQueue_t::whenTrueUnlocked(void *p)
{
    tlMsg_t *m= reinterpret_cast<tlMsg_t*>(p);

//NOTIFY("tlmQueue_t::whenTrueUnlocked: msgKind %04x\n", m->msgKind);
    __thrLoop->dispatchMsg(m);

    delete m;

    return NULL;
}

void
tlmQueue_t::doChange(void *p)
{
    tlMsg_t *q= reinterpret_cast<tlMsg_t*>(p);
    tlMsg_t *m= __thrLoop->__tlEnqueueHook(q);

    if(!m) return;

    if(__tail)
        __tail->next= m;

    __tail= m;

    if(!__head)
        __head= m;
}


//
// threadedLoop -- an event loop in a separate thread
//
tlMsg_t *
__threadedLoop_t::__tlEnqueueHook(tlMsg_t *m)
{
    return m;
}
void
__threadedLoop_t::__tlDequeueHook(tlMsg_t *)
{
}

#ifdef WIN32
DWORD WINAPI
#else
void *
#endif
__threadedLoop_t::__threadStartUp(void *__thr)
{
    __threadedLoop_t *thr= static_cast<__threadedLoop_t*>(__thr);

    thr->__runFlag= true;

    thr->debugMsg(dbg_K_Normal, "__threadedStartUp", "thread is now running\n");

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    return thr->__waitActivation();
#elif defined(WIN32)
    thr->__waitActivation();
    return 0;
#else
#error "Please, cast your return type for your O.S."
#endif
}

void *
__threadedLoop_t::__waitActivation(void)
{
    while(1)
        __sysAct.wait();

    return NULL;
}


bool
__threadedLoop_t::dispatchMsg(tlMsg_t *m)
{
    switch(m->msgKind) {
        case tlMsg_t::MSG_QUIT:
            debugMsg(
                dbg_K_Normal,
                "dispatchMsg",
                "got MSG_QUIT, bailing out"
            );
            __runFlag= false;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_exit(NULL);
#elif defined(WIN32)
            ExitThread(0); // Codigo de salida = 0
#else
#error "please, call thread exit in your OS"
#endif
            break;
        default:
            return false;
    }
    return true;
}

__threadedLoop_t::__threadedLoop_t(void)
: __sysAct(this)
{
    __runFlag= false;
}

__threadedLoop_t::~__threadedLoop_t(void)
{
    debugMsg(
        dbg_K_Normal,
        "~__threadedLoop_t",
        "finished destructor"
    );
}

void
__threadedLoop_t::tlRun(void)
{
    //
    // this must be fixed with some locks
    //
    if(!__runFlag) {
        debugMsg(dbg_K_Normal, "tlRun", "creating thread\n");
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
        pthread_create(&__thrID,
                       &attr,
                       threadedLoop_t::__threadStartUp,
                       (void *)this
                      );
#elif defined(WIN32)
        __threadID= CreateThread(NULL,
                                 0,
                                 __threadStartUp,
                                 (void *)this,
                                 0,
                                 &__thrID
                                );
#else
#error "Please, create thread in your OS"
#endif
    }

    debugMsg(dbg_K_Normal, "tlRun", "waiting activation\n");

    while(!(int)(__runFlag)) {
    }
}

void
__threadedLoop_t::tlPostMsg(tlMsg_t *m)
{
    debugMsg(dbg_K_Normal, "tlPostMsg", "posting message %08x\n", m->msgKind);
    __sysAct.signal(m);
}


void
threadedLoop_t::funeraryRites(void)
{
    if(__runFlag) {
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        if(pthread_self() != __thrID) {
#elif defined(WIN32)
        if((DWORD)GetCurrentThreadId() != __thrID) {
#else
#error "Please, ask if this is the same thread in your OS"
#endif
            debugMsg(
                dbg_K_Normal,
                "funeraryRites",
                "waiting in destructor for thread [%d] to end",
                __thrID
            );
            tlPostMsg(new tlMsg_t(tlMsg_t::MSG_QUIT));
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_join(__thrID, NULL);
#elif defined(WIN32)
            WaitForSingleObject(__threadID, INFINITE);
#else
#error "Please, join thread in your O.S."
#endif
        } else {
            debugMsg(
                dbg_K_Normal,
                "funeraryRites",
                "[%d] threaded loop is self destroying!!!",
                __thrID
            );
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_exit(NULL);
#elif defined(WIN32)
            ExitThread(0); // Exit code = 0;
#else
#error "Please, exit thread in your O.S."
#endif
        }
    } else {
        debugMsg(
            dbg_K_Normal,
            "funeraryRites",
            "thread [%d] not running, easy death!!!",
            __thrID
        );
    }
}


threadedLoop_t::~threadedLoop_t(void)
{
    debugMsg(
        dbg_K_Normal,
        "~threadedLoop_t",
        "finished destructor"
    );

    funeraryRites();
}


#endif

