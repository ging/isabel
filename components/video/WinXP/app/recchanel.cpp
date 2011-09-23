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
   <name>recchannel.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of recchannel.h.
   </descr>
//</head>
**/

#include "recchannel.h"
#include "namedguid.h"
#include "videoRTPSession.h"

recChannel_t::recChannel_t(
		int ID,			//channel ID
        const char *title, //Window Title
        windowInfo_t *geom //Window info
		):channel_t(ID,title,geom){

    __CONTEXT("recChannel_t::recChannel_t");
   
    int hr = 0;
	frameRate = 0;
	pSource  = NULL;
	pSender  = NULL;
#ifdef _WINDOWS
    fControl = NULL;
    hThread = 0;
    hThreadPos = 0;
#endif
	frameRate = 1.0;
	all = true;


	looper = new looper_t(this);

	Kind = REC;
	
	camInfo = NULL;
	capInfo.heigth = DEFAULT_CAPTURE_HEIGTH;
	capInfo.width  = DEFAULT_CAPTURE_WIDTH;

	if (freeSources())
	{
		for (int j = 0; j < camArray.size(); j++)
		{
			camInfo_t *camInfo= camArray.elementAt(j);
			if (camInfo->getFree() && camInfo->getKind() == CAM)
			{
				sourceId = camInfo->getID();
				memset(sourceFormat, 0, 100);
				all = true;
				hr = select_source(camInfo);
				camInfo->setFree(false);
				break;
			}
		}
	}
	else
	{
		set_file_source(NULL);
	}

	errorCheck(hr);	

	pSender = new sender_t(this);
	channelList->insert(getId(),this);

	rtpSession->assignSender(getId(), false, static_cast<sched_t*>(NULL));
}

#ifdef _WINDOWS
//file control methods
void 
recChannel_t::fileControlThread(void)
{
    //file control dialog box
    if (!fControl)
    {
        fControl = new fileControl();
    }

    MSG Message;
    
    while(TRUE == GetMessage(&Message, 0, 0, 0))
    {
	      TranslateMessage(&Message);
	      DispatchMessage(&Message);
    }
}

void 
recChannel_t::posReaderThread(void)
{
    __int64 position = 0;
    __int64 duration = 0;

    Sleep(1000);
    while (!fControl)
    {
        Sleep(1000);
    }

    fControl->slideMoved = true;

    IMediaSeeking * pSeek = NULL;
    while(true)
    {
        int hr = pGraph->QueryInterface(IID_IMediaSeeking,(void **)&pSeek);
        //read file pos and put at slicer 
        if (pSeek && hr == S_OK)
        {
            if (!fControl->play & fControl->event)
            {
                pControl->StopWhenReady();
                fControl->event = false;
                continue;
            }

            if (fControl->play & fControl->event)
            {
                pControl->Run();
                fControl->event = false;
            }
                
            if (fControl->slideMoved)
            {
                position = fControl->m_slide.GetPos();
                position *=1000000;
                fControl->slideMoved = false;
                pSeek->SetPositions(&position,
                                     AM_SEEKING_AbsolutePositioning,
                                     NULL,
                                     AM_SEEKING_NoPositioning);
            }else{

                if (!duration)
                {
                    pSeek->GetDuration(&duration);
                    duration /= 1000000;
                    fControl->m_slide.SetRangeMin(0,true);
                    fControl->m_slide.SetRangeMax(duration,true);
                }
                pSeek->GetCurrentPosition(&position);
                position /= 1000000;
                fControl->m_slide.SetPos(position);
            }
        }
        pSeek->Release();
        Sleep(1000);
    }
}
  
void
recChannel_t::RunThread(void)
{
if (!fControl)
{
   	hThread = CreateThread( 
				NULL,              // no security attribute 
				0,                 // default stack size 
				Run, //Routine 
				(void *)this,      // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 
}

  	hThreadPos = CreateThread( 
				NULL,              // no security attribute 
				0,                 // default stack size 
				RunPos, //Routine 
				(void *)this,    // thread parameter 
				0,                 // not suspended 
				&dwThreadIdPos); // returns thread ID 

}

void 
recChannel_t::EndThread(void)
{
    if (hThreadPos)
    {
        TerminateThread(hThreadPos,0); 
    }
    if (fControl)
    {
        fControl->CWnd::ShowWindow(SW_HIDE);
    }
    //TerminateThread(hThread,0); 
}

DWORD WINAPI 
recChannel_t::Run(void * param)
{
    recChannel_t * recChannel = static_cast<recChannel_t *>(param);
    recChannel->fileControlThread();
    return 0;
}

DWORD WINAPI 
recChannel_t::RunPos(void * param)
{
    recChannel_t * recChannel = static_cast<recChannel_t *>(param);
    recChannel->posReaderThread();
    return 0;
}

#endif
  
int 
recChannel_t::select_source(camInfo_t * source)
{
    __CONTEXT("recChannel_t::select_source");

    if (camInfo)
    {
        if (camInfo->getKind() == TEST) 
        {
            looper->EndThread();
        }
#ifdef _WINDOWS        
        if (camInfo->getKind() == MEDIA) 
        {
            EndThread();
        }
#endif
        camInfo->setFree(true);
    }
    
	int hr = 0;
	
    pControl->StopWhenReady();
    //pControl->Stop();
	
    bool sharedDisplaySource = false;
	RECT sharedDisplayRect;
	char auxName[100];

	if (source != NULL)
	{
		if (!(source->getKind() == CAM || source->getKind() == SHARED))
		{
			capInfo.heigth = 0;
			capInfo.width  = 0;
		}else{

			if (source->getKind() == CAM)
			{
				if (!capInfo.heigth) 
					capInfo.heigth = DEFAULT_CAPTURE_HEIGTH;
				if (!capInfo.width)
					capInfo.width  = DEFAULT_CAPTURE_WIDTH;
			}
			if (source->getKind() == SHARED)
			{
					sharedDisplay_t * sharedDisplay = static_cast<sharedDisplay_t *>(source);
					sharedDisplayRect = sharedDisplay->getSharedRect();
					capInfo.heigth = sharedDisplayRect.bottom - sharedDisplayRect.top;
					capInfo.width  = sharedDisplayRect.right - sharedDisplayRect.left;
                    mapping = true; //always mapping shDisplay channel
					sharedDisplaySource = true;
					strcpy(auxName,source->getCamName());
					
			}
		}
	}
	refresh_channel(all);

	if (sharedDisplaySource && pSource == NULL)
	{
		camInfo_t *camInfo = createSharedDisplay(this,auxName);
		sharedDisplay_t *sharedDisplay = static_cast<sharedDisplay_t *>(camInfo);		
		sharedDisplay->setSharedRect(sharedDisplayRect);
	}
#ifdef _WINDOWS
    if (source->getKind() == MEDIA) 
    {   
        if (fControl)
        {
            if (source!=camInfo)
            {
                fControl->m_slide.SetPos(0);
            }
        }
        RunThread();
    }
#endif

	camInfo = source;
	pOutput = camInfo->output;
	pOutput->Disconnect();
	sourceId = camInfo->getID();
	
	if (!strlen(sourceFormat))
	{
		memset(sourceFormat,0,100);
		char supportedFormats[100];
		strcpy(supportedFormats,camInfo->getSupportedFormats());
		for (int j=1;supportedFormats[j]!=';';j++)
				sourceFormat[j-1]=supportedFormats[j];	
		ql_t<AM_MEDIA_TYPE *> auxFormats = camInfo->getFormatList();
		actualFormat = *(auxFormats.nth(0));
        for (int k=0;k<auxFormats.len() ; k++)
        {
            AM_MEDIA_TYPE format = *(auxFormats.nth(k));
            char subtypeName [100];
	    	memset(subtypeName,0,100);
		    GetGUIDString(subtypeName,&format.subtype);	
	        if (strcmp(subtypeName,"MEDIASUBTYPE_RGB24")==0)
            {
                actualFormat = format;
                strcpy(sourceFormat,"MEDIASUBTYPE_RGB24");
            }
        }
	}
	pSource = camInfo->pSource;
    
	hr = pGraph->AddFilter(pSource, L"Capture Video Source");

	errorCheck(hr);
	
	hr = grab_geometry(capInfo);
	camInfo->setFree(false);	
    //leave critical section
	return hr;

}


int 
recChannel_t::grab_geometry(captureInfo_t wInfo)
{

     __CONTEXT("recChannel_t::grab_geometry");
	pControl->StopWhenReady();
	capInfo.width = wInfo.width;
	capInfo.heigth = wInfo.heigth;
	int hr = source_format(sourceFormat);

	return hr;
}


char * 
recChannel_t::list_source_formats(void)
{
     __CONTEXT("recChannel_t::list_source_formats");
	return camInfo->getSupportedFormats();
}


int 
recChannel_t::source_format(char* newFormat)
{
     __CONTEXT("recChannel_t::source_format");

	int hr = 0;
    bool formatFound = false;

	IAMStreamConfig *pConfig = NULL;
	AM_MEDIA_TYPE * format = NULL;
	
	pControl->StopWhenReady();

    ql_t<AM_MEDIA_TYPE *> auxFormats = camInfo->getFormatList();
	
    for(int i = 0; i<auxFormats.len() ; i++)
    {
		AM_MEDIA_TYPE format = *(auxFormats.nth(i));
		IAMStreamConfig *pConfig = NULL;
		IVideoWindow * pWindow = NULL;
		
		char subtypeName [100];
		memset(subtypeName,0,100);
		GetGUIDString(subtypeName,&format.subtype);	
	
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) format.pbFormat;
		
		if((pVih==NULL && strcmp(newFormat,sourceFormat)==0 )||
		   (pVih->bmiHeader.biHeight == capInfo.heigth &&
		    pVih->bmiHeader.biWidth == capInfo.width &&
		    strcmp(subtypeName,newFormat)==0) || 
			camInfo->getKind() == SHARED
			)
		{
		
			if (strcmp(sourceFormat,newFormat))
			{
				memset(sourceFormat,0,100);
				strcpy(sourceFormat,newFormat);
			}
			
			if (!hr && (camInfo->getKind() == CAM || camInfo->getKind() == SHARED)){
				camInfo->output->Disconnect();
				hr = camInfo->output->QueryInterface(IID_IAMStreamConfig, (void**)&pConfig);
			    //pVih->AvgTimePerFrame = 666666;//
                pVih->AvgTimePerFrame = 333333/(frameRate);
				int hr = pConfig->SetFormat(&format);
				actualFormat = format;	
				pConfig->Release();
			}
	        formatFound = true;
			break;
		}
	    
    }
	
    if (!formatFound)
    {
        IAMStreamConfig *pConfig = NULL;
		if (camInfo->getKind() == CAM || 
            camInfo->getKind() == SHARED)
        {
            VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) actualFormat.pbFormat;
            camInfo->output->Disconnect();
            hr = camInfo->output->QueryInterface(IID_IAMStreamConfig, (void**)&pConfig);
            //pVih->AvgTimePerFrame = 666666;
            if (pConfig)
            {
                int hr = pConfig->SetFormat(&actualFormat);
                pConfig->Release();
            }
        }
    }
        
	    NOTIFY("reChannel_t"
			   "\r\n=========================================\r\n"
               "Channel %d : Source Description...\r\n"
               "- sourceName: %s\r\n"
               "- capture Size: %dx%d\r\n"
               "- supported Formats: %s\r\n"
               "- Window Info: (%d,%d,%d,%d)\r\n"
               "- Title: %s\r\n"
               "=========================================\r\n",
               getId(),
               camInfo->getCamName(),
               capInfo.width,
               capInfo.heigth,
               camInfo->getSupportedFormats(),
               windowInfo.top,
               windowInfo.left,
               windowInfo.width,
               windowInfo.heigth,
               title);

	remap();

	if (mapping){
		map();
	}
	return 0;
		
}

HRESULT 
recChannel_t::map(void)
{

    __CONTEXT("recChannel_t::map");
       
	int hr = 0;
	IBaseFilter * pFilter = NULL;
	IBaseFilter * pFilter2 = NULL;
	IPin * pVideoInputPin = NULL;
	pControl->StopWhenReady();
	
	mapping = true;
	pOutput = camInfo->output;


	if (remaped){
		
	    //refresh Codec BW before creation
        pSender->sampleGrabber->BWController->refreshBW();
		pSender->rebind();
	
		hr = pGraph->Render(pOutput);
		{
				
				// Enumerate the filters in the graph.
				IEnumFilters *pEnum = NULL;
				int hr = pGraph->EnumFilters(&pEnum);
				if (SUCCEEDED(hr))
				{
					IBaseFilter *pFilter = NULL;
					pEnum->Reset();
					while (S_OK == pEnum->Next(1, &pFilter, NULL))
					{
						CLSID filterId;
						pFilter->GetClassID(&filterId);
						if(filterId == CLSID_AviSplitter)
			   			{

							IEnumPins * pEnumpin = NULL;
								
							hr = pFilter->EnumPins(&pEnumpin);
							if (!hr)
							{
								IPin * pPin = NULL;
								pEnumpin->Reset();
								while (pEnumpin->Next(1, &pPin, 0) == S_OK)
								{
									bool break_loop = false;
									AM_MEDIA_TYPE * mediaType;
									IEnumMediaTypes * enumMedia = NULL;
						
									hr = pPin->EnumMediaTypes(&enumMedia);
									if(!hr)
									{
										enumMedia->Reset();
										while(enumMedia->Next(1,&mediaType , NULL) == S_OK)
										{
											if (mediaType->majortype == MEDIATYPE_Audio)
											{
												pPin->Disconnect();
												pGraph->Render(pPin);
												pPin->Release();
												break_loop = true;
												break;
											}
										}
										enumMedia->Release();
										if (break_loop)
											break;
									}
								}
								pEnumpin->Release();
							}
							
						}
						pFilter->Release();
					}
					pEnum->Release();
				}
		}

		pipeCreated = true;
	
		if (hr)
		{
				errorCheck(hr);
				NOTIFY("[recChannel_t::map]WARNING :: Can't render actual format, restoring default settings...\r\n");
				capInfo.heigth = DEFAULT_CAPTURE_HEIGTH;
				capInfo.width = DEFAULT_CAPTURE_WIDTH;
				ql_t<AM_MEDIA_TYPE *> auxFormats = camInfo->getFormatList();
				pSender->SetActualCodec(DEFAULT_CODEC_STR);
		}
	}

	if (fullScreen){
		set_full_screen(true);
	}else{
		hr = setWindowGeometry(windowInfo);
		errorCheck(hr);
	}

//	IVideoWindow *pWindowInfo = NULL;
//	hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
//	if (!hr)
//	{
//		wchar_t wtext[100];
//		long windowStyle,windowStyleEx;
//		lText(wtext,title);
//		pWindowInfo->get_WindowStyle(&windowStyle);
//        pWindowInfo->get_WindowStyleEx(&windowStyleEx);
//		windowStyle = windowStyle + DEFAULT_WINDOW_PROPS - DEFAULT_WINDOW_NON_PROPS;
//		windowStyleEx = windowStyleEx - WS_EX_APPWINDOW;
//		pWindowInfo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
//        pWindowInfo->put_WindowStyleEx(WS_EX_TOOLWINDOW);
//		pWindowInfo->put_Caption(wtext);
//
//#ifdef _WINDOWS
//        if (camInfo->getKind() == MEDIA)
//        {
//            fControl->setGeometry(windowInfo);
//
//        }
//#endif  	
////Ares daemon don't show local windows on
////recChannels
//#ifndef __ARES		
//		if (camInfo->getKind() != SHARED)
//		{
//			pWindowInfo->put_Visible(OATRUE);
//			pWindowInfo->put_AutoShow(OATRUE);
//		}
//		else
//		{
//#endif
//			pWindowInfo->put_Visible(OAFALSE);
//			pWindowInfo->put_AutoShow(OAFALSE);
//#ifndef __ARES
//		}
//#endif
//
//		pWindowInfo->Release();
//		setOwner();
//	}
	
	IMediaSeeking * pSeek = NULL;
    pGraph->QueryInterface(IID_IMediaSeeking,(void **)&pSeek);
    if (pSeek)pSeek->SetRate(1);
        
	pControl->Run();

	if (camInfo->getKind() == SHARED)
    {
		camInfo->RunSource();
    }
		
	if (camInfo->getKind() == TEST) 
    {        
        if (pSeek) pSeek->SetRate(0.5);
        looper->Run();
    }
	
    remaped = false;
	return hr;
}

HRESULT 
recChannel_t::unmap(void)
{
     __CONTEXT("recChannel_t::unmap");
	
	IBaseFilter * pFilter = NULL;
	int hr =0;
	hr = pGraph->FindFilterByName(L"Video Renderer",&pFilter);
	
	if (!hr)
	{
	 	IVideoWindow *pWindowInfo = NULL;
		hr = pFilter->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
		errorCheck(hr);
		pWindowInfo->put_Visible(OAFALSE);
		pWindowInfo->put_AutoShow(OAFALSE);
		pWindowInfo->Release();
	}
	
	pControl->StopWhenReady();
#ifdef _WINDOWS
    if (fControl)
    {
        fControl->CWnd::ShowWindow(SW_HIDE);
    }
#endif
	mapping = false;
	return 0;
}

	
HRESULT
recChannel_t::remap(void)
{
     __CONTEXT("recChannel_t::remap");
	IBaseFilter * pFilter = NULL;
	int hr = 0;	
	pControl->StopWhenReady();
	IPin * pin = NULL;

	hr = pGraph->FindFilterByName(L"AVI Decompressor",&pFilter);
	
	if (!hr) 
	{
		hr = pGraph->RemoveFilter(pFilter);
		errorCheck(hr);
		pFilter->Release();
	}

	hr = pGraph->FindFilterByName(L"Video Renderer",&pFilter);
	if (!hr)
	{
	
		while ((hr = GetConnectedPin(pFilter,PINDIR_INPUT,&pin))==0)
		{
			pin->Disconnect();
		}
	}

	if (pOutput) pOutput->Disconnect();

	remaped = true;
	return 0;

}

recChannel_t::~recChannel_t(void)
{
     __CONTEXT("recChannel_t::~recChannel_t");

	IBaseFilter * pFilter = NULL;
	if (camInfo->getKind() == TEST) 
	{
		looper->EndThread();
	}
	
	unmap();
	camList->lookUp(sourceId)->setFree(true);
	 
	pControl->Stop();
	
    looper->EndThread();
 
    delete looper;
    delete pSender;

    remap();
	
	int hr = 0;

	// Enumerate the filters in the graph.
	IEnumFilters *pEnum = NULL;
	hr = pGraph->EnumFilters(&pEnum);
	if (SUCCEEDED(hr))
	{
		IBaseFilter *pFilter = NULL;
		while (S_OK == pEnum->Next(1, &pFilter, NULL))
		{
				pGraph->RemoveFilter(pFilter);
				pFilter->Release();
				pEnum->Reset();
		}
		pEnum->Release();
	}
	pControl->Release();
	pEvent->Release();
	pGraph->Release();
	
	channelList->remove(getId());
	rtpSession->deleteSender (getId(), "Channel deleted");
#ifdef _WINDOWS
    EndThread();
    TerminateThread(hThread,0); 
#endif
    

}

HRESULT 
recChannel_t::set_window_title(char * newTitle)
{
     __CONTEXT("recChannel_t::set_window_title");
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

}

HRESULT 
recChannel_t::set_window_geometry(windowInfo_t wInfo)
{
	 __CONTEXT("recChannel_t::set_window_geometry");
    if (wInfo.width)
    {
	    windowInfo.width  = wInfo.width;
    }
    if (wInfo.heigth)
    {
	    windowInfo.heigth = wInfo.heigth;
    }
	windowInfo.left   = wInfo.left;
	windowInfo.top    = wInfo.top;
	
	setWindowGeometry(windowInfo);
#ifdef _WINDOWS   
    if (camInfo->getKind() == MEDIA)
    {
        fControl->setGeometry(windowInfo);
    }
#endif
	return 0;

}

HRESULT 
recChannel_t::set_file_source(char * fileName)
{
	 __CONTEXT("recChannel_t::set_file_source");
	int hr = 0;

	pControl->StopWhenReady();
	
    // If any valid source is already created

	for (int i = 0; i < camArray.size(); i++)
	{
		camInfo_t *camInfo= camArray.elementAt(i);
		if (fileName != NULL)
		{
			if (camInfogetFree() && strcmp(camInfo->getCamName(), fileName) == 0)
			{
				all = true;
				select_source(camInfo);
				return 0;
			}
		}
		else
		{
	        if (camInfo->getFree() && camInfo->getKind() == TEST)
			{
				all = true;
				select_source(camInfo);
				return 0;
			}
		}
	}

    // if we must create a source

	camInfo_t *camInfo = createFileSource(fileName);
		
    all = true;
	select_source(camInfo);

	return 0;
}

HRESULT 
recChannel_t::set_rate(float FR)
{
     __CONTEXT("recChannel_t::set_rate");
	if (FR<1)
    {
        return S_OK;
    }
	float factorRate = FR/30;
	int hr = 0;

	if (factorRate<0.1) factorRate = 0.1;
	frameRate = factorRate;

	IAMStreamConfig *pConfig = NULL;		
				
	if ((camInfo->getKind() == SHARED ||
         camInfo->getKind() == CAM)   && 
		actualFormat.pbFormat != NULL)
	{
			VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) actualFormat.pbFormat;
            double newFR = 10000000.0/FR;
			pVih->AvgTimePerFrame = newFR;
            camInfo->setRate(pVih->AvgTimePerFrame);
            if (camInfo->getKind() == CAM)
            {
                IPin * pInput = NULL;
                get_camInfo()->output->ConnectedTo(&pInput);
                if (mapping)
                {
                    pControl->Stop();
                }
                if (pInput)
                {
                    get_camInfo()->output->Disconnect();
                    pInput->Disconnect();
                }
             	hr = get_camInfo()->output->QueryInterface(IID_IAMStreamConfig, (void**)&pConfig);
                if (pConfig)
                {
                    int hr = pConfig->SetFormat(&actualFormat);
                    errorCheck(hr);
                    pConfig->Release();
                }
                if (pInput)
                {
                    hr = pGraph->Connect(get_camInfo()->output,pInput);
                    errorCheck(hr);
                }
                errorCheck(hr);
                if (mapping)
                {
                    pControl->Run();
                }
			}
    }
    return hr;
	
}
		
float
recChannel_t::get_rate(void)
{
	return (int)(frameRate*30);
}
	
void 
recChannel_t::refresh_channel(bool all)
{
     __CONTEXT("recChannel_t::refresh_channel");
    pControl->Stop();
#ifdef _WINDOWS
    if (fControl)
    {
        fControl->CWnd::ShowWindow(SW_HIDE);
    }
#endif
	
	if (pSource!=NULL)
    {
		pGraph->RemoveFilter(pSource);
		camInfo->setFree(true);
	}
	
	// Enumerate the filters in the graph.
	IEnumFilters *pEnum = NULL;
	int hr = pGraph->EnumFilters(&pEnum);
	if (SUCCEEDED(hr))
	{
		IBaseFilter *pFilter = NULL;
		while (S_OK == pEnum->Next(1, &pFilter, NULL))
		{
			CLSID filterId;
			pFilter->GetClassID(&filterId);
			if(filterId == CLSID_VideoRenderer ||
			   filterId == CLSID_VideoMixingRenderer)
			{
				IVideoWindow *pWindowInfo = NULL;
				pFilter->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
				pWindowInfo->get_Height(&windowInfo.heigth);
				pWindowInfo->get_Left(&windowInfo.left);
				pWindowInfo->get_Top(&windowInfo.top);
				pWindowInfo->get_Width(&windowInfo.width);
           		pWindowInfo->put_AutoShow(OAFALSE);
			    pWindowInfo->put_Visible(OAFALSE);
        	   	if (all)
				{
                   	pGraph->RemoveFilter(pFilter);
					pFilter->Release();
					pEnum->Reset();
					pWindowInfo->Release();
				}

			}else{
                
                pGraph->RemoveFilter(pFilter);    
                pFilter->Release();
                pEnum->Reset();
			}
		}
		pEnum->Release();
	}
	if (all)
	{
		pGraph->Release();
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                          IID_IGraphBuilder, (void **)&pGraph);
		errorCheck(hr);
	}
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	errorCheck(hr);

}

HRESULT 
recChannel_t::get_AM_MEDIA_TYPE(char * newFormat ,AM_MEDIA_TYPE ** mediaFormat)
{
    __CONTEXT("recChannel_t::get_AM_MEDIA_TYPE");
	ql_t<AM_MEDIA_TYPE *> auxFormats = camInfo->getFormatList();
	for(int i = 0; i<auxFormats.len() ; i++)
    {
		AM_MEDIA_TYPE *format = auxFormats.nth(i);
		char subtypeName [100];
		memset(subtypeName,0,100);
		GetGUIDString(subtypeName,&format->subtype);	
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) format->pbFormat;
		
		if((pVih==NULL && strcmp(newFormat,sourceFormat)==0) ||
		   (pVih->bmiHeader.biHeight == capInfo.heigth &&
		    pVih->bmiHeader.biWidth == capInfo.width &&
		    strcmp(subtypeName,newFormat)==0))
		{
					*mediaFormat = format;
					return 0;
		}
	}
	return -1;
}

char *
recChannel_t::get_source_format(void)
{
     __CONTEXT("recChannel_t::get_source_format");
     strcpy(auxStr,sourceFormat);
     return auxStr;
}
	

camInfo_t *
recChannel_t::get_camInfo(void)
{
	return camInfo;
}

captureInfo_t
recChannel_t::get_captureInfo(void)
{
	return capInfo;
}

bool
recChannel_t::set_captureInfo(captureInfo_t newCapInfo)
{
    __CONTEXT("recChannel_t::set_captureInfo");
    char auxHeight[5];
    char auxWidth[5];
    char auxRes[11];
    memset(auxRes,0,strlen(auxRes));

    itoa(newCapInfo.heigth,auxHeight,10);
  	itoa(newCapInfo.width,auxWidth,10);
		    
    strcat(auxRes,auxWidth);
    strcat(auxRes,"x");
    strcat(auxRes,auxHeight);
    strcat(auxRes,";");
    
	char * supportedRes = camInfo->getSupportedRes();
    
    if ((strstr(supportedRes,auxRes))<=0)
    {
        NOTIFY("recChannel_t %d:: Source does not support resolution %s\n",
                getId(),             
                auxRes);
        int i=0;
        for (;i<1000;i++)
        {
            itoa(newCapInfo.width+i,auxWidth,10);
            char * c = NULL;
            if((c = strstr(supportedRes,auxWidth))>0)
            {
                char * w = strstr(c,";");
                char * x = strstr(c,"x");
                if(x<w)
                {
                    int j = 0;
                    while(c[j]!=w[0] && c[j]!=0)
                    {
                        auxRes[j] = c[j];
                        j++;
                    }
                    break;
                }
            }
        }

        if ((strstr(supportedRes,auxRes))<=0)
        {
            return false;
        }else{
            
            char width[5];
	        char heigth[5];
	        memset(width,0,5);
	        memset(heigth,0,5);
	
	        for (i = 0;auxRes[i]!='x' && auxRes[i]!=0;i++)
	        {
                width[i] = auxRes[i];
            }
            newCapInfo.width = atoi(width);
	        int j=0;
	        for(i++;auxRes[i]!=0;i++,j++)
            {
                heigth[j] = auxRes[i];
            }
            newCapInfo.heigth= atoi(heigth);
        }
    }
     
    capInfo = newCapInfo;
    
    NOTIFY("recChannel_t %d:: Source resolution changed to %dx%d\n",
            getId(),             
            newCapInfo.width,
            newCapInfo.heigth);
 
    return true;
}

HRESULT
recChannel_t::set_bucket_size(int newBucketSize)
{
	return pSender->SetBucketSize(newBucketSize);
}


