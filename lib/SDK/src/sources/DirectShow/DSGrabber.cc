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

#pragma warning (disable:4995)//deshabilitados deprecados
#pragma warning (disable:4482)// y uso no estandar de enumeradores

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <streams.h>
#include <strsafe.h>
#include "DSGrabber.hh"
#include "DSUtils.hh"
#include <uuids.h>

using namespace std;
using namespace DSUtils;

CDSGrabber::CDSGrabber(CDSGrabberDesc desc):m_Desc(desc)
{
	IBaseFilter *pFilter = NULL;
	
	m_mFormats[RGB24_FORMAT] = MEDIASUBTYPE_RGB24;
	m_mFormats[I411P_FORMAT] = GUID_NULL;
	m_mFormats[I420P_FORMAT] = GUID_NULL;
	m_mFormats[I422i_FORMAT] = GUID_NULL;
	m_mFormats[I422P_FORMAT] = GUID_NULL;
	m_mFormats[BGR24_FORMAT] = GUID_NULL;
	
	m_eFormat = RGB24_FORMAT;		
	m_pGraph = NULL;
	m_pControl = NULL;
	m_pEvent = NULL;
	m_pCam = NULL;
	m_pGrabberCB = new CSampleGrabberCB();
	//Creamos el grafo
	HRESULT	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraph);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pControl);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pEvent);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	//Creamos el renderer
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&m_pRender);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	hr = m_pGraph->AddFilter(m_pRender,L"Renderer");
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	//Creamos el sample grabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void **)&m_pGrabber);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	hr = m_pGrabber->SetCallback(m_pGrabberCB,1/*Para que llame a la funcion BufferCB*/);
	//Agregamos el sampleGrabber al grafo
	hr = m_pGrabber->QueryInterface(IID_IBaseFilter,(void**)&pFilter);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	hr = m_pGraph->AddFilter(pFilter,L"SampleGrabber");
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	RELEASE_FILTER(pFilter);

	//Cargamos la camara
	hr = InitSource(desc,&m_pCam);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	//Agregamos la camara al grafo
	hr = m_pGraph->AddFilter(m_pCam,L"CaptureCamera");
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	//Renderizamos el grafo
	IPin * pOutPin =  NULL;
	IPin * pInPin  = NULL;
	hr = GetUnconnectedPin(m_pCam,PINDIR_OUTPUT,&pOutPin);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	hr = m_pGraph->Render(pOutPin);
	if (FAILED(hr)) { errorCheck(hr); goto error; }
	RELEASE_FILTER(pOutPin);
	
	//Obtenemos los formatos soportados
	m_vSupportedFormats = GetSupportedFormats();
	//Seleccionamos el formato correcto
	setGrabFormat(m_eFormat);

	//Ponemos a correr el grafo
	hr = m_pControl->Run();

	return;
error:
	Unload();
}

CDSGrabber::~CDSGrabber()
{
	Unload();
}


HRESULT 
CDSGrabber::Unload(void)
{
	IBaseFilter * pFilter = NULL;
	if (m_pControl)
	{
		m_pControl->Stop();
	}
	//Liberamos camara y grafo
	IEnumFilters * pEnum;
	HRESULT hr = m_pGraph->EnumFilters(&pEnum);
	if (SUCCEEDED(hr))
	{
		while (pEnum->Next(1,&pFilter,NULL)==S_OK)
		{
			m_pGraph->RemoveFilter(pFilter);
			pFilter->Release();
		}
		pEnum->Release();
	}
	for (std::vector<AM_MEDIA_TYPE*>::iterator iter = m_vSupportedFormats.begin();
		 iter != m_vSupportedFormats.end(); ++iter)
	{
		DeleteMediaType(*iter);
	}
	m_vSupportedFormats.clear();
	m_pCam->Release();
	m_pRender->Release();
	m_pGrabberCB->Release();
	m_pGrabber->Release();
	m_pEvent->Release();
	m_pControl->Release();
	m_pGraph->Release();	
	return S_OK;
}

std::vector<CDSGrabberDesc>
CDSGrabber::GetDeviceList(void)
{
	return DSUtils::EnumVideoSources();
}

std::vector<AM_MEDIA_TYPE*> 
CDSGrabber::GetSupportedFormats(void)
{
	std::vector<AM_MEDIA_TYPE*> formats;
	IPin * pPinOut = NULL;
	IEnumMediaTypes * pEnum = NULL;
	if (m_pCam)
	{
		HRESULT hr = GetPin(m_pCam,PINDIR_OUTPUT,&pPinOut);
		if (FAILED(hr)) { errorCheck(hr); goto end; }
		hr = pPinOut->EnumMediaTypes(&pEnum);
		if (FAILED(hr)) { errorCheck(hr); goto end; }

		AM_MEDIA_TYPE* pMediaType;
		while(pEnum->Next(1, &pMediaType , NULL) == S_OK)
		{	
			formats.insert(formats.end(),pMediaType);
		}	
	}
end:
	RELEASE_FILTER(pEnum);
	RELEASE_FILTER(pPinOut);
	return formats;
}

AM_MEDIA_TYPE* 
CDSGrabber::GetCurrentFormat(void)
{
	AM_MEDIA_TYPE * mtype = NULL;
	IAMStreamConfig *pConfig = NULL;		
	IPin* pPinOut = NULL;
	HRESULT hr = GetPin(m_pCam,PINDIR_OUTPUT,&pPinOut);
	if (SUCCEEDED(hr))
	{
		hr = pPinOut->QueryInterface(IID_IAMStreamConfig,(void**)&pConfig);
		RELEASE_FILTER(pPinOut);
	}
	if (SUCCEEDED(hr))
	{
		pConfig->GetFormat(&mtype);
		RELEASE_FILTER(pConfig);
	}
	return mtype;
}

image_t*
CDSGrabber::getImage(void)
{
	unsigned int w,h,bpp;
	AM_MEDIA_TYPE * mtype = GetCurrentFormat();
	VIDEOINFOHEADER * Vih = (VIDEOINFOHEADER *)mtype->pbFormat;
	h= Vih->bmiHeader.biHeight;
	w= Vih->bmiHeader.biWidth;
	bpp= Vih->bmiHeader.biBitCount;
	DeleteMediaType(mtype);
	image_t *img = NULL;
	DWORD elapsed = timeGetTime()*1000; //usecs
	long bufferLen = w*h*4;
	BYTE * pBuffer = new BYTE[bufferLen]; //Maximo tamaño en formato de 32 bits.
	int len = m_pGrabberCB->GetImage(pBuffer,bufferLen,w,h,bpp,true);
	if (len > 0)
	{
		img = new image_t(pBuffer,len, m_eFormat, w, h, elapsed);		
	}else{
		DELETE_ARR(pBuffer);
	}
	return img;
}

bool
CDSGrabber::setFrameRate(double fps)
{
    bool ret = false;
	if (fps > 0.0)
	{
		IAMStreamConfig *pConfig = NULL;		
		IPin* pPinOut = NULL;
		HRESULT hr = GetPin(m_pCam,PINDIR_OUTPUT,&pPinOut);
		if (SUCCEEDED(hr))
		{
			hr = pPinOut->QueryInterface(IID_IAMStreamConfig,(void**)&pConfig);
			RELEASE_FILTER(pPinOut);
		}
		if (SUCCEEDED(hr))
		{
            AM_MEDIA_TYPE * mtype = GetCurrentFormat();
            if (mtype)
	        {
		        VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) mtype->pbFormat;
		        if(pVih!=NULL)
		        {
                    pVih->AvgTimePerFrame = 10000000.0/fps;
                    m_pControl->Stop();
					hr = pConfig->SetFormat(CreateMediaType(mtype));
					m_pControl->Run();
		        }
		        DeleteMediaType(mtype);
                if (fps == getFrameRate())
                {
                    ret = true;
                }
            }	
        }
    }
    return ret;
}

double 
CDSGrabber::getFrameRate(void)
{
    double fps = 0.0;
    AM_MEDIA_TYPE * mtype = GetCurrentFormat();
    if (mtype)
	{
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) mtype->pbFormat;
		if(pVih!=NULL)
		{
            fps = 10000000.0/(pVih->AvgTimePerFrame);
		}
		DeleteMediaType(mtype);
    }	
    return fps;
}

bool  
CDSGrabber::SetGrabSizeAndFormat(unsigned width, unsigned height,u32 fId)
{
	bool ret = false;
	GUID format = m_mFormats[fId];
	if (format != GUID_NULL)
	{

		IAMStreamConfig *pConfig = NULL;		
		IPin* pPinOut = NULL;
		HRESULT hr = GetPin(m_pCam,PINDIR_OUTPUT,&pPinOut);
		if (SUCCEEDED(hr))
		{
			hr = pPinOut->QueryInterface(IID_IAMStreamConfig,(void**)&pConfig);
			RELEASE_FILTER(pPinOut);
		}
		if (SUCCEEDED(hr))
		{
			vector<AM_MEDIA_TYPE*> formats = m_vSupportedFormats;
			for (vector<AM_MEDIA_TYPE*>::iterator iter = formats.begin();
				iter != formats.end(); ++iter)
			{
				AM_MEDIA_TYPE * mtype = *iter;
				VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) mtype->pbFormat;
				if (mtype->subtype == format &&
					pVih->bmiHeader.biWidth == width &&
					pVih->bmiHeader.biHeight == height)
				{
					m_pControl->Stop();
					hr = pConfig->SetFormat(CreateMediaType(mtype));
					if (SUCCEEDED(hr))
					{
						m_eFormat = fId;
						ret = true;
					}else{
						NOTIFY ("CDSGrabber::setGrabFormat: SetFormat error\n");
						ret = false;
					}
					m_pControl->Run();
				}
			}
			RELEASE_FILTER(pConfig);
		}else{
			NOTIFY ("CDSGrabber::setGrabFormat: pConfig error\n");
			ret = false;
		}
	}else{
		NOTIFY ("CDSGrabber::setGrabFormat: Bad format\n");
		ret = false;
	}
	return ret;
}

bool  
CDSGrabber::setGrabSize(unsigned width, unsigned height)
{
	return SetGrabSizeAndFormat(width,height,m_eFormat);
}

void  
CDSGrabber::getGrabSize(unsigned *width, unsigned *height)
{
	AM_MEDIA_TYPE * mtype = GetCurrentFormat();
	if (mtype)
	{
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) mtype->pbFormat;
		if(pVih!=NULL)
		{
			*width = pVih->bmiHeader.biWidth;
			*height = pVih->bmiHeader.biHeight;
		}
		DeleteMediaType(mtype);
	}else{
		*width = 0;
		*height = 0;
	}	
}

bool      
CDSGrabber::setGrabFormat(u32 fId)
{ 	
	bool ret = false;
	unsigned int w,h;
	getGrabSize(&w,&h);
	ret = SetGrabSizeAndFormat(w,h,fId);
	return ret;
}

u32 
CDSGrabber::getGrabFormat(void)
{
	return m_eFormat;
}

sourceDescriptor_ref 
CDSGrabber::getDescriptor(void) const
{
	sourceDescriptor_t * desc = static_cast<sourceDescriptor_t*>(new CDSGrabberDesc(m_Desc));
	sourceDescriptor_ref desc_ref = static_cast<sourceDescriptor_ref>(desc);
	return  desc_ref;
}
