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
/////////////////////////////////////////////////////////////////////////
//
// $Id: traffic.h 6363 2005-03-18 16:31:53Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_TRAFFIC_H_
#define _MCU_TRAFFIC_H_

#include "general.h"
#include "filter.h"
#include <icf2/icfTime.hh>

class traffic_t: public filter_t
{

private:

    //
	//Time to control (milliseconds)
	u32 interval;
	
	//Tokens allowed
	u32	tokensPerInterval;
	u32 tokensLeft;
	
	//Timestamps
	timeval initialTime;
	timeval actualTime;

public:

    traffic_t(void);
    traffic_t(u32);
    virtual ~traffic_t(void);

    //set max BW in bps
    HRESULT setBW(u32);

    //receives RTPPacket and discards it if nedded
    virtual HRESULT deliver(RTPPacket_t * pkt);
};

#endif

