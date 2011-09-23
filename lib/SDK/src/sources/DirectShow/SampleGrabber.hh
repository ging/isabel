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
#ifndef __CSampleGrabberCB_H__
#define __CSampleGrabberCB_H__

#include "DSUtils.hh"
//Estos defines son por una cagada de Microsoft en el archivo Qedit.h
//-->http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=2617687&SiteID=1

// bugfix in headerfile
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include <Qedit.h>

class CSampleGrabberCB : public ISampleGrabberCB						
{
private:

	static const int FRAME_TIMEOUT = 5000; //5 seconds

	HANDLE m_hMutex;
	HANDLE m_hEvent;
	BYTE * m_pFrame;
	int    m_RefCount;
	int    m_iFrameLen;
	BOOL   m_bProcessed;

public:

	CSampleGrabberCB(void);
	virtual ~CSampleGrabberCB(void);
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample);
    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);
	int          GetImage(BYTE * pBuffer, long BufferLen,int w=0,int h=0,int bpp=0,bool flip=false);
	// Reference counting.
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);
};

#endif