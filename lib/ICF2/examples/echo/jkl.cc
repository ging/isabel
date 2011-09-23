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
// $Id: jkl.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/stdTask.hh>


class echoApp_t: public application_t
{
    public:
        struct jkl_t: public simpleTask_t {
            jkl_t(io_ref &io): simpleTask_t(io) {
                printf("%ld\n", getpid());
            };

            virtual void IOReady(io_ref &io) {
                char b[65536];
                int  n = io->read(b, sizeof(b)); // Lee datos

                printf("%ld\n", getpid());       // Imprime PID

                if(n<= 0)
                    *get_owner()-this;
            };
        };
    
        const char *echoPort;
    
        echoApp_t(int &argc, argv_t &argv): application_t(argc, argv) {
            enum myOptions { optPort, optDummy };
            optionDefList_t opt;
            appParamList_t *parList;

            opt
                << new optionDef_t("@port", optPort)
                << new optionDef_t("dummy", optDummy);

            parList= getOpt(opt, argc, argv);

            echoPort= "4545";  // default value

            for( ; parList->len(); parList->behead()) {
                switch(parList->head()->parId) {
                case optPort:
                    echoPort= strdup(parList->head()->parValue);
                    break;
                case optDummy:
                    printf("Dummy parameter!\n");
                    break;
                default:
                    fprintf(stderr, 
                            "echoApp_t::echoApp_t():: bogus parList "
                            "received from application_t::getOpt()\n");
                    abort();
                }
            }

            // creamos la tarea y la insertamos
            inetAddr_t addr(NULL, echoPort, serviceAddr_t::STREAM);
            streamSocket_t *streamSock= new streamSocket_t(addr);
            tcpServer_t<jkl_t> *streamTask
                  = new tcpServer_t<jkl_t>(*streamSock);

            *this<< streamTask;

            fork();
       };
};


main(int argc, char *argv[])
{
    echoApp_t theApp(argc, argv);
    
    theApp.run();
    
    return 0; // keeps compiler happy
}
