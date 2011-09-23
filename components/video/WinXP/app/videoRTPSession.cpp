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
/**
 <head> 
   <name>videoRtpSession.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of videoRTPSession.h.
   </descr>
//</head>
**/

#include "recchannel.h"
#include "playchannel.h"
#include "videoRTPSession.h"
#include "videoApp.h"

videoRTPSession_ref rtpSession;

videoRTPSession_t::videoRTPSession_t(videoApp_t *vApp,
				     char *rtpHost,
				     char *rtpPort,
				     char *rtcpHost,
				     char *rtcpPort,
                     double tsUnit
                                    )
: RTPSession_t (static_cast<sched_t*>(vApp), rtpHost, rtpPort, rtcpHost, rtcpPort, tsUnit)
{
    videoApp = vApp;
}


void
videoRTPSession_t::receivedData (RTPPacket_t * packet)
{
    debugMsg(dbg_App_Normal,
             "videoRTPSession_t",
             "receivedData 1 SSRC=%d\n", 
             packet->getSSRC()
            ); 

    channel_t * channel = NULL;
    if ((channel = channelList->lookUp(packet->getSSRC())) != NULL)        
    {
        if(channel->getKind() == PLAY_CHANNEL)
        {
            playChannel_t * playChannel = static_cast<playChannel_t *>(channel);
            playChannel->addFragment(packet);
        }
        else
        {
            delete packet;
        }
	
   }
   else if(*SetAutoChannel)
   {
       NOTIFY("Receiving new flow SSRC = %d. Building playChannel_t...\n",
              packet->getSSRC()
             );
       char name[20]="";
       sprintf(name,"Remote %d",packet->getSSRC());
       playChannel_t * playChannel = new playChannel_t(packet->getSSRC(),name);
   }
}


void
videoRTPSession_t::eliminateReceiver (u32 chId, char *reason)
{
   	channel_t * channel;
	if ((channel = channelList->lookUp(chId))!=NULL
      && channelList->lookUp(chId)->getKind() == PLAY_CHANNEL) 
	{
       // delete channel;
        debugMsg (dbg_App_Normal,
			    "videoRTPSession_t",
                "eliminateReceiver:Channel identifier: %d. Reason: %s\n",
                 chId, reason);
    }
}


