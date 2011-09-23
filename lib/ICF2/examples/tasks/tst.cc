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
// $Id: tst.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include <icf2/general.h>
#include <icf2/ql.hh>
#include <icf2/io.hh>
#include <icf2/sockIO.hh>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>



struct cookie_t: public simpleTask_t
{
    cookie_t(void): simpleTask_t(1000000) {;};

    virtual void heartBeat(void) { printf("cookie! "); fflush(stdout);};
	
    virtual void timeOut(const alarm_t *al) {
        char *bobada[]= {
            "glup!",
            "glob!",
            "argh!",
            "that's all folks",
            NULL
        };
	printf("%s\n", al->args?al->args:"hi!\n");
	set_alarm(1500000, bobada[random()%5]);
    };
};


struct echoServer_t: public simpleTask_t
{
    echoServer_t(io_ref &io): simpleTask_t(io) { }; 

    virtual void IOReady(io_ref &io) {
        char b[65536];
        int n= io->read(b, sizeof(b));

        if(n> 0)
            io->write(b, n);
        else 
            *get_owner()-this;
    };
};


main()
{
    sched_t s(128);
    cookie_t t;
    // simpleTask_t t2(5000000);

    inetAddr_t addr1(NULL, "14545", serviceAddr_t::DGRAM);
    inetAddr_t addr2(NULL, "14545", serviceAddr_t::STREAM);

    io_ref sock1 = new dgramSocket_t (addr1);
    streamSocket_t *sock2 = new streamSocket_t(addr2);

    if (sock2->listen(addr2)) {
       fprintf(stderr, "Error tcp server socket:: System halted!\n");
       abort();
    }

    echoServer_t echo1(sock1);
    tcpServer_t<echoServer_t> echo2(*sock2);

    //s<<&t<<&t2<<&echo1<<&echo2;
    s<<&t<<&echo1<<&echo2;

    t.set_alarm(1500000, NULL);
    //t2.set_alarm(1500000, NULL);

    s.run();

    return 0;
}

