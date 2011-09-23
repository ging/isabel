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
// $Id: echo.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>

#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>



class cookie_t: public simpleTask_t
{
    public:
        cookie_t(void): simpleTask_t(1000000) {
            // Every period=1000000 micro seconds = 1 sec
            // heartBeat method is invoked
        }

        virtual void heartBeat(void) {
            printf("cookie! "); fflush(stdout);
        }

        char const *className(void) const { return "cookie_t";}
};


class echoServer_t: public simpleTask_t
{
    public:
        echoServer_t(io_ref &io): simpleTask_t(io) {}

        virtual void IOReady(io_ref &io) {
            char b[65536];
            int n= io->read(b, sizeof(b));

            printf("IOReady:: activity\n");

            if(n> 0)
                io->write(b, n);
            else 
                *get_owner()-this;
        }

        char const *className(void) const { return "echoServer_t";}
};


int
main(void)
{
    sched_t  s(128);
    cookie_t t;

    // UDP echo Server
    inetAddr_t  addr1(NULL, "32000", serviceAddr_t::DGRAM);
    io_ref      sock1 = new dgramSocket_t(addr1);
    task_ref    echo1 = new echoServer_t (sock1); 

    // TCP echo Server
    inetAddr_t      addr2(NULL, "32000", serviceAddr_t::STREAM);
    streamSocket_t *sock2 = new streamSocket_t(addr2);

    if (sock2->listen(addr2) <0) {
       fprintf(stderr, "Error tcp server socket:: System halted!\n");
       abort();
    }

    task_ref        echo2 = new tcpServer_t<echoServer_t>(*sock2);

    s << &t << echo1 << echo2;

    s.run();

    return 0;
}

