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
// $Id: rttTest.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <unistd.h>

#include <icf2/general.h>
#include <icf2/sched.hh>
#include <icf2/stdTask.hh>
#include <icf2/sockIO.hh>


//
// Tomando el codigo de sanchez...
//

static char key[] = "Hola caracola";


// Estructura del paquete que se envia

//    ------------------------------------------------------------
//   | Palabra magica | contador | tiempo |  vacio hasta pktSize char |
//    ------------------------------------------------------------

class sendingTask_t: public simpleTask_t 
{
    private:
        int contador;
        int __pktSize;
        io_ref __myIO;
        struct timeval lastTime;

    public:
        sendingTask_t(io_ref &io, int delay, int pktSize)
        : simpleTask_t(io, delay) {
            __myIO = io;
            __pktSize = pktSize;
            contador = 0;
            gettimeofday(&lastTime, NULL);
        }

        virtual void IOReady (io_ref &io) {
            struct timeval tv, tvRec;
            int index;
            unsigned long long diff;
            char message[256] = "";
            char buffer[__pktSize+512];
            inetAddr_t addr;

            gettimeofday(&tv, NULL);

            io_t *iop = static_cast<io_t *>(io);
            dgramSocket_t *s = static_cast<dgramSocket_t *>(iop);
            s->recvFrom(addr, buffer, sizeof(buffer));

            memcpy(message, buffer, sizeof(key));
            memcpy(&index, buffer+sizeof(key), sizeof(index));
            memcpy(&tvRec, buffer+sizeof(key)+sizeof(index), sizeof(tvRec));
            diff = (tv.tv_sec*1000000+tv.tv_usec) - 
                   ( tvRec.tv_sec*1000000+tvRec.tv_usec);
            printf("------------------------------------------------------\n");
            printf("Receiving seq=%d\t", index);
            printf("%ld:%ld -> %ld:%ld\n", 
                    tv.tv_sec, tv.tv_usec, tvRec.tv_sec, tvRec.tv_usec);
            printf("   ELAPSED=%llu ms  FROM_SOURCE=%s\n", 
                   diff/1000, addr.getStrValue());
            printf("------------------------------------------------------\n");
            fflush(stdout);
        }



        virtual void heartBeat () {
            unsigned char buffer[__pktSize];
            unsigned long long diff;
            struct timeval tv;

            memcpy(buffer, key, sizeof(key));
            memcpy(buffer+sizeof(key), &contador, sizeof(contador));
            gettimeofday(&tv, NULL);
            memcpy(buffer+sizeof(key)+sizeof(contador), &tv, sizeof(tv));
            if (__myIO->write(buffer, __pktSize) < 0 ) {
                fprintf(stderr, "error sending\n");
            }
            diff = (tv.tv_sec*1000000+tv.tv_usec) - 
                   ( lastTime.tv_sec*1000000+lastTime.tv_usec);
            printf("Sending seq=%d. Period=%llu ms\n", 
                   contador, diff/1001);

            memcpy(&lastTime, &tv, sizeof(lastTime));
            contador++;
        }
   
        virtual ~sendingTask_t() {}
};


class myApp_t: public application_t {
    public:
        myApp_t(int &argc, argv_t &argv): application_t(argc, argv) {
 
            char *lPort=NULL;
            char *rPort=NULL;
            char *rHost=NULL;
            int delay=0;
            int pktSize =0;

            enum myOptions { optLocalPort, optRemotePort, 
                             optRemoteHost, optDelay, optPktSize};
            optionDefList_t opt;
            appParamList_t *parList;

            opt
                << new optionDef_t("@localPort",  optLocalPort)
                << new optionDef_t("@remotePort", optRemotePort)
                << new optionDef_t("@remoteHost", optRemoteHost)
                << new optionDef_t("@pktSize",    optPktSize)
                << new optionDef_t("@delay",      optDelay);

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
                    case optDelay:
                        delay= atoi(parList->head()->parValue);
                        break;
                    case optPktSize:
                        pktSize= atoi(parList->head()->parValue);
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

            if ((!lPort) || (!rHost) || (!rPort) || 
                (delay==0) || (pktSize==0)) 
            { 
                fprintf(stderr, 
                        "Usage: ./rttTest -localPort <localPort> "
                        "-remotePort <remotePort> -remoteHost <remoteHost> "
                        "-delay <delay> -pktSize <pktSize>\n"
                        " ... try again :)\n");
                exit(-1);
            }

            inetAddr_t addr1(NULL, lPort, serviceAddr_t::DGRAM);
            inetAddr_t addr2(rHost, rPort, serviceAddr_t::DGRAM);

            io_ref io = new dgramSocket_t(addr1, addr2);

            task_ref t = new sendingTask_t(io, delay, pktSize);

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
