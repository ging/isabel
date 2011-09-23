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
#include "general.h"

// Global variables

windowInfo_t DEFAULT_WINDOW_INFO;
sched_t *s; 
bool *SetAutoChannel;


/**
 <function> 
   <name>ErrorCheck</name>
   <descr>Decodes error codes and prints the error description</descr>
 </function>
**/

 void errorCheck(int hr){
	if (hr){
		char errorText[100];
		AMGetErrorText(hr,errorText,100);
		NOTIFY("[] WARNING :: %s \r\n",errorText);
	}
}

/**
 <function> 
   <name>GetUnconnectedPin</name>
   <descr>
	  Obtains from pFilter an UNCONNECTED  PIN
	  with PinDir direction and saves it in ppPin
   </descr>  
 </function>
**/

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


/**
 <function> 
   <name>GetPin</name>
   <descr>
   Obtains from pFilter a connected or not PIN with PinDir
   direction  and saves it in ppPin.
   </descr>  
 </function>
**/
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

/**
 <function> 
   <name>GetPin</name>
   <descr>
   Obtains from pFilter a connected PIN with PinDir
   direction and saves it in ppPin 
   </descr>  
 </function>
**/

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

/**
 <function> 
   <name>lText</name>
   <descr>
   Gets from text the equivalent Wide Character String
   and saves it in wText, usefull for many DirectX functions
   input arguments
   </descr>  
 </function>
**/

 void lText(wchar_t * wtext,char * text){
	
	int n = mbstowcs(wtext,text,strlen(text));
	wtext[n]=0;
	return;

}

 void mbText(char * text,wchar_t * wtext){
	
	int n = wcstombs(text,wtext,wcslen(wtext));
	wtext[n]=0;
	return;

}



/**
 <function> 
   <name>enumVideoSources</name>
   <descr>
   Enumerates installed Video Sources in system,
   adds testSource and fileSource and creates a String with
   correct format.
   </descr>  
 </function>
**/

 void 
 enumVideoSources(char *deviceArray){

	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;
	
	memset(deviceArray,0,4096);
	char name[100];
	memset(name,0,100);
	deviceArray[0]='{';

	int i=1;
	
	// Create the System Device Enumerator.
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
    CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
    reinterpret_cast<void**>(&pDevEnum));
	if (SUCCEEDED(hr))
	{
	
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(
        CLSID_VideoInputDeviceCategory,
        &pEnum, 0);
		}

		if (pEnum==NULL){
			strcpy(deviceArray, "{testSource,file}");
			return;
		}
	
		while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
			(void**)(&pPropBag));
			if (FAILED(hr))
			{
				pMoniker->Release();
				continue;  // Skip this one, maybe the next one will work.
			} 
	
	// Find the description or friendly name.
    VARIANT varName;
    VariantInit(&varName);
    hr = pPropBag->Read(L"Description", &varName, 0);
    if (FAILED(hr))
    {
        hr = pPropBag->Read(L"FriendlyName", &varName, 0);
    }
    if (SUCCEEDED(hr))
    {
	
		wcstombs(name,varName.bstrVal,100);
		strcat(deviceArray,name);
        strcat(deviceArray,",");
		VariantClear(&varName); 
    }
	
    pPropBag->Release();
    pMoniker->Release();
	i = i + strlen(name) + 1;
	}
	
	strcat(deviceArray,"testSource,file}");
}

/**
 <function> 
   <name>defaultWindowInit</name>
   <descr>
   Fills default window channel settings,
   changing default defines, default window struct will be changed	
   </descr>  
 </function>
**/

 void defaultWindowInit(void){

		
	DEFAULT_WINDOW_INFO.width = DEFAULT_WINDOW_WIDTH;
	DEFAULT_WINDOW_INFO.left= DEFAULT_WINDOW_LEFT;
	DEFAULT_WINDOW_INFO.top = DEFAULT_WINDOW_TOP;
	DEFAULT_WINDOW_INFO.heigth = DEFAULT_WINDOW_HEIGTH;

}



/**
 <function> 
   <name>initSource</name>
   <descr>
   Loads a source with specific name and saves it in
   pFilter. Returns error code if can't load specified source 	
   </descr>  
 </function>
**/

 HRESULT initSource(char * name,IBaseFilter ** pSource)
{
	
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	IMoniker *pMoniker = NULL;
					
	// Create the System Device Enumerator.
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
    CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
    reinterpret_cast<void**>(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(
        CLSID_VideoInputDeviceCategory,
        &pEnum, 0);
	}

						
	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
        (void**)(&pPropBag));
		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;  // Skip this one, maybe the next one will work.
			} 
						
		// Find the description or friendly name.
		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"Description", &varName, 0);
		if (FAILED(hr))
		{
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		}
		if (SUCCEEDED(hr))
		{
			char sourceDev[100];
			wcstombs(sourceDev,varName.bstrVal,100);
			if(strcmp(sourceDev,name)==0)
			{
				pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
				(void**)&pSource);
			}
			VariantClear(&varName); 
		}
		pPropBag->Release();
		pMoniker->Release();
	}
	
	return hr;

}