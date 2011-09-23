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
   <name>sender.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of sender.h.
   </descr>
//</head>
**/

#include "general.h"
#include "sender.h"
#include "recChannel.h"
#include "videoRTPSession.h"
#include "sharedDisplay.h"
#include "MPEGimp.h"
#include "W32reg.h"


STDMETHODIMP 
sampleGrabber_t::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{

	// This code saves the frame in a file (frames.hex).
	// It's used to study carefully MPEG4's headers 
	// to use as explained in RFC 3016 

#if DBG_SAVE_FRAMES

		char * str = "--------NEW FRAME------\n";
		dbgFile->write(str,strlen(str));
		for (int k=0;k<BufferLen;k++)
		{
			BYTE byte = *(pBuffer + k);
			char num[10];
			itoa(k,num,10);
			strcat(num,":\t");
			dbgFile->write(num,strlen(num));
			for (int j=0;j<8;j++)
			{
				if ((byte >> 7-j) & 0x01)
					dbgFile->write("1",1);
				else
					dbgFile->write("0",1);
			}
			dbgFile->write(":\t",2);
			char hexNum[4];
			sprintf(hexNum,"%p",byte);
			dbgFile->write(hexNum+6,2);
		    dbgFile->write("\n",1);
		}

#endif

		recChannel_t * recChannel = static_cast<recChannel_t *>(channel);
	
		if (recChannel->get_camInfo()->getKind() != CAM &&
			recChannel->get_camInfo()->getKind() != SHARED)
		{
			fileSource_t * fileSource = static_cast<fileSource_t *>(recChannel->get_camInfo());
			fileSource->setCurTime(Time);
		}

		// Get Width and Height
		u16 auxWidth  = 0;
		u16 auxHeight = 0;
 		AM_MEDIA_TYPE auxVideoType;
		recChannel->pSender->GetVideoType(&auxVideoType);
		
		checkPayLoadType(auxVideoType);

		if (auxVideoType.pbFormat)
		{
			VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) auxVideoType.pbFormat;
			auxWidth  = pVih->bmiHeader.biWidth;
			auxHeight = pVih->bmiHeader.biHeight;
			/*NOTIFY("Sending :: Width = %d, Heigth = %d, Offset = %d \n",
			          pVih->bmiHeader.biWidth,
					  pVih->bmiHeader.biHeight,
					  (fragmenter->getOffset() - size));*/

		}else{
			captureInfo_t auxCapInfo = recChannel->get_captureInfo();
			auxWidth  = auxCapInfo.width;
			auxHeight = auxCapInfo.heigth;
		}
	

		// Now we make the RTP packet
		fragmenter->setFrame(pBuffer,BufferLen,payLoad,auxWidth,auxHeight);
	    timeInc = Time - lastTime;
		
		if (recChannel->get_camInfo()->getKind() == TEST)
			timeStamp += (1.0/30.0)*VIDEO_TIMESTAMP_INV;
		else
			timeStamp += timeInc*VIDEO_TIMESTAMP_INV;

		bool lastFragment = false;
		bool padding = false;
		int size = bucketSize - DEFAULT_PAYLOAD_HEAD; //rtpSession->getMaxPacketSize(channel->getId(), 0);
		BYTE * fragment= new BYTE[DEFAULT_MTU_SIZE];
		memset(fragment,0,size);
		
		int length = 0;
        int previousLength = 0;

		u16   extID = 0;
		u16   extLength = 0;
		BYTE *extData = NULL;

		int payloadHeaderLength  = 0;
		if (payLoad == __MPEG4)
				payloadHeaderLength  = 0;
		if (payLoad == __MPEG1)
				payloadHeaderLength  = sizeof(MPEG1Header_t);
		if (payLoad == __H263)
				payloadHeaderLength  = sizeof(H263_0Header_t);


		int n = 0,i=0;
   		while ((n=fragmenter->getFragment((unsigned char **)(&fragment),&size,payloadHeaderLength ))>=0) 
		{
			i++;
			if (!n){
              
				lastFragment = true;
				padding = false;
			//	NOTIFY("Sender_t :: Last Packet, n = %d, timeStamp = %d\n",i,timeStamp);
			 
			}
			
			rtpSession->sendData(channel->getId(),
			                     fragment,
			                     size,
			                     getPayLoad(),
			                     lastFragment,
                                 seqNum,
			                     timeStamp,
                                 padding); 

            seqNum++;
            previousLength = size;
			
			// Here we call bandwidth control 
			BWController->watchBW(size*8); //Data in Kbits
			memset(fragment,0,size);
			
		}
		lastTime = Time;
		delete fragment;
			
		return 0;
}


sender_t::sender_t(channel_t * pChannel)
{
		ID = 0;
		pCodec   = NULL;
		pSampler = NULL;
		IGrabber = NULL;
		memset(actualCodec,0,100);
		strcpy(actualCodec,DEFAULT_CODEC_STR);
		this->channel = pChannel;
		channel->pControl->StopWhenReady();
		init_codec_list();
		sampleGrabber = new sampleGrabber_t(channel);
			
}

HRESULT
sender_t::add_external_filter(wchar_t * name,IBaseFilter ** ppFilter)

{
	
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&pSysDevEnum);

	if (FAILED(hr))
	{
		return hr;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
										(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
            
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				int i;
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
				if ((i=wcscmp(varName.bstrVal,name))==0){
					IBaseFilter *pFilter;
					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
											   (void**)&pFilter);
					*ppFilter=pFilter;
					break;
				}
			}
            VariantClear(&varName);
            // To create an instance of the filter, do the following:
            pPropBag->Release();
        }
        pMoniker->Release();
    }
    pEnumCat->Release();
	}

pSysDevEnum->Release();
return hr;

}

int
sender_t::GetVideoExtension(void)
{
	return videoExt;
}

void
sender_t::SetVideoExtension(int newVideoExt)
{
	videoExt = newVideoExt;
}

void
sender_t::init_codec_list(void){
	
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	int i=0;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
    IID_ICreateDevEnum, (void **)&pSysDevEnum);
	errorCheck(hr);
	
	memset(codecList,0,2000);
	codecList[0]='{';
	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
										(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
            
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					char codec[100];
					memset(codec,0,100);
					wcstombs(codec,varName.bstrVal,100);
					strcat(codecList,codec);
					strcat(codecList,";");
					VariantClear(&varName); 
				}
			}
			
            // To create an instance of the filter, do the following:
            pPropBag->Release();
        }
		strcat(codecList,"}");
        pMoniker->Release();
    }
    pEnumCat->Release();
	pSysDevEnum->Release();
}

HRESULT
sender_t::video_select_codec()
{
	int hr = 0;
	wchar_t lName[100];
	channel->pControl->StopWhenReady();

	lText(lName,actualCodec);
	
	if (pCodec!=NULL)
	{
			hr = channel->pGraph->RemoveFilter(pCodec);
			pCodec->Release();
	}

	hr = add_external_filter(lName,&pCodec);
	
	if (pCodec == NULL)
	{
			hr = add_external_filter(DEFAULT_CODEC,&pCodec);
	}

	hr = channel->pGraph->AddFilter(pCodec,L"Codec");
	
	debugMsg(dbg_App_Normal, 
			 "sender_t",
			 "%s loaded",
			 actualCodec);


	connectSender();
	
	return hr;
}

int 
sender_t::video_bind(char * dest,char* port)
{

    return rtpSession->rtpBind(/*channel->getId(),*/ dest , port);
}

HRESULT 
sender_t::video_unbind(int ID)
{
	return rtpSession->rtpUnbind(/*channel->getId(),*/ ID);
}

char *  
sender_t::video_show_bindings(void)
{
	return rtpSession->rtpShowBindings(/*channel->getId()*/);
}


void
sender_t::createSampler(void)
{
	
	// Sampler
	int hr = 0;
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, 
						  IID_IBaseFilter, (void **)&pSampler);


	hr = pSampler->QueryInterface(IID_ISampleGrabber, (void **)&IGrabber);
	IGrabber->SetBufferSamples(true);
	IGrabber->SetCallback(sampleGrabber,1);

}

void
sender_t::connectSender(void)
{
    __CONTEXT("sender_t::connectSender");
	int hr = 0;
	//channel->pControl->StopWhenReady();
	IPin * aux    = NULL;
	IPin * input  = NULL;
	IBaseFilter * pFilter = NULL;
	sharedDisplayPin_t * output = NULL;
	AM_MEDIA_TYPE mediaType;


	recChannel_t * recChannel = static_cast<recChannel_t *>(channel);
	if (recChannel->get_camInfo()->getKind() == SHARED)
	{
		output = static_cast<sharedDisplayPin_t *>(recChannel->pOutput);
		output->Disconnect();
		output->GetMediaType((CMediaType *)&mediaType);
	}

	hr = GetUnconnectedPin(pCodec, PINDIR_INPUT ,&input);
	channel->pOutput->Disconnect();
	input->Disconnect();

	if (output)
	{
        VIDEOINFOHEADER * videoInfo = (VIDEOINFOHEADER *)mediaType.pbFormat;

		hr = output->Connect(input,&mediaType);
        errorCheck(hr);
	}else{
		hr = channel->pGraph->Connect(channel->pOutput,input);
        errorCheck(hr);
	}
	
	interConnect();
	GetPin(pSampler,PINDIR_OUTPUT,&(channel->pOutput));	
}

void
sender_t::rebind()
{
	video_select_codec();
}

void
sender_t::interConnect(void)
{
    __CONTEXT("sender_t::interConnect");
	IPin * aux    = NULL;
	IPin * input  = NULL;
	IPin * output = NULL;
	IBaseFilter * pFilter = NULL;

    channel->pControl->StopWhenReady();
	int hr = 0;
	int i = 0;
	
    while ((hr = channel->pGraph->FindFilterByName(L"Sampler",&pFilter))==0)
	{
		hr = channel->pGraph->RemoveFilter(pFilter);
        errorCheck(hr);
		hr = pFilter->Release();
        errorCheck(hr);
        i++;
        if (i>20)
        {
            NOTIFY("[sender_t::interConnect]WARNING :: Can't delete pSampler");
            break;
        }
	}

    
	createSampler();
	
	hr = channel->pGraph->AddFilter(pSampler,L"Sampler");
	errorCheck(hr);

	if((hr= GetPin(pSampler ,PINDIR_INPUT,&input))==0)
    {
		if ((input->ConnectedTo(&aux))==0)
        {
			 input->Disconnect();
             aux->Disconnect();
        }
    }
    if((hr = GetPin(pCodec ,PINDIR_OUTPUT,&output))==0)
    {
        output->Disconnect();
    }

    hr = channel->pGraph->Connect(output,input);
    errorCheck(hr);
    if (FAILED(hr))
    {
        exit(-1);
    }

	//Save video output type
	hr = output->ConnectionMediaType(&videoType);
	errorCheck(hr);
}

bool    
sender_t::set_bandwidth(long newBW)
{
		return sampleGrabber->BWController->setBW(newBW);
}

void
sender_t::RunBWControl(bool run)
{
		if (run)
			sampleGrabber->BWController->set_period(DEFAULT_BW_PERIOD);
		else
			sampleGrabber->BWController->set_period(0);
}

void
sender_t::GetVideoType(AM_MEDIA_TYPE * auxVideoType)
{
	memcpy(auxVideoType,&videoType,sizeof(AM_MEDIA_TYPE));
	return;
}


sender_t::~sender_t(void)
{
	sampleGrabber->timeToDie();
	if (pCodec)
	{
		channel->pGraph->RemoveFilter(pCodec);
		pCodec->Release();
	}

	debugMsg(dbg_App_Normal,
			 "sender_t",
			 "~sender_t");

			 
}

char *
sender_t::GetCodecList(void)
{
	strcpy(auxStr,codecList);
	return auxStr;
}

void
sender_t::SetActualCodec(char * newCodec)
{

#if 0
	if (newCodec != NULL)
	{
		strcpy(actualCodec,newCodec);
	}
#else
    // if codec == ffdShow we can access BW Codec param
    // in windows register.
	recChannel_t * recChannel = static_cast<recChannel_t *>(channel);
    if (strcmp(recChannel->pSender->GetActualCodec(),"ffdshow Video Codec")==0 && 
        strstr(SUPPORTED_CODECS,newCodec)>=0) 
    {
        registry_t winReg("HKEY_CURRENT_USER/Software/GNU/ffdshow_enc");
        DWORD codecID,oldCodecID;
        DWORD fourCC,oldFourCC;

        codecID = -1;

        if (strcmp("MPEG4",newCodec)==0 ||
            strcmp("MPEG4::MPEG4",newCodec)==0)
        {
            codecID = 10;
            fourCC = 0x44495658;
        } else if (strcmp("MPEG1",newCodec)==0 ||
                   strcmp("MPEG1::MPEG1",newCodec)==0)
        {
            codecID = 1;
            fourCC = 0x4745504d;
            
        }else if (strcmp("H263",newCodec)==0 ||
                  strcmp("H263::H263",newCodec)==0)
        {
            codecID = 2;
            fourCC = 0x33363248;
        }
        
        if (codecID==-1)
            return;

        winReg.read("codecId",(BYTE *)&oldCodecID,sizeof(DWORD));
        winReg.read("fourcc",(BYTE *)&oldFourCC,sizeof(DWORD));

        if(codecID!= oldCodecID || 
           fourCC != oldFourCC)
        {
                NOTIFY("Sender_t::NEW CODEC = %s\n",newCodec);
                winReg.write("codecId",(BYTE *)&codecID,sizeof(DWORD),REG_DWORD);
                winReg.write("fourcc",(BYTE *)&fourCC,sizeof(DWORD),REG_DWORD);

                recChannel->set_all(false);
		    	recChannel->select_source(recChannel->get_camInfo()); 
        }
     }
#endif
}

char *
sender_t::GetActualCodec(void)
{
    return actualCodec;
}

HRESULT
sender_t::SetBucketSize(int newBucketSize)
{ 
	
	return sampleGrabber->SetBucketSize(newBucketSize);

}


