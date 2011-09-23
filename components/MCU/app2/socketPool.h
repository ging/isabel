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
// $Id: socketPool.h 6363 2005-03-18 16:31:53Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_SOCKET_POOL_H_
#define _MCU_SOCKET_POOL_H_

#include "general.h"
#include <icf2/sockIO.hh>

#define PORT_LEN 0xffff


class socketPool_t
{
private:

    class socketInfo_t 
    {
        private:

            io_ref io;
            u16    refCount;
            u16    port;
     
        public:
            
            socketInfo_t(u16 port);
            ~socketInfo_t(void);
    
        friend class socketPool_t;
    };

    socketInfo_t  * UDPArray[PORT_LEN]; //to save UDP refs
    
public:

    socketPool_t(void);
    ~socketPool_t(void);

    //gets a valid socket from the pool
    //if not exists, create it
    dgramSocket_t * get(u16 port = 0);
    dgramSocket_t * operator[](u16);

    //remove socket reference
    void release(dgramSocket_t *);
};

extern socketPool_t * socketPool;
#endif

