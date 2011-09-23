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
// $Id: task.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/io.hh>
#include <icf2/task.hh>
#include <icf2/sched.hh>
#include <icf2/icfMessages.hh>


void
abstractTask_t::__sysHoldIO(io_ref &io)
{
    if(__owner)
        __owner->adjust_holdIO(task_ref(this), io, 1);
}


void
abstractTask_t::add_IO(io_ref io)
{
    __IOList<< io;

    if(__owner)
        __owner->adjust_setIO(task_ref(this), io);
}

void
abstractTask_t::del_IO(io_ref io)
{
    if(__owner)
        __owner->adjust_clrIO(task_ref(this), io);
}

void
abstractTask_t::hold_IO(io_ref io)
{
    if(__owner)
        __owner->adjust_holdIO(task_ref(this), io);
}

void
abstractTask_t::unhold_IO(io_ref io)
{
    if(__owner)
        __owner->adjust_unholdIO(task_ref(this), io);
}

void
abstractTask_t::unhold_allIO(void)
{
    if(__owner)
        __owner->adjust_unholdAllIO();
}


u32
abstractTask_t::set_period(u32 p)
{
    u32 retVal= __period;

    __period= p;
    __deviation= 0;

    if (__owner)
    {
        // remove deadline, if pending
        __owner->cancelDeadLine(this);
        if ( __period)
        {
            deadLine_ref d = new deadLine_t(this, this->__period);
            __owner->newDeadLine(d);
        }
    }

    return retVal;
}

    
alarm_ref 
abstractTask_t::set_alarm(u32 t, void *args)
{
    if(__owner)
        return __owner->adjust_setAlarm(task_ref(this), t, args);
    else
        return NULL;
}

void
abstractTask_t::del_alarm(alarm_ref &al)
{
    if(__owner)
        __owner->adjust_clrAlarm(task_ref(this), al);
}


void
abstractTask_t::IOReady(io_ref &io)
{
    char b[1024];
    NOTIFY("abstractTask_t::IOReady: IO ready\n");
    io->read(b, 1024);
}

void
abstractTask_t::heartBeat(void)
{
    NOTIFY("abstractTask_t::heartBeat: tic-tac\n");
}

void
abstractTask_t::timeOut(const alarm_ref &)
{
    NOTIFY("abstractTask_t::timeOut: it's about time\n");
}

void
abstractTask_t::bailOut(void)
{
    if(__owner)
        __owner->removeTask(static_cast<task_ref>(this));

    while(__IOList.len()> 0) {
        __IOList.behead();
    }
}





simpleTask_t::simpleTask_t(u32 _p)
{
    __period= _p;
    __deviation= 0;
    __owner= NULL;

    __threaded= false;
}

simpleTask_t::simpleTask_t(const io_ref &io, u32 p)
{
    __IOList<< io;
    __period= p;
    __deviation= 0;
    __owner= NULL;

    __threaded= false;
}

simpleTask_t::simpleTask_t(const IOList_t &iol, u32 p)
{
    __IOList<< iol;
    __period= p;
    __deviation= 0;
    __owner= NULL;

    __threaded= false;
}

simpleTask_t::~simpleTask_t(void)
{
    bailOut();
}














#ifdef __THREADED_ICF

msgTask_Io_t::msgTask_Io_t(io_ref &io): tlMsg_t(MSG_IO)
{
    msgIo= io;
}

msgTask_Io_t::~msgTask_Io_t(void)
{
    msgIo= NULL;
}


msgTask_HeartBeat_t::msgTask_HeartBeat_t(void): tlMsg_t(MSG_HEARTBEAT)
{
}

msgTask_HeartBeat_t::~msgTask_HeartBeat_t(void)
{
}


msgTask_Alarm_t::msgTask_Alarm_t(const alarm_ref &al): tlMsg_t(MSG_ALARM)
{
    msgAlarm= al;
}

msgTask_Alarm_t::~msgTask_Alarm_t(void)
{
    msgAlarm= NULL;
}
#endif












#ifdef __THREADED_ICF
alarm_ref 
threadedTask_t::set_alarm(u32 t, void *a)
{
    alarm_ref retVal= abstractTask_t::set_alarm(t,a);
    if(__owner) __owner->__notify(-1);

	return retVal;
}
void
threadedTask_t::del_alarm(alarm_ref &al)
{
    abstractTask_t::del_alarm(al);
    if(__owner) __owner->__notify(-1);
}



tlMsg_t  *
threadedTask_t::__tlEnqueueHook(tlMsg_t *msg)
{
    if(msg->msgKind== MSG_HEARTBEAT) {
        if(__heartBeatsPending>0) {
//            NOTIFY("threadedTask_t::__tlEnqueueHook: is missing deadLines\n");
            delete msg;

            return NULL;
        } else
            __heartBeatsPending++;
    }

    return threadedLoop_t::__tlEnqueueHook(msg);
}
void
threadedTask_t::__tlDequeueHook(tlMsg_t *msg)
{
    if(msg->msgKind== MSG_HEARTBEAT) {
        __heartBeatsPending--;

        assert(__heartBeatsPending>= 0);
    }
}

threadedTask_t::threadedTask_t(u32 _p)
{
    __heartBeatsPending= 0;

    __period= _p;
    __deviation= 0;
    __owner= NULL;

    __threaded= false;
}

threadedTask_t::threadedTask_t(const io_ref &io, u32 p)
{
    __heartBeatsPending= 0;

    __IOList<< io;

    __period= p;
    __deviation= 0;
    __owner= NULL;

    __threaded= false;
}

threadedTask_t::threadedTask_t(const IOList_t &iol, u32 p)
{
    __heartBeatsPending= 0;

    __IOList<< iol;

    __period= p;
    __deviation= 0;
    __owner= NULL;

    __threaded= false;
}

threadedTask_t::~threadedTask_t(void)
{
//    NOTIFY("threadedTask_t::~threadedTask_t: destructor pre die()\n");
//    die();
//    NOTIFY("threadedTask_t::~threadedTask_t: destructor post die()\n");
}


bool
threadedTask_t::dispatchMsg(tlMsg_t *msg)
{
    switch(msg->msgKind) {
        case MSG_IO:
            {
                task_ref  myself= this; // ugly!! XREF ecastro dancing with
                                        // the boojums

                msgTask_Io_t *m     = static_cast<msgTask_Io_t*>(msg);
                IOReady(m->msgIo);

                //
                // notify has a quick and dirty implementation now
                // must be corrected
                //
                if(__owner) __owner->__notify(m->msgIo->sysHandle());

                myself= NULL; // si alguien tiene dudas que le
                              // pregunte a eva
            } break;
        case MSG_HEARTBEAT:
            {
                task_ref  myself= this; // ugly!! XREF ecastro dancing with
                                        // the boojums

                heartBeat();

                myself= NULL; // si alguien tiene dudas que le
                              // pregunte a eva
            } break;
        case MSG_ALARM:
            {
                task_ref  myself= this; // ugly!! XREF ecastro dancing with
                                        // the boojums

                msgTask_Alarm_t *m= static_cast<msgTask_Alarm_t *>(msg);
                if (m->msgAlarm->hasOwnerTask()) {
                    timeOut(m->msgAlarm);
                } else {
                    debugMsg(dbg_K_Normal, "dispatchMsg",
                             "Alarma cancelada, no se llama a timeOut");
                    ; // La alarma se ha cancelado
                }

                myself= NULL; // si alguien tiene dudas que le
                              // pregunte a eva
            } break;
        default:
            return threadedLoop_t::dispatchMsg(msg);
    }


    return true;
}

void
threadedTask_t::__doIOReady(io_ref &io)
{
    if(tlRunning()) {
        __sysHoldIO(io);
//        hold_IO(io);
        tlPostMsg(new msgTask_Io_t(io));
    } else {
        IOReady(io);
    }
}

void
threadedTask_t::__doHeartBeat(void)
{
    if(tlRunning()) {
        tlPostMsg(new msgTask_HeartBeat_t);
    } else {
        heartBeat();
    }
}

void
threadedTask_t::__doTimeOut(const alarm_ref &al)
{
    if(tlRunning()) {
        tlPostMsg(new msgTask_Alarm_t(al));
    } else {
        timeOut(al);
    }
}
#endif



