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
   <name>sharedDisplay.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_SHARED_DISPLAY_SOURCE_H
#define _IVIDEO_SHARED_DISPLAY_SOURCE_H

#include "camMgr.h"
#include "channel.h"
#include <icf2/item.hh>
#include <afxwin.h>

/**
 <class> 
   <name>sharedDisplayPin_t</name> 
   <descr>
   </descr>
**/


class sharedDisplayPin_t: public CSourceStream
{
private:
	
  	CBitmap	*		  Bitmap;
	RECT		      sharedRect;
	BYTE *			  Data;
	HDC               hScrDC, hMemDC;         // screen DC and memory DC    
	channel_t *		  channel;
	struct _timeb     time; //initial time
	struct _timeb     actualTime; // incremental time
	int				  frameRate;
    HICON iconMouse;		

    u8  bits;

    int convert32to16(BYTE * data,int len);
    int convert24to16(BYTE * data,int len);

public:

	sharedDisplayPin_t(TCHAR *pObjectName,
					   HRESULT *phr,
                       CSource *pms,
                       LPCWSTR pName,
					   channel_t * pChannel);

	virtual ~sharedDisplayPin_t(void);
	
    virtual HRESULT DecideBufferSize(IMemAllocator *pAlloc,
								     ALLOCATOR_PROPERTIES *ppropInputRequest);
	
	virtual HRESULT FillBuffer(IMediaSample *pData);

	void    GetSample(IMediaSample ** Sample);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual HRESULT GetMediaType(CMediaType *pMediaType);
	HBITMAP CopyScreenToBitmap(LPRECT lpRect);
	virtual void Free(void){;}
	void    SetSharedRect(RECT);
	void	SetRate(int rate);

  	// Fake referance counting.
//    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }


};
//</class>
/**
 <class> 
   <name>sharedDisplay_t</name> 
   <descr>
   </descr>
**/


class sharedDisplay_t:public CSource,
					  public camInfo_t
					  
{
private:

	long			  frameRate;
	HRESULT		      hr;
	channel_t *	      channel;
	RECT		      sharedRect;
	sharedDisplayPin_t *   OutputPin;
    
   
public:

	

	sharedDisplay_t(channel_t * pChannel,
					TCHAR *pName,
					LPUNKNOWN lpunk,
					CLSID clsid,
					long period = DEFAULT_SHARED_DISPLAY_FR);

	virtual ~sharedDisplay_t(void);


	virtual bool setSharedRect(RECT newRect);
	virtual RECT getSharedRect(void){ return sharedRect; } 
	virtual void setRate(int newFR);
	

	virtual HRESULT RunSource(void){
		OutputPin->Pause();
		return 0;
	}

  	// Fake referance counting.
//    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }



};
//</class>

/**
 <function> 
   <name>createSharedDisplay(channel_t * channel, char * sourceName)</name>
   <descr>
   </descr>  
 </function>
**/
static camInfo_t*
createSharedDisplay(channel_t *channel, char *sourceName)
{
	camInfo_t *camInfo=
		new sharedDisplay_t(channel, sourceName, NULL, CLSID_NULL);

	int newId= 0;
	if (camArray.size() > 0)
	{
		newId= camArray.elementAt(camArray.size()-1)->getID();
	}
	
	camInfo->setID(newId);
	camInfo->setFree(true);
	camInfo->setKind(SHARED);
	camInfo->setCamName(sourceName);

	camArray.add(camInfo);

	return camInfo;
}

#endif