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
// $Id: sched.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__sched_hh__
#define __icf2__sched_hh__

#include <icf2/sockIO.hh>
#include <icf2/task.hh>
#include <icf2/icfTime.hh>
#include <icf2/lockSupport.hh>

class _Dll_ deadLine_t: public virtual item_t,
                        public virtual collectible_t
{
public:
    enum deadType_e {
        PERIODIC,
        ALARM
    };

protected:
    deadType_e      deadType;

    int             time;
    struct timeval  stamp;
    struct timeval  deadStamp;  // stamp + time = deadStamp

    abstractTask_t  *task;

    //
    // una larga historia -- args
    //
    char    kkShit[64];

    smartReference_t<deadLine_t>  next;          // linked list

public:
    deadLine_t(const abstractTask_t *_tsk, int _t);

    bool hasOwnerTask() const;

    virtual const char *className(void) const { return "deadLine_t"; }

    friend class sched_t;
    friend class smartReference_t<deadLine_t>;
};

typedef _Dll_ smartReference_t<deadLine_t> deadLine_ref;


class _Dll_ alarm_t: public deadLine_t
{
public:
    void *args;

    alarm_t(const abstractTask_t *_tsk, int _t, void *_args= NULL);

    virtual const char *className(void) const { return "alarm_t"; }

    friend class smartReference_t<alarm_t>;
};

typedef smartReference_t<alarm_t> alarm_ref;

// basic scheduling facility
class _Dll_ sched_t: public virtual item_t, public rwLockedItem_t
{
private:
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    int  __notificationPipe[2];
    int  __notificationDeleteTaskPipe[2];
#elif defined(WIN32)
    // as pipes does not wake up "select" in WIN32,
    // let's rock with a socket
    inetAddr_t  *__notificationAddr[2];
    inetAddr_t  *__notificationDeleteTaskAddr[2];
    dgramSocket_t *__notificationSocket[2];
    dgramSocket_t *__notificationDeleteTaskSocket[2];
#else
#error "Please, declare __notificationPipe in your OS"
#endif

    void __notify(int n);

    struct timeval  *timeOut;

    void __wakingUpSelect();

protected:
    int        __nTasks;
    int        __maxTasks;
    task_ref  *__taskTable;

    ql_t<int>   heldIO;
    IOList_t    IOList;
    fd_set      selectList;
    deadLine_ref  firstDeadLine;
    deadLine_ref  lastDeadLine;

    struct timeval  __currTime__;

    struct timeval  deadTime;
    struct timeval  currTimeInfo;
    struct timeval  _timeOut;

public:
    sched_t(int);
    virtual ~sched_t(void);

    virtual sched_t & insertTask(const task_ref &);
    virtual sched_t & removeTask(const task_ref &);

    virtual sched_t & operator <<(const task_ref &t) { return insertTask(t); };
    virtual sched_t & operator  -(const task_ref &t) { return removeTask(t); };

    //
    // Only to be used by tasks!!
    //
private:
    virtual void __unlocked_adjust_clrIO(const task_ref &, io_ref);
    virtual void __unlocked_adjust_setIO(const task_ref &, io_ref);
    virtual void __unlocked_adjust_holdIO(const task_ref &, io_ref, int system);
    virtual void __unlocked_adjust_unholdIO(const task_ref &, io_ref);
    virtual void __unlocked_adjust_unholdAllIO(void);

    virtual void      __unlocked_adjust_clrAlarm(const task_ref &, alarm_ref &);
    virtual alarm_ref __unlocked_adjust_setAlarm(const task_ref &, u32, void *);


    virtual void adjust_clrIO   (const task_ref &, io_ref);
    virtual void adjust_setIO   (const task_ref &, io_ref);
    virtual void adjust_holdIO  (const task_ref &, io_ref, int system= 0);
    virtual void adjust_unholdIO(const task_ref &, io_ref);
    virtual void adjust_unholdAllIO(void);

    virtual void      adjust_clrAlarm(const task_ref &, alarm_ref &);
    virtual alarm_ref adjust_setAlarm(const task_ref &, u32, void *);

    virtual void dispatch(deadLine_ref &dLine);

    void adjust_setPeriod(const task_ref &t);
    void __unlocked_adjust_setPeriod(const task_ref &t);

    virtual void handleNotifications();
    virtual void handleDeleteTaskNotifications();

public:
    virtual void run(void);
    virtual void shutdown(void);

    virtual const struct timeval &currTime(void) { return __currTime__; };

    virtual const char *className(void) const { return "sched_t"; };

protected:
//    virtual abstractTask_t *dispatch(void);
    virtual abstractTask_t *dispatch(io_t &);

    virtual deadLine_ref newDeadLine(deadLine_ref &);
    virtual void removeFirstDeadLine(void);
    // period 0 requires the following
    virtual void cancelDeadLine(const abstractTask_t *tsk);

    virtual deadLine_ref checkDeadLines(void);

    friend class abstractTask_t;
    friend class task_t;
    friend class threadedTask_t;
};


#endif

