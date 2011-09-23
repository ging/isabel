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
   <name>samplereader.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of samplereader.h.
   </descr>
//</head>
**/

#include "samplereader.h"

#include <icf2/icfTime.hh>

frameContainer_t::frameContainer_t(void)
{
    __CONTEXT("frameContainer_t::frameContainer_t");
	memset(frameBuff,NULL,sizeof(frame_t *)*FRAME_CONTAINER_SIZE);
	frameCount = 0;
	actualFrame = 0;
}

void
frameContainer_t::insert(frame_t * newFrame)
{
    __CONTEXT("frameContainer_t::insert");
	if(frameCount+1 == actualFrame){
		frameCount = actualFrame+1;
	}

	if(frameCount >= FRAME_CONTAINER_SIZE){
		frameCount = 0 + frameCount - FRAME_CONTAINER_SIZE;
	}

	if(frameBuff[frameCount] != NULL){
		NOTIFY("MACHACANDO!!!!");
		delete frameBuff[frameCount]->data;
		delete frameBuff[frameCount];
	}

	frameBuff[frameCount] = newFrame;
	frameCount++;
}

frame_t *
frameContainer_t::popFrame(void)
{
    __CONTEXT("frameContainer_t::getFrame");
	if(actualFrame == FRAME_CONTAINER_SIZE){
		actualFrame = 0;
	}

	frame_t * frame = NULL;

	if(frameBuff[actualFrame] != NULL){
		frame = frameBuff[actualFrame];
		frameBuff[actualFrame] = NULL;
		actualFrame++;
	}
	
	return frame;
}


void
frameContainer_t::freeContainer(void)
{
    __CONTEXT("frameContainer_t::freeContainer");
	for (int i=0; i<FRAME_CONTAINER_SIZE; i++)
	{
		if (frameBuff[i]!=NULL)
		{
			delete frameBuff[i]->data;
			delete frameBuff[i];
			frameBuff[i] = NULL;
			
		}
	}
	frameCount = 0;
	actualFrame = 0;
}
	

sampleReader_t::sampleReader_t(TCHAR *pName,LPUNKNOWN lpunk,CLSID clsid)
:CSource(pName,lpunk,clsid)
{
    __CONTEXT("sampleReader_t::sampleReader_t");	 
	HRESULT hr = S_OK;
	OutPin = new SourcePin_t("OutputStream",&hr,(CSource *)(this),L"OutputStream");
    frameList = new frameContainer_t();  
    mutex = CreateMutex(NULL,FALSE,NULL);
    mutex2 = CreateMutex(NULL,FALSE,NULL);
    hThread = NULL;
    dwThreadId = NULL;
}

sampleReader_t::~sampleReader_t(void)
{
	CAutoLock lock_it(m_pLock);	
	__CONTEXT("sampleReader_t::~sampleReader_t");
	CloseHandle(mutex);
	CloseHandle(mutex2);
	OutPin->Release();
}

void
sampleReader_t::TimeToDie(void)
{
    EndThread();
	frameList->freeContainer();		
}

HRESULT
sampleReader_t::GetMediaType(CMediaType *pMediaType)
{
	return OutPin->GetMediaType(pMediaType);
}

void
sampleReader_t::receiveFrame(frame_t * frame)
{		
	__CONTEXT("sampleReader_t::receiveFrame");
	DWORD ret = WaitForSingleObject(mutex,1000);
	if(ret == WAIT_TIMEOUT || ret == WAIT_FAILED){
		NOTIFY("sampleReader_t::receiveFrame waitForSingleObject timeouted, throwing frame away\n");
		delete frame->data;
		delete frame;
		ReleaseMutex(mutex);
		return;
	}
	//NOTIFY("Entra y TS: %u, %u\n",frame->sequenceNumber,frame->timestamp);
	//NOTIFY("Entra: %u\n",frame->sequenceNumber);
	frameList->insert(frame);
	ReleaseMutex(mutex);
}

void 
sampleReader_t::Run(void)
{
	//Create Server Thread
	hThread = CreateThread( 
				NULL,              // no security attribute 
				0,                 // default stack size 
				threadStart, //Routine 
				(void *)this,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 
}

void 
sampleReader_t::EndThread(void)
{
    WaitForSingleObject(mutex2,INFINITE);
	TerminateThread(hThread,0); 
    ReleaseMutex(mutex2);
}

DWORD WINAPI
sampleReader_t::threadStart(void * param)
{
    sampleReader_t * sampler = static_cast<sampleReader_t *>(param);
	sampler->__Run();
	return 0;
} 

void 
sampleReader_t::__Run(void) 
{
    // Receiver thread started
	while (true)
    {
		//Event.Wait(INFINITE); 
		WaitForSingleObject(mutex,INFINITE);
		frame_t * frame = frameList->popFrame();
		if(frame != NULL && frame->data != NULL){
			//NOTIFY("Sale: %u\n",frame->sequenceNumber);
			//NOTIFY("Sale y TS: %u, %u\n",frame->sequenceNumber,frame->timestamp);
			OutPin->SendSample(frame->data,frame->dataLen);
			delete frame->data;
			delete frame;
		}
		ReleaseMutex(mutex);
    }
}

HRESULT 
sampleReader_t::SetMediaType(CMediaType * videoType)
{
	return OutPin->SetMediaType(videoType);
}

SourcePin_t::SourcePin_t(TCHAR *pObjectName,
				HRESULT *phr,
                CSource *pms,
                LPCWSTR pName)
:CSourceStream(pObjectName,phr,pms,pName)
{	
}

SourcePin_t::~SourcePin_t(void)
{

}

HRESULT 
SourcePin_t::DecideBufferSize(IMemAllocator *pAlloc,
							  ALLOCATOR_PROPERTIES *ppropInputRequest)
{
	ALLOCATOR_PROPERTIES pprop;
	ppropInputRequest->cbBuffer = MAX_SAMPLE_DATA;
	ppropInputRequest->cBuffers = 5;
	pAlloc->SetProperties(ppropInputRequest,&pprop);
	m_pAllocator=pAlloc;
	return S_OK;
}

HRESULT 
SourcePin_t::FillBuffer(IMediaSample *pData)
{
	return S_FALSE;
}

HRESULT 
SourcePin_t::CheckMediaType(const CMediaType *pmt)
{
	return S_OK;
}

HRESULT
SourcePin_t::GetMediaType(CMediaType *pMediaType)
{
	    *pMediaType = m_mt;
		return S_OK;
}

void
SourcePin_t::SendSample(BYTE * pBuffer,  unsigned long BufferLen)
{		
	//--Volcado de datos del pin de entrada--
	HRESULT  hr = m_Connected->BeginFlush();
	errorCheck(hr);				
	hr = m_Connected->EndFlush();
	errorCheck(hr);				
	//----------------
	
	{
		CAutoLock lock_it(m_pLock);			
		IMediaSample * Sample  = NULL;
		GetDeliveryBuffer(&Sample,NULL,NULL,0);
		CMediaSample * CSample = (CMediaSample *)Sample;
    	CSample->SetSyncPoint(1);
		CSample->SetPointer(pBuffer,BufferLen);
		
		//--Pasamos nuestro nuevo sample--//
		hr = Deliver(Sample);
		errorCheck(hr);				
		//liberamos el sample
		CSample->Release();
	}
}