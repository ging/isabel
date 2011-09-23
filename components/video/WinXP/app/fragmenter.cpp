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
   <name>fragmenter.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of fragmenter.h
   </descr>
//</head>
**/

#include "fragmenter.h"

fragmenter_t::fragmenter_t()
{
    __CONTEXT("video_InterfaceDef_t::video_bucket_size");
    frame = NULL;
    fragment = NULL;
    frameCount = 0;
}

fragmenter_t::~fragmenter_t()
{
    __CONTEXT("video_InterfaceDef_t::video_bucket_size");
	delete frame;
}

void
fragmenter_t::setFrame(BYTE *pBuffer, long BufferLen,int payLoad,int Width,int Height)
{
    __CONTEXT("video_InterfaceDef_t::video_bucket_size");
	frameCount = 0;

	if (frame)
		delete frame;

	this->payLoad = payLoad;
	this->Width   = Width;
	this->Height  = Height;
	

	if (payLoad == __MPEG4)
	{
	
		frame = new mpegEncodedImage_t();
		
		frame->buffer   = pBuffer;
		frame->numBytes = BufferLen;
		
		return;
	}

	// If payload == _MPEG1 we've to add some extra header

	if (payLoad == __MPEG1) 
	{
	
		frame = new mpegEncodedImage_t();
		
		mpegEncodedImage_t * mpegFrame = static_cast<mpegEncodedImage_t *>(frame);

		mpegFrame->buffer = pBuffer;
		mpegFrame->w = Width;
		mpegFrame->h = Height;
		mpegFrame->bytesPopped = 0;
		mpegFrame->numBytes = BufferLen;

		//we need it for creating the RTP/MPEG1 headers
		for (unsigned int i = 0; i < BufferLen ; i++){
			if ((pBuffer[i]   == 0x00) && 
				(pBuffer[i+1] == 0x00) && 
				(pBuffer[i+2] == 0x01) && 
				(pBuffer[i+3] == 0x00))
			{
			  u16 aux = pBuffer[i+5]; aux &= 0x00c0; aux >>= 6;
			  u16 aux2 = pBuffer[i+4]; aux2 <<= 2; aux2 &= 0x03fc;
			  mpegFrame->temporal_reference = aux | aux2;

			  u8 aux3 = pBuffer[i+5]; aux3 &= 0x38; aux3 >>= 3;
			  mpegFrame->picture_type = aux3;

			  if (mpegFrame->picture_type == 2){
				  u8 aux4 = pBuffer[i+7]; aux4 <<= 1; aux4 &= 0x06;
				  u8 aux5 = pBuffer[i+8]; aux5 &= 0x80; aux5 >>= 7;
				  mpegFrame->FFC = aux4 | aux5;
			  }
			  else{
				  mpegFrame->FFC = 0;
			  }
			}
		 }
		
		return;
	}

	if ( payLoad == __H263)
	{
		frame = new h263EncodedImage_t();
		h263EncodedImage_t * H263frame = static_cast<h263EncodedImage_t *>(frame);
		H263frame->buffer   = pBuffer;
		H263frame->numBytes = BufferLen;
		H263frame->mode = 0;
		H263frame->w = Width;
		H263frame->h = Height;
		
		return;
	}

	return;
}

int  
fragmenter_t::getFragment(BYTE ** fragment,int * size,int offset)
{
    __CONTEXT("video_InterfaceDef_t::video_bucket_size");

	if (frameCount<frame->numBytes)
	{
		if (payLoad == __MPEG4)
		{
			if ((frameCount + *size)<frame->numBytes)
			{
				memcpy(*(fragment)+offset,frame->buffer + frameCount, *size);
				frameCount += *size;
				
			}else{
				memcpy(*(fragment)+offset,frame->buffer + frameCount,frame->numBytes - frameCount);
				*size = (frame->numBytes - frameCount);
				frameCount = frame->numBytes;
			
			}
			return (frame->numBytes - frameCount); //return remain frame 
		}

		if (payLoad == __MPEG1)
		{
		     mpegEncodedImage_t * mpegFrame = static_cast<mpegEncodedImage_t *>(frame);

			 MPEG1Header_t header1;
    		 header1.word1 = mpegFrame->temporal_reference;
			 header1.word1 &= 0x03ff;
			 header1.word1 = htons (header1.word1);

			 header1.AN = 0;
			 header1.N = 0;
			 header1.P = mpegFrame->picture_type;

			 header1.FBV = 0;
			 header1.BFC = 0;
			 header1.FFV = 0;
			 header1.FFC = mpegFrame->FFC;

			 int dataSize = 0;

			 if (mpegFrame->bytesPopped == 0) // first Packet
			 {
						if ( (mpegFrame->buffer[0] == 0) && 
							 (mpegFrame->buffer[1] == 0) && 
							 (mpegFrame->buffer[2] == 1) && 
							 (mpegFrame->buffer[3] == 0xb3))
						{
							header1.S = 1;
						} else {
							header1.S = 0;
						}
						header1.B = 1;
						// All in one packet
						if (mpegFrame->numBytes < (*size - sizeof (MPEG1Header_t))) 
						{
							dataSize = mpegFrame->numBytes;
							header1.E = 1;
						} else {
							// All in various packets
							dataSize = *size - sizeof (MPEG1Header_t);
							if ((mpegFrame->buffer[dataSize] == 0)
								&& (mpegFrame->buffer[dataSize+1] == 0)
								&& (mpegFrame->buffer[dataSize+2] == 1)
								&& (mpegFrame->buffer[dataSize+3] >= 0x01)
								&& (mpegFrame->buffer[dataSize+3] <= 0xaf) ) {
									 header1.E = 1;
							}
						}
					} else {
						// second and later packets
						header1.S = 0;
						if (   (mpegFrame->buffer[mpegFrame->bytesPopped] == 0)
							&& (mpegFrame->buffer[mpegFrame->bytesPopped+1] == 0)
							&& (mpegFrame->buffer[mpegFrame->bytesPopped+2] == 1)
							&& (mpegFrame->buffer[mpegFrame->bytesPopped+3] >= 0x01)
							&& (mpegFrame->buffer[mpegFrame->bytesPopped+3] <= 0xaf) ) {
							header1.B = 1;
							//last Packet
							if ( (mpegFrame->numBytes - mpegFrame->bytesPopped) < (*size - sizeof (MPEG1Header_t)) ) {
								
								dataSize = mpegFrame->numBytes - mpegFrame->bytesPopped;
								header1.E = 1;
			
							//not last	
							} else {
								dataSize = *size - sizeof (MPEG1Header_t);
								if (   (mpegFrame->buffer[mpegFrame->bytesPopped+dataSize] == 0)
									&& (mpegFrame->buffer[mpegFrame->bytesPopped+dataSize+1] == 0)
									&& (mpegFrame->buffer[mpegFrame->bytesPopped+dataSize+2] == 1)
									&& (mpegFrame->buffer[mpegFrame->bytesPopped+dataSize+3] >= 0x01)
									&& (mpegFrame->buffer[mpegFrame->bytesPopped+dataSize+3] <= 0xaf)){
									header1.E = 1;
								}
							}
						} else {
							header1.B = 0;
							bool newSlice = false;
							unsigned int i;
							for (i = mpegFrame->bytesPopped; i < mpegFrame->numBytes; i++) {
								if (   (mpegFrame->buffer[i] == 0)
									&& (mpegFrame->buffer[i+1] == 0)
									&& (mpegFrame->buffer[i+2] == 1)
									&& (mpegFrame->buffer[i+3] >= 0x01)
									&& (mpegFrame->buffer[i+3] <= 0xaf) ){
										newSlice = true;
										break;
								}
							}
							if (newSlice) {
								//last packet
								if ( (i - mpegFrame->bytesPopped) < (*size - sizeof (MPEG1Header_t)) )
								{
									dataSize = i - mpegFrame->bytesPopped - sizeof (MPEG1Header_t);
									header1.E = 1;
								// not last
								} else {
									dataSize = *size - sizeof (MPEG1Header_t);
									header1.E = 0;
								}
							} else {
								//last packet
								if ( (mpegFrame->numBytes - mpegFrame->bytesPopped) < (*size - sizeof (MPEG1Header_t)) ){
									dataSize = mpegFrame->numBytes - mpegFrame->bytesPopped;
									header1.E = 1;
								} else {
									//not last
									dataSize = *size - sizeof (MPEG1Header_t);
									header1.E = 0;
								}
							}
						}
					}

					memcpy (*(fragment), &header1, sizeof(MPEG1Header_t));
					memcpy (*(fragment) + offset, mpegFrame->buffer + mpegFrame->bytesPopped, dataSize);
					mpegFrame->bytesPopped += dataSize;
					
					if ((frameCount + dataSize)<mpegFrame->numBytes)
					{
						frameCount += dataSize;
				
					}else{
						
						*size = (mpegFrame->numBytes - frameCount) + offset;
						frameCount = mpegFrame->numBytes;
			
					}
					
					return (mpegFrame->numBytes - frameCount); //return remain frame 
					
		}
		
		if (payLoad == __H263)
		{
			
			h263EncodedImage_t *img = static_cast<h263EncodedImage_t *>(frame);

			if (img -> mode == 0)
			{
		
				H263_0Header_t header;
				header.F = 0;
				header.P = 0; 
		

				header.sbit = 0;
				header.ebit = 0; 
					
				if ((img->w == 128) && 
						(img->h == 96))
						header.src = SQCIF;
				else if((img->w == 176) && 
					    (img->h == 144))
						header.src = QCIF;
				else if((img->w == 352) && 
					    (img->h == 288))
						header.src = CIF;
				else if((img->w == 704) &&
					    (img->h == 576))
						header.src = CIF4;
				else if ((img->w==1408) &&
					     (img->h==1152))
						header.src = CIF16;
				else 
						header.src = OTRO;
					
				header.I = 0;
				header.U = 0;
				header.S = 0;
				header.A = 0;
				header.R1 = 0;
				header.R = 0;
				header.dbq = 0;
				header.trb = 0;
				header.tr = 0; 
				memcpy (*fragment, &header, sizeof (H263_0Header_t));
				memcpy (*fragment + offset, img -> buffer + frameCount, *size - offset);
				int dataSize = *size - offset;
				if ((frameCount + dataSize)<img->numBytes)
				{
					frameCount += dataSize;
	
				}else{
						
					*size = (img->numBytes - frameCount) + offset;
					frameCount = img->numBytes;
			
				}
					
				return (img->numBytes - frameCount); //return remain frame 
					
			// end of if (mode == 0)

			}
			NOTIFY("Fragmenter_t :: H263 GetFragment: Mode not valid\n");
			return -1;
	
		} // End payload = __H263   
		
		NOTIFY("Fragmenter_t :: I Can't understand this frame!\n");
	}

	return -1;
}

long
fragmenter_t::getOffset(void)
{    
	return frameCount;
}


		