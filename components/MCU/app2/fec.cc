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
// $Id: fec.cc 7032 2005-07-04 09:01:11Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>
#include <rtp/RTPFec.hh>

#include "fec.h"
#include "general.h"
#include "returnCode.h"

//
// FEC Sender
//
fecSender_t::fecSender_t(void)
{
    rtpFecSender= new RTPFecSender_t(FEC_PT);
}

fecSender_t::~fecSender_t(void)
{
    delete rtpFecSender;
}

HRESULT
fecSender_t::setK(u8 k)
{
    rtpFecSender->setK(k);

    return S_OK;
}

HRESULT
fecSender_t::deliver(RTPPacket_t *pkt)
{
    if ( ! next)
    {
        NOTIFY("fecSender_t::deliver: "
               "trying to deliver without next filter.Aborting...\n"
              );
        abort();
    }

    next->deliver(pkt);
    RTPPacket_t *fecPkt= rtpFecSender->sendPacket(pkt);
    if (fecPkt != NULL)
    {
        next->deliver(fecPkt);
    }

    return S_OK;
}

