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
// $Id: conditionalVar.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__conditional_var__hh__
#define __icf2__conditional_var__hh__

#include <icf2/general.h>

#ifdef WIN32
#include <icf2/item.hh>
#include <windows.h>

class _Dll_ Event_t: public item_t {
private:
  HANDLE event;
 
public:
  Event_t(void) {    // Inicializador a 0
     event = CreateEvent(NULL,FALSE,FALSE,NULL);
  }

  virtual ~Event_t(void) { //Destruimos evento
      CloseHandle(event);
  }

  long Wait(long timeOut = INFINITE) { // Esperamos una señal
    ResetEvent(event);
    debugMsg(dbg_K_Normal,"Wait Event","waiting signal...");
    long hr = WaitForSingleObject(event, timeOut);
    debugMsg(dbg_K_Normal,"Wait Event","Signal received");
    return hr;
  }

  long Wait(HANDLE lock,long timeOut = INFINITE){ 
    ResetEvent(event);
    debugMsg(dbg_K_Normal,"Wait Event","waiting signal...");
    long hr = SignalObjectAndWait(lock,event,timeOut,FALSE);
    debugMsg(dbg_K_Normal,"Wait Event","Signal received");
    return hr;
  }

  void Post(void) {
    SetEvent(event);
  }  
};

#endif
#ifdef __THREADED_ICF
//
// condVar_t -- C++ wrapper for pthread_cond_t.
//
//
// This is an abstract class which must be redefined from user code.
// It's designed as a wrapper for data items which must pe protected
// with a conditional var (such as a message queue).
//
// Two public methods are supplied:
//
//    + void  signal(void *value);
//    + void *wait(void);
//
// The first is used to signal a change to the data monitored by
// the condition variable, usual locking is a performed automatically
// to ensure safety.  The second is used to wait for a change on the
// condition variable. These methods must NOT be redefined.
// The interpretation of the (void *) pointer is up to the class
// implementor (see below).
//
// Four protected virtual methods are left abstract and must be defined
// by the user:
//
//    + int   evalCondition    (void  )
//    + void *whenTrueLocked   (void  )
//    + void *whenTrueUnlocked (void *)
//    + void  doChange         (void *)
//
// The ´doChange()´ method is called by ´signal()´, after getting hold of
// the required locks, to accomplish the modification of data associated
// with the condition variable. The parameter to ´doChange()´ is the
// same data passed to ´signal()´
//
// ´evalCondition()´ is used by ´wait()´ to check wether a suitable
// modification has been done. It's called with locks held and
// must return either true (1==1) or false (1==0)
//
// Once ´evalCondition()´ returns true, changes to the data are asserted
// in two stages. First ´whenTrueLocked()´ is called with locks held.
// The return value of this method is given to ´whenTrueUnlocked()´
// with locks released. The return value of ´whenTrueUnlocked()´ is passed
// back to the caller of ´wait()´ (with locks also released).
//
// The rationale behind this two-stage assertion of changes is to
// hold locks only the strictly required time. For instance: the
// condition variable could protect a message queue, ´whenTrueLocked()´
// simply retrieves a message from the queue and ´whenTrueUnlocked()´
// can eat as many CPU cycles as required processing the message without
// blocking other threads trying to put messages in the message queue,
// and the results of this computations can be passed back to the
// caller of ´signal()´ to, again, eat as many cycles as required.
//
//
// A sample usage class could be:
//
//    class message_t {...};
//    typedef smartReference_t<message_t> message_ref;
//    class msgQueue_t: public virtual condVar_t
//    {
//    private:
//        ql_t<message_ref> __queue;
//
//        virtual void doChange(void *m) {
//            message_ref mr= reinterpret_cast<message_t*>(m);
//            __queue.insert(mr);
//        }
//        virtual evalCondition(void) {
//             return __queue.len()>0;
//        }
//        virtual void *whenTrueLocked(void) {
//            message_t *m= __queue.head();
//            __queue.behead();
//            return reinterpret_cast<void*>(m);
//        }
//        virtual void *whenTrueUnlocked(void *m) {
//    #ifdef IMPLEMENTATION_A
//            return m; // the message will be handled
//                      // by the caller to wait();
//    #else
//            mesage_ref mr= reinterpret_cast<message_t*>(m);
//
//            void *retVal;
//            // do something with mr, dispatch it
//
//            return retVal;
//    #endif
//        }
//    }
//
// A more elaborate example can be checked at <icf2/threadedLoop.h>
// Details on smartReference_t<> can be checked at <icf2/smartReference.h>
// Details on ql_t<> can be checked at <icf2/ql.h>
//
class _Dll_ condVar_t
{
private:
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_t  mutex;
    pthread_cond_t   cond;
#elif defined(WIN32)
    HANDLE  mutex;
    Event_t cond;
#else
#error "Please, provide implementation for mutex in your O.S."
#endif

public:
    condVar_t(void);
    virtual ~condVar_t(void);

protected:
    virtual int   evalCondition(void)= 0;
    virtual void *whenTrueLocked(void) = 0;
    virtual void *whenTrueUnlocked(void *) = 0;
    virtual void  doChange(void *)= 0;

public:
    void  signal(void *arg);
    void *wait  (void);
};
#endif


#endif
