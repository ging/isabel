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
// $Id: extRTPSession.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __ext_rtp_session_hh__
#define __ext_rtp_session_hh__

#include <rtp/RTPSession.hh>

class netChannel_t;

class extRTPSession_t: public RTPSession_t
{
public:
    extRTPSession_t (sched_t      *vApp,
                     netChannel_t *myCh,
                     const char   *rtpHost,
                     const char   *rtpPort,
                     const char   *rtcpHost,
                     const char   *rtcpPort,
                     double        tsUnit
                    );
    virtual ~extRTPSession_t(void)
    {
        debugMsg(dbg_App_Paranoic, "~extRTPSession_t", "destructor invoked");
    };

    virtual void receivedData (RTPPacket_t *packet);
    virtual void eliminateReceiver (u32 chId, const char *reason);

    friend class smartReference_t<extRTPSession_t>;

private:
    netChannel_t *myChannel;
    u16           lastSeqNumber;

public:
    virtual const char *className(void) const { return "extRTPSession_t"; }
};
typedef smartReference_t<extRTPSession_t> extRTPSession_ref;

extern extRTPSession_ref extrtpSession;

#endif

