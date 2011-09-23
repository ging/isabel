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
// $Id: threadedLoop.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__threaded_loop_hh__
#define __icf2__threaded_loop_hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/smartReference.hh>
#include <icf2/conditionalVar.hh>


#ifdef __THREADED_ICF

//
// see below
//

class _Dll_ __threadedLoop_t;

//
// tlMsg_t -- threadedLoop message
//
//    Messages sent to threadedLoops (see below). Characterized by
// a ´msgKind´ and optional ´msgData´.
//
// Message kind identifiers are reseserved as follows:
//
//    + 0x0000 -- 0x0fff   ; base system  (baseMsg_e below)
//    + 0x1000 -- 0x1fff   ; icf messages (icfMessages_e at <icf2/icfMessages.hh>)
//    + 0x2000 -- 0x2fff   ; Isabel SDK messages
//    + 0x3000 -- 0x7fff   ; reserved
//    + 0x8000 -- MAX_INT  ; avail for user classes
//
// The message data is an opaque pointer. No attempt is made by
// the system to clean up the data pointed by ´msgData´ in case
// any resource referred should be collected, it must be done
// at ´threadedLoop_t.dispatchMsg(tlMsg_t*)´  see below.
//
class _Dll_ tlMsg_t
{
public:
    enum {MSG_QUIT= 0} baseMsg_e;

    int      msgKind;
    tlMsg_t  *next;

    tlMsg_t(int k): msgKind(k) { next= NULL; };
    virtual ~tlMsg_t(void)     { next= NULL; };
};

//
// tlmQueue_t -- threadedLoop message queue
//
//    ´tlmQueue_t´ is used by threadedLoops to hold all
// the incoming messages. It´s derived from ´condVar_t´  so the
// threadedLoop will loop forever in ´tlmQueue_t.wait()´ and other
// threads will post messages thru ´tlmQueue_t.signal()´
// (see <icf2/conditionalVar.h>)
//
// The message queue is held as a linked list of messages,
// and messages are delivered in a FIFO fashion.
//
//    + ´evalCondition()´     -- return true upon queue not empty.
//    + ´doChange()´          -- appends a message
//    + ´whenTrueLocked()´    -- retrieves a message from the queue
//    + ´whenTrueUnlocked()´  -- dispatchs the message thru
//                              ´threadedLoop_t::dispatchMsg()´
//
class _Dll_ tlmQueue_t: public virtual condVar_t
{
private:
    tlMsg_t   *__head;
    tlMsg_t   *__tail;

    __threadedLoop_t *__thrLoop;


public:
    tlmQueue_t(__threadedLoop_t *t) {
       __head= __tail= NULL;
       __thrLoop= t;
    };

private:
    virtual int evalCondition(void);

    virtual void *whenTrueLocked(void);
    virtual void *whenTrueUnlocked(void *p);

    virtual void  doChange(void *p);
};




//
// threadedLoop_t -- an event loop running in its own thread
//
//
//    The ´threadedLoop_t´ is used within the ICF library to
// generate a thread running some kind of work. Communications
// between this thread/class and other threads is done via messages.
//
// The class ´tlmQueue_t´ implements the message queue and all
// necessary locking for it to run safely. Each threadedLoop
// has its own tlmQueue_t, which is marked private for safety
// reasons.
//
// The idea is to have a thread looping around ´tlmQueue.wait()´
// and acting on the messages retrieved. Messages are signalled from
// the message queue thru the protected virtual function ´dispatchMsg()´
// The user must define his own message types for his threadedLoop (see
// ´tlMsg_t´) and must redefine ´dispatchMsg()´. User´s ´dispatchMsg()´
// must either perform the requested action if the message is of known
// class, or call the inherited ´dispatchMsg()´ if not. Typically
// this method will consistt of a ´switch´ construction with a call
// to some virtual method per branch and a call to the inherited
// dispatch method in the ´default´ branch.
//
// [NOTE: to ensure safety of the system, ´dispatchMsg_t()´ must be always
//        a **protected** member function.
// ]
//
//
// The only public access method to interact with a ´threadedLoop_t´
// is the member function ´postMsg(tlMsg_t*)´ which calls the
// ´signal´ method of message queue.
//
class _Dll_ __threadedLoop_t: public virtual item_t, public virtual collectible_t
{
private:
    volatile bool __runFlag;
    void *__waitActivation(void);
    tlmQueue_t   __sysAct;

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_t  __thrID;
    static void *__threadStartUp(void *);
#elif defined(WIN32)
    HANDLE        __threadID;
    DWORD         __thrID;
    static DWORD WINAPI __threadStartUp(void *);
#else
#error "Please, provide definition for threadID and related for your OS"
#endif


protected:
    virtual tlMsg_t *__tlEnqueueHook(tlMsg_t*);
    virtual void     __tlDequeueHook(tlMsg_t*);
    virtual bool dispatchMsg(tlMsg_t *);

public:

    __threadedLoop_t(void);
    virtual ~__threadedLoop_t(void);

    void tlRun(void);

    void tlPostMsg(tlMsg_t *);

    bool tlRunning(void) const { return __runFlag; }//;


    const char *className(void) const { return "__threadedLoop_t"; }//;

#ifdef WIN32
    void EndThread(void){TerminateThread(__threadID,0);}
#endif

    friend class tlmQueue_t;
    friend class threadedLoop_t;
};


class _Dll_ threadedLoop_t: public __threadedLoop_t
{
protected:
    virtual void funeraryRites(void);

public:
    virtual ~threadedLoop_t(void);


    const char *className(void) const { return "threadedLoop_t"; }//;


    friend class smartReference_t<threadedLoop_t>;
};
typedef _Dll_ smartReference_t<threadedLoop_t> threadedLoop_ref;

#endif

#endif
