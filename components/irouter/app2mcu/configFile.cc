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
// $Id: configFile.cc 6628 2005-05-10 12:43:36Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/hostArch.h>

#include "transApp.hh"
#include "configFile.hh"

#define CIID_t confIrouter_InterfaceDef

#define APP static_cast<transApp_t *>(static_cast<confIrouter_Interface_t *>(this)->getApp())

#define MY_VRFY_ARGC(x, msg)                                         \
    if (argc< (x))  {                                                \
       NOTIFY("irouter configFile syntax error. Usage:: "            \
              "%s\n", msg);                                          \
       exit(0);                                                      \
    }


binding_t<confIrouter_InterfaceDef> confIrouter_methodBinding[]=
{
    { "DEFINE_IROUTER_CTRL_PORT", &CIID_t::irouter_ctrl_port },

    { "DEFINE_MULTICAST",         &CIID_t::irouter_multicast },
    { "DEFINE_MULTICAST_TTL",     &CIID_t::irouter_multicast_ttl },
    { "DEFINE_MULTICAST_BW",      &CIID_t::irouter_multicast_bw },
    { "DEFINE_MULTICAST_GROUP",   &CIID_t::irouter_multicast_group },

    { "DEFINE_LINK_MGR",          &CIID_t::irouter_link_mgr },
    { "DEFINE_CONNECT_FLOWSERVER",&CIID_t::irouter_connect_flowserver },
    { "DEFINE_UP_STREAM_BW",      &CIID_t::irouter_up_stream_bw },
    { "DEFINE_DOWN_STREAM_BW",    &CIID_t::irouter_down_stream_bw },

    { "DEFINE_FLOW",              &CIID_t::irouter_define_flow },

    { "DEFINE_LOCAL_CLIENT",      &CIID_t::irouter_define_local_client },

    { NULL,                       NULL }

};


const char *
confIrouter_Interface_t::irouter_ctrl_port(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_IROUTER_CTRL_PORT(bdPort)");

   int bdPort = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_ctrl_port", "baseDataPort=%d\n", bdPort);

   APP->irouterParam->setBaseDataPort(bdPort);
   return NULL;
}


// MULTICAST PARAMETERS

const char *
confIrouter_Interface_t::irouter_multicast(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_MULTICAST(1|0)");

   bool mcastMode = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_multicast", "Multicast=%d\n", 
            (int)mcastMode);

   APP->irouterParam->setMulticastMode(mcastMode);
   return NULL;
}


const char *
confIrouter_Interface_t::irouter_multicast_ttl(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_MULTICAST_TTL(mcastTTL)");

   int mcastTTL = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_multicast_ttl", 
            "TTL=%d\n", mcastTTL);

   APP->irouterParam->setMulticastTTL(mcastTTL);
   return NULL;
}


const char *
confIrouter_Interface_t::irouter_multicast_bw(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_MULTICAST_BW(mcastBw)");

   int mcastBw = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_multicast_bw", 
            "BW=%d\n", mcastBw);

   APP->irouterParam->setMulticastBw(mcastBw);
   return NULL;
}


const char *
confIrouter_Interface_t::irouter_multicast_group(int argc, char **argv)
{
   MY_VRFY_ARGC(3, "DEFINE_MULTICAST_BW(mcastBw)");

   char *flowName    = argv[1];
   char *mcastGroup  = argv[2];

   debugMsg(dbg_App_Verbose, "irouter_multicast_group", 
            "flowName=%s mcastGroup=%s\n", flowName, mcastGroup);

   APP->irouterParam->configFlowMcastGroup(flowName, mcastGroup);

   return NULL;
}

// CONNECTION TO/FROM OTHER FLOWSERVERS

const char *
confIrouter_Interface_t::irouter_link_mgr(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_LINK_MGR(1|0)");
   bool linkMgr = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_link_mgr", 
            "linkMgr=%d\n", (int)linkMgr);

   APP->irouterParam->setLinkMgr(linkMgr);
   return NULL;
}


const char *
confIrouter_Interface_t::irouter_connect_flowserver(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_CONNECT_FLOWSERVER(flowserverIP)");

   debugMsg(dbg_App_Verbose, "irouter_connect_flowserver", 
            "flowServer=%s\n", argv[1]);

   APP->irouterParam->connectFlowServer(argv[1]);
   APP->irouterParam->setFlowserverMode(true);

   return NULL;
}


const char *
confIrouter_Interface_t::irouter_up_stream_bw(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_UP_STREAM_BW(bw)");

   int upStreamBw = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_up_stream_bw", 
            "Bw=%d\n", upStreamBw);

   APP->irouterParam->setUpStreamBw(upStreamBw);
   return NULL;
}


const char *
confIrouter_Interface_t::irouter_down_stream_bw(int argc, char **argv)
{
   MY_VRFY_ARGC(2, "DEFINE_DOWN_STREAM_BW(bw)");

   int downStreamBw = atoi(argv[1]);

   debugMsg(dbg_App_Verbose, "irouter_down_stream_bw", 
            "Bw=%d\n", downStreamBw);

   APP->irouterParam->setDownStreamBw(downStreamBw);
   return NULL;
}


// FLOW DEFINITION
const char *
confIrouter_Interface_t::irouter_define_flow(int argc, char **argv)
{
   MY_VRFY_ARGC(4, "DEFINE_FLOW(flowName, priority, lifeTime)");

   char *flowName = argv[1];

   debugMsg(dbg_App_Verbose, "irouter_define_flow", 
            "flowName=%s\n", 
            flowName);

   //APP->irouterParam->configFlow(flowName);

   return NULL;
}


const char *
confIrouter_Interface_t::irouter_define_local_client(int argc, char **argv)
{
   MY_VRFY_ARGC(4, "DEFINE_LOCAL_CLIENT(flowName, irouterPort, componentPort)");

   char *flowName=argv[1];
   char *irouterPort = argv[2];
   char *componentPort = argv[3];

   debugMsg(dbg_App_Verbose, "irouter_define_local_client", 
            "flowName=%s irouterPort=%d componentPort=%d\n", 
            flowName, irouterPort, componentPort);

   if(!APP->irouterParam->defineClient(flowName,irouterPort,componentPort)) {
       NOTIFY("Review the irouter config file...\n");
       exit(0);
   }

   APP->irouterParam->activateClient(flowName);
       
   return NULL;
}
