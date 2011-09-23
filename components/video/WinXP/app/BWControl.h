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
   <name>BWControl.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_BANDWIDTH_CONTROL_H
#define _IVIDEO_BANDWIDTH_CONTROL_H

#include "channel.h"
#include <icf2/item.hh>



/**
 <class> 
   <name>BWController_t</name> 
   <descr>
	  Every "period" BWController_t obtains the "actualBW"  
 	  used by recChannel_t and acts as follows: $/			    
	  1) If "actualBW" is too high it calculates a new frame
 	  rate (newFR) so the new BW will be equal to the max    
  	  BW set by the user. $/
 	  2) If "actualBW" is higher (not too much)than "BW"      
 	  set by the user we can assume we are aproximating     
 	  to the correct frame rate(FR) so BWController_t       
 	  decreases it in one unit. $/
 	  3) If "actualBW" is too low it calculates "newFR"     
 	  so the new BW will be equal to the max BW set by      
 	  the user.	$/						    
 	  4) If "actualBW" is lower (not too low) than "BW"     
 	  set by the user we can assume we are aproximating      
 	  to the correct frame rate(FR) so BWController_t	    
  	  increases it in one unit.  
   </descr>
**/

class BWController_t: public simpleTask_t
{

private:
	
	long BW;
	unsigned long actualBW;
	LONGLONG data; 
	channel_t * channel;
	virtual void heartBeat(void);

#if DBG_SCHED 
	fileIO_t * dbgFile;
#endif

public:

	BWController_t(channel_t * channel,long BW,u32 period = DEFAULT_BW_PERIOD);
    void Release(void);
	virtual ~BWController_t(void);
	bool setBW(long BW);
    bool refreshBW(void);
	void watchBW(long Data);
	
};

//</class>
#endif