/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 <head> 
   <name>videoRTPSession.h</name> 
   <version>1.0</version>
   <author>Gabriel Huecas and Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef __VIDEO_RTP_SESSION_HH__
#define __VIDEO_RTP_SESSION_HH__



#include <rtp/RTPSession.hh>


class videoApp_t;

/**
 <class> 
   <name>videoRTPSession_t</name> 
   <descr>
   </descr>
**/
class videoRTPSession_t: public RTPSession_t
{
public:

    videoRTPSession_t (videoApp_t *vApp,
		       char *rtpHost,
		       char *rtpPort,
		       char *rtcpHost,
		       char *rtcpPort,
		       double tsUnit
		      );
    virtual ~videoRTPSession_t(void) {
        debugMsg(dbg_App_Paranoic, "~videoRTPSession_t", "destructor invoked");
    };

    virtual void receivedData (RTPPacket_t * packet);
    virtual void eliminateReceiver (u32 chId, char *reason);

    friend class smartReference_t<videoRTPSession_t>;

private:
    videoApp_t *videoApp;

public:
    virtual const char *className(void) const { return "videoRTPSession_t"; }
};
typedef smartReference_t<videoRTPSession_t> videoRTPSession_ref;

extern videoRTPSession_ref rtpSession;
//</class>
#endif

