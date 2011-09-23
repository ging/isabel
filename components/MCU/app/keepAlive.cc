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
// $Id: keepAlive.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPUtils.hh>

#include "keepAlive.h"
#include "returnCode.h"

keepAlive_t::keepAlive_t(unsigned long period)
: simpleTask_t(0)
{
    counter = 0;
    active  = true;
    set = false;
}

keepAlive_t::~keepAlive_t(void)
{
}

HRESULT
keepAlive_t::setAlive(bool boolean)
{
    set = boolean;
    if (set)
    {
        set_period(ALIVE_PERIOD);
    }
    else
    {
        set_period(0);
    }
    return S_OK;
}

// receives RTPPacket
void
keepAlive_t::heartBeat(void)
{
    if ( ! next || ! set)
    {
        return;
    }

    if (active)
    {
         RTPHeader_t *rtpHeader = (RTPHeader_t*)new u8[sizeof(RTPHeader_t)];
         memset(rtpHeader, 0, sizeof(RTPHeader_t));

#if defined(WIN32)
        u16 SQ = (u16)rand();
        u32 TS = (u32)rand();
        u32 SSRC = (u32)rand();
#elif defined(__BUILD_FOR_LINUX)
        u16 SQ = (u16)random();
        u32 TS = (u32)random();
        u32 SSRC = (u32)random();
#else
#error "Please, implement random() for your OS"
#endif

        rtpHeader->version       = RTP_VERSION;
        rtpHeader->padding       = 0;
        rtpHeader->extension     = 0;
        rtpHeader->cc            = 0;
        rtpHeader->marker        = 1;
        rtpHeader->payloadtype   = ALIVE_PT;
        rtpHeader->seqnum        = htons(SQ);
        rtpHeader->timestamp     = htonl(TS);
        rtpHeader->ssrc          = htonl(SSRC);

        RTPPacket_t *pkt= new RTPPacket_t((u8 *)rtpHeader,sizeof(RTPHeader_t));
        next->deliver(pkt);

        delete pkt;
    }
    else
    {
        counter ++;
        if (counter > (5*get_period()/1000000))
        {
            active = true;
        }
    }
}

HRESULT
keepAlive_t::deliver(RTPPacket_t *pkt)
{
    if ( ! next)
    {
        NOTIFY("keepAlive_t::deliver: "
               "trying to deliver without next filter.Aborting...\n"
              );
        abort();
    }
    active = false;
    counter = 0;
    next->deliver(pkt);

    return S_OK;
}

