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
   <name>videoApp.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_APP_H
#define _IVIDEO_APP_H

#include "general.h"
#include "ctrlProtocol.h"
#include <icf2/stdTask.hh>
#include <icf2/strOps.hh>
#include <sys/timeb.h>
#include <time.h>


/**
 <class> 
   <name>videoApp_t</name> 
   <descr>
   </descr>
**/
class videoApp_t: public application_t {
 
private:
    
	char   __rtpPort[10];
    char   __rtcpPort[10];
    char   *__controlPort;
    double __bandwidth;

public:
	
	videoApp_t(int &argc, argv_t &argv);

};
//</class>
#endif
	