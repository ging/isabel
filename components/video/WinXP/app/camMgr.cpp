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
   <name>camMgr.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of camMgr.h
   </descr>
//</head>
**/


#include "camMgr.h"
#include "namedguid.h"

char camInfo_t::Str[STR_AUX_SIZE];

/**
 <function> 
   <name>CamListInit()</name>
   <descr>
   Initializes CamList with the (CAM) 
   sources installed in the system. 
   </descr>  
 </function>
**/
void camListInit(void)
{
	char camStr[100];
	int i, k= 0;
	i=1;
	memset(camStr,0,100);
	char aux[4096];
	enumVideoSources(aux);
	strcpy(camStr,aux);

	for ( ; camStr[i] != '}'; i++)
	{
	   	char camName[100];
		memset(camName,0,100);
		
		for (int j=0; camStr[i]!=',' && camStr[i]!='}'; i++)
        {
			camName[j]= camStr[i];
			j++;
		}

        if (camStr[i]=='}')
	        break;
        
        camInfo_t *camInfo = new camInfo_t;
		camInfo->setCamName(camName);
		
		if (strcmp(camInfo->getCamName(),"testSource")!=0 &&
			strcmp(camInfo->getCamName(),"file") !=0 &&
			strcmp(camInfo->getCamName(),"TV Series VfW Driver") != 0
		   )
		{
			camInfo->setID(k++);
			camInfo->setKind(CAM);
			camInfo->setFree(true);
			camInfo->initSource();
			camInfo->initFormats();
            camInfo->initRes();
			GetUnconnectedPin(camInfo->pSource,PINDIR_OUTPUT,&(camInfo->output));
			camArray->add(camInfo);
		}
	}
}

// Is there any freeSources() ??
/**
 <function> 
   <name>freeSources()</name>
   <descr>
   Returns a boolean telling if there are any   
   free sources in camList, if freesources() returns false     
   a new source will be loaded for that channel: by default   
   this source is a TEST source.  
   </descr>  
 </function>
**/
bool freeSources(void)
{
	for (int j = 0; j < camArray->size(); j++)
	{
        camInfo_t *cam= camArray->elementAt(j);
		if (cam->getFree() && cam->getKind() == CAM)
		{
			return true;
		}
	}

	return false;
}



vector_t<camInfo_t*> camArray;

camInfo_t::camInfo_t(void)
{
    __CONTEXT("camInfo_t::camInfo_t");
    memset(supportedRes,0,strlen(supportedRes));
}

sourceKind_e 
camInfo_t::getKind(void)
{
    __CONTEXT("camInfo_t::getKind");
	return Kind;
}


HRESULT
camInfo_t::setKind(sourceKind_e newKind)
{
    __CONTEXT("camInfo_t::setKind");
	
	if (newKind == CAM  ||
		newKind == TEST ||
		newKind == MEDIA||
		newKind == SHARED)
	{
		Kind = newKind;
		return 0;
	}else{
		return -1;
	}

}

bool  
camInfo_t::getFree(void)
{
    __CONTEXT("camInfo_t::getFree");
	return free;
}

void
camInfo_t::setFree(bool newFree)
{
    __CONTEXT("camInfo_t::setFree");
	free = newFree;
}
	
char *  
camInfo_t::getCamName(void)
{
    __CONTEXT("camInfo_t::getCamName");
	return camName;
}

void
camInfo_t::setCamName(char * newName)
{
    __CONTEXT("camInfo_t::setCamName");
	memset(camName,0,100);
	strcpy(camName,newName);
}

char *  
camInfo_t::getSupportedFormats(void)
{
    __CONTEXT("camInfo_t::getSupportedFormats");
	return supportedFormats;
}


void    
camInfo_t::setSupportedFormats(char * newSupportedFormats)
{
    __CONTEXT("camInfo_t::setSupportedFormats");
	memset(supportedFormats,0,100);
	strcpy(supportedFormats,newSupportedFormats);
}

int     
camInfo_t::getID()
{
    __CONTEXT("camInfo_t::getID");
	return ID;
}

void    
camInfo_t::setID(int newID)
{
    __CONTEXT("camInfo_t::setID");
	ID = newID;
}


HRESULT 
camInfo_t::initSource(void)
{
    __CONTEXT("camInfo_t::initSource");
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
			if(strcmp(sourceDev,camName)==0)
			{
				pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
				(void**)&pSource);
				
				debugMsg(dbg_App_Normal,
						 "camInfo_t",
						 "InitSource:: "
						 "Loaded source % s succesfully",
						 camName);

			}
			VariantClear(&varName); 
		}
		pPropBag->Release();
		pMoniker->Release();
	}

	char * aux = list_source_formats();
	
	setSupportedFormats(aux);

	return hr;

}

char * 
camInfo_t::list_source_formats(void)
{
    __CONTEXT("camInfo_t::list_source_formats");
	IEnumMediaTypes * pEnumMedia = NULL;
	AM_MEDIA_TYPE * pMediaType = NULL;
	int hr = 0;	

	memset((void *)Str,0,STR_AUX_SIZE);
	Str[0] = '{';

	hr = GetPin(pSource,PINDIR_OUTPUT,&output);
	errorCheck(hr);
	if (hr)
	{
		
		IEnumPins * pinEnum = NULL;
        hr = pSource->EnumPins(&pinEnum);
		errorCheck(hr);
		while(pinEnum->Next(1, &output, 0) == S_OK)
		{
			break;
		}
		if (output == NULL)
			NOTIFY("CamInfo_t :: Can't load source output PIN, potential error source\r\n");
   
	}
	
	output->EnumMediaTypes(&pEnumMedia);
	while(pEnumMedia->Next(1, &pMediaType , NULL) == S_OK)
    {
    	char subtypeName [100];
		memset(subtypeName,0,100);
		GetGUIDString(subtypeName,&pMediaType->subtype);		
		if(strstr(Str,subtypeName)==NULL){
				strcat(Str,subtypeName);
				strcat(Str,";");
		}
				DeleteMediaType(pMediaType);	

	}

	pEnumMedia->Release();
	strcat(Str,"}");

	return Str;
}

HRESULT 
camInfo_t::initFormats(void)
{
    __CONTEXT("camInfo_t::initFormats");
	
	int hr = 0;
	IEnumMediaTypes * pEnumMedia = NULL;
	AM_MEDIA_TYPE * pMediaType = NULL;
	
	hr = GetPin(pSource,PINDIR_OUTPUT,&output);
	errorCheck(hr);
	output->EnumMediaTypes(&pEnumMedia);
	while(pEnumMedia->Next(1, &pMediaType , NULL) == S_OK)
    {
		formats.insert(pMediaType);

	}
	
	return hr;
}

char *
camInfo_t::getSupportedRes(void)
{
    __CONTEXT("camInfo_t::getSupportedRes");
    return supportedRes;
}

HRESULT
camInfo_t::initRes(void)
{
    __CONTEXT("camInfo_t::initRes");
    
    memset(supportedRes,0,strlen(supportedRes));
    strcat(supportedRes,";");
	for (ql_t<AM_MEDIA_TYPE *>::iterator_t i = formats.begin();
		 i != formats.end();
		 i++
		)
    {
	    AM_MEDIA_TYPE *format = i;
		VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) format->pbFormat;
		
		if(pVih!=NULL)
        {
		   
            char auxHeight[5];
            char auxWidth[5];
            
            itoa(pVih->bmiHeader.biHeight,auxHeight,10);
        	itoa(pVih->bmiHeader.biWidth,auxWidth,10);
		    
            strcat(supportedRes,auxWidth);
            strcat(supportedRes,"x");
            strcat(supportedRes,auxHeight);
            strcat(supportedRes,";");

        }
	}
    return 0;
}

ql_t<AM_MEDIA_TYPE *>
camInfo_t::getFormatList(void)
{
    __CONTEXT("camInfo_t::getFormatList");
	return formats;
}

camInfo_t::~camInfo_t(void)
{
    __CONTEXT("camInfo_t::~camInfo_t");
}



