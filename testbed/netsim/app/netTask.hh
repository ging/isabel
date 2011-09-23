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
// $Id: netTask.hh 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __NET__TASK__
#define __NET__TASK__

#include <string.h>
#include "messages.hh"

class netTask_t;

typedef netTask_t * (*initFunction_t) (int, char **);

class netsimInit_t: public virtual item_t {
    private:
        char                *__name;
        initFunction_t       __initFunct;
        netsimInit_t        *__next;

    public:
        static netsimInit_t *first;

        netsimInit_t(char *name, initFunction_t funct)
        :__name(name), __initFunct(funct)
        {
            debugMsg(dbg_App_Normal, "netsimInit_t",
                     "Registrando [%s]\n", name);
            __next=first;
            first=this; 
        }

        static initFunction_t getInitFunct(char *n) {
            netsimInit_t *node=first;
            while (node) {
                if (strcmp(node->__name,n)==0) {
                    return node->__initFunct;
                }
                node= node->__next;
            }
            return NULL;
        }

        virtual const char *className() const { return "netsimInit_t";}
};


class netTask_t: public threadedTask_t 
{
    private:
        threadedTask_t *__nextTask;

        void __setNextTask(threadedTask_t *th) { __nextTask=th;}

    public: 
        netTask_t(){}

        virtual bool dispatchMsg(tlMsg_t *tlMsg);
        virtual void sendPkt(pktBuff_ref &pktBuff, int pktLen);

        virtual void recvPkt(pktBuff_ref &pktBuff, int pktLen)=0;

        virtual const char *className() const { return "netTask_t";}
   friend class netsimApp_t;

};

#endif
