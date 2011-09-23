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
// $Id: echoClient.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include <icf2/general.h>
#include <icf2/io.hh>
#include <icf2/sockIO.hh>
#include <icf2/task.hh>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>



struct echoClient_t: public simpleTask_t
{
    io_ref  remote;
    io_ref  stdInput;
    io_ref  stdOutput;

    echoClient_t(io_ref &io): 
        remote(io), stdInput(new io_t(0)), stdOutput(new io_t(1)) // ,
        // simpleTask_t(IOList_t()<< io<< stdInput)
    {
        add_IO(io);
        add_IO(stdInput);
    };


    virtual void IOReady(io_ref &io) {
	char  b[65536];
	int   n= io->read(b, sizeof(b));   // Lee datos
	if(n> 0) {
	    if(io== stdInput)                // datos de entrada estandar
		remote->write(b, n);         // manda a servidor de eco
	    else                             // datos del servidor de eco
	        stdOutput->write(b, n);      // escribe en salida estandar
            
	} else 
	    *get_owner()-this;
    };


    virtual const char *className(void) const { return "echoClient_t"; };
};


main(int argc, char *argv[])
{
    sched_t	s(128);

#if 1 
    // Socket TCP
    inetAddr_t	addr1(NULL, NULL, serviceAddr_t::STREAM);
    inetAddr_t	addr2(argv[1], argv[2], serviceAddr_t::STREAM);
    streamSocket_t *sock= new streamSocket_t(addr1, socketIO_t::CLIENT_SOCK);

    if (sock->connect(addr2) < 0) {
        fprintf(stderr, "Not server listening!\n");
        abort();
    }
    task_ref echo = new echoClient_t(*sock);
#else

    // Socket UDP
    inetAddr_t addr1(NULL, NULL, serviceAddr_t::DGRAM);
    inetAddr_t addr2(argv[1], argv[2], serviceAddr_t::DGRAM);
    io_ref sock = new dgramSocket_t(addr1, addr2);

    // Cliente de eco UDP/TCP
    task_ref echo = new echoClient_t(sock);
#endif

    s << echo;

    s.run();

    return 0;
}

