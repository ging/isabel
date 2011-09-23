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
   <name>testSource.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of testSource.h.
   </descr>
//</head>
**/

#include "testsource.h"

double
fileSource_t::getCurTime(void)
{
	return curTime;
}

void
fileSource_t::setCurTime(double newCurTime)
{
	curTime = newCurTime;
}

fileSource_t::fileSource_t(char * file)
{
	int hr = 0;
	IBaseFilter * pFilter = NULL;
	IGraphBuilder * pGraph = NULL;
	IPin * input = NULL;
	
	setCurTime(0);

	wchar_t lFile[100];
	memset(lFile,0,100);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
	errorCheck(hr);

		
	hr = CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, 
    						IID_IBaseFilter, (void **)&pSource);
	
	hr = pGraph->AddFilter(pSource,L"Video Test Source");
	
	hr = pSource->QueryInterface(IID_IFileSourceFilter,(void **)&pConfig);
	if (file != NULL)
	{
		lText(lFile,file);
		hr = pConfig->Load(lFile,NULL);
        errorCheck(hr);
		setCamName(file);
	} else {
		hr = pConfig->Load(DEFAULT_TEST_FILE, NULL);
		errorCheck(hr);
		setCamName(DEFAULT_TEST_FILE_STR);
	}
	
	if (hr)
	{
		hr = pConfig->Load(DEFAULT_TEST_FILE,NULL);
		errorCheck(hr);
        if (hr)
        {
            AfxMessageBox("FATAL ERROR!!!\n"
                          "No webCams found, testSource.avi not found!!!\n"
                          "Quitting video daemon...",MB_OK,0);
            ExitProcess(0);
        }
		setCamName(DEFAULT_TEST_FILE_STR);
	}

	GetUnconnectedPin(pSource,PINDIR_OUTPUT,&output);
	hr = pGraph->Render(output);
	errorCheck(hr);
    if (hr)
    {
        AfxMessageBox("FATAL ERROR!!!\n"
                      "Can't decode file, check your codec/decoder",MB_OK,0);
        exit(-1);
    }
            
	hr = pGraph->FindFilterByName(L"AVI Decompressor",&pFilter);
	
	if (!hr)
	{
	
		hr = GetConnectedPin(pFilter,PINDIR_INPUT,&input);
	
	}else{

		hr = pGraph->FindFilterByName(L"Video Renderer",&pFilter);
		errorCheck(hr);
        if (pFilter)
        {
            hr = GetConnectedPin(pFilter,PINDIR_INPUT,&input);
            errorCheck(hr);
        }
	}

	input->ConnectedTo(&output);
	
	output->Disconnect();
	input->Disconnect();

	hr = pGraph->FindFilterByName(L"AVI Decompressor",&pFilter);
	
	if (!hr) 
	{
		hr = pGraph->RemoveFilter(pFilter);
		errorCheck(hr);
		pFilter->Release();
	}
	
	char * aux = list_source_formats();
	setSupportedFormats(aux);
	
	debugMsg( dbg_App_Normal,
			  "fileSource_t",
			  "File %s loaded succesfully",
			  getCamName());
			  
	pGraph->Release();
}

fileSource_t::~fileSource_t(void)
{
	camList->remove(getID());
	output->Release();
	pSource->Release();
}

	
