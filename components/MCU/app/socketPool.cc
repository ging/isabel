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
// $Id: socketPool.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "socketPool.h"

socketPool_t * socketPool = new socketPool_t();
            
socketPool_t::socketInfo_t::socketInfo_t(u16 port)
{
    this->port = port;
    refCount   = 1;

    char portStr[6];
    sprintf(portStr,"%d",port);    
	dgramSocket_t * socket = NULL;
	if (getenv("MCU_USE_ONLY_IPv4"))
	{
		inetAddr_t addr("0.0.0.0",portStr,SOCK_DGRAM);
		socket = new dgramSocket_t(addr);
	}else{
		inetAddr_t addr(NULL,portStr,SOCK_DGRAM);
		socket = new dgramSocket_t(addr);
	}
    io_t * ioxx = static_cast<io_t *>(socket);
    io = static_cast<io_ref>(ioxx);

	//set big receiver buffer
	int good_res;
    int res_in;
    int res_out;
#ifdef __BUILD_FOR_LINUX
    socklen_t res_len = sizeof(int);
#else
    int res_len= sizeof(int);
#endif

    getsockopt(io->sysHandle(), SOL_SOCKET, SO_RCVBUF, (char *)&good_res, &res_len);
    res_in= res_out= good_res;

    while((res_in == res_out) && (res_in<= 2048 KBYTES))
    {
        res_in+= 4 KBYTES;
        res_len= sizeof(int);
        setsockopt(io->sysHandle(), SOL_SOCKET, SO_RCVBUF, (char *)&res_in, res_len);
        getsockopt(io->sysHandle(), SOL_SOCKET, SO_RCVBUF, (char *)&res_out, &res_len);

        if(res_out== res_in)
            good_res= res_in;
    }

    res_len= sizeof(int);
    setsockopt(io->sysHandle(), SOL_SOCKET, SO_RCVBUF, (char *)&good_res, res_len);
}

socketPool_t::socketInfo_t::~socketInfo_t(void)
{

}
    
socketPool_t::socketPool_t(void)
{
    memset(UDPArray,0,PORT_LEN*sizeof(socketInfo_t*));
}

socketPool_t::~socketPool_t(void)
{
    for (u16 i = 0;i<PORT_LEN;i++)
    {
        if (UDPArray[i])
        {
            delete UDPArray[i];
            UDPArray[i] = NULL;
        }
    }
}

//gets a valid socket from the pool
//if not exists, create it
dgramSocket_t * 
socketPool_t::get(u16 port)
{
    dgramSocket_t * socket = NULL;

    if (UDPArray[port])
    {
        UDPArray[port]->refCount++;
        io_t * ioxx = static_cast<io_t *>(UDPArray[port]->io);
        socket = static_cast<dgramSocket_t*>(ioxx);
    }

    if (!socket)
    {
        UDPArray[port] = new socketInfo_t(port);
        io_t * ioxx = static_cast<io_t *>(UDPArray[port]->io);
        socket = static_cast<dgramSocket_t*>(ioxx);
    }
    return socket;
}

dgramSocket_t * 
socketPool_t::operator[](u16 port)
{
    return get(port);
}
//remove socket reference
void 
socketPool_t::release(dgramSocket_t * socket)
{
    for (u16 i = 0; i< PORT_LEN; i++)
    {
        if (UDPArray[i])
        {
            if ((void *)UDPArray[i]->io == (void *)socket)
            {
                UDPArray[i]->refCount--;
                if (!UDPArray[i]->refCount)
                {
                    delete UDPArray[i];
                    UDPArray[i] = NULL;
                }
            }
        }
    }
}
