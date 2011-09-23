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
// $Id: task.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__task_hh__
#define __icf2__task_hh__

#include <icf2/item.hh>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>
#include <icf2/threadedLoop.hh>
#include <icf2/io.hh>


class _Dll_ sched_t;     // basic scheduling facility
class _Dll_ alarm_t;     // basic scheduling facility

typedef _Dll_ smartReference_t<alarm_t> alarm_ref;


//
// abstractTask_t -- taskInterface
//
class _Dll_ abstractTask_t: public virtual item_t, public virtual collectible_t
{
private:
    u32         __period;
    i32         __deviation;
    IOList_t    __IOList;
    sched_t    *__owner;
    bool        __threaded;

private:
    virtual void __doIOReady  (io_ref        &io)= 0;  // called on IO events
    virtual void __doHeartBeat(void             )= 0;  // called on periodic events
    virtual void __doTimeOut  (const alarm_ref &al)= 0;  // called on time events


    virtual void __preInSchedRites (sched_t *)      = 0;  // called just before beeing inserted in scheduler
    virtual void __pstInSchedRites (void     )      = 0;  // called just after  beeing inserted in scheduler
    virtual void __preOutSchedRites(void     )      = 0;  // called just before beeing removed from scheduler
    virtual void __pstOutSchedRites(void     )      = 0;  // called just after  beeing removed from scheduler

    virtual void __sysHoldIO(io_ref &io);

    virtual void set_deviation(i32 d) { __deviation= d; };

    virtual ~abstractTask_t(void) { __owner= NULL; };

public:
    virtual void   add_IO(io_ref)                                   ;
    virtual void   del_IO(io_ref)                                   ;

    virtual void   hold_IO     (io_ref);
    virtual void   unhold_IO   (io_ref);
    virtual void   unhold_allIO(void  );


    virtual u32 get_period(void )  const { return __period; };
    virtual u32 set_period(u32 p)                            ;


    virtual alarm_ref set_alarm(u32, void *);
    virtual void     del_alarm(alarm_ref &);


    virtual sched_t *get_owner(void) const { return __owner; };



    virtual void IOReady(io_ref &);             // called on IO events
    virtual void heartBeat(void);               // called on periodic events
    virtual void timeOut(const alarm_ref &);    // called on time events


//protected:
public:
    virtual void bailOut(void);

public:
    virtual const char *className(void) const { return "abstractTask_t"; };

    friend class simpleTask_t;
    friend class threadedTask_t;
    friend class sched_t;

    friend class smartReference_t<abstractTask_t>;

//private:
//    virtual void die(void);
};
typedef _Dll_ smartReference_t<abstractTask_t> task_ref;


class _Dll_ simpleTask_t: public abstractTask_t
{
private:
    virtual void __doIOReady  (io_ref        &io) { IOReady(io); };  // called on IO events
    virtual void __doHeartBeat(void             ) { heartBeat(); };  // called on periodic events
    virtual void __doTimeOut  (const alarm_ref &al) { timeOut(al); };  // called on time events


    virtual void __preInSchedRites (sched_t *)    { };  // called just before beeing inserted in scheduler
    virtual void __pstInSchedRites (void     )    { };  // called just after  beeing inserted in scheduler
    virtual void __preOutSchedRites(void     )    { };  // called just before beeing removed from scheduler
    virtual void __pstOutSchedRites(void     )    { };  // called just after  beeing removed from scheduler


public:
    simpleTask_t(u32= 0);
    simpleTask_t(const io_ref &, u32= 0);
    simpleTask_t(const IOList_t &, u32= 0);

protected:
    virtual ~simpleTask_t(void);

public:
    virtual const char *className(void) const { return "simpleTask_t"; };

    friend class sched_t;
    friend class smartReference_t<simpleTask_t>;
};

typedef _Dll_ smartReference_t<simpleTask_t> simpleTask_ref;





#ifdef __THREADED_ICF

class _Dll_ threadedTask_t: public abstractTask_t, public virtual threadedLoop_t
{
private:
    int       __heartBeatsPending;
protected:
    tlMsg_t  *__tlEnqueueHook(tlMsg_t *);
    void      __tlDequeueHook(tlMsg_t *);


private:
    virtual void __doIOReady  (io_ref        &io);  // called on IO events
    virtual void __doHeartBeat(void             );  // called on periodic events
    virtual void __doTimeOut  (const alarm_ref &al);  // called on time events


    virtual void __preInSchedRites (sched_t *)    { };  // called just before beeing inserted in scheduler
    virtual void __pstInSchedRites (void     )    { };  // called just after  beeing inserted in scheduler
    virtual void __preOutSchedRites(void     )    { };  // called just before beeing removed from scheduler
    virtual void __pstOutSchedRites(void     )    { };  // called just after  beeing removed from scheduler


protected:
    virtual bool dispatchMsg(tlMsg_t *);


public:
    threadedTask_t(u32= 0);
    threadedTask_t(const io_ref &, u32= 0);
    threadedTask_t(const IOList_t &, u32= 0);

    virtual alarm_ref set_alarm(u32, void *);
    virtual void      del_alarm(alarm_ref &);

protected:
    virtual ~threadedTask_t(void);


public:
    virtual const char *className(void) const { return "threadedTask_t"; };


    friend class sched_t;
    friend class smartReference_t<threadedTask_t>;
};

typedef smartReference_t<threadedTask_t> threadedTask_ref;

#endif



#endif




