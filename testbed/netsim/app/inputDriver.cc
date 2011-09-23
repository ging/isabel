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
// $Id: inputDriver.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <assert.h>

#include "inputDriver.hh"
#include "messages.hh"


inputDriver_t::inputDriver_t(io_ref &io, threadedTask_t *outTask)
: threadedTask_t(io), __outTask(outTask) 
{
   assert(__outTask && "No out task!!!");
}

inputDriver_t::~inputDriver_t() {
}

bool
inputDriver_t::dispatchMsg(tlMsg_t *tlMsg) {
     return threadedTask_t::dispatchMsg(tlMsg);
}

void
inputDriver_t::IOReady(io_ref &io) {
    inetAddr_t addr;

    io_t *iop =static_cast<io_t *>(io);
    dgramSocket_t *dgramSocket = static_cast<dgramSocket_t *>(iop);

    char *data = (char *)malloc(MAX_PKT_SIZE);
    int len = dgramSocket->recvFrom(addr, data, MAX_PKT_SIZE);

    pktBuff_ref pktBuff = new pktBuff_t(data); 
    pktMsg_t *pktMsg = new pktMsg_t(pktBuff, len);

    __outTask->tlPostMsg(pktMsg);
}
