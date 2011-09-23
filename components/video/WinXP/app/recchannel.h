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
   <name>recchannel.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_REC_CHANNEL_H
#define _IVIDEO_REC_CHANNEL_H


#include "general.h"
#include "looper.h"
#include "sender.h"
#include "testsource.h"
#include "sharedDisplay.h"

#ifdef _WINDOWS
//control seek for file sources
#include "resource/fileControl.h"
#endif

/**
 <class> 
   <name>recChannel_t</name> 
   <descr>
   This class represent a single source channel. recChannel_t  
   inherits from channel_t and creates instances of			
   sender_t and looper_t to control playing and send frames.	
   recChannel_t uses CamList to choose a camInfo_t source.     
   recChannel_t can send CAM, TEST and MEDIA FILES.
   </descr>
**/

class recChannel_t : public channel_t
{

private:
	
	//source Info
	float  frameRate;
    bool   all;
	AM_MEDIA_TYPE actualFormat;
	int  sourceId;
	char sourceFormat[100];

	captureInfo_t capInfo;
	camInfo_t *camInfo;

#ifdef _WINDOWS
    //file control box dialog
    fileControl * fControl;

    //message events thread reader
    DWORD       dwThreadId;
    HANDLE      hThread;

    //file position thread reader
    DWORD       dwThreadIdPos;
    HANDLE      hThreadPos;
#endif

	// auxiliar
     char auxStr[100];

public:

	//filter pointers
	IBaseFilter * pSource;
	looper_t    * looper;
	sender_t	* pSender;

private:

	//private methods
	int source_format(char*);
    int grab_geometry(captureInfo_t);
	void  refresh_channel(bool all = true);

#ifdef _WINDOWS
    //file control methods
    void RunThread(void);
        
    //methods to work each thread
    void fileControlThread(void);
    void posReaderThread(void);
    //start and end
    void EndThread(void); //kill both threads
    static DWORD WINAPI Run(void *); 
    static DWORD WINAPI RunPos(void *); 
#endif
  

public:

	//public methods

	recChannel_t(
		int ID,			   //channel ID
        const char *title, //Window Title
        windowInfo_t *geom = &DEFAULT_WINDOW_INFO //Window info
    );

	virtual ~recChannel_t(void);
	char *   list_source_formats(void);
    int      select_source(camInfo_t * source);
    char *   current_source(void){ return camInfo->getCamName(); } 
 	
	HRESULT map(void);
	HRESULT remap(void);
	HRESULT unmap(void);
	HRESULT set_window_title(char *);
	HRESULT set_window_geometry(windowInfo_t);
	HRESULT set_file_source(char *);
	HRESULT set_rate(float);
	HRESULT set_bucket_size(int);
	float   get_rate(void);
	HRESULT get_AM_MEDIA_TYPE(char *,AM_MEDIA_TYPE**);
	void    set_all(bool boolean){ all = boolean; }
	void    set_actual_format(AM_MEDIA_TYPE newFormat){ actualFormat = newFormat; }
	
	char *  get_source_format(void); 
    camInfo_t * get_camInfo(void);
    captureInfo_t get_captureInfo(void);
	bool    set_captureInfo(captureInfo_t);

	
	
};
//</class>
#endif