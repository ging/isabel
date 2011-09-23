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
   <name>channel.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef IVIDEO_CHANNEL_H
#define IVIDEO_CHANNEL_H

#include "general.h"

/**
 <class> 
   <name>channel_t</name> 
   <descr>
   This class represent a single channel. Channel_t acts as	
   superclass of recChannel_t and playChannel_t, channel_t 
   has defined common methods used by both kinds of channels. $/
   ChannelList is a dictionary_t of channels. It is formed by  
   pairs <ID,channel_t*> allowing any other method in the  	
   program to find a pointer to any channel registered in the  
   list.
   </descr>
**/

class channel_t: public virtual item_t
				 
{
private:
	
	int chId;
	

protected:

	bool pipeCreated;
	channelKind_e Kind;
	char title[100];
	char acronym[100];
	windowInfo_t windowInfo;
	bool mapping;
	bool fullScreen;
	bool remaped;
	OAHWND Owner;


public:
	

	//Common filter pointers

	IGraphBuilder *pGraph;
	//IBaseFilter   *pWindow;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;
	IPin *input;
	IPin *pOutput;

public:
    
	channel_t(
        int ID,			//channel ID
        const char *title, //Window Title
        windowInfo_t *geom //Window info
    );

    virtual ~channel_t(void){};


    //
    // general administrivia
    //

    int      getId(void){ return chId; };
    HRESULT  setId(int);
	
	bool     getMapping(void){return this->mapping;}

	void	 setFullScreen(bool active) { fullScreen = active; }

	channelKind_e getKind (void) const { return Kind;  }

    //
    // ctrl protocol administrivia
    //
    virtual int setWindowGeometry(windowInfo_t); //obsolete method
    virtual int setWindowTitle(const char *t);   //obsolete method

	virtual HRESULT map(void){return 0;};
	virtual HRESULT remap(void){return 0;};
	virtual HRESULT unmap(void){return 0;};
	virtual HRESULT set_window_title(char * newTitle)
	{
		int hr = 0;
	
		memset(title,0,100);
		strcpy(title,newTitle);
		IVideoWindow * pWindow = NULL;
		pGraph->QueryInterface(IID_IVideoWindow,(void **) &pWindow);
		wchar_t wtext[100];
		lText(wtext,title);
		pWindow->put_Caption(wtext);
		pWindow->Release();

		return hr;
	};

	virtual HRESULT set_acronym(char * newAcronym)
	{
		if (newAcronym)
		{
			strcpy(acronym,newAcronym);
			return S_OK;
		}else{
			return E_FAIL;
		}
	}

	virtual HRESULT set_window_geometry(windowInfo_t wInfo)
	{
		windowInfo.heigth = wInfo.heigth;
		windowInfo.left   = wInfo.left;
		windowInfo.top    = wInfo.top;
		windowInfo.width  = wInfo.width;
	
		setWindowGeometry(windowInfo);

		return 0;
	};
	virtual HRESULT set_full_screen(bool active);
	virtual windowInfo_t get_window_geometry(){return windowInfo;};
	virtual HRESULT setOwner(OAHWND HWnd = NULL);
};

//</class>

extern dictionary_t<int,channel_t*> *channelList;

#endif
