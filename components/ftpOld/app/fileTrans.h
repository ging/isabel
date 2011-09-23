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
// $Id: fileTrans.h 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#ifndef	__fileTrans_H__
#define	__fileTrans_H__

#include <icf2/stdTask.hh>
#include <icf2/general.h>

#include "packet.h"
//#include "bindMgr.h"
#include "protMgr.h"



typedef enum
{
    IDLE_STATE, 
    SENDING_STATE,   /* Sending a file */
    RECEIVING_STATE  /* Receiving a file */
} fstateVal_e;




class fileTrans_t: public virtual item_t//bindMgr 
{
public:
    
    fileTrans_t (u32);
    ~fileTrans_t ();
    
    /// interface for control shell
    
    bool send (char *, char *);
    bool recv (char *, char *);
    bool end ();
    
    //////////////
    
    u32   id;
    
    char * path;
    char *file;
    
    protState_ref prot_mgr;
        
    
    /* Set bandwidth
     * 
     * bw = bandwidth to traffic shaping function
     */
    bool setBw(int bw);
    
    virtual const char *className() const { return "fileTrans_t"; }
    
private:
    
    int  timeOut;  // timeOut, traffic shaping function
    fstateVal_e    g_state;

};


///////////////////////////

#endif
