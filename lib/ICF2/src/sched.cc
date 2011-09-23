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
// $Id: sched.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <icf2/general.h>

#if defined(__BUILD_FOR_WINXP)
#include "win32/resource.hh"
#elif defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/resource.h>
#else
#error "Please, import fcntl.h and resource.hh for your O.S."
#endif

#include <icf2/notify.hh>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/task.hh>
#include <icf2/sched.hh>
#include <icf2/icfTime.hh>

#define __ICF_STR_BUF_LEN 1024

deadLine_t::deadLine_t(const abstractTask_t *_tsk, int _t)
{
    time= _t;
    gettimeofday(&stamp, NULL); // get time from system

    // calculate deadLine= currentTime + relativeTime;
    deadStamp.tv_sec = stamp.tv_sec + (time/1000000);
    deadStamp.tv_usec= stamp.tv_usec+ (time%1000000);

    deadStamp.tv_sec += deadStamp.tv_usec/1000000;
    deadStamp.tv_usec%= 1000000;

    deadType= PERIODIC;

    // deadLine owner task
    task= (abstractTask_t *)_tsk;

    // next DeadLine
    next= NULL;
}

bool
deadLine_t::hasOwnerTask (void) const
{
    if (task) return true;
    return false;
}


alarm_t::alarm_t(const abstractTask_t *_tsk, int _t, void *_args)
: deadLine_t(_tsk, _t), args(_args)
{
    deadType= ALARM;
}


//
// __icf_ioMap_t --
//
//
static class __icf_ioMap_t: public item_t
{
private:
    class ioMapEntry_t {
    public:
        bool                  __usrHeldIO;
        bool                  __sysHeldIO;
        const abstractTask_t *__ownerTask;

        ioMapEntry_t(void):
            __usrHeldIO(false),
            __sysHeldIO(false),
            __ownerTask(NULL)
        {
        };
    };

public:
    unsigned      ioMapLength;
    ioMapEntry_t *ioMapData;

    __icf_ioMap_t(void) {
        debugMsg(dbg_K_Internal, "__icf_ioMap_t", "Building __icf_ioMap\n");

        struct rlimit rlp;

        getrlimit(RLIMIT_NOFILE, &rlp);

        ioMapLength= rlp.rlim_max;

        ioMapData= new ioMapEntry_t[ioMapLength];

        debugMsg(dbg_K_Internal, "__icf_ioMap_t", "Built __icf_ioMap\n");
    };

    ~__icf_ioMap_t(void) {
        debugMsg(dbg_K_Internal, "__icf_ioMap_t", "Destroying __icf_ioMap\n");
        delete [] ioMapData;
        debugMsg(dbg_K_Internal, "__icf_ioMap_t", "Destroyed __icf_ioMap\n");
    }

} __icf_ioMap;




sched_t::sched_t(int _n)
{
    int i;

    __nTasks= 0;
    __maxTasks= _n;
    __taskTable= new task_ref[_n];

    assert(__taskTable);

    for(i=0; i< _n; i++)       // not really required
        __taskTable[i]= NULL;  // since task_ref are already invalid

    FD_ZERO(&selectList);

    firstDeadLine= NULL;
    lastDeadLine= NULL;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    assert(pipe(__notificationPipe)>=0);
    FD_SET(__notificationPipe[0], &selectList);

    assert(pipe(__notificationDeleteTaskPipe)>=0);
    FD_SET(__notificationDeleteTaskPipe[0], &selectList);
    int err1 = fcntl(__notificationPipe[0],  F_SETFL, O_NONBLOCK);
    if (err1< 0) {
        NOTIFY("sched_t::sched_t: fcntl 1: %s\n", strerror(errno));
    }

    int err2 = fcntl(__notificationPipe[1],  F_SETFL, O_NONBLOCK);
    if (err2< 0) {
        NOTIFY("sched_t::sched_t: fcntl 2: %s\n", strerror(errno));
    }
#elif defined(__BUILD_FOR_WINXP)
    __notificationAddr[0] = new inetAddr_t("127.0.0.1",NULL,SOCK_DGRAM);
    __notificationAddr[1] = new inetAddr_t("127.0.0.1",NULL,SOCK_DGRAM);
    __notificationDeleteTaskAddr[0] = new inetAddr_t("127.0.0.1",NULL,SOCK_DGRAM);
    __notificationDeleteTaskAddr[1] = new inetAddr_t("127.0.0.1",NULL,SOCK_DGRAM);

    __notificationSocket[1] = new dgramSocket_t (*__notificationAddr[1],*__notificationAddr[0]); //abrimos las conexiones
    __notificationSocket[0] = new dgramSocket_t (*__notificationAddr[0]); //abrimos las conexiones
    FD_SET(__notificationSocket[0]->sysHandle(), &selectList);

    __notificationDeleteTaskSocket[1] = new dgramSocket_t (*__notificationDeleteTaskAddr[1],*__notificationDeleteTaskAddr[0]); //abrimos las conexiones
    __notificationDeleteTaskSocket[0] = new dgramSocket_t (*__notificationDeleteTaskAddr[0]); //abrimos las conexiones
    FD_SET(__notificationDeleteTaskSocket[0]->sysHandle(), &selectList);
#endif
    timeOut =NULL;
}

sched_t::~sched_t(void)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    close(__notificationPipe[0]);
    close(__notificationPipe[1]);

    close(__notificationDeleteTaskPipe[0]);
    close(__notificationDeleteTaskPipe[1]);
#elif defined(__BUILD_FOR_WINXP)
    delete __notificationSocket[1];
    delete __notificationSocket[0];
    delete __notificationDeleteTaskSocket[1];
    delete __notificationDeleteTaskSocket[0];
#endif
    delete [] __taskTable;

    __taskTable= NULL;
}

void
sched_t::__notify(int n)
{
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    write(__notificationPipe[1], &n, sizeof(n));
#elif defined(__BUILD_FOR_WINXP)
    __notificationSocket[1]->write(&n, sizeof(n));
#endif
}

sched_t &
sched_t::insertTask(const task_ref &task)
{
    if(!task.isValid())
        return *this;  // bail out

    task->__preInSchedRites(this);
    {
#ifdef __TREADED_ICF
        locker_t locker= wrLock();
#endif

        if(__nTasks< __maxTasks) {
            __taskTable[__nTasks]= task;
            __nTasks++;
        } else {
            NOTIFY("sched_t::operator << {nTasks} {system halted}\n");
            abort();
        }

        //
        // set owner
        //
        task->__owner= this;


        //
        // schedule periodic tasks
        //
        if(task->__period)
            __unlocked_adjust_setPeriod(task);

        debugMsg(
            dbg_K_Verbose, "<<(task_t)",
            "task->_IOList.len()== %d", task->__IOList.len()
        );


        //
        // schedule I/O tasks
        //
        for(IOList_t::iterator_t i= task->__IOList.begin();
            i != task->__IOList.end();
            i++) {
            io_ref ior= static_cast<io_ref>(i);
            __unlocked_adjust_setIO(task, ior);
        }
    }
    task->__pstInSchedRites();

    return *this;
}


sched_t &
sched_t::removeTask(const task_ref &task)
{
    if(!task.isValid())
        return *this;  // bail out

    task->__preOutSchedRites();
    {
#ifdef __TREADED_ICF
        locker_t locker= wrLock();
#endif

        for(int i= 0; i< __nTasks; i++) {
            if(__taskTable[i]== task) {

                debugMsg(dbg_K_Verbose, "-(task_t)", "removing task %d", i);
                for(int j= i; j< __nTasks-1; j++)
                    __taskTable[j]= __taskTable[j+1];

                __nTasks--;
                __taskTable[__nTasks]= NULL;

                debugMsg(
                    dbg_K_Verbose,
                    "-(task_t)",
                    "removed task, %d running",
                    __nTasks
                );

                //
                // remove from schedule periodic task & alarms
                //
                deadLine_ref  d= firstDeadLine;
                while(d.isValid()) {
                    if(d->task== task)
                        d->task= NULL;
                    d= d->next;
                }

                //
                // remove from schedule I/O task
                //
                for(IOList_t::iterator_t k= task->__IOList.begin();
                    k != task->__IOList.end();
                    k++) {
                    io_ref ior= static_cast<io_ref>(k);

                    int h= ior->sysHandle();

                    // puede que la haya eliminado antes, pero no
                    // la he quitado de IOList para no tener que poner
                    // cerrojos para cada iteracion de run()

                    if(__icf_ioMap.ioMapData[h].__ownerTask)
                         __unlocked_adjust_clrIO(task, ior);

                }

                break;
            }
        }

        task->__owner= NULL;
    }
    task->__pstOutSchedRites();

    __wakingUpSelect();

    return *this;
}


void
sched_t::__unlocked_adjust_clrIO(const task_ref &tsk, io_ref io)
{
    //
    // remove from schedule I/O task
    //
    int h= io->sysHandle();
    if(__icf_ioMap.ioMapData[h].__ownerTask!= tsk) {
        NOTIFY("sched_t::__unlocked_adjust_clrIO: error with handle= %d\n", h);
    } else {
        int wakeUp = -1;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        write(__notificationDeleteTaskPipe[1], &wakeUp, sizeof(wakeUp));
#elif defined(__BUILD_FOR_WINXP)
        __notificationDeleteTaskSocket[1]->write(&wakeUp, sizeof(wakeUp));
#endif
        __icf_ioMap.ioMapData[h].__usrHeldIO= false;
        __icf_ioMap.ioMapData[h].__sysHeldIO= false;
        __icf_ioMap.ioMapData[h].__ownerTask= NULL;

        FD_CLR(io->sysHandle(), &selectList);
    }
}
void
sched_t::adjust_clrIO(const task_ref &tsk, io_ref io)
{
#ifdef __TREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_clrIO(tsk, io);
}


void
sched_t::__unlocked_adjust_setIO(const task_ref &tsk, io_ref io)
{
    //
    // add to schedule I/O task
    //
    int h= io->sysHandle();
    if(__icf_ioMap.ioMapData[h].__ownerTask!= NULL) {
        NOTIFY("error in sched_t::adjust_setIO, handle= %d\n", h);
    } else {
        __icf_ioMap.ioMapData[h].__usrHeldIO= false;
        __icf_ioMap.ioMapData[h].__sysHeldIO= false;
        __icf_ioMap.ioMapData[h].__ownerTask= tsk;

        FD_SET(h, &selectList);

        __wakingUpSelect();
    }
}
void
sched_t::adjust_setIO(const task_ref &tsk, io_ref io)
{
#ifdef __TREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_setIO(tsk, io);
}


void
sched_t::__unlocked_adjust_holdIO(const task_ref &tsk, io_ref io, int system)
{
    //
    // set io_t to usrHeld state
    //
    int h= io->sysHandle();
    if(__icf_ioMap.ioMapData[h].__ownerTask!= tsk) {
        NOTIFY("error in sched_t::adjust_holdIO, handle= %d\n", h);
    } else {
           if(system)
            __icf_ioMap.ioMapData[h].__sysHeldIO= true;
           else
            __icf_ioMap.ioMapData[h].__usrHeldIO= true;

        FD_CLR(io->sysHandle(), &selectList);
    }

    heldIO<< io->sysHandle();
}
void
sched_t::adjust_holdIO(const task_ref &tsk, io_ref io, int system)
{
#ifdef __TREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_holdIO(tsk, io, system);
}


void
sched_t::__unlocked_adjust_unholdIO(const task_ref &tsk, io_ref io)
{
    //
    // cleart io_t from usrHeld state
    //
    int h= io->sysHandle();
    if(__icf_ioMap.ioMapData[h].__ownerTask!= tsk) {
        NOTIFY("error in sched_t::adjust_unholdIO, handle= %d\n", h);
    } else {
        __icf_ioMap.ioMapData[h].__usrHeldIO= false;
        if(!__icf_ioMap.ioMapData[h].__sysHeldIO)
            FD_SET(io->sysHandle(), &selectList);
    }
}
void
sched_t::adjust_unholdIO(const task_ref &tsk, io_ref io)
{
#ifdef __THREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_unholdIO(tsk, io);
}


void
sched_t::__unlocked_adjust_unholdAllIO(void)
{
    for(; heldIO.len()> 0; heldIO.behead()) {
        FD_SET(heldIO.head(), &selectList);
    }
}
void
sched_t::adjust_unholdAllIO(void)
{
#ifdef __THREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_unholdAllIO();
}


void
sched_t::__unlocked_adjust_clrAlarm(const task_ref &, alarm_ref &alarm)
{
    alarm->task= NULL; // no creo que el cerrojo sea necesario
}

void
sched_t::adjust_clrAlarm(const task_ref &tsk, alarm_ref &alarm)
{
#ifdef __THREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_clrAlarm(tsk, alarm);
}


alarm_ref
sched_t::__unlocked_adjust_setAlarm(const task_ref &t, u32 p, void *al)
{
    alarm_ref alarm = new alarm_t(t, p, al);
    deadLine_ref d = static_cast<deadLine_t *>(static_cast<alarm_t *>(alarm));
    newDeadLine(d);
    return alarm;
}

alarm_ref
sched_t::adjust_setAlarm(const task_ref &t, u32 p, void *al)
{
#ifdef __THREADED_ICF
    locker_t locker= wrLock();
#endif

    return __unlocked_adjust_setAlarm(t, p, al);
}


void
sched_t::adjust_setPeriod(const task_ref &t)
{
#ifdef __THREADED_ICF
    locker_t locker= wrLock();
#endif

    __unlocked_adjust_setPeriod(t);
}

void
sched_t::__unlocked_adjust_setPeriod(const task_ref &t)
{
    deadLine_ref aux = new deadLine_t(t, t->__period - t->__deviation);
    newDeadLine(aux);
}

void
sched_t::__wakingUpSelect(void)
{
    int wakeUp= -1;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    write(__notificationPipe[1], &wakeUp, sizeof(wakeUp));
#elif defined(__BUILD_FOR_WINXP)
    __notificationSocket[1]->write(&wakeUp, sizeof(wakeUp));
#endif


}

void
sched_t::run(void)
{
    int i;
    int n;
    fd_set rdFdSet;

#if 0
    struct timeval  deadTime;
    struct timeval  currTimeInfo;
    struct timeval  _timeOut;
#endif



    //
    // this code loops while _priv_ntask is non zero... effectively
    // it quits when the scheduler runs out of tasks.
    //

    while(__nTasks) {
        timeOut= NULL;

        memcpy(&rdFdSet, &selectList, sizeof(selectList));

        int escapeHatch= __nTasks+4;

        while(firstDeadLine.isValid() && !timeOut && escapeHatch> 0) {
            deadLine_ref currDeadLine=checkDeadLines();
            if (currDeadLine.isValid()) {
                dispatch(currDeadLine);
                if(__nTasks<= 0) return; // CHAPUZA, habria que arreglarlo...
                                         // de momento evita el pure virtual
                                         // method called al llamar a
                                         // shutdown del sched
            }

            escapeHatch--;
        }

        //
        // may be someone killed all tasks in the above event
        // dispatching... so recheck for avail tasks
        //
        if(__nTasks<= 0) return;

        //
        // Too many time events? Let's give a brief window
        // of opportunity to IO tasks
        //
        if(escapeHatch<= 0) {
            timeOut= &_timeOut; // Y esto para que sirve???
            timeOut->tv_sec= timeOut->tv_usec= 0;
        }

        //
        // do the select 'ritual dance'
        //
        debugMsg(
            dbg_K_Verbose,
            "run(void)",
            "falling into select(), wait time= %.3f",
            float(timeOut?timeOut->tv_sec:-1.0)+
            float(timeOut?timeOut->tv_usec/1000000.0:0.0)
        );
        n= ::select(256 /*8*sizeof(selectList)*/,
                    &rdFdSet, NULL, NULL, timeOut);

        debugMsg(dbg_K_Verbose, "run", "select() returned %d", n);

        if((n< 0) && (errno== EINTR)) continue;

        if( n < 0) {

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            char __icf_strerr[__ICF_STR_BUF_LEN];
            strerror_r(errno, __icf_strerr, __ICF_STR_BUF_LEN);
            NOTIFY("sched_t::run: system halted: %s\n", __icf_strerr);
            abort();
#elif defined(__BUILD_FOR_WINXP)
            NOTIFY("Error: %s\n",DecodeError(WSAGetLastError()));
            NOTIFY("sched_t::run {select} {system halted}\n");
            abort();
#endif
        }

        if (n == 0)
            continue;

        //
        // check the notification delete task pipe...
        // clean select descriptor list...
        //
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        if(FD_ISSET(__notificationDeleteTaskPipe[0], &rdFdSet)) {
#elif defined(__BUILD_FOR_WINXP)
        if(FD_ISSET(__notificationDeleteTaskSocket[0]->sysHandle(), &rdFdSet)) {
#endif
            handleDeleteTaskNotifications();
            continue;
        }


        //
        // check the notification pipe... see comment above
        //
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        if(FD_ISSET(__notificationPipe[0], &rdFdSet)) {
#elif defined(__BUILD_FOR_WINXP)
        if(FD_ISSET(__notificationSocket[0]->sysHandle(), &rdFdSet)) {
#endif
            handleNotifications();
        }

        //
        // Dispatch at most one IO event per task.
        // Too many active tasks can make this to go flakey...
        // hope you've got enough horse power (CPUs are cheap :)
        //
        for(i= 0; i< __nTasks; i++) {
            if(__taskTable[i]->__IOList.len()) {
                for(IOList_t::iterator_t j = __taskTable[i]->__IOList.begin();
                                         j!= __taskTable[i]->__IOList.end();
                                         j++
                   )
                {

                    io_ref io= j;
                    int    handle=io->sysHandle();
                    if(FD_ISSET(handle, &rdFdSet)) {
                        debugMsg(
                            dbg_K_Verbose,
                            "run",
                            "activating IO task %d on descriptor [%d]",
                            i, handle
                        );

                        assert(__icf_ioMap.ioMapData[handle].__ownerTask &&
                               "IO en tarea activada en el select");

                        __taskTable[i]->__doIOReady(io);

                        debugMsg(dbg_K_Verbose, "run", "IO task returned");

                        break; // just 1 IO per task
                    }
                }
            }
        }
    }
}

deadLine_ref
sched_t::checkDeadLines(void)
{

#define later(x, y)                                         \
(                                                           \
    (x.tv_sec> y.tv_sec)                                    \
    || ((x.tv_sec== y.tv_sec) && (x.tv_usec>=  y.tv_usec))  \
)

#ifdef __THREADED_ICF
    locker_t locker = wrLock();
#endif

    //
    // first remove "alarms" for already dead tasks
    //
    while (firstDeadLine.isValid() && !firstDeadLine->task)
        removeFirstDeadLine();


    if (!firstDeadLine.isValid()) {
         debugMsg(dbg_K_Paranoic, "run", "no hay mas temporizadores");
         return NULL; // out of deadlines
    }

    //
    // eval, currTimeInfo and deadTime of current deadLine
    //
    gettimeofday(&currTimeInfo,NULL);
    memcpy(&__currTime__, &currTimeInfo, sizeof(currTimeInfo));
    deadTime= firstDeadLine->deadStamp;

    //
    // if currTimeInfo is "late" for first alarm deadTime
    // dispatch it... else eval timeOut
    //
    if(later(currTimeInfo, deadTime)) {
        deadLine_ref currDeadLine = firstDeadLine;

        // deviation computing
        i32 deviation=   (currTimeInfo.tv_sec*1000000 + currTimeInfo.tv_usec)
                       - (deadTime.tv_sec*1000000     + deadTime.tv_usec);
        currDeadLine->task->set_deviation(deviation);

        removeFirstDeadLine();
        return currDeadLine;

    } else {
        _timeOut.tv_sec= deadTime.tv_sec - currTimeInfo.tv_sec;
        _timeOut.tv_usec= deadTime.tv_usec - currTimeInfo.tv_usec;
        if(_timeOut.tv_usec< 0) {
            _timeOut.tv_usec+= 1000000;
            _timeOut.tv_sec-= 1;
        }

        timeOut= &_timeOut;
        debugMsg(dbg_K_Paranoic, "run", "No se han cumplido "
                 "mas temporizadores, ajusto el timeOut=%d.%d",
                 timeOut->tv_sec, timeOut->tv_usec);
        return NULL;
    }
#undef later
}

void
sched_t::dispatch(deadLine_ref &dLine)
{
    debugMsg(dbg_K_Paranoic, "dispatch", "Atendiendo alarm/heartBeat");
    assert(dLine.isValid());

#if 0
    abstractTask_t *currTask= dLine->task;
    assert(currTask); // CUIDADO ESTO PUEDE NO SER CIERTO!!!
#else
    task_ref currTask= dLine->task;
    if (!currTask.isValid()) return;
#endif

    assert(currTask.isValid());

    switch(dLine->deadType) {

    case deadLine_t::PERIODIC:
        {
            if(currTask->__period) {
                // New heartBeat deadLine with period
                adjust_setPeriod(currTask);
                currTask->__doHeartBeat();

            } else {
                deadLine_ref         aux= dLine;
                abstractTask_t   *task= aux->task;
                while(aux.isValid()) {
                    if (aux->task== task &&
                        aux->deadType== deadLine_t::PERIODIC)
                            aux->task= NULL;
                    aux= aux->next;
                }
            }
        } break;

    case deadLine_t::ALARM:
        {
            deadLine_t *auxAl1 = static_cast<deadLine_t *>(dLine);
            alarm_ref auxAl = static_cast<alarm_t *>(auxAl1);

            assert(auxAl.isValid());
            currTask->__doTimeOut(auxAl);
        } break;

    default:
        NOTIFY("sched_t:: dispatch:: Unknown deadline!!!\n");
    }
}

void
sched_t::handleNotifications(void)
{
    debugMsg(dbg_K_Normal,
             "handleNotifications",
             "threadedTask {end IO} or {wake up} notification"
            );

    int nn;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    int nread=::read(__notificationPipe[0], &nn, sizeof(nn));
#elif defined(__BUILD_FOR_WINXP)
    int nread = __notificationSocket[0]->read(&nn, sizeof(nn));
#endif

    if (nread < 0) return; // what the hell!!!
    if (nn < 0) return;    // just a wake up, do nozing


    // me pueden haber borrado en el transcurso de la atencion
    // del IO, no puedo volver a activarlo!
    if (!__icf_ioMap.ioMapData[nn].__ownerTask) {
       debugMsg(dbg_K_Normal, "handleNotifications",
                "Deleted io_ref from IOList in IOReady method! "
                "do not activate!");
        return;
    }

    //
    // IO descriptor administrivia
    //
    __icf_ioMap.ioMapData[nn].__sysHeldIO= false;

    if(!__icf_ioMap.ioMapData[nn].__usrHeldIO)
        FD_SET(nn, &selectList);
}



void
sched_t::handleDeleteTaskNotifications(void)
{
    debugMsg(dbg_K_Normal,
             "handleDeleteTaskNotifications",
             "actualizating select list..."
            );

    int nn;
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    int nread= ::read(__notificationDeleteTaskPipe[0], &nn, sizeof(nn));
#elif defined(__BUILD_FOR_WINXP)
    int nread= __notificationDeleteTaskSocket[0]->read(&nn,sizeof(nn));
#endif

    if (nread < 0) return; // what the hell!!!
    if (nn < 0) return;    // just a wake up, do nozing

#ifdef __THREADED_ICF
    locker_t locker= wrLock(); //let adjust_clrIO actualize select io list
#endif
}

void
sched_t::shutdown(void)
{
    debugMsg(dbg_K_Normal, "shutdown", "begin");
    for(int i= 0; i< __maxTasks; i++) {
        if(__taskTable[i].isValid()) {
            __taskTable[i]->__owner= NULL;
            __taskTable[i]= NULL;
        }
    }
    __nTasks  = 0;
    __wakingUpSelect();
    debugMsg(dbg_K_Normal, "shutdown", "end");
}


abstractTask_t *
sched_t::dispatch(io_t &)
{
    return NULL;
}



deadLine_ref
sched_t::newDeadLine(deadLine_ref &d)
{
    deadLine_ref  p;
    deadLine_ref  q;

#define later(x, y)                                         \
(                                                           \
    (x.tv_sec> y.tv_sec)                                    \
    || ((x.tv_sec== y.tv_sec) && (x.tv_usec>=  y.tv_usec))  \
)

    bool wakeUpSelect = false;

    if(!firstDeadLine.isValid()) {
        firstDeadLine= lastDeadLine= d;
        wakeUpSelect = true;

    } else {
        p= firstDeadLine;
        q= NULL;
        while(p.isValid() && later(d->deadStamp, p->deadStamp))  {
            q= p, p= p->next;
        }

        if(p.isValid()) {
            d->next=p;
            assert(d->next.isValid());
            if(q.isValid()) {
                q->next= d;

            } else {
                firstDeadLine= d;
                wakeUpSelect = true;
            }

        } else {
            lastDeadLine->next= d;
            lastDeadLine= d;
        }
    }

    if (wakeUpSelect)
        __wakingUpSelect();

    return d;
}


void
sched_t::removeFirstDeadLine(void)
{
    if(!firstDeadLine.isValid()) {
        NOTIFY("%s::sched_t::removeFirstDeadLine: empty deadLine list\n",
               item_t::className()
              );
        abort();
    }


    deadLine_ref aux= firstDeadLine->next;
    firstDeadLine=aux;

    if(!firstDeadLine.isValid()) {
        lastDeadLine= NULL;
    }
}

// when a task sets period to zero, pending periodic deadline must
// be cancelled,
// otherwise, a (fast) period re-setting to a non-zero value would
// create TWO periodic deadlines for one task
void
sched_t::cancelDeadLine(const abstractTask_t *task)
{
#ifdef __TREADED_ICF
    locker_t locker= wrLock();
#endif

    deadLine_ref aux= firstDeadLine;
    while (aux.isValid())
    {
        if (aux->task == task && aux->deadType == deadLine_t::PERIODIC)
        {
            aux->task= NULL;
        }
        aux= aux->next;
    }
}

