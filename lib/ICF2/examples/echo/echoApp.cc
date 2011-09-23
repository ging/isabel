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
// $Id: echoApp.cc 20206 2010-04-08 10:55:00Z gabriel $
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
        struct echoTask_t: public simpleTask_t {
            echoTask_t(io_ref &io): simpleTask_t(io) {
            };

            virtual void IOReady(io_ref &io) {
                    char  b[65536];
                    int	  n = io->read(b, sizeof(b));   // lee datos

                    debugMsg(dbg_App_Normal, "IOReady", "activity");
	    
                    if(n> 0)
                        io->write(b, n);                // escribe datos
                    else 
                        *get_owner()-this;
            };

            virtual const char *className(void) const { return "echoTask_t"; };
        };

    private:
        const char *echoPort;
        tcpServer_t<echoTask_t> *streamTask;

    public:
        echoApp_t(int &argc, argv_t &argv): application_t(argc, argv) {
	    enum myOptions { optPort, optDummy, optHost};
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
                                "echoApp_t::echoApp_t():: bogus "
                                "parList received from "
                                "application_t::getOpt()\n");
		        abort();
                        break;
	        }
	    }
	
	    // servidor de eco UDP
	    inetAddr_t dgramAddr(NULL, echoPort, serviceAddr_t::DGRAM);
	    io_ref dgramSock= new dgramSocket_t(dgramAddr);
	    task_ref dgramTask= new echoTask_t(dgramSock);
	
	    // servidor de eco TCP
	    inetAddr_t streamAddr(NULL, echoPort, serviceAddr_t::STREAM);
	    streamSocket_t *streamSock= new streamSocket_t(streamAddr);

            if (streamSock->listen(streamAddr)<0) {
                fprintf(stderr, "Error tcp server socket:: System halted!\n");
                abort();
             }


	    streamTask= new tcpServer_t<echoTask_t>(*streamSock);
	
	    *this<< dgramTask<< streamTask;
        };

        virtual const char *className(void) const { return "echoApp_t"; };
};


main(int argc, char *argv[])
{
    echoApp_t theApp(argc, argv);
    
    theApp.run();
    
    return 0; // keeps compiler happy
}
