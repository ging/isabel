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
// $Id: protMgr.h 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#ifndef	__protMgr_H__
#define	__protMgr_H__

#include <icf2/stdTask.hh>
#include <icf2/general.h>

#include "packet.h"
#include "cftpRTPSession.h"

//
// Generic task. Send and Receive tasks inherit from this one.
//
class protState: public simpleTask_t {

public:
    // protState constructor and destructor
    protState ();
    ~protState ();
    
    // initialize task
    virtual  void  start() = 0;
    
    // receive packets
    virtual  bool receive (appData_t  *) = 0;

    virtual void setTimeOut(int tout) = 0;

    appData_t  * packets; /* packets data */
    
    int  npackets;  /* number of packets to store a file  */
    int  index;     /* pkt index in the file              */
    
    u32 channelId;  /* channelId                          */
    
    virtual const char *className () const { return "protState";}
    
    friend class smartReference_t<protState>;
};


typedef smartReference_t<protState> protState_ref;

//----------------------------------------------------------------

//
// protS_Send - task to send a file
//
class protS_Send : public protState {
public:
    // protS_Send constructor
    protS_Send (char *fname, u32 chanId, char * f);
    
    // protS_Send destructor
    virtual ~protS_Send ();
    
    // Start to send a file
    void start();
    
    // If sender receives a packet... delete it
    bool receive (appData_t  *);

    void setTimeOut(int tout);
    
    // Timeout alarm, send next packet of file
    virtual void timeOut(const alarm_ref &);
    
    virtual const char *className () const { return "protS_Send";}

private:
    
    // read file and fill the packets[] data
    virtual  bool  read_file (char *fullName, u32 chanId, char *fname); 
    
    // Send a packet an activate alarm to send next pkt
    void __sendingPkt();
    
    // when I must send the next packet.
    unsigned long nextStart;        
    
    int timeout;
    
    alarm_ref   alr; /* alarm */
};


//----------------------------------------------------------------

//
// protS_Recv  --  task to receive file
//
class protS_Recv : public protState 
{
public:
    
    // protS_Recv constructor
    protS_Recv  (char * f, u32 chanId);
    
    // protS_Recv destructor
    virtual ~protS_Recv ();
    
    void start();
    
    // receive data
    bool receive (appData_t  *); 

    void setTimeOut(int tout);
    
    virtual const char *className () const { return "protS_Recv";}

private:
    
    // write file from the packets[] data
    virtual  bool  write_file (char *);
    
    void dir_check (char * file);

    bool * done; 
    
    char* fname;   /* file name  */
};


#endif
