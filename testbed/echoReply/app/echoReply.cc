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
// $Id: echoReply.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/sched.hh>
#include <icf2/stdTask.hh>
#include <icf2/sockIO.hh>


class echoTask_t: public simpleTask_t 
{
    private:
        io_ref __myIO;

    public:
        echoTask_t(io_ref &io): simpleTask_t(io) {
            __myIO = io;
        }

        virtual void IOReady (io_ref &io) {
            char buffer[1024];
            inetAddr_t addr;

            io_t *iop = static_cast<io_t *>(io);
            dgramSocket_t *s = static_cast<dgramSocket_t *>(iop);
            s->recvFrom(addr, buffer, sizeof(buffer));
            io->write(buffer, sizeof(buffer));
        }

        virtual ~echoTask_t() {}
};


class myApp_t: public application_t {
    public:
        myApp_t(int &argc, argv_t &argv): application_t(argc, argv) {
 
            char *lPort=NULL;
            char *rPort=NULL;
            char *rHost=NULL;

            enum myOptions { optLocalPort, optRemotePort, optRemoteHost};
            optionDefList_t opt;
            appParamList_t *parList;

            opt
                << new optionDef_t("@localPort",  optLocalPort)
                << new optionDef_t("@remotePort", optRemotePort)
                << new optionDef_t("@remoteHost", optRemoteHost);

            parList= getOpt(opt, argc, argv);

            for( ; parList->len(); parList->behead()) {
                switch(parList->head()->parId) {
                    case optLocalPort:
                        lPort= strdup(parList->head()->parValue);
                        break;
                    case optRemotePort:
                        rPort= strdup(parList->head()->parValue);
                        break;
                    case optRemoteHost:
                        rHost= strdup(parList->head()->parValue);
                        break;
                    default:
                        fprintf(stderr,
                                "myApp_t::myApp_t():: bogus "
                                "parList received from "
                                "application_t::getOpt()\n");
                        abort();
                        break;
                }
            }

            if ((!lPort) || (!rHost) || (!rPort)) { 
                fprintf(stderr, 
                        "Usage: ./test -localPort <localPort> "
                        "-remotePort <remotePort> -remoteHost <remoteHost>\n"
                        " ... try again :)\n");
                exit(-1);
            }

            inetAddr_t addr1(NULL, lPort, serviceAddr_t::DGRAM);
            inetAddr_t addr2(rHost, rPort, serviceAddr_t::DGRAM);

            io_ref io = new dgramSocket_t(addr1, addr2);

            task_ref t = new echoTask_t(io);

            insertTask(t);

        }
};


int 
main(int argc, char *argv[])
{
     myApp_t myApp(argc, argv);

     myApp.run();

     return 0;
}
