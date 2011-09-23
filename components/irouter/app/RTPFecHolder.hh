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
// $Id: RTPFecHolder.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_fec_holder_hh__
#define __rtp_fec_holder_hh__

#include <icf2/notify.hh>
#include <icf2/vector.hh>
#include <icf2/dictionary.hh>

#include <rtp/RTPPacket.hh>
#include <rtp/RTPFec.hh>

#include "flow.hh"

// Class RTPFecHolder_t: stores a RTP FEC Receiver/Sender objects
// for each RTP Flow
class RTPFecHolder_t
{
private:

    dictionary_t<u32, RTPFecSender_t*> __sendChannelsDict;
    dictionary_t<u32, RTPFecReceiver_t*> __recvChannelsDict;
    u8 fecPT;
    u8 k;

public:

    RTPFecHolder_t(u8 nfecPT);
    ~RTPFecHolder_t(void);

    RTPPacket_t *sendPacket(RTPPacket_t *pkt);
    RTPPacket_t *recvPacket(RTPPacket_t *pkt);

    void setFecParams(const char *fecType, int new_n, int new_k);
    void getFecParams(int &n, int &k);

    void delRTPFec(u32 ssrc);
};

#endif

