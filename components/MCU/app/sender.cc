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
// $Id: sender.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "sender.h"
#include "socketPool.h"
#include "returnCode.h"

#include <icf2/notify.hh>

sender_t::sender_t(inetAddr_t &destAddr,u16 localPort)
{
    addr = new inetAddr_t(destAddr);
    socket = socketPool->get(localPort);
}

    
sender_t::~sender_t(void)
{
    socketPool->release(socket);
}

HRESULT 
sender_t::setNext(filter_t *n)
{
    if (n)
    {
        NOTIFY("sender_t :: trying to setNext in a sink filter!, aborting...\n");
        abort();
    }
    return S_OK;
}

HRESULT 
sender_t::deliver (RTPPacket_t * pkt)
{
    int n = socket->writeTo(*addr,pkt->getPacket(),pkt->getTotalLength());

    if (n < 0)
    {
        NOTIFY("sender_t::socket write returns %d\n",n);
        return E_ERROR;
    }
    return S_OK;
}

