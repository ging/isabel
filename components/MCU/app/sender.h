/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: sender.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_SENDER_H_
#define _MCU_SENDER_H_

#include "general.h"
#include "filter.h"

#include <icf2/sockIO.hh>

class sender_t: public filter_t
{

private:

    dgramSocket_t * socket;
    inetAddr_t    * addr;

public:

    sender_t(inetAddr_t &destAddr,u16 localPort = 0);
    virtual ~sender_t(void);

    virtual HRESULT setNext(filter_t *n); // aborta si se le pone
    virtual HRESULT deliver (RTPPacket_t * pkt);
};
#endif

