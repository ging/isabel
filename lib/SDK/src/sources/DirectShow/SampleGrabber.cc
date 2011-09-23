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
#include <icf2/general.h>
#include <icf2/notify.hh>

#include "SampleGrabber.hh"

using namespace DSUtils;

CSampleGrabberCB::CSampleGrabberCB(void)
{
	m_pFrame = NULL;
	m_iFrameLen = -1;
	m_RefCount = 0;
	m_hMutex = CreateMutex(NULL,FALSE,NULL);
	m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_bProcessed = TRUE;
	AddRef();
}

CSampleGrabberCB::~CSampleGrabberCB(void)
{
	DELETE_ARR(m_pFrame);
	CloseHandle(m_hMutex);
	CloseHandle(m_hEvent);
}


STDMETHODIMP_(ULONG) 
CSampleGrabberCB::AddRef(void)
{
	m_RefCount++;
	return S_OK;
}

STDMETHODIMP_(ULONG) 
CSampleGrabberCB::Release(void)
{
	m_RefCount--;
	if (m_RefCount <= 0)
	{
		delete this;
	}
	return S_OK;
}

STDMETHODIMP 
CSampleGrabberCB::QueryInterface(REFIID riid, void **ppvObject)
{
    if (NULL == ppvObject) return E_POINTER;
    if (riid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
         return S_OK;
    }
    if (riid == __uuidof(ISampleGrabberCB))
    {
        *ppvObject = static_cast<ISampleGrabberCB*>(this);
         return S_OK;
    }
    return E_NOTIMPL;
}

STDMETHODIMP 
CSampleGrabberCB::SampleCB(double Time, IMediaSample *pSample)
{
    return E_NOTIMPL;
}

STDMETHODIMP 
CSampleGrabberCB::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
	HRESULT result = S_OK;
	
	WaitForSingleObject(m_hMutex,INFINITE);	
	if (m_bProcessed)
	{
		DELETE_ARR(m_pFrame);
		m_pFrame = new BYTE[BufferLen];
		if (m_pFrame)
		{
			memcpy(m_pFrame,pBuffer,BufferLen);
			m_iFrameLen = BufferLen;
		}else{
			NOTIFY("CSampleGrabberCB::BufferCB: Out of memory!\n");
			result = E_OUTOFMEMORY;
		}
		m_bProcessed = FALSE;
		SetEvent(m_hEvent);
	}
	ReleaseMutex(m_hMutex);	
	return result;
}

int          
CSampleGrabberCB::GetImage(BYTE * pBuffer, long BufferLen,int w,int h,int bpp,bool flip)
{
	int ret = 0;
	WaitForSingleObject(m_hEvent,FRAME_TIMEOUT);	
	WaitForSingleObject(m_hMutex,INFINITE);
	if (m_iFrameLen < 0)
	{
		ret = 0;
	}else if (m_iFrameLen > BufferLen){
		ret = -m_iFrameLen;
	}else {
		if (flip)
		{
			bpp /= 8;
			int linesize = w*bpp;
			BYTE * pOut = pBuffer;
			BYTE * pIn  = m_pFrame+(h-1)*linesize;
			for (int y = h; y>0; --y)
			{
				for (int x = 0; x<w; ++x)
				{
					pOut[0] = pIn[2];
					pOut[1] = pIn[1];
					pOut[2] = pIn[0];
					pOut+=bpp;
					pIn+=bpp;
				}
				pIn-=2*linesize;
			}
		}else{
			memcpy(pBuffer,m_pFrame,m_iFrameLen);
		}
		ret = m_iFrameLen;
	}
	m_bProcessed = TRUE;
	ReleaseMutex(m_hMutex);		
	return ret;
}