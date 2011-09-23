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
// $Id: cftpRTPSession.h 20657 2010-06-23 14:20:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __FTP_RTP_SESSION_H__
#define __FTP_RTP_SESSION_H__

#include <map>

#include <icf2/general.h>

const double FTP_RTP_TSU= 1.0/90000.0;
const u8     FTP_PT= 43;

#include <rtp/RTPSession.hh>

class cftpApp_t;

class cftpRTPSession_t: public RTPSession_t
{
public:
    cftpRTPSession_t(cftpApp_t  *ftpApp,
		     const char *rtpHost,
		     const char *rtpPort,
		     const char *rtcpHost,
		     const char *rtcpPort,
		     double      tsUnit
		    );
    virtual ~cftpRTPSession_t(void);

    void receivedData (RTPPacket_t *packet);
    void eliminateReceiver (u32 chId, const char *reason);
    bool ftpBind (u32 cid, const char *host, const char *port, u32 bId);
    bool ftpUnbind (u32 cid, u32 bId);

    friend class smartReference_t<cftpRTPSession_t>;

private:
    cftpApp_t *theApp;
    std::map<u32, u32> bindDict;

public:
    const char *className(void) const { return "cftpRTPSession_t"; }
};
typedef smartReference_t<cftpRTPSession_t> cftpRTPSession_ref;

extern cftpRTPSession_ref rtpSession;

#endif

