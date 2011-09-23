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
// $Id: testThreadedLoops.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/general.h>
#include <icf2/threadedLoop.hh>
#include <icf2/item.hh>

class fibMsg_t: public tlMsg_t
{
public:
    static unsigned const fibMsgId= 0x12345678;
    unsigned fib2eval;

    fibMsg_t(int f): tlMsg_t(fibMsgId), fib2eval(f) {};
};


class fibWorker_t: public virtual threadedLoop_t
{
    char *id;

    virtual bool dispatchMsg(tlMsg_t *m) {
        debugMsg(dbg_App_Normal, "dispatchMsg", "Dispatching...");
        switch(m->msgKind) {
            case fibMsg_t::fibMsgId:
            {
                fibMsg_t *mf= static_cast<fibMsg_t*>(m);

                printf("%s fib(%d)== %d\n",
                       id,
                       mf->fib2eval,
                       fib(mf->fib2eval)
                      );

                return true;
            } break;

            default:
                return threadedLoop_t::dispatchMsg(m);
        }
    }

    unsigned fib(unsigned f) { return f<2?f:fib(f-1)+fib(f-2); }

public:
    fibWorker_t(char *s): id(s) {}
};


main()
{
    fibWorker_t f1("worker 1");
    fibWorker_t f2("worker 2");
    threadedLoop_ref f3= new fibWorker_t("worker 3");
    threadedLoop_ref f4= new fibWorker_t("worker 4");

    f1.tlRun();
    f2.tlRun();
    f3->tlRun();
    f4->tlRun();

    f1.tlPostMsg(new fibMsg_t(40));
    f2.tlPostMsg(new fibMsg_t(35));
    f3->tlPostMsg(new fibMsg_t(30));
    f4->tlPostMsg(new fibMsg_t(25));

    f1.tlPostMsg(new fibMsg_t(25));
    f2.tlPostMsg(new fibMsg_t(30));
    f3->tlPostMsg(new fibMsg_t(35));
    f4->tlPostMsg(new fibMsg_t(40));

    sleep(1); //bug, tlRun should wait until the thread is fired
}

