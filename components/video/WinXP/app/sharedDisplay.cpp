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
   <name>sharedDisplay.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of sharedDisplay.h.
   </descr>
//</head>
**/



#include "recChannel.h"

#include <Winuser.h>

/* Desktop capture don't show mouse pointer, so we must draw it
   before sending every frame */

/* Mouse pointer*/
u8   mouse[]={ 0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,2,1,2,2,2,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,2,1,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,2,1,1,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,2,1,0,0,1,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,1,0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,1,0,0,0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,1,0,0,0,0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* color palette */
u8 palette[]={0x00/*trasparent*/,
              0x00/*black*/     ,
              0xff/*white*/    };


sharedDisplayPin_t::sharedDisplayPin_t(TCHAR *pObjectName,
					   HRESULT *phr,
                       CSource *pms,
                       LPCWSTR pName,
					   channel_t * pChannel)

:CSourceStream(pObjectName,phr,pms,pName) 
{
	
    HWND desktop = GetDesktopWindow();
    GetWindowRect(desktop,&sharedRect);
	sharedRect.left  = sharedRect.right - 800;
    sharedRect.bottom = 600;
			
    hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);     
	hMemDC = CreateCompatibleDC(hScrDC);      // get points of rectangle to grab  

    
    bits = GetDeviceCaps(hScrDC, BITSPIXEL);     // get screen depth
#if 1
    if (bits!=16)
    {
        char msg[1024];
        sprintf(msg,
                "WARNING:\n"
                "Your desktop depth is %d bits, to improve\n"
                "performance change it to 16 bits\n",bits);
        AfxMessageBox(msg,MB_OK,0);
    }
#endif

    Bitmap = NULL;
	Data = NULL;
	m_pFilter = pms;
   	m_dir = PINDIR_OUTPUT;
	channel = pChannel;

	_ftime(&time);
	actualTime = time;
	frameRate = DEFAULT_SHARED_DISPLAY_FR; //Rate in 100-nanoseconds unit;
			
}

sharedDisplayPin_t::~sharedDisplayPin_t(void)
{
	 // clean up      
     DeleteDC(hScrDC);     
     DeleteDC(hMemDC); 
	 if (Data)
		delete Data;
}
  
void
sharedDisplayPin_t::SetSharedRect(RECT newRect)
{
	if (newRect.bottom > newRect.top &&
		newRect.right  > newRect.left&&
		newRect.left   >= 0 &&
		newRect.top    >= 0)
	{
		sharedRect.bottom = newRect.bottom;
		sharedRect.left   = newRect.left;
		sharedRect.right  = newRect.right;
		sharedRect.top    = newRect.top;
	}
	return;
}

void 
sharedDisplayPin_t::GetSample(IMediaSample ** Sample)
{
    int hr = GetDeliveryBuffer(Sample,NULL,NULL,0);
	errorCheck(hr);
}


HRESULT
sharedDisplayPin_t::GetMediaType(CMediaType *pMediaType)
{
		memcpy(pMediaType,&m_mt,sizeof(AM_MEDIA_TYPE));
		return 0;
}

HRESULT 
sharedDisplayPin_t::CheckMediaType(const CMediaType *pmt)
{
		return S_OK;
}

HRESULT
sharedDisplayPin_t::DecideBufferSize(IMemAllocator *pAlloc,
								 ALLOCATOR_PROPERTIES *ppropInputRequest)
{
		ALLOCATOR_PROPERTIES pprop;
		ppropInputRequest->cbBuffer = MAX_SAMPLE_DATA;
		ppropInputRequest->cBuffers = 2;
		pAlloc->SetProperties(ppropInputRequest,&pprop);
		m_pAllocator=pAlloc;
		return S_OK;
}

int
sharedDisplayPin_t::convert24to16(BYTE * data,int len)
{
    BYTE * p8  = (BYTE*)data;
    u16  * p16 = (u16 *)data;
    int i = 0,j=0;
    for(;i<(len*2/3)/2-10;i++)
    {
        j = i;
        p8  = &Data[i*3];
        p16 = (u16*)&Data[i*2];

        WORD RED   = 0x0000; 
        WORD GREEN = 0x0000;
        WORD BLUE  = 0x0000;
 
        BLUE   = * p8>>3;
        GREEN  = *(p8+1)>>2;
        RED    = *(p8+2)>>3;
        
        *p16   = RED<<11 | GREEN<<5| BLUE;
        
    }        
    return len*2/3;
}

int
sharedDisplayPin_t::convert32to16(BYTE * data,int len)
{
    BYTE * p8  = (BYTE*)data;
    u16  * p16 = (u16 *)data;
    int i = 0;
 
    WORD RED   = 0x0000; 
    WORD GREEN = 0x0000;
    WORD BLUE  = 0x0000;
 
    for(;i<(len/4);i++)
    {
        p8  = &Data[i*4];
        p16 = (u16*)&Data[i*2];

        BLUE   = * p8>>3;
        GREEN  = *(p8+1)>>2;
        RED    = *(p8+2)>>3;
        
        *p16   = RED<<11 | GREEN<<5| BLUE;
        
    }        
    return len/2;
}

HRESULT 
sharedDisplayPin_t::FillBuffer(IMediaSample *pData)
{
		if (!channel->getMapping()) return -1;
	    sharedDisplay_t *source = static_cast<sharedDisplay_t *>(m_pFilter);
		HBITMAP screen = NULL;
		screen = CopyScreenToBitmap(&sharedRect);
    	Bitmap = CBitmap::FromHandle(screen);
  		
		if (Data)
		{
			delete Data;
			Data = NULL;
		} 
		
        int samplesize = (sharedRect.bottom - sharedRect.top)*(sharedRect.right  - sharedRect.left)*bits/8;
			
		Data = new BYTE[samplesize];
		
		
		int n = Bitmap->GetBitmapBits(samplesize,Data);
        DeleteObject(screen);
		        
        switch (bits)
        {
        case 32:
            {
                n = convert32to16(Data,n);
                break;
            }
        case 24:
            {
                n = convert24to16(Data,n);
                break;
            }
        }

        int nLines = sharedRect.bottom - sharedRect.top;
		int nWidth = (sharedRect.right  - sharedRect.left)*2;

        POINT position;
        GetCursorPos(&position);
        
        if (position.y<sharedRect.top)
        {
            position.y = sharedRect.top;
        }
        if (position.x<sharedRect.left)
        {
            position.x = sharedRect.left;
        }
        if (position.y>sharedRect.bottom)
        {
            position.y = sharedRect.bottom;
        }
        if (position.x>sharedRect.right)
        {
            position.x = sharedRect.right;
        }
        
        //get relative position
        position.x -= sharedRect.left;
        
        int writePos = (position.y*(nWidth-1)) + position.x*2;
        if (writePos<samplesize)
        {       
            int n = 0;
            for (int j = position.y; j <position.y+32 ; j++)
            {                
                for (int k = position.x; k <position.x+32 ; k++)
                {
                    int writePos = (j*(nWidth)) + k*2;
                    
                    if (writePos<samplesize && 
                        j == writePos/nWidth &&
                        mouse[n])
                    {
                        *(Data + writePos) = palette[mouse[n]];
                        *(Data + writePos + 1) = palette[mouse[n]];
                    }
                    n++;
                }
            }
        }

        //Now we flip the image
		BYTE * array1 = new BYTE[nWidth];
		BYTE * array2 = new BYTE[nWidth];

		for (int i = 0; i<nLines/2 ; i++)
		{
			memcpy(array1,Data + i*nWidth , nWidth);
			memcpy(array2,Data + (nLines - 1 - i)*nWidth, nWidth);
			memcpy(Data + i*nWidth,array2, nWidth);
			memcpy(Data + (nLines - 1 - i)*nWidth,array1,nWidth);
		}

        
		delete[] array1;
		delete[] array2;

#if 0             
        // Now zoom the image
        // the end image must be %16
        nLines -= nLines%16;
        nWidth -= nWidth%16;

        int u16Width = nWidth/2;

        u16 * buffer = (u16 *)Data; // 2 bytes pointer to begin of array (RGB16)
        u16 * buffer2= (u16 *)Data;

        for (i= 0; i<nLines; i+=2) //rows
        {
            unsigned row = i*u16Width;
            for (int j= 0;j<u16Width;j+=2) //columns 
            {
                WORD RED   = 0x0000; 
                WORD GREEN = 0x0000;
                WORD BLUE  = 0x0000;

                RED   = (((buffer2[row + j]&0xF800) + (buffer2[row + j + 1]&0xF800))>>11)/2;// + (buffer2[row + u16Width + j]&0xF800) + (buffer2[row + u16Width + j + 1]&0xF800))/4;

                GREEN = (((buffer2[row + j]&0x07E0) + (buffer2[row + j + 1]&0x07E0))>>5)/2;// + (buffer2[row + u16Width + j]&0x07E0) + (buffer2[row + u16Width + j + 1]&0x07E0))/4;

                BLUE  = ((buffer2[row + j]&0x001F) + (buffer2[row + j + 1]&0x001F))/2;// + (buffer2[row + u16Width + j]&0x001F) + (buffer2[row + u16Width + j + 1]&0x001F))/4;
                
                * buffer = RED<<11 | GREEN<<5| BLUE;
                
                buffer++;
            }
        }

        nLines/=2;
        nWidth/=2;
        n=nWidth*nLines;
		
#endif        
        
		if (n>0)
		{
		
		
		//	CAutoLock lock_it(m_pLock);	
	
			if (pData == NULL)
			{
				GetSample(&pData);
			}
		
			CMediaSample * auxSample = (CMediaSample *)pData;
			if (auxSample)
			{
				REFERENCE_TIME	  startTime, endTime;
				_ftime(&actualTime);
				startTime = ((actualTime.time - time.time)*1000 + actualTime.millitm - time.millitm)*1000;
				endTime   = startTime + DEFAULT_SHARED_DISPLAY_FR;
				auxSample->SetPointer((BYTE *)Data,n);
				auxSample->SetTime(&startTime,&endTime);
		
			}
	
		}
        Sleep(500);
	return 0;
} 

HBITMAP 
sharedDisplayPin_t::CopyScreenToBitmap(LPRECT lpRect)
{
	 
	int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab     
	int         nWidth, nHeight;        // DIB width and height     
	int         xScrn, yScrn;           // screen resolution      

	HGDIOBJ			  hOldBitmap = NULL;
	HGDIOBJ           hBitmap = NULL;

		
	// check for an empty rectangle 
    if (IsRectEmpty(lpRect))       
	   return NULL;      
	   // create a DC for the screen and create     
	   // a memory DC compatible to screen DC          

   nX = lpRect->left;     
   nY = lpRect->top;     
   nX2 = lpRect->right;     
   nY2 = lpRect->bottom;      
   
   // get screen resolution      
   
   xScrn = GetDeviceCaps(hScrDC, HORZRES);     
   yScrn = GetDeviceCaps(hScrDC, VERTRES);      
   
   //make sure bitmap rectangle is visible      
   
   if (nX < 0)         
	  nX = 0;     
   
   if (nY < 0)         
      nY = 0;     
   
   if (nX2 > xScrn)         
      nX2 = xScrn;     
   
   if (nY2 > yScrn)         
      nY2 = yScrn;      

   nWidth = nX2 - nX;     
   nHeight = nY2 - nY;      
   
   // create a bitmap compatible with the screen DC     
   
   hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);      
   
   
   // select new bitmap into memory DC     
   
   hOldBitmap =   SelectObject (hMemDC, hBitmap);      
   
   // bitblt screen DC to memory DC     
   
   BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);     
   
   // select old bitmap back into memory DC and get handle to     
   // bitmap of the screen          
   
   hBitmap = SelectObject(hMemDC, hOldBitmap);      

  
   // return handle to the bitmap
     
   return (HBITMAP)hBitmap; 

}

void
sharedDisplayPin_t::SetRate(int rate)
{
	if (rate>0)	
		frameRate = rate;
}

sharedDisplay_t::sharedDisplay_t(channel_t * pChannel,
								 TCHAR *pName,
								 LPUNKNOWN lpunk,
								 CLSID clsid,
								 long period)

:CSource(pName,lpunk,clsid)
{

	    this->channel = pChannel;
		frameRate = DEFAULT_SHARED_DISPLAY_FR;

		OutputPin = new sharedDisplayPin_t("dOutputStream2",&hr,(CSource *)(this),L"OutputStream",channel);
	
        HWND desktop = GetDesktopWindow();
        GetWindowRect(desktop,&sharedRect);
        sharedRect.left  = sharedRect.right - 800;
        sharedRect.bottom = 600;			
        RECT tmpRect = sharedRect;
/*
        tmpRect.bottom = (tmpRect.bottom - tmpRect.bottom%16)/2;
        tmpRect.top    = (tmpRect.top - tmpRect.top%16)/2;
        tmpRect.left   = (tmpRect.left - tmpRect.left%16)/2;
        tmpRect.right  = (tmpRect.right - tmpRect.right%16)/2;
*/       
		setFree(true);
		setSupportedFormats("{MEDIASUBTYPE_RGB555};");

		VIDEOINFOHEADER * videoInfo = new VIDEOINFOHEADER;

		videoInfo->AvgTimePerFrame = DEFAULT_SHARED_DISPLAY_FR;
		videoInfo->bmiHeader.biSizeImage = 0;
		videoInfo->bmiHeader.biWidth = (tmpRect.right - tmpRect.left);
		videoInfo->bmiHeader.biHeight = (tmpRect.bottom - tmpRect.top);
		videoInfo->bmiHeader.biCompression = BI_RGB;
		videoInfo->bmiHeader.biPlanes = 1; // 1 plane
		videoInfo->bmiHeader.biBitCount = 16; // 16 bits color
		videoInfo->bmiHeader.biXPelsPerMeter = 0; // Not specified
		videoInfo->bmiHeader.biYPelsPerMeter = 0; 
		videoInfo->bmiHeader.biClrUsed = 2^16-1; 
		videoInfo->bmiHeader.biClrImportant = 0; //All
		videoInfo->rcSource.left   = 0;
		videoInfo->rcSource.right  = (tmpRect.right - tmpRect.left);
		videoInfo->rcSource.top    = 0;
		videoInfo->rcSource.bottom = (tmpRect.bottom - tmpRect.top);
		videoInfo->rcTarget.left   = 0;
		videoInfo->rcTarget.right  = (tmpRect.right - tmpRect.left);
		videoInfo->rcTarget.top    = 0;
		videoInfo->rcTarget.bottom = (tmpRect.bottom - tmpRect.top);
		videoInfo->bmiHeader.biSize = sizeof(*videoInfo);
		
		AM_MEDIA_TYPE * outputFormat = new AM_MEDIA_TYPE;

		outputFormat->majortype = MEDIATYPE_Video;
		outputFormat->subtype   = MEDIASUBTYPE_RGB555;
		outputFormat->bTemporalCompression = 0;
		outputFormat->bFixedSizeSamples = true;
		outputFormat->formattype = FORMAT_VideoInfo;
		outputFormat->lSampleSize = (tmpRect.right - tmpRect.left)*(tmpRect.bottom - tmpRect.top)*16/8;
		outputFormat->cbFormat = videoInfo->bmiHeader.biSize;
		outputFormat->pbFormat = (BYTE *)videoInfo;
		outputFormat->pUnk = NULL;

		OutputPin->SetMediaType((CMediaType *)outputFormat);
		
		CSource::m_pGraph = pChannel->pGraph;
		CSource::AddPin(OutputPin);
		CSource::m_iPins = 1;
		
		this->output  = static_cast<IPin *>(OutputPin);		
		this->pSource = static_cast<IBaseFilter *>(this);

		OutputPin->SetSharedRect(sharedRect);

		channel->pGraph->SetDefaultSyncSource();
		
}



sharedDisplay_t::~sharedDisplay_t(void)
{
        debugMsg(dbg_App_Normal,
				 "sharedDisplay_t",
				 "~sharedDisplay_t::Deleting...");
		camList->remove(getID());
		recChannel_t * recChannel = static_cast<recChannel_t *>(channel);
		recChannel->pSource = NULL;

}


bool
sharedDisplay_t::setSharedRect(RECT newRect)
{ 
	 
    if ((newRect.bottom>= 0 &&
         newRect.right >= 0 &&
         newRect.left < newRect.right &&
         newRect.top < newRect.bottom) && (
         newRect.bottom != sharedRect.bottom ||
         newRect.left   != sharedRect.left   ||
         newRect.right  != sharedRect.right  ||
         newRect.top    != sharedRect.top))
    {
	    IPin * auxPin = OutputPin->GetConnected();

	    sharedRect = newRect; 
	    OutputPin->SetSharedRect(sharedRect);

        RECT tmpRect = sharedRect;
    /*
        tmpRect.bottom = (tmpRect.bottom - tmpRect.bottom%16)/2;
        tmpRect.top    = (tmpRect.top - tmpRect.top%16)/2;
        tmpRect.left   = (tmpRect.left - tmpRect.left%16)/2;
        tmpRect.right  = (tmpRect.right - tmpRect.right%16)/2;
    */    	
        AM_MEDIA_TYPE outputFormat;

	    OutputPin->GetMediaType((CMediaType *)&outputFormat); 
	    
        VIDEOINFOHEADER * auxVideoInfo = new VIDEOINFOHEADER;
	    VIDEOINFOHEADER * videoInfo = (VIDEOINFOHEADER *)outputFormat.pbFormat;
    
        videoInfo->bmiHeader.biWidth = (tmpRect.right - tmpRect.left);
	    videoInfo->bmiHeader.biHeight = (tmpRect.bottom - tmpRect.top);
        *auxVideoInfo = *videoInfo;

	    outputFormat.lSampleSize = (tmpRect.right - tmpRect.left)*(tmpRect.bottom - tmpRect.top)*16/8;
	    outputFormat.pbFormat =  (BYTE *)auxVideoInfo;

	    if (auxPin)
        {
		    auxPin->Disconnect();
        }

        OutputPin->Disconnect();
	    OutputPin->SetMediaType((CMediaType *)&outputFormat);

        return true;
    }else{
        return false;
    }
}

void    
sharedDisplay_t::setRate(int newFR)
{ 
	if (newFR>0)
	{
		frameRate = newFR; 
		AM_MEDIA_TYPE outputFormat;

		OutputPin->GetMediaType((CMediaType *)&outputFormat); 
	
		VIDEOINFOHEADER * videoInfo = (VIDEOINFOHEADER *)outputFormat.pbFormat;

		videoInfo->AvgTimePerFrame = frameRate;
		OutputPin->SetMediaType((CMediaType *)&outputFormat);
		OutputPin->SetRate(newFR);
	}

	return;
}


