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
/////////////////////////////////////////////////////////////////////////
//
// $Id: RTPPacket.hh 8078 2006-02-28 17:38:59Z sirvent $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_packet_hh__
#define __rtp_packet_hh__

#include <stdlib.h>

#include <icf2/general.h>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>

#ifdef WIN32
#include <winsock2.h>
#else
#include <icf2/sockIO.hh>
#endif

#define RTP_VERSION 2

#include "RTPStructs.hh" 

/**
* This class represents a received RTP packet.
*/
class _Dll_ RTPPacket_t: public virtual collectible_t{
public:

	/**
	* RTPPacket_t constructor.
	* @param packet: buffer received in RTP socket. It contains a RTP packet.
	* @param length: buffer length.
	*/
        RTPPacket_t (u8 *packet, int length); 
        
        RTPPacket_t (const RTPPacket_t &packet); 
	
	/**
	* RTPPacket_t desctructor.
	*/
	~RTPPacket_t ();
      
        u8 *getPacket();
 
	/** 
	* Method to get data from a RTP packet.
	*/  
	u8 *getData ();
	
	/**
	* Method to get data lenght.
	*/
        int getDataLength ();

	/**
	* Method to get total length: RTP header + data.
	*/
        int getTotalLength ();

	/**
	* Method to get SSRC identifier of the RTP packet source.
	*/ 
        u32 getSSRC ();
	
	/**
	* Method to get the sequence number of the RTP packet.
	*/
        u16 getSequenceNumber ();

	/**
	* Method to get the value of marker bit in the RTP header.
	*/
        bool getMark ();

	/**
	* Method to get the payload type of the RTP packet.
	*/
        u8 getPayloadtype ();

	/**
	* Method to get the RTP timestamp which represents the instant in which data have been generated.
	*/ 
	u32 getTimestamp ();

	/**
	* Method to get the list of contributing source identifiers, if any. It returns an empty list if there aren't any CSRC.
	*/
        ql_t<u32> *getCSRCList ();
	
	/**
	* Method to get extension header data length, if any.
	*/
	u16 getExtensionLength ();
	u16 getTotalExtensionLength ();
	
	/**
	* Method to get extension header data, if any. It returns NULL if extension header doesn't exist.
	*/
	u8 *getExtensionData ();

	/**
	* Method to get extension header id, if any.
	*/
	u16 getExtensionId ();

        bool getExt();
        u8 getCC();
        bool getPadding();

        inline unsigned int getPlayTime() { return playTime; };
        inline void setPlayTime(unsigned int t) { playTime = t; };
        inline unsigned int getDuration() { return duration; };
        inline void setDuration(unsigned int t) { duration = t; };
        inline struct timeval *getTV() { return &tv; };
        inline void setTV(struct timeval *t) { tv.tv_sec =t->tv_sec;
                                               tv.tv_usec=t->tv_usec; };
	
        friend class smartReference_t<RTPPacket_t>;
private:
        int totalLength;
        u8 * packet;
        
        RTPHeader *rtpHeader;
        int rtpFixedHeaderLength;
        int rtpHeaderLength;
        ql_t<u32> list;
        u8 *data;
        int dataLength;
        
	u8 *extensionData;
        u16 extLength;
	u16 extensionId;

        struct timeval tv;
        unsigned int playTime;
        unsigned int duration;
};
typedef smartReference_t<RTPPacket_t> RTPPacket_ref;

#endif

