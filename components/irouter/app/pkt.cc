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
// $Id: pkt.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "pkt.hh"

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/item.hh>

#include <assert.h>
#include <stdlib.h>

#ifdef __BUILD_FOR_LINUX
#include <netinet/in.h>
#endif

pktBuf_t::pktBuf_t(void)
{
    len = 0;
}

pktBuf_t::pktBuf_t(const pktBuf_t &other)
{
    len = other.len;
    memcpy(data, other.data, len);
}

pktBuf_t::~pktBuf_t(void)
{
}

pktBuf_t &
pktBuf_t::operator =(const pktBuf_t &other)
{
    len = other.len;
    memcpy(data, other.data, len);
    return *this;
}



sharedPkt_t::sharedPkt_t(void)
{
    flowId= noFlowId;
    target= 0;

    sharedBuf= new pktBuf_t;

    pktInLink= NULL;
    pktOutLink= NULL;

    memset(&time, 0, sizeof(struct timeval));
}


sharedPkt_t::sharedPkt_t(const sharedPkt_t &other)
{
    flowId= other.flowId;
    target= other.target;

    sharedBuf= other.sharedBuf;

    pktInLink= other.pktInLink;
    pktOutLink= other.pktOutLink;

    memcpy(&time, &other.time, sizeof(struct timeval));
}

sharedPkt_t::~sharedPkt_t(void)
{
    cancel();
}

sharedPkt_t &
sharedPkt_t::operator =(const sharedPkt_t &other)
{
    //
    // BUG!!! No check for autoasignment
    //
    flowId= noFlowId;
    target= 0;

    sharedBuf= other.sharedBuf;

    pktInLink= other.pktInLink;
    pktOutLink= other.pktOutLink;

    memcpy(&time, &other.time, sizeof(struct timeval));

    return *this;
}


u32
pktBuf_t::clientLen(void)
{
    return len;
}


