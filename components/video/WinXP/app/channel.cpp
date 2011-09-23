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
/**
 <head> 
   <name>channel.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of channel.h
   </descr>
//</head>
**/

#include "channel.h"

dictionary_t<int,channel_t*> *channelList = new dictionary_t<int,channel_t*>();

channel_t::channel_t(int ID,
					 const char *title,
					 windowInfo_t *wInfo)
{
    __CONTEXT("channel_t::channel_t");
	

	pGraph = NULL;
	//pWindow  = NULL;
	pControl = NULL;
	input = NULL;
	pOutput = NULL;
	Owner = NULL;

	mapping = false;
	fullScreen = false;
	remaped = false;
	pipeCreated = false;

	strcpy(acronym,"");

	memset((void *)(this->title),0,200);
	strcpy(this->title,title);
	
	setId(ID);
	this->windowInfo.top    = wInfo->top;
	this->windowInfo.left   = wInfo->left;
	this->windowInfo.heigth = wInfo->heigth;
	this->windowInfo.width  = wInfo->width;

	int	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
	errorCheck(hr);
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	errorCheck(hr);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	errorCheck(hr);
	
	
}

HRESULT
channel_t::setId(int newChId)
{
    __CONTEXT("channel_t::setId");
	// first look if the channel id already exists
	if (channelList->lookUp(newChId)==NULL)
	{
		this->chId = newChId;
		return 0;
	}else{
		return -1;
	}
}

int 
channel_t::setWindowTitle(const char *t)
{
    __CONTEXT("channel_t::setWindowTitle");
	
	int ret=0;
	memset((void *)title,0,100);
	strcpy(title,t);

	return ret;

}



int 
channel_t::setWindowGeometry(windowInfo_t wInfo)
{
    __CONTEXT("channel_t::setWindowGeometry");
	
	IVideoWindow *pWindowInfo = NULL;
	int hr = 0;
	
	hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
	if (!hr)
	{
		hr = pWindowInfo->SetWindowPosition(windowInfo.left,
											windowInfo.top,
											windowInfo.width,
											windowInfo.heigth);
	}	
				
	return hr;

}

HRESULT
channel_t::set_full_screen(bool active)
{
    __CONTEXT("channel_t::set_full_screen");
	
	int hr = 0;

	IVideoWindow *pWindowInfo = NULL;
	hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
	if (!hr){
		
		pControl->StopWhenReady();
		if (active)
		{
			fullScreen = true;
			hr = pWindowInfo->put_FullScreenMode(OATRUE);
		
		
		}else{
			
			fullScreen = false;
			pWindowInfo->put_FullScreenMode(OAFALSE);
			setWindowGeometry(windowInfo);
		}
	}
	return hr;
}

HRESULT
channel_t::setOwner(OAHWND HWnd)
{
	if (HWnd) Owner = HWnd;

	if (!Owner) return S_OK;

	IVideoWindow *pWindowInfo = NULL;
	HRESULT hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
	if (!hr){
		//pWindowInfo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
		pWindowInfo->put_Owner((OAHWND)Owner);
		pWindowInfo->put_Visible(TRUE);
		pWindowInfo->SetWindowPosition(0,
			                           0, 
                                       windowInfo.width,
									   windowInfo.heigth);		
		pWindowInfo->Release();
	}else{
		NOTIFY("PUT OWNER::");errorCheck(hr);
	}
    return hr;
}

