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
// $Id: mcastTalk.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/sockIO.hh>
#include <icf2/task.hh>
#include <icf2/sched.hh>
#include <icf2/stdTask.hh>

static void
usage(void)
{
    fprintf(stderr, "Usage error!!!\n"); // complain and exit
    exit(0);
}

class
talkTask_t: public virtual simpleTask_t
{
    private:
        io_ref __myConsole;
        io_ref __mySocket;

    public:
        talkTask_t(char *port) {
            inetAddr_t localAddr("224.31.10.72", port, serviceAddr_t::DGRAM);
            inetAddr_t remoteAddr("224.31.10.72", port, serviceAddr_t::DGRAM);

            dgramSocket_t *sock = new dgramSocket_t(localAddr, remoteAddr);

            __myConsole= new shellIO_t(0, 1);
            __mySocket = sock;

            sock->joinGroup(remoteAddr); //should not be required
        
            add_IO(__myConsole);
            add_IO(__mySocket);
        }

        virtual void IOReady(io_ref &activeIO) {
            char buffer[4096];

            int nread= activeIO->read(buffer, sizeof(buffer));

            if(nread<= 0) {
                fprintf(stderr, "dying my death... goodbye child!!!\n");
                *get_owner()-this;
                return;
            }


            if(activeIO== __myConsole) 
                __mySocket->write(buffer, nread);
            else if(activeIO== __mySocket) 
                __myConsole->write(buffer, nread);
            else 
                fprintf(stderr, "Bogus IOReady\n");
            
        }
};

int
main(int argc, char *argv[])
{
    sched_t s(128);

    if(argc!= 2) usage();

    int usePort= atoi(argv[1]);

    if(usePort< 0) usage();

    s<< new talkTask_t(argv[1]);

    s.run();

    return 0;
}
