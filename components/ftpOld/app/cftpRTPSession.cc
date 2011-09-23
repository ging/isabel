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
// $Id: cftpRTPSession.cc 20657 2010-06-23 14:20:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "cftpApp.h"
#include "cftpRTPSession.h"

cftpRTPSession_ref rtpSession;

cftpRTPSession_t::cftpRTPSession_t(cftpApp_t  *ftpApp,
                                   const char *rtpHost,
                                   const char *rtpPort,
                                   const char *rtcpHost,
                                   const char *rtcpPort,
                                   double      tsUnit
                                  )
: RTPSession_t (static_cast<sched_t*>(ftpApp), rtpHost, rtpPort, rtcpHost, rtcpPort, tsUnit)
{
    theApp = ftpApp;
    deliverAnyPacket(true);
}

cftpRTPSession_t::~cftpRTPSession_t(void)
{
    debugMsg(dbg_App_Paranoic, "~ftpRTPSession_t", "destructor invoked");
};

void
cftpRTPSession_t::receivedData (RTPPacket_t *packet)
{
    debugMsg(dbg_App_Paranoic, "IOReady", "entro en IO\n");

    appData_t *cftpPacket = new appData_t;
    memcpy((u8*)cftpPacket, packet->getData(), sizeof(appData_t));

    cftpPacket->dataChannel = ntohl(cftpPacket->dataChannel);
    cftpPacket->frameNumber = ntohl(cftpPacket->frameNumber);
    cftpPacket->dataSize    = ntohl(cftpPacket->dataSize);

    debugMsg( dbg_App_Paranoic, "IOReady",
              "packetLen=%d\n" "channel (%d)\n"
              "frm=%d\n"
              "DataSize=%d\n",
              packet->getDataLength(), cftpPacket->dataChannel,
              cftpPacket->frameNumber,
              cftpPacket->dataSize);

    packet= NULL; // paranoid

    debugMsg(dbg_App_Normal, "IOReady", "receiving PKT\n");
    fileTrans_t *currChannel =
        theApp->theChanMgr->getChannel(cftpPacket->dataChannel);

    if(!currChannel || (!currChannel->prot_mgr.isValid())) {
        debugMsg(dbg_App_Normal,"IOReady",
                 "receving pkts and i dont have "
                 "this channel=%d... forget it!\n",
                 cftpPacket->dataChannel);
        delete cftpPacket;
        return;
    }

    debugMsg(dbg_App_Normal, "IOReady", "delivering pkt to manager...\n");

    currChannel->prot_mgr->receive(cftpPacket);
}

void
cftpRTPSession_t::eliminateReceiver (u32 chId, const char *reason)
{
}

bool 
cftpRTPSession_t::ftpBind (u32 cid,
                           const char *host,
                           const char *port,
                           u32 bId
                          )
{
    u32 rtpBindId= rtpBind(cid, host, port);
    bindDict[bId]= rtpBindId;

    return true;
}

bool
cftpRTPSession_t::ftpUnbind(u32 cid, u32 bId)
{
    std::map<u32,u32>::iterator iter= bindDict.find(bId);
    if (iter == bindDict.end())
    {
        return false;
    }

    u32 rtpBindId= iter->second;

    rtpUnbind(cid, rtpBindId);

    bindDict.erase(bId);

    return true; 
}


