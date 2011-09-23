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
   <name>samplereader.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef IVIDEO_SAMPLE_READER_H
#define IVIDEO_SAMPLE_READER_H

#include "general.h"
#include "channel.h"

#define FRAME_CONTAINER_SIZE 4

/**
 <class> 
   <name>frameContainer_t</name> 
   <descr>
   This class is used by sampleReader_t as frames buffer.
   </descr>
**/
class frameContainer_t
{
private:

	int frameCount;
	int actualFrame;
	frame_t * frameBuff[FRAME_CONTAINER_SIZE];

public:

	frameContainer_t(void);
	void freeContainer(void);
	void insert(frame_t * newFrame);
	frame_t * popFrame(void);
};


class SourcePin_t: public CSourceStream
{
public:

	SourcePin_t(TCHAR *pObjectName,
				HRESULT *phr,
                CSource *pms,
                LPCWSTR pName);

	virtual ~SourcePin_t(void);

    virtual HRESULT DecideBufferSize(IMemAllocator *pAlloc,
								     ALLOCATOR_PROPERTIES *ppropInputRequest);
	virtual HRESULT FillBuffer(IMediaSample *pData);	
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
	virtual HRESULT GetMediaType(CMediaType *pMediaType);
	void    SendSample(BYTE * pBuffer,  unsigned long BufferLen);
};

class sampleReader_t : public CSource					   
{

private:

	SourcePin_t * OutPin;
	Event_t Event;
	frameContainer_t * frameList;
	
	HANDLE mutex;
    HANDLE mutex2;

    HANDLE hThread;
    DWORD  dwThreadId;

    void __Run(void);

public:

	sampleReader_t(TCHAR *pName,LPUNKNOWN lpunk,CLSID clsid);
	virtual ~sampleReader_t(void);
	void TimeToDie(void);
	void receiveFrame(frame_t * frame);
	virtual void Free(void){;}
	inline IPin *  getOutputPin(void){return OutPin;};
    static DWORD WINAPI threadStart(void *);
    void Run(void);	
    void EndThread(void);
	virtual HRESULT SetMediaType(CMediaType * videoType);
	virtual HRESULT GetMediaType(CMediaType *pMediaType);
};
 
#endif

