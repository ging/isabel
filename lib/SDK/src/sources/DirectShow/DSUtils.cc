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
#include "DSUtils.hh"
#include <wchar.h>
#include <strsafe.h>

namespace DSUtils
{

#define MAX_STR_LEN 1024



void errorCheck(int hr){
	if (hr){
		char errorText[100];
		AMGetErrorText(hr,errorText,100);
		NOTIFY("[DSUtils] WARNING :: %s \r\n",errorText);
	}
}

HRESULT GetUnconnectedPin(
						  IBaseFilter *pFilter,   // Pointer to the filter.
						  PIN_DIRECTION PinDir,   // Direction of the pin to find.
						  IPin **ppPin)           // Receives a pointer to the pin.
{
	*ppPin = 0;
	IEnumPins *pEnum = 0;
	IPin *pPin = 0;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = 0;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // Already connected, not the pin we want.
			{
				pTmp->Release();
			}
			else  // Unconnected, this is the pin we want.
			{
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	// Did not find a matching pin.
	return E_FAIL;
}


HRESULT GetPin(IBaseFilter *pFilter,
			   PIN_DIRECTION PinDir, 
			   IPin **ppPin)
{
	IEnumPins  *pEnum = NULL;
	IPin       *pPin = NULL;
	HRESULT    hr;

	if (ppPin == NULL)
	{
		return E_POINTER;
	}

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		hr = pPin->QueryDirection(&PinDirThis);
		if (FAILED(hr))
		{
			pPin->Release();
			pEnum->Release();
			return hr;
		}
		if (PinDir == PinDirThis)
		{
			// Found a match. Return the IPin pointer to the caller.
			*ppPin = pPin;
			pEnum->Release();
			return S_OK;
		}
		// Release the pin for the next time through the loop.
		pPin->Release();
	}
	// No more pins. We did not find a match.
	pEnum->Release();
	return E_FAIL;  
}

HRESULT GetConnectedPin(IBaseFilter *pFilter, 
						PIN_DIRECTION PinDir, 
						IPin **ppPin)
{
	IEnumPins  *pEnum = NULL;
	IPin       *pPin = NULL;
	HRESULT    hr;

	if (ppPin == NULL)
	{
		return E_POINTER;
	}

	hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_DIRECTION PinDirThis;
		hr = pPin->QueryDirection(&PinDirThis);
		if (FAILED(hr))
		{
			pPin->Release();
			pEnum->Release();
			return hr;
		}
		if (PinDir == PinDirThis)
		{
			AM_MEDIA_TYPE aux;
			if((hr = pPin->ConnectionMediaType(&aux))==0){
				// Found a match. Return the IPin pointer to the caller.
				*ppPin = pPin;
				pEnum->Release();
				return S_OK;
			}
		}
		// Release the pin for the next time through the loop.
		pPin->Release();
	}
	// No more pins. We did not find a match.
	pEnum->Release();
	return E_FAIL;  

}

std::vector<CDSGrabberDesc> EnumVideoSources(void)
{
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;
	char devicePath[MAX_STR_LEN];
	char deviceName[MAX_STR_LEN];

	std::vector<CDSGrabberDesc> device_list;

	// Create the System Device Enumerator.
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&pDevEnum));
	if (FAILED(hr)){errorCheck(hr); goto end;}
	
	// Create an enumerator for the video capture category.
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnum, 0);

	if (FAILED(hr)){errorCheck(hr); goto end;}

	while (pEnum && pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;  // Skip this one, maybe the next one will work.
		} 

		memset(devicePath,0,MAX_STR_LEN);
		memset(deviceName,0,MAX_STR_LEN);

		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		if (SUCCEEDED(hr))
		{
			wcstombs(deviceName,varName.bstrVal,MAX_STR_LEN);
			VariantClear(&varName); 
		}
		VariantInit(&varName);
		hr = pPropBag->Read(L"DevicePath", &varName, 0);
		if (SUCCEEDED(hr))
		{
			wcstombs(devicePath,varName.bstrVal,MAX_STR_LEN);
			VariantClear(&varName); 			
			device_list.insert(device_list.end(),CDSGrabberDesc(devicePath,deviceName));
		}
		pPropBag->Release();
		pMoniker->Release();		
	}
end:
	RELEASE_FILTER(pEnum);
	RELEASE_FILTER(pDevEnum);

	return device_list;
}

HRESULT InitSource(CDSGrabberDesc desc,IBaseFilter ** pSource)
{
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;
	*pSource = NULL;
	// Create the System Device Enumerator.
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,	CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 	reinterpret_cast<void**>(&pDevEnum));

	if (FAILED(hr)){errorCheck(hr); goto end; }
	
	// Create an enumerator for the video capture category.
	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEnum, 0);
	
	if (FAILED(hr)){errorCheck(hr); goto end; }
	hr = E_INVALIDARG;
	while (pEnum->Next(1, &pMoniker, NULL) == S_OK && *pSource == NULL)
	{
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&pPropBag));
		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;  // Skip this one, maybe the next one will work.
		} 

		// Find the description or friendly name.
		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"DevicePath", &varName, 0);
		if (SUCCEEDED(hr))
		{
			char sourceDev[MAX_STR_LEN];
			memset(sourceDev,0,MAX_STR_LEN);
			wcstombs(sourceDev,varName.bstrVal,MAX_STR_LEN);
			if(strcmp(sourceDev,desc.getID())==0)
			{
				pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,	(void**)pSource);
				hr = S_OK;
			}
			VariantClear(&varName); 
		}
		pPropBag->Release();
		pMoniker->Release();
	}
end:
	RELEASE_FILTER(pEnum);
	RELEASE_FILTER(pDevEnum);
	return hr;
}

}