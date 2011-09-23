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
// $Id: icfMessages.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__messages_hh__
#define __icf2__messages_hh__

#include <icf2/sched.hh>
#include <icf2/smartReference.hh>


enum icfMessages_e {
    MSG_IO        = 0x1000,
    MSG_HEARTBEAT = 0x1001,
    MSG_ALARM     = 0x1002
};



#ifdef __THREADED_ICF

//
// msgTask_Io_t -- identified by MSG_IO
//
class _Dll_ msgTask_Io_t: public tlMsg_t {
public:
    io_ref msgIo;

    msgTask_Io_t(io_ref &io);
    virtual ~msgTask_Io_t(void);
};

//
// msgTask_HeartBeat_t -- identified by MSG_HEARTBEAT
//
class _Dll_ msgTask_HeartBeat_t: public tlMsg_t {
public:
    msgTask_HeartBeat_t(void);
    virtual ~msgTask_HeartBeat_t(void);
};
 
//
// msgTask_Alarm_t -- identified by MSG_ALARM
//
class _Dll_ msgTask_Alarm_t: public tlMsg_t {
public:
    alarm_ref msgAlarm;

    msgTask_Alarm_t(const alarm_ref &al);
    virtual ~msgTask_Alarm_t(void);
};

#endif



#endif
