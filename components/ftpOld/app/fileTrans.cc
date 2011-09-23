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
// $Id: fileTrans.cc 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include "fileTrans.h"
#include "cftpApp.h"
#include "fchannelMgr.h"
#include <icf2/sockIO.hh>


#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>


extern    cftpApp_t * theApp;


fileTrans_t::fileTrans_t(u32 cid) {

    id       = cid;
    g_state  = IDLE_STATE;
    path     = NULL;
    file     = NULL;
    timeOut  = 30000;

    prot_mgr = NULL;
    rtpSession->assignSender(id);
}    


fileTrans_t::~fileTrans_t() {

    end();
}


bool 
fileTrans_t::send (char *pth, char* fl) {

    debugMsg(dbg_App_Normal, "send", 
             "sending path=%s, fileName=%s\n", pth, fl);
    char fullname[256];

    sprintf (fullname, "%s/%s", pth, fl);
  
    if (g_state != IDLE_STATE)
        end ();

    path   = strdup (pth);
    file   = strdup (fl);

    g_state  = SENDING_STATE;

    debugMsg(dbg_App_Normal, "send", "creating sending task\n");

    protS_Send *ps = new protS_Send (fullname, id, fl);
    prot_mgr = ps;
    task_ref t = static_cast<abstractTask_t *>(
                       static_cast<protState *>(prot_mgr));

    assert (t.isValid());

    debugMsg(dbg_App_Normal, "send", "inserting sending task in sched\n");

    *theApp << t;

    ps->setTimeOut(timeOut);  
    prot_mgr->start();

    return true;
}


bool 
fileTrans_t::recv (char * pth, char* fl) {
    char fullname[256];

    sprintf (fullname, "%s/%s", pth,fl);

    if (g_state != IDLE_STATE)
        end ();

    path   = strdup (pth);
    file   = strdup (fl);

    g_state  = RECEIVING_STATE;

    prot_mgr = new protS_Recv (fullname, id);
    task_ref t = static_cast<abstractTask_t *>(
                       static_cast<protState *>(prot_mgr));

    *theApp << t;
  
    prot_mgr->start();
    
    return true;
}



bool 
fileTrans_t::end () {
    if (path) {
        free (path);  // adquirido con strdup
        path=NULL;
    }

    if (file) {
       free (file);  // adquirido con strdup
       file =NULL;
    }

    if (prot_mgr.isValid()) {
	protState *psp = static_cast <protState *>(prot_mgr);
	abstractTask_t *stp = static_cast <abstractTask_t *>(psp);
	theApp->removeTask(stp);
    }

    g_state = IDLE_STATE;
    return false;
}


bool fileTrans_t::setBw(int bw) {

    int pktSize = sizeof(appData_t);
    bw = bw/8; // en octetos
    timeOut= (int)(pktSize*(1000000.0/bw));

    if (prot_mgr.isValid()) {
       prot_mgr->setTimeOut(timeOut);  
    }

    return true;
}

