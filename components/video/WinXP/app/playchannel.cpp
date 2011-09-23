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
   <name>playchannel.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of playChannel.h.
   </descr>
//</head>
**/


#include "playchannel.h"
#include <math.h>

RTPContainer_t::RTPContainer_t(void)
{
    __CONTEXT("RTPContainer_t::RTPContainer_t");
	payloadHeaderLength = 0;
	
	for (int j=0; j<PACKET_BUFFER_SIZE;j++)
	{
			pkts[j] = NULL; 
	}
}

RTPContainer_t::~RTPContainer_t(void)
{    
	free();
}

void
RTPContainer_t::free(void)
{
    __CONTEXT("RTPContainer_t::free");
	for (int j=0; j<PACKET_BUFFER_SIZE;j++)
	{
		if (pkts[j]!=NULL)
		{
			delete pkts[j];
			pkts[j] = NULL; 
		}
	}
}

bool 
RTPContainer_t::insert(RTPPacket_t * packet)
{
    __CONTEXT("RTPContainer_t::insert");
	switch (packet->getPayloadType())
	{
	
	case __MPEG4:
		{
		payloadHeaderLength = DEFAULT_PAYLOAD_HEAD;
		break;
		}
	case __MPEG1:
		{
		payloadHeaderLength = sizeof (MPEG1Header_t);
		break;
		}
	case __H263:
		{
		Mode_Header_t *modeHeader = (Mode_Header_t*)packet->getData();
		if ((modeHeader->F == 0)&&(modeHeader->P == 0))
			payloadHeaderLength = 4; 
		
		if ((modeHeader->F == 0)&&(modeHeader->P == 1))
			payloadHeaderLength = 8;
		}	
		break;

	default:
		{
		payloadHeaderLength = DEFAULT_PAYLOAD_HEAD;
		break;
		}
	}

	int diff = 0;
	if (pkts[0]!= NULL)
		diff = packet->getSequenceNumber()-pkts[0]->getSequenceNumber();
	    
	// sequence cicle test
	if (abs(diff) > MAX_SEQ_DIFF)
	{
		if (diff>0)
			diff-=MAX_SEQ_NUM;
		else
			diff+=MAX_SEQ_NUM;
	}

	// duplication test
	for (int j = 0; j<PACKET_BUFFER_SIZE;j++)
	{ 
		if (pkts[j]!=NULL && pkts[j] != INVALID_HANDLE_VALUE)
		{
				if (pkts[j]->getSequenceNumber() == packet->getSequenceNumber())
				{
					debugMsg(dbg_App_Normal,
						 "playChannel_t",
						 "addFragment: receiving a duplicated packet. Throwing it\n");
					return true;
				}
		}
	}
	
	//if we receive a delayed packet
	if (diff < 0)
	{
			for (int j=PACKET_BUFFER_SIZE+diff; j>=0; j--)
			{
				pkts[j-diff] = pkts[j];
            }
			pkts[0] = packet;
			return true;
	}

	//Save the frame
	pkts[diff] = packet;
	return true;
}

bool
RTPContainer_t::hasInfo(void)
{
    __CONTEXT("RTPContainer_t::hasInfo");
	if (pkts[0] == NULL) 
		return false;
	else
		return true;
}
	
long
RTPContainer_t::getData(BYTE * data)
{
    __CONTEXT("RTPContainer_t::getData");
	int dataLen = 0;
	
    if (!pkts[0]) return 0;

	memset(data,0,1000000);
#if 0
	for(int j=0; j < PACKET_BUFFER_SIZE; j++)
	{
        if(pkts[j])
        {
            if(pkts[j]->getMark())
                break;
        }else{
            return 0; //losses
        }
    }
#endif 

	// H263 uses special defragmentation algoritm (H263 is bit oriented)
	if (pkts[0]->getPayloadType() == __H263)
	{
		for(int j=0; j < PACKET_BUFFER_SIZE; j++)
		{
			if (pkts[j]!=NULL && pkts[j] != INVALID_HANDLE_VALUE)	
			{
				Mode_Header_t *modeHeader = (Mode_Header_t*)pkts[j]->getData();
				BYTE * pData = pkts[j]->getData() + payloadHeaderLength;
				int size = pkts[j]->getDataLength()-payloadHeaderLength;
				
				if (modeHeader -> sbit)
				{
					int mask = 0xff;
					switch (modeHeader -> sbit) 
					{
						case 1: mask = 0x7f; break;
						case 2: mask = 0x3f; break;
						case 3: mask = 0x1f; break;
						case 4: mask = 0x0f; break;
						case 5: mask = 0x07; break;
						case 6: mask = 0x03; break;
						case 7: mask = 0x01; break;
					} 
				    data[dataLen-1] |= (pData[0] & mask);
					pData++;
					size--;
				}
				
				memcpy(data + dataLen,pData,size);   
				dataLen += size;

				if (modeHeader -> ebit != 0)
				{
					int mask = 0xff;
					switch (modeHeader -> ebit) 
					{
						case 1: mask = 0xfe; break;
						case 2: mask = 0xfc; break;
						case 3: mask = 0xf8; break;
						case 4: mask = 0xf0; break;
						case 5: mask = 0xe0; break;
						case 6: mask = 0xc0; break;
						case 7: mask = 0x80; break;
					} 
				    data[dataLen-1] &= mask; 
				
				}
				if (pkts[j]->getMark())
				{
						break;
				}
			}
		}

		return dataLen;
		
	}else{

		for(int j=0; j < PACKET_BUFFER_SIZE; j++)
		{
			if (pkts[j]!=NULL && pkts[j] != INVALID_HANDLE_VALUE)	
			{
				if (dataLen + pkts[j]->getDataLength()-payloadHeaderLength>100000)
					break;

				memcpy(data + dataLen,
							   pkts[j]->getData()+payloadHeaderLength,
							   pkts[j]->getDataLength()-payloadHeaderLength<10000?pkts[j]->getDataLength()-payloadHeaderLength:1000000);   

				dataLen += pkts[j]->getDataLength()-payloadHeaderLength;
						
				if (pkts[j]->getMark())
				{
						break;
				}
			}
		}
		return dataLen;
	}
}

unsigned long
RTPContainer_t::getTimestamp(void)
{
    __CONTEXT("RTPContainer_t::getTimestamp");
	if (pkts[0]!=NULL)
		return pkts[0]->getTimestamp();
	else
		return 0;
}


playChannel_t::playChannel_t(int ID,
							 const char * title, 
							 windowInfo_t * geom)
:channel_t(ID,title,geom),
 lastSeqNum(-1)

{
    __CONTEXT("playChannel_t::playChannel_t");
       
	frameCount = 0;
	buffer = 5;
	lastTS = 0;
	frameSequenceNumber = 0;

 	videoType.majortype = GUID_NULL;
	videoType.subtype = GUID_NULL;
	videoType.pUnk = 0;
	videoType.formattype = GUID_NULL;
	videoType.pbFormat = NULL;


	firstVideoType = true;
	followSize = false;

	sampleReader = new sampleReader_t("SampleReader",NULL,CLSID_NULL);
	sampleReader->Run();
	
    int hr = pGraph->AddFilter((IBaseFilter *)sampleReader,L"SampleReader");	
	errorCheck(hr);

	Kind = PLAY;
	rtpSession->newFlow(getId());
	channelList->insert(getId(),this);
	hr = pGraph->SetDefaultSyncSource();

}

playChannel_t::~playChannel_t(void)
{
    __CONTEXT("playChannel_t::~playChannel_t");
	unmap();
    sampleReader->TimeToDie();
	// Enumerate the filters in the graph.
	IEnumFilters *pEnum = NULL;
	
	if (pipeCreated)
	{
		int hr = pGraph->EnumFilters(&pEnum);
		
		if (SUCCEEDED(hr))
		{
				IBaseFilter *auxFilter = NULL;
				pEnum->Reset();
				if (!hr)
				{
					while (S_OK == pEnum->Next(1, &auxFilter, NULL))
					{
						// Remove the filter.
						pGraph->RemoveFilter(auxFilter);
						auxFilter->Release();
						//pEnum->Reset();
						
					}
					pEnum->Release();
				}
		}
	}

	pGraph->Release();
	channelList->remove(getId());
    rtpSession->deleteFlow(getId());
	for (int i=0; i<DEFAULT_FRAME_SIZE;i++)
	{
		frameBuffer[i].free();
	}
	
}

//check if pipeline is valid
HRESULT
playChannel_t::checkPipeLine(RTPPacket_t * packet)
{

    __CONTEXT("playChannel_t::checkPipeLine");
	u8  *data = NULL;
	int Width = 0;
	int Height = 0;
	bool makePipeLine = false;


	switch (packet->getPayloadType())
	{

	////////////////////////////////////////////////////////
	//  MPEG4 checkPipeline								  //
	////////////////////////////////////////////////////////
	
	case __MPEG4:
	{
		
		data  = packet->getData(); // begin of data 
		
		// RFC3016 VOL header
		
		if (data[0] == 0x00 &&  // START_SEQ = 0x000001B0
			data[1] == 0x00 &&
			data[2] == 0x01 &&
			data[3] == 0xB0 &&
			data[15]== 0x00 &&	// VIDEO_OBJECT_LAYER_START_CODE = 0x00000120
			data[16]== 0x00 &&
			data[17]== 0x01 &&
			data[18]== 0x20)    // We've received a new I-Frame
		{
		
		
			Width  = (*(data + 25) & 0xFF) << 5 | 
                     (*(data + 26) & 0xF8) >> 3; 
			Height = (*(data + 26) & 0x03) << 11|
                     (*(data + 27) & 0xFF) << 3 | 
                     (*(data + 28) & 0xE0) >> 5; 
		
	
			if ( !(Width * Height) ) 
			{
			
				NOTIFY("XVIDFragmenter_t::addFragment: "
					   "new MPEG4 image. "
					   "Bad size: %dx%d\n",
					   Width, Height
					   );
				return -1;
			}
		}
		
		//Some codecs use these VOL distribution, different than RFC3016
		//(what the hell were they thinking in when buildin' theese codecs???!!!)
		
		if (data[0] == 0x00 &&  // START_SEQ = 0x000001B0
			data[1] == 0x00 &&
			data[2] == 0x01 &&
			data[3] == 0xB0 &&
			data[14]== 0x00 &&	// VIDEO_OBJECT_LAYER_START_CODE = 0x00000120
			data[15]== 0x00 &&  // changes position :(
			data[16]== 0x01 &&
			data[17]== 0x20)    // We've received a new I-Frame
		{
		
		
			Width  = (*(data + 23) & 0x03) << 11 | 
                     (*(data + 24) & 0xFF) << 3  | 
                     (*(data + 25) & 0xE0  >> 5); //changes position !!
			Height = (*(data + 25) & 0x0F) << 9  | 
                     (*(data + 26) & 0xFF) << 1  | 
                     (*(data + 27) & 0x80) >> 7; //changes position !!
		
	
			if ( !(Width * Height) ) 
			{
			
				NOTIFY("XVIDFragmenter_t::addFragment: "
					   "new MPEG4 image. "
					   "Bad size: %dx%d\n",
					   Width, Height
					   );
				return -1;
			}
		}

		//If we've readed any resolution
		if (Width * Height) 
		{
			VIDEOINFOHEADER * auxVideoInfo = (VIDEOINFOHEADER *)videoType.pbFormat;
		
			if (firstVideoType || lastPayLoad != __MPEG4)
				makePipeLine = true;
			else
				if (auxVideoInfo)
				if (Width  != auxVideoInfo->bmiHeader.biWidth ||
					Height != auxVideoInfo->bmiHeader.biHeight)
					makePipeLine = true;
	
			if (makePipeLine)
			{
				lastPayLoad = __MPEG4;
				NOTIFY("PlayChannel_t %d :: Making MPEG4 pipeline :: Width = %d, Height = %d\n",getId(),Width,Height);
				if (!firstVideoType && videoType.pbFormat!=NULL)
				{
					free(videoType.pbFormat);
					videoType.pbFormat=NULL;
				}
			
				firstVideoType = false;
			
				VIDEOINFOHEADER * videoInfo = (VIDEOINFOHEADER *)malloc(sizeof(VIDEOINFOHEADER));
				
				videoInfo->AvgTimePerFrame = 0;
				videoInfo->rcSource.left   = 0;
				videoInfo->rcSource.right  = 0;
				videoInfo->rcSource.top    = 0;
				videoInfo->rcSource.bottom = 0;
				videoInfo->rcTarget.left   = 0;
				videoInfo->rcTarget.right  = 0;
				videoInfo->rcTarget.top    = 0;
				videoInfo->rcTarget.bottom = 0;
				videoInfo->bmiHeader.biSizeImage = (Width*Height)*24/8;
				videoInfo->bmiHeader.biWidth = Width;
				videoInfo->bmiHeader.biHeight = Height;
				videoInfo->bmiHeader.biCompression = FCC('XVID');
				videoInfo->bmiHeader.biPlanes = 1; // 1 plane
				videoInfo->bmiHeader.biBitCount = 24; // 24 bits color
				videoInfo->bmiHeader.biXPelsPerMeter = 0; // Not specified
				videoInfo->bmiHeader.biYPelsPerMeter = 0; 
				videoInfo->bmiHeader.biClrUsed = 0; 
				videoInfo->bmiHeader.biClrImportant = 0; //All
				videoInfo->bmiHeader.biSize = sizeof(videoInfo->bmiHeader);
			
				videoType.majortype = MEDIATYPE_Video;
				videoType.subtype   = (GUID)FOURCCMap(FCC('XVID'));
				videoType.bTemporalCompression = 1;
				videoType.bFixedSizeSamples = false;
				videoType.formattype = FORMAT_VideoInfo;
				videoType.lSampleSize = (Width * Height)*16/8;
				videoType.cbFormat = sizeof(*videoInfo);
				videoType.pbFormat = (BYTE *)videoInfo;
				videoType.pUnk = NULL;

    			sampleReader->SetMediaType((CMediaType *)&videoType);
                pipeCreated = false;
				
			}
		}
	}
	////////////////////////////////////////////////////////
	//  MPEG4 End										  //
	////////////////////////////////////////////////////////
	return 0;

	////////////////////////////////////////////////////////
	//  MPEG1 checkPipeline								  //
	////////////////////////////////////////////////////////
	
	case __MPEG1:
	{

		data = packet->getData();
		MPEG1Header_t *header = (MPEG1Header_t*)data;
        u8 *mpegData = data + sizeof (MPEG1Header_t);

        if (header -> S == 1) 
		{

			//NOTIFY("PlayChannel_t :: MPEG1 Keyframe received\n"); 
			u16 aux = mpegData[5]; aux &= 0x00f0; aux >>= 4;
            u16 aux2 = mpegData[4]; aux2 <<= 4; aux2 &= 0x0ff0;
            Width = aux | aux2;

            aux = mpegData[6]; aux &= 0x00ff;
            aux2 = mpegData[5]; aux2 <<= 8; aux2 &= 0x0f00;
            Height = aux | aux2;
		    
			if ( !(Width * Height) ) 
			{
		
				NOTIFY("MPEGFragmenter_t::addFragment: "
					   "new MPEG1 image. "
					   "Bad size: %dx%d\n",
					   Width, Height
					   );
				return -1;
			}

			VIDEOINFOHEADER * auxVideoInfo = (VIDEOINFOHEADER *)videoType.pbFormat;
		
			if (firstVideoType || lastPayLoad != __MPEG1)
				makePipeLine = true;
			else
				if (auxVideoInfo)
				if (Width  != auxVideoInfo->bmiHeader.biWidth ||
					Height != auxVideoInfo->bmiHeader.biHeight)
						makePipeLine = true;

			if (makePipeLine)
			{
				NOTIFY("PlayChannel_t %d:: Making MPEG1 pipeline :: Width = %d, Height = %d\n",getId(),Width,Height);
				if (!firstVideoType && videoType.pbFormat!=NULL)
				{
					free(videoType.pbFormat);
					videoType.pbFormat=NULL;
				}
			
				firstVideoType = false;
				lastPayLoad = __MPEG1;

			
				VIDEOINFOHEADER * videoInfo = (VIDEOINFOHEADER *)malloc(sizeof(VIDEOINFOHEADER));
				
				videoInfo->AvgTimePerFrame = 0;
				videoInfo->rcSource.left   = 0;
				videoInfo->rcSource.right  = 0;
				videoInfo->rcSource.top    = 0;
				videoInfo->rcSource.bottom = 0;
				videoInfo->rcTarget.left   = 0;
				videoInfo->rcTarget.right  = 0;
				videoInfo->rcTarget.top    = 0;
				videoInfo->rcTarget.bottom = 0;
				videoInfo->bmiHeader.biSizeImage = (Width*Height)*24/8;
				videoInfo->bmiHeader.biWidth = Width;
				videoInfo->bmiHeader.biHeight = Height;
				videoInfo->bmiHeader.biCompression = FCC('MPEG');
				videoInfo->bmiHeader.biPlanes = 1; // 1 plane
				videoInfo->bmiHeader.biBitCount = 24; // 24 bits color
				videoInfo->bmiHeader.biXPelsPerMeter = 0; // Not specified
				videoInfo->bmiHeader.biYPelsPerMeter = 0; 
				videoInfo->bmiHeader.biClrUsed = 0; 
				videoInfo->bmiHeader.biClrImportant = 0; //All
				videoInfo->bmiHeader.biSize = sizeof(videoInfo->bmiHeader);
			
				videoType.majortype = MEDIATYPE_Video;
				videoType.subtype   = (GUID)FOURCCMap(FCC('MPEG'));
				videoType.bTemporalCompression = 1;
				videoType.bFixedSizeSamples = false;
				videoType.formattype = FORMAT_VideoInfo;
				videoType.lSampleSize = (Width * Height)*16/8;
				videoType.cbFormat = sizeof(*videoInfo);
				videoType.pbFormat = (BYTE *)videoInfo;
				videoType.pUnk = NULL;
			
				sampleReader->SetMediaType((CMediaType *)&videoType);
                pipeCreated = false;
							
	
			}
		}		
	}
	////////////////////////////////////////////////////////
	//  MPEG1 End										  //
	////////////////////////////////////////////////////////
	return 0;

	////////////////////////////////////////////////////////
	//  H263 checkPipeline								  //
	////////////////////////////////////////////////////////
	
	case __H263:
	{

		data = packet->getData();
		Mode_Header_t *modeHeader = (Mode_Header_t*)data;
	
		int mode;
		
		if ((modeHeader->F == 0)&&(modeHeader->P == 0))
		{
			mode = 0;
		}else{
			if ((modeHeader->F == 0)&&(modeHeader->P == 1))
			{
				mode = 1;
			}else{ 
				NOTIFY ("H263 AddFragment: Mode not valid\n");
				return -1;
			}	
		}

		if (mode == 0 || mode == 1) 
		{
			H263_0Header_t *header = (H263_0Header_t*)data;
			switch (header -> src) 
			{
            
			case 1:
				Width = 128;
				Height = 96;
		        break;
			case 2:
                Width = 176;
                Height = 144;
                break;
    		case 3:
                Width = 352;
                Height = 288;
                break;
  			case 4:
                Width = 704;
                Height = 576;
                break;
			case 5:
               	Width = 1408;
                Height = 1152;
                break;
			case 7:
                Width = 1408;
                Height = 1152;	
                break;
            
			default:
                Width = 0;
                Height = 0;	
			
			}
					
	   
		}

		if ( !(Width * Height) ) 
			{
		
				NOTIFY("H263Fragmenter_t::addFragment: "
					   "new H263 image. "
					   "Bad size: %dx%d\n",
					   Width, Height
					   );
				return -1;
			}

			VIDEOINFOHEADER * auxVideoInfo = (VIDEOINFOHEADER *)videoType.pbFormat;
		
			if (firstVideoType || lastPayLoad != __H263)
				makePipeLine = true;
			else
				if (auxVideoInfo)
				if (Width  != auxVideoInfo->bmiHeader.biWidth ||
					Height != auxVideoInfo->bmiHeader.biHeight)
						makePipeLine = true;

			if (makePipeLine)
			{
                NOTIFY("PlayChannel_t %d :: Making H263 pipeline :: Width = %d, Height = %d\n",getId(),Width,Height);
				if (!firstVideoType && videoType.pbFormat!=NULL)
				{
					free(videoType.pbFormat);
					videoType.pbFormat=NULL;
				}
			
				firstVideoType = false;
				lastPayLoad = __H263;

			
				VIDEOINFOHEADER * videoInfo = (VIDEOINFOHEADER *)malloc(sizeof(VIDEOINFOHEADER));
				
				videoInfo->AvgTimePerFrame = 0;
				videoInfo->rcSource.left   = 0;
				videoInfo->rcSource.right  = 0;
				videoInfo->rcSource.top    = 0;
				videoInfo->rcSource.bottom = 0;
				videoInfo->rcTarget.left   = 0;
				videoInfo->rcTarget.right  = 0;
				videoInfo->rcTarget.top    = 0;
				videoInfo->rcTarget.bottom = 0;
				videoInfo->bmiHeader.biSizeImage = (Width*Height)*16/8;
				videoInfo->bmiHeader.biWidth = Width;
				videoInfo->bmiHeader.biHeight = Height;
				videoInfo->bmiHeader.biCompression = FCC('H263');
				videoInfo->bmiHeader.biPlanes = 1; // 1 plane
				videoInfo->bmiHeader.biBitCount = 16; // 16 bits color
				videoInfo->bmiHeader.biXPelsPerMeter = 0; // Not specified
				videoInfo->bmiHeader.biYPelsPerMeter = 0; 
				videoInfo->bmiHeader.biClrUsed = 0; 
				videoInfo->bmiHeader.biClrImportant = 0; //All
				videoInfo->bmiHeader.biSize = sizeof(videoInfo->bmiHeader);
			
				videoType.majortype = MEDIATYPE_Video;
				videoType.subtype   = (GUID)FOURCCMap(FCC('H263'));
				videoType.bTemporalCompression = 1;
				videoType.bFixedSizeSamples = false;
				videoType.formattype = FORMAT_VideoInfo;
				videoType.lSampleSize = (Width * Height)*16/8;
				videoType.cbFormat = sizeof(*videoInfo);
				videoType.pbFormat = (BYTE *)videoInfo;
				videoType.pUnk = NULL;
				IPin * auxOutput = sampleReader->getOutputPin();
				auxOutput->Disconnect();
				sampleReader->SetMediaType((CMediaType *)&videoType);
                pipeCreated = false;							
			    return -1;
                
			}

			return 0;
			
	}	
	////////////////////////////////////////////////////////
	//  H263 End										  //
	////////////////////////////////////////////////////////
	
	}
	
	return 0;
}



// Here we defragment the frames

void
playChannel_t::addFragment(RTPPacket_t * packet)
{

    __CONTEXT("playChannel_t::addFragment");
	bool found = false;
    lastSeqNum = packet->getSequenceNumber();
	
	//Check if we receive video type
	
	for (int i = 0;i<DEFAULT_FRAME_SIZE;i++)
	{	
			if (frameBuffer[i].getTimestamp() == packet->getTimestamp())
			{
				found = true;
				frameCount = i;
				break;
			}
	}
	//if we detect a new Frame 
	if (!found)
	{
		//Check if the pipeLine is valid

		if (!checkPipeLine(packet))
		{
            bool full = true;
            int pos = 0;
            u32 TS = frameBuffer[frameCount].getTimestamp();
            if (lastTS == 0)
                lastTS = packet->getTimestamp();
            for (int i=0;i<buffer;i++)
            {
                
                if (frameBuffer[i].hasInfo())
                {
                    if(TS>frameBuffer[i].getTimestamp() &&
                       frameBuffer[i].getTimestamp() != packet->getTimestamp())
                    {
                        frameCount = i;
                        TS = frameBuffer[i].getTimestamp();
                    }
                     
                }else{
                    frameCount = i;
                    full=false;
                    break;
                }
			}
            //NOTIFY("pop TS= %d \n",TS);

			if (full)
			{
	
				//NOTIFY("Last TS, frameCountTs, diff: %u, %u, %u\n",lastTS,frameBuffer[frameCount].getTimestamp(),lastTS - frameBuffer[frameCount].getTimestamp());
				if (frameBuffer[frameCount].getTimestamp() > lastTS ||
		            lastTS - frameBuffer[frameCount].getTimestamp()>0xffff)
			    {
					
				    if (mapping && !pipeCreated)
					{
						map();
		                if (!pipeCreated)
			                return;
				    }
	
			        if (mapping)
				    {
					    frame_t * auxFrame = new frame_t();
						auxFrame->sequenceNumber = frameSequenceNumber++;
				    
					    debugMsg(dbg_App_Normal,
							     "playChannel_t",
							     "addFragment: Complete Frame, sending to sampleReader.\n");
				    
					    BYTE * data = new BYTE[1000000]; 
					    auxFrame->dataLen = frameBuffer[frameCount].getData(data);
					    auxFrame->data = data;
					    
					    if (auxFrame->dataLen && mapping) 
						{
	                        lastTS = auxFrame->timestamp = frameBuffer[frameCount].getTimestamp();
					        sampleReader->receiveFrame(auxFrame);
					    }else{
				            NOTIFY("PlayChannel %d :: Lost frame\n",getId());
						    delete auxFrame->data;
						    delete auxFrame;
					    }
				    }

			    }else{
				    NOTIFY("PlayChannel %d :: Old frame, lastTS = %d, frameTS = %d\n",
					        getId(),
						    lastTS,
							frameBuffer[frameCount].getTimestamp());
				}
			}
		
			frameBuffer[frameCount].free();
		}
	}
	if (!frameBuffer[frameCount].insert(packet))
	{
		NOTIFY("PlayChannel :: RX Buffer Overflow\r\n");
		frameBuffer[frameCount].free();
	}	
    //NOTIFY("push TS= %d \n",packet->getTimestamp());
}

HRESULT
playChannel_t::map(void)
{
    __CONTEXT("playChannel_t::map");
	remap();
    mapping = true;
	int hr = 0;
	AM_MEDIA_TYPE * filterVideoType = new AM_MEDIA_TYPE;
	memset(filterVideoType,0,sizeof(AM_MEDIA_TYPE));
	sampleReader->GetMediaType((CMediaType *)filterVideoType);
	
	VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) videoType.pbFormat;
	
	
	if (videoType.subtype != GUID_NULL && videoType.subtype != filterVideoType->subtype)
	{
		sampleReader->SetMediaType((CMediaType *)&videoType);
	}

	if (videoType.subtype!= GUID_NULL)
	{
		pOutput = sampleReader->getOutputPin();
		if (pOutput)
			pOutput->Disconnect();
		GetUnconnectedPin(sampleReader,PINDIR_OUTPUT,&pOutput);
		hr = pGraph->Render(pOutput);
		pControl->Run();
		pipeCreated = true;
		if (hr)
		{
					errorCheck(hr);
					NOTIFY("WARNING :: Can't render actual format\r\n");

		}

		IEnumFilters *pEnum = NULL;
		int hr = pGraph->EnumFilters(&pEnum);

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
				pWindowInfo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER);
				setOwner();
			}
		}
	/*	
		if (fullScreen){
			set_full_screen(true);
		}else{
		
			if (followSize)
			{
				
				windowInfo.width  = pVih->bmiHeader.biWidth;
				windowInfo.heigth = pVih->bmiHeader.biHeight;
			}
			hr = setWindowGeometry(windowInfo);
			errorCheck(hr);
		}

		IVideoWindow *pWindowInfo = NULL;
		hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pWindowInfo);
		if (!hr)
		{
		    wchar_t wtext[100];
		    long windowStyle,windowStyleEx;
		    lText(wtext,title);
		    pWindowInfo->get_WindowStyle(&windowStyle);
            pWindowInfo->get_WindowStyleEx(&windowStyleEx);
		    windowStyle = windowStyle + DEFAULT_WINDOW_PROPS - DEFAULT_WINDOW_NON_PROPS;
            windowStyleEx = windowStyleEx - WS_EX_APPWINDOW;
		    pWindowInfo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
            pWindowInfo->put_WindowStyleEx(WS_EX_TOOLWINDOW);
		    pWindowInfo->put_Caption(wtext);
			pWindowInfo->put_Visible(OATRUE);			
			pWindowInfo->put_AutoShow(OATRUE);			
			pWindowInfo->Release();
			setOwner();
        }
		*/
		mapping = true;
	}else{
		NOTIFY(" PlayChannel_t :: map() :: Not receiving video flow on channel %d.\r\n",getId());
		hr = 1;
	}
	return hr;
}

HRESULT
playChannel_t::unmap(void)
{
    __CONTEXT("playChannel_t::unmap");
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
	
	//pControl->StopWhenReady();
    mapping = false;
	debugMsg(dbg_App_Normal,
	         "recChannel_t",
			 "channel %d unmapped successfully",
			  getId());
		
	return 0;
}
	
HRESULT
playChannel_t::remap(void)
{
    __CONTEXT("playChannel_t::remap");
    sampleReader->EndThread();
    for (int i=0; i<DEFAULT_FRAME_SIZE;i++)
	{
		frameBuffer[i].free();
	}
	// Enumerate the filters in the graph.
	IEnumFilters *pEnum = NULL;
	int hr = pGraph->EnumFilters(&pEnum);
	
    if (SUCCEEDED(hr) && mapping)
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
			   
				pWindowInfo->put_AutoShow(OATRUE);
			    pWindowInfo->put_Visible(OAFALSE);
			   	pGraph->RemoveFilter(pFilter);
				pFilter->Release();
				pEnum->Reset();
				pWindowInfo->Release();
			
			}else{
				pGraph->RemoveFilter(pFilter);
				//pFilter->Release();
				pEnum->Reset();
			}
		}
		
	}
    
	if (pEnum)
		pEnum->Release();	

	pGraph->Release();
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                          IID_IGraphBuilder, (void **)&pGraph);
	errorCheck(hr);
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	errorCheck(hr);

 	sampleReader = new sampleReader_t("SampleReader",NULL,CLSID_NULL);
    sampleReader->Run();
   
	hr = pGraph->AddFilter((IBaseFilter *)sampleReader,L"SampleReader");	
	
	errorCheck(hr);

	return S_OK;	
}

void
playChannel_t::setFollowSize(bool followFlag)
{
    __CONTEXT("playChannel_t::setFollowSize");
	followSize = followFlag;
}


HRESULT
playChannel_t::setBuffer(int newBuffer)
{
    __CONTEXT("playChannel_t::setBuffer");
	if (newBuffer<=200 && newBuffer>=5)
	{
		buffer = newBuffer;
		return S_OK;
	}

	return E_FAIL;
}

		