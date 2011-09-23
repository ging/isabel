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

#include "looper.h"

looper_t::looper_t(channel_t * channel) 
{
    this->channel = channel;
	this->dwThreadId = 0;
}



void 
looper_t::Run(void)
{
    //Create Server Thread
	
	hThread = CreateThread( 
				NULL,              // no security attribute 
				0,                 // default stack size 
				loop, //Routine 
				(void *)this,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 
}

void 
looper_t::EndThread(void)
{
    NOTIFY("looper_t::killing thread...\n");  
    TerminateThread(hThread,0); 
}

looper_t::~looper_t(void)
{
    
}

DWORD WINAPI
looper_t::loop(void * param)
{
    looper_t * looper = static_cast<looper_t *>(param);
	looper->__loop();
	return 0;
} 

void 
looper_t::__loop(void) 
{
     NOTIFY("looper_t::looper created\n");   
	 while(true)
     {
		   long retCode = 0; 
           HRESULT hr = 0;
                
		   hr = channel->pGraph->QueryInterface(IID_IMediaEvent, (void **)&channel->pEvent);
           if (hr == S_OK)
           {
       		    channel->pEvent->WaitForCompletion(INFINITE, &retCode);
			    SeektoBegin();
                channel->pEvent->Release();
           }
           
           Sleep(1000);
       }
}


void 
looper_t::SeektoBegin(void)	
{
	Seek(0);
}

void 
looper_t::Seek(LONGLONG position)
{
    IMediaSeeking * pSeek = NULL;
    int hr = channel->pGraph->QueryInterface(IID_IMediaSeeking,(void **)&pSeek);
	if (pSeek && hr == S_OK)
    {
        position = position * 10000000; // Time in 100 nanoseconds unit
        pSeek->SetPositions(&position,
            AM_SEEKING_AbsolutePositioning,
            NULL,
            AM_SEEKING_NoPositioning);
        pSeek->Release();
    }
}
