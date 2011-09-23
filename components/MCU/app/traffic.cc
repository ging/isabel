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
// $Id: traffic.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "traffic.h"
#include "returnCode.h"

// Default constructor
traffic_t::traffic_t(void)
{
    // Time where interval begins
    gettimeofday(&initialTime,NULL);

    // By default, interval length is 2 sec
    interval = 2000;
    tokensPerInterval = 0;
}

// Additional constructor in case
// observation interval wants to be set.
traffic_t::traffic_t(u32 timeSlot)
{
    gettimeofday(&initialTime,NULL);
    interval = timeSlot;
    tokensPerInterval = 0;
}

// Default destructor
traffic_t::~traffic_t(void)
{
}


// Sets the maximum bandwidth allowed for a concrete flow
HRESULT
traffic_t::setBW(u32 limit)
{
    // Sets how many bits I can
    // send for each interval
    tokensPerInterval = limit*interval/1000;

    // At the beginning, no tokens have been used
    tokensLeft = tokensPerInterval;

    return S_OK;
}


// Checks that the client has enough tokens to send the packet.
// If he doesn't, the filter drops it.
HRESULT
traffic_t::deliver(RTPPacket_t *pkt)
{
    HRESULT hr = S_OK;

    if ( ! next)
    {
        NOTIFY("traffic_t::deliver: "
               "trying to deliver without next filter.Aborting...\n"
              );
        abort();
    }

    if ( ! tokensPerInterval)
    {
        hr = next->deliver(pkt);
        errorCheck(hr, "traffic_t");
    }
    else
    {
        // Actual time
        gettimeofday(&actualTime,NULL);

        // Check if I'm still looking into the correct interval.
        // If I don't, it resets tokens count
        if ((u32)((actualTime.tv_sec*1000 + actualTime.tv_usec/1000)-
            (initialTime.tv_sec*1000 + initialTime.tv_usec/1000)) > interval)
        {
            tokensLeft = tokensPerInterval;
            gettimeofday(&initialTime,NULL);
        }

        // Length of the actual packet, in bits
        u16 tokensNeeded = pkt->getTotalLength()*8;

        // If the client has enough credit,
        // packet is sent and tokensLeft updated
        if (tokensNeeded < tokensLeft)
        {
            if (!next)
            {
                return E_NEXT_NOT_EXISTS;
            }

            next->deliver(pkt);
            tokensLeft -= tokensNeeded;

            return S_OK;
        }
        else
        {
            // If the client doesn't have credit, return an error
            return E_TRAFFIC_OVERLOAD;
        }
    }

    return hr;
}

