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
// $Id: jitterTask.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <icf2/strOps.hh>
#include <icf2/stdTask.hh>
#include "jitterTask.hh"


netTask_t *initJitterTask(int argc, char **argv) {

     if (argc!=7) {
         fprintf(stderr, "initLostTask:: ERROR incorrect number of arguments\n"
                 "   Usage: lostTask -prob <LostPktProb %% 100 > "
                 "-jitter <jitter>\n");
         return NULL;
     }

     enum myOptions {
         optMinBurst,
         optMaxBurst,
         optProb
     };

     optionDefList_t opt;
     appParamList_t *parList;

     opt
        << new optionDef_t("@pron",      optProb)
        << new optionDef_t("@minBurst",  optMinBurst)
        << new optionDef_t("@maxBurst",  optMaxBurst);

     parList= getOpt(opt, argc, argv);

     int minBurst=0;
     int maxBurst=0;
     int prob=0;
     for( ; parList->len(); parList->behead()) {
         switch(parList->head()->parId) {
             case optMinBurst:
                  minBurst= atoi(strdup(parList->head()->parValue));
                  break;

             case optMaxBurst:
                  maxBurst= atoi(strdup(parList->head()->parValue));
                  break;

             case optProb:
                  prob= atoi(strdup(parList->head()->parValue));
                  break;

             default:
                 fprintf(stderr, "initJitterTask:: ERROR unknown argument\n"
                         "   Usage: jitter -prob <LostPktProb %% 100> "
                         "-minBurst <minBurstSize> -maxBurst <maxBurstSize>");
                 return NULL;
         }
     }

     if ((minBurst<=0) || (maxBurst< minBurst) || (prob <0) || (prob > 100)) {
           fprintf(stderr, "initJitterTask:: ERROR burst size range should be "
                   "minBurst >0 and minBurst <=maxBurst and prob "
                   "should be 100 module!!\n");
           return NULL;
     }

     return new jitterTask_t(minBurst, maxBurst, prob);
};

netsimInit_t jitterTask_t::netsimInit("jitter", initJitterTask);

void 
jitterTask_t::recvPkt(pktBuff_ref &pktBuff, int pktLen) {

    pktNode_ref node = new pktNode_t(pktBuff, pktLen);
    pktNodeList.insert(node);
    
    __dispatchPktNode();
    
};

void 
jitterTask_t::__dispatchPktNode() {
    
    if (__burstSize!=0) {
        // Acumulando pkts para una rafaga
        if (pktNodeList.len()== (unsigned)__burstSize) {
            debugMsg( dbg_App_Normal, 
                      "__dispatchPktNode", 
                      "Enviando pkt de rafaga\n"
                    );
            pktNode_ref node =NULL;
            while (pktNodeList.len()) {
                node = pktNodeList.head();
                sendPkt(node->pBuff, node->pLen);
                pktNodeList.behead();
            }
            __burstSize=0;
        }  
        return;
    }

    // Se produce rafaga con probabilidad __prob
    int random = 1+(int) (100.0*rand()/(RAND_MAX+1.0));
    if(random<=__prob) {
        debugMsg( dbg_App_Normal, 
                  "__dispatchPktNode", 
                  "Enviando pkt\n"
                );
        pktNode_ref node = pktNodeList.head();
        sendPkt(node->pBuff, node->pLen);
        pktNodeList.behead();

    } else {
        float range=(float)__maxBurst-__minBurst+1.0;
        __burstSize= __minBurst + (int) (range*rand()/(RAND_MAX+1.0)); 

        debugMsg( dbg_App_Normal, 
                  "__dispatchPktNode", 
                  "Rafaga=%d\n", 
                  __burstSize
                );
    }
}

