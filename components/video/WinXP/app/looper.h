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
   <name>looper.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef IVIDEO_LOOPER_H
#define IVIDEO_LOOPER_H

#include "general.h"
#include "channel.h"

/**
 <class> 
   <name>Looper_t</name> 
   <descr>
   Waits file ending to seek the begining, this   
   loop is used by video test source. Looper_t sets file    
   frame rate too. pSeek allows theese two procedures.
   </descr>
**/

class looper_t: public item_t
{

private:
	
	channel_t * channel;
	LONGLONG    Duration;
	DWORD       dwThreadId;
    HANDLE      hThread;

    void __loop(void);

public:
	
	
	looper_t(channel_t * channel);
	virtual ~looper_t(void);

    void Run(void);
    void EndThread(void);
	
    static DWORD WINAPI loop(void *);
    
    void SeektoBegin(void);
    void Seek(LONGLONG position = 0);
	
};

//</class>

#endif