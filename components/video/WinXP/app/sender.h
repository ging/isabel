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
   <name>sender.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_SENDER_H
#define _IVIDEO_SENDER_H

#include "fragmenter.h"
#include "channel.h"
#include "BWControl.h"
#include <stdlib.h>
#include <time.h>


/**
 <class> 
   <name>SampleGrabber_t</name> 
   <descr>
   This class receives the video frames and sends them to the   
   places saved in BindList, it calls the BWController_t's      
   method WatchBW to control the transmision rate.	
   </descr>
**/
class sampleGrabber_t : public ISampleGrabberCB,
						public item_t
{
private:

	#if DBG_SAVE_FRAMES
		fileIO_t * dbgFile;
	#endif

	//channel pointer
	channel_t * channel;
	fragmenter_t * fragmenter;
	unsigned long timeStamp;
	int  seqNum;
	int lastSeqNum;
	double lastTime;
	double timeInc;
	int payLoad;
	int bucketSize;

private:

	int  checkPayLoadType(AM_MEDIA_TYPE videoType)
	{
		if (videoType.subtype == (GUID)FOURCCMap(FCC('XVID')) ||
			videoType.subtype == (GUID)FOURCCMap(FCC('DIVX')) ||
			videoType.subtype == (GUID)FOURCCMap(FCC('FVFW')) ||
			videoType.subtype == (GUID)FOURCCMap(FCC('DX50')))
			
		{
			payLoad = __MPEG4; 
		}

		if (videoType.subtype == (GUID)FOURCCMap(FCC('MPEG')) ||
			videoType.subtype == (GUID)FOURCCMap(FCC('MPG1')))
		{
			payLoad = __MPEG1;
		}

		if (videoType.subtype == (GUID)FOURCCMap(FCC('H263'))) 
		{
			payLoad = __H263;
		}

		return payLoad;
	}


public:
   
	//bandwidth control
	BWController_t * BWController;
	
#if 0 // (obsolete code) -> TODO: Erase

	void SetSendVideoType(void)
	{
		SendVideoType = true;
		return;
	}

#endif


	sampleGrabber_t(channel_t * owner)
	{	
		#if DBG_SAVE_FRAMES
			dbgFile = new fileIO_t("frames.hex",modeWrite|modeCreate);
		#endif

		channel = owner;
		BWController = new BWController_t(owner,DEFAULT_CHANNEL_BW); 
		fragmenter = new fragmenter_t();
		lastTime = 0.0;
		timeInc  = 0.0;
		seqNum = 0;
		lastSeqNum = 0;
		initTimeStamp();
	    initSeqNum();
		payLoad = __MPEG4; // MPEG4 Payload
		bucketSize = DEFAULT_MTU_SIZE;
		
	}

	HRESULT SetPayLoad(char * payLoad)
	{
		if (!strcmp("MPEG4",payLoad))
		{
			this->payLoad = __MPEG4;
			return S_OK;
		}

		if (!strcmp("MPEG1",payLoad))
		{
			this->payLoad = __MPEG1;
			return S_OK;
		}
		
		return -1;
	}
	
	void initTimeStamp(void)
	{
		srand( (unsigned)time( NULL ) );
		timeStamp = rand();
		debugMsg(dbg_App_Normal,
			 "sampleGrabber_t",
			 "InitTimeStamp :: timeStamp = %d",
			 timeStamp);	
		return;
	}

	void initSeqNum(void)
	{
	while (lastSeqNum <= seqNum + 5000 &&
		   lastSeqNum >= seqNum - 5000)
		{
			seqNum = rand();
			debugMsg(dbg_App_Normal,
				 "sampleGrabber_t",
				 "InitSeqNum :: seqNum = %d",
				 seqNum);
		}
			lastSeqNum = seqNum;
		return;
	}
	
	void setPayLoad(int newPayLoad)
	{
		payLoad = newPayLoad;
		return;
	}

	int getPayLoad(void)
	{
		return payLoad;
	}

	void
	timeToDie(void)
	{
		BWController->Release();
		delete fragmenter;
		delete this;
	}

	virtual ~sampleGrabber_t(void)
	{
		#if DBG_SAVE_FRAMES
			dbgFile->close();
			delete dbgFile;
		#endif
	}

	// Fake referance counting.
    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
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

    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
        return E_NOTIMPL;
    }

    virtual STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);

	HRESULT SetBucketSize(int newBucketSize)
	{
		if (newBucketSize>0)
		{
			bucketSize = newBucketSize;
			return S_OK;
		}else{
			return E_FAIL;
		}
	}
     
};
//</class>

/**
 <class> 
   <name>sender_t</name> 
   <descr>
   This class creates codec filter and SampleGrabber_t and      
   connects then into the pGraph's chain. After SampleGrabber_t 
   filter the frames are re-decodec and rendered. 	
   </descr>
**/
class sender_t: public item_t
{

private:

	// Channel pointers
	
	channel_t * channel;
	
	
	// Filter poniters
	
	IBaseFilter *pCodec;
	IBaseFilter *pSampler;
	ISampleGrabber  * IGrabber;
	
	// Output Type
	
	AM_MEDIA_TYPE videoType;


	// Bind Info
	int			ID;
	struct bindInfo_t{	
		int ID;
		dgramSocket_t * socket;
	};
	
	dictionary_t<int,bindInfo_t *> bindList;
	int videoExt;
	
	//Codec Info
	char  codecList[2000];
	char  actualCodec[100];
	
	// auxiliars
	char auxStr[2000];

public:

	sampleGrabber_t * sampleGrabber;

private:
	
	void init_codec_list(void);
	HRESULT add_external_filter(wchar_t *name, IBaseFilter **ppFilter);
	void createSampler(void);
	void connectSender(void);
	void interConnect(void);
	


public:

	sender_t(channel_t * pChannel);
	virtual ~sender_t (void);
	//
    // codecs
    //
    HRESULT video_select_codec();
	char * GetCodecList(void);
	void SetActualCodec(char *);
    char * GetActualCodec(void);
    //char * video_describe_codec(char* codec);

	//
	// bind
	//
    int video_bind(char * dest,char* port);
    HRESULT video_unbind(int ID);
    char *  video_show_bindings(void);
	bool set_bandwidth(long);
	void RunBWControl(bool);
	void rebind();

	//
	// Video 
	//
	void GetVideoType(AM_MEDIA_TYPE *);
	int  GetVideoExtension();
	void SetVideoExtension(int);
	HRESULT SetBucketSize(int);
	
};

//</class>

#endif
