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
// $Id: keepAlive.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_KEEP_ALIVE_H_
#define _MCU_KEEP_ALIVE_H_

#include "general.h"
#include "filter.h"
#include <icf2/sched.hh>

#define ALIVE_PERIOD 5000000 //5 seconds

class keepAlive_t: public filter_t, 
	               public simpleTask_t
{

private:

	u8    counter;
	bool  active;
	bool  set;
    	
public:
    
    keepAlive_t(unsigned long = ALIVE_PERIOD);
    virtual ~keepAlive_t(void);

	HRESULT setAlive(bool);
    
	//receives RTPPacket 
	virtual void heartBeat(void);
    virtual HRESULT deliver(RTPPacket_t * pkt);
};

#endif

