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
// $Id: netsimApp.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <icf2/general.h>
#include <icf2/task.hh>
#include <icf2/sched.hh>
#include <icf2/sockIO.hh>

#include "netsimApp.hh"
#include "outputDriver.hh"
#include "inputDriver.hh"


// netsimInit_t *netsimInit_t::first=NULL;


netsimApp_t::netsimApp_t(int &argc, argv_t &argv): application_t(argc, argv)
{
    char *inPort=NULL;
    char *outPort=NULL;
    char *outHost=NULL;

    enum myOptions {
        optSim,
        optInPort,
        optOutPort,
        optOutHost
    };

    optionDefList_t opt;
    appParamList_t *parList;

    opt
        << new optionDef_t("@sim",  optSim)
        << new optionDef_t("@inPort",  optInPort)
        << new optionDef_t("@outPort", optOutPort)
        << new optionDef_t("@outHost", optOutHost);

    parList= getOpt(opt, argc, argv);

    netTask_t *first=NULL;
    netTask_t *last=NULL;

    for( ; parList->len(); parList->behead()) {
        switch(parList->head()->parId) {
            case optInPort:
                inPort= strdup(parList->head()->parValue);
                break;

            case optOutHost:
                outHost= strdup(parList->head()->parValue);
                break;

            case optOutPort:
                outPort= strdup(parList->head()->parValue);
                break;

            case optSim: {
                netTask_t *th=__simulation(parList->head()->parValue);
                if (th) {
                    th->tlRun();
                    if (!first) {
                        first=th;
                        last =th;
                    } else {
                        last->__setNextTask(th);
                        last=th;
                    }
                    insertTask(th);
                }
            } break;

            default:
                // se deberia lanzar una excepcion
                fprintf(stderr, "noisyApp_t::noisyApp_t:: bogus parList "
                        "received from application_t::getOpt()");
                exit(-1);
                break;
        }
    }

    if ((!outPort) || (!outHost) || (!inPort)) {
           fprintf(stderr, "ERROR:: Usage:\n"
                   "   netsimApp -inPort <inPort> -outHost <dstHostName> "
                   "-outPort <dstPort>\n");
           exit(-1);
    }


    inetAddr_t outAddr1((char*)NULL, (char*)NULL, serviceAddr_t::DGRAM);
    inetAddr_t outAddr2(outHost, outPort, serviceAddr_t::DGRAM);
    io_ref outSock = new dgramSocket_t(outAddr1, outAddr2);
    threadedTask_t *outTask = new outputDriver_t(outSock);
    outTask->tlRun();
    task_ref oTask = outTask;
    this->insertTask(oTask);

    inetAddr_t inAddr(NULL, inPort, serviceAddr_t::DGRAM);
    io_ref inSock = new dgramSocket_t(inAddr);

    threadedTask_t *next=outTask;
    if (first) {
        next = static_cast<threadedTask_t *>(first);
        last->__setNextTask(outTask);
    }

    threadedTask_t *inTask = new inputDriver_t(inSock, next);

    inTask->tlRun();
    task_ref iTask = inTask;
    this->insertTask(iTask);

};


netTask_t *
netsimApp_t::__simulation(char *params) {
     netTask_t *th=NULL;
     int simArgc=0;
     
     debugMsg(dbg_App_Normal, "__simulation", "Simulating..\n");

     char **simArgv= buildArgv(params, simArgc);
     initFunction_t f = netsimInit_t::getInitFunct(simArgv[0]);
     if (f)
         th = (*f)(simArgc, simArgv);
     else
         fprintf(stderr, "netsimApp_t::__simulation:: There is not "
                 "a simArgv[0] function defined!\n");

     freeArgv(simArgc, simArgv);
     return th;
}


netsimApp_t::~netsimApp_t() {
    debugMsg( dbg_App_Paranoic,
              "~netsimApp_t",
              "Destroying noisyApp_t"
            );
}


int
main(int argc, char *argv[]) {

   netsimApp_t theApp(argc, argv);
   theApp.run();

   return 1;
}
