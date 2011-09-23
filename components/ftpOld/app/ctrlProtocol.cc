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
// $Id: ctrlProtocol.cc 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/general.h>
#include <icf2/stdTask.hh>

#include "cftpApp.h"
#include "ctrlProtocol.h"
#include "fileTrans.h"
#include "fchannelMgr.h"

const char *ERROR = "ERROR\n";
const char *OK    = "OK\n";

#define APP static_cast<cftpApp_t*>(static_cast<cftp_Interface *>(this)->get_owner())


binding_t<cftp_InterfaceDef> cftp_methodBinding[]=
{
  
  { "cftp_create_channel",        &cftp_InterfaceDef::cftp_create_channel  },
  { "cftp_delete_channel",        &cftp_InterfaceDef::cftp_delete_channel  },

  { "cftp_bind",                  &cftp_InterfaceDef::cftp_bind            },
  { "cftp_unbind",                &cftp_InterfaceDef::cftp_unbind          },

  { "cftp_send",                  &cftp_InterfaceDef::cftp_send            },
  { "cftp_recv",                  &cftp_InterfaceDef::cftp_recv            },

  { "cftp_end",                   &cftp_InterfaceDef::cftp_end             },

  { "cftp_bw",                    &cftp_InterfaceDef::cftp_bw              },

  { "cftp_nop",                   &cftp_InterfaceDef::cftp_nop             },
  { "cftp_quit",                  &cftp_InterfaceDef::cftp_quit            },
  
  { NULL, NULL }

};

/////////////////////////////////////////////////////////////////////


const char *
cftp_InterfaceDef::cftp_create_channel(int argc, char **argv)
{

    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 2);
  
    u32 chid = atoi(argv[1]);
  
    fileTrans_t *channel = new fileTrans_t (chid);

    if (APP->theChanMgr->insertChan (channel))
        return OK; 

    // there was a problem inserting the channel
    NOTIFY("cftp_create_channel:: could not insert channel [%d]\n", chid);

    delete channel;
    return ERROR;
}



const char *
cftp_InterfaceDef::cftp_delete_channel(int argc, char **argv)
{
    u32   chid;
    static char retVal[512];
  
    INTER_VRFY_ARGC(retVal, 2);
  
    chid = atoi(argv[1]);
  
    if (APP->theChanMgr->delete_channel (chid)) 
        return OK; 

    NOTIFY("cftp_delete_channel:: there is not a channel [%d]\n", chid);

    return ERROR;
}


const char *
cftp_InterfaceDef::cftp_send (int argc, char **argv)
{
    static  char    retVal[512];  

    INTER_VRFY_ARGC(retVal, 4);

    u32 cid = atoi(argv[1]);
    char *path = argv [2];
    char *file = argv [3];

    fileTrans_t *channel = APP->theChanMgr->getChannel (cid);
    if (channel) {
        channel->end (); // limpiando restos
        channel->send (path, file); 
        return OK; 
    }

    NOTIFY("cftp_send:: there is not a channel [%d]\n", cid);

    return ERROR;
}



const char *
cftp_InterfaceDef::cftp_recv (int argc, char **argv)
{
    static  char    retVal[512];  

    INTER_VRFY_ARGC(retVal, 4);

    u32 cid = atoi(argv[1]);
    char *path = argv [2];
    char *file = argv [3];

    fileTrans_t *channel = APP->theChanMgr->getChannel (cid);
    if (channel) {
        channel->end ();
        channel->recv (path, file); 
        return OK; 
    }

    NOTIFY("cftp_recv:: there is not a channel [%d]\n", cid);

    return ERROR;
}



const char *
cftp_InterfaceDef::cftp_end (int argc, char **argv)
{
    static  char    retVal[512];  

    INTER_VRFY_ARGC(retVal, 2);

    u32 cid = atoi(argv[1]);

    fileTrans_t *channel = APP->theChanMgr->getChannel (cid);
    if (channel) {
        channel->end ();
        return OK; 
    }

    NOTIFY("cftp_end:: there is not a channel [%d]\n", cid);

    return ERROR;
}


/////////////////////////////////////////////////////////


const char *
cftp_InterfaceDef::cftp_bind  (int argc, char **argv)
{
    static  char    retVal[512];  

    INTER_VRFY_ARGC(retVal, 5);

    u32 bind_id = atoi(argv[1]);
    u32 cid     = atoi(argv[2]);
    const char *hostid= argv[3];
    const char *portid= argv[4];

    fileTrans_t *channel = APP->theChanMgr->getChannel (cid);
    if (channel && (rtpSession->ftpBind(cid, hostid, portid, bind_id)))
        return OK;

    NOTIFY("cftp_bind:: there is not a channel [%d]\n", cid);

    return ERROR;
}



const char *
cftp_InterfaceDef::cftp_unbind (int argc, char **argv)
{
    static  char    retVal[512];  

    INTER_VRFY_ARGC(retVal, 3);

    u32 cid = atoi(argv[1]);
    u32 bid = atoi(argv[2]);

    fileTrans_t *channel = APP->theChanMgr->getChannel(cid);
    if (channel && (rtpSession->ftpUnbind (cid, bid)))
        return OK;

    NOTIFY("cftp_unbind:: there is not a channel [%d]\n", cid);

    return ERROR;
}

////////////////////////////////////////////////////

const char *
cftp_InterfaceDef::cftp_bw (int argc, char **argv)
{
    static  char    retVal[512];  

    INTER_VRFY_ARGC(retVal, 3);

    u32 cid = atoi(argv[1]);
    int bw  = atoi(argv[2])*1000; // bandwidth en bps, me lo pasan en kbps

    NOTIFY("cftp_bw:: Bandwidth configuration: ChId=%u BW=%d kbps\n", cid, bw);

    fileTrans_t *channel = APP->theChanMgr->getChannel(cid);
    if (!channel)
        return  ERROR;

    channel->setBw(bw);
    
    return OK;
}



//////////////////////////////////////////

const char *
cftp_InterfaceDef::cftp_nop (int argc, char **argv)
{
    return OK;
}



const char *
cftp_InterfaceDef::cftp_quit (int argc, char **argv)
{
    APP->shutdown();
    return reinterpret_cast<char *>(-1);
}




//////////////////////////////////////////////////

