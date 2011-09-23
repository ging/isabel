/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 <head> 
   <name>playchannel.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef _IVIDEO_PLAY_CHANNEL_H
#define _IVIDEO_PLAY_CHANNEL_H


#include "general.h"
#include "looper.h"
#include "sender.h"
#include "testsource.h"
#include "samplereader.h"
#include "videoRTPSession.h"
#include "MPEGimp.h"
#include "H263imp.h"


#define PACKET_BUFFER_SIZE 300


/**
 <class> 
   <name>RTPContainer_t</name> 
   <descr>
   This class is used by playChannel_t to contain RTP packets, it orders the
   packets using the sequence number.
   </descr>
**/
class RTPContainer_t : public item_t
{
private:
	RTPPacket_t * pkts[PACKET_BUFFER_SIZE];
	int payloadHeaderLength;

public:
	RTPContainer_t(void);
	virtual ~RTPContainer_t(void);
	bool insert(RTPPacket_t *);
	void free(void);
	long getData(BYTE * = NULL);
	unsigned long getTimestamp(void);
	bool hasInfo(void);
};
//</class>

/**
<class> 
   <name>playChannel_t</name> 
   <descr>
   This class represent a single receiver channel. playChannel_t 
   inherits from channel_t and creates instances of		
   receiver_t and decodes received frames.	
   </descr>
**/
class playChannel_t : public channel_t
{

private:

	int frameSequenceNumber;

	int frameCount;
	sampleReader_t * sampleReader;
	RTPContainer_t frameBuffer[DEFAULT_FRAME_SIZE]; 
	AM_MEDIA_TYPE       videoType;
	bool firstVideoType;
    bool followSize;
	int  lastPayLoad;
	int  buffer;
    int  lastSeqNum;
    unsigned long lastTS;

private:

	HRESULT checkPipeLine(RTPPacket_t * packet);
	
public:

	playChannel_t(
		int ID,			   //channel ID
        const char *title, //Window Title
        windowInfo_t *geom = &DEFAULT_WINDOW_INFO //Window info
    );

	virtual ~playChannel_t(void);
	HRESULT map(void);
	HRESULT remap(void);
	HRESULT unmap(void);
	void addFragment(RTPPacket_t* packet);
	void setFollowSize(bool);
	HRESULT setBuffer(int);
};
//</class>
#endif