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
// $Id: lostPktsTask.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <icf2/strOps.hh>
#include <icf2/stdTask.hh>
#include "lostPktsTask.hh"


netTask_t *initLostTask(int argc, char **argv) {

     if (argc!=3) {
         fprintf(stderr, "initLostTask:: ERROR incorrect number of arguments\n"
                 "   Usage: lostTask -prob <LostPktProb %% 100 > \n");
         return NULL;
     }

     enum myOptions {
         optProb
     };

     optionDefList_t opt;
     appParamList_t *parList;

     opt
        << new optionDef_t("@prob",  optProb);

     parList= getOpt(opt, argc, argv);

     int prob=0;
     for( ; parList->len(); parList->behead()) {
         switch(parList->head()->parId) {
             case optProb:
                  prob= atoi(strdup(parList->head()->parValue));
                  break;
             default:
                 fprintf(stderr, "initLostTask:: ERROR unknown argument\n"
                         "   Usage: lostTask -prob <LostPktProb %% 100>");
                 return NULL;
         }
     }

     if ((prob<=0) || (prob>=100)) {
           fprintf(stderr, "initLostTask:: ERROR probability should be "
                   "100 module\n");
           return NULL;
     }
     return new lostPktsTask_t(prob);
};

netsimInit_t lostPktsTask_t::netsimInit("lostPkts", initLostTask);

void 
lostPktsTask_t::recvPkt(pktBuff_ref &pktBuff, int pktLen) {

    int random = 1+(int) (100.0*rand()/(RAND_MAX+1.0));
    if(random >= __lostProb) 
        sendPkt(pktBuff, pktLen);
    else
        debugMsg(dbg_App_Normal, "recvPkt", "TIRO pkt random=%d!\n", random);

};

