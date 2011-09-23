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

#ifndef __RTPHEADER_H__
#define __RTPHEADER_H__

// INCLUDES
#include <icf2/sockIO.hh>

// FORWARD DECLARATIONS

// CLASS DECLARATION

class RTPHeader
{
public:  // constants

    /**
    * KSize
    * Longitud de la cabecera en bytes.
    */
    static const int SIZE = 12;

public:  // Constructor
    inline RTPHeader(): cc(0), x(0), p(0), ver(2), pt(0), m(0), seq(0), time(0), ssrc(0)
    {
        // No implementation required
    }

public:  // Member functions

    /**
    * Get the marker bit from the RTP header.
    * @return 1 if marker bit is set 0 if is not set.
    */
    inline u8 GetMarker() const
    {
        return m;
    }

    /**
    * Set the marker bit from the RTP header.
    * @param aMarker 1 to set marker bit, 0 to unset it.
    */
    inline void SetMarket(u8 aMarker)
    {
        m = aMarker;
    }

    /**
    * Get the payload type from the RTP header.
    * @return A TInt8 holding the value.
    */
    inline u8 GetPayloadType() const
    {
        return pt;
    }

    /**
    * Set the payload type from the RTP header.
    * @param aType the payload type. Valid range between 0x00 to 0x7F
    */
    inline void SetPayloadType(u8 aType)
    {
        pt = aType;
    }

    /**
    * Get the sequence number field from the RTP header.
    * @return A TInt16 holding the value.
    */
    inline u16 GetSeqNumber() const
    {
        return ntohs(seq);
    }

    /**
    * Set the seq number from the RTP header.
    * @param aSeqNumber The seq number. Valid range between 0x0000 to 0xFFFF
    */
    inline void SetSeqNumber(u16 aSeqNumber)
    {
        seq = htons(aSeqNumber);
    }

    /**
    * Get the Timestamp field from the RTP header.
    * @return A TInt32 holding the value.
    */
    inline u32 GetTimestamp() const
    {
        return ntohl(time);
    }

    /**
    * Set the Timestamp from the RTP header.
    * @param aTimestamp The Tmestamp. Valid range between 0x00000000 to 0xFFFFFFFF
    */
    inline void SetTimestamp(u32 aTimestamp)
    {
        time = htonl(aTimestamp);
    }

    /**
    * Get the SSRC field from the RTP header.
    * @return A TInt32 holding the value.
    */
    inline u32 GetSSRC() const
    {
        return ntohl(ssrc);
    }

    /**
    * Set the Timestamp from the RTP header.
    * @param aTimestamp The Tmestamp. Valid range between 0x00000000 to 0xFFFFFFFF
    */
    inline void SetSSRC(u32 aSSRC)
    {
        ssrc = htonl(aSSRC);
    }

private:  // Data

    u8 cc:4;
    u8 x:1;
    u8 p:1;
    u8 ver:2;

    u8 pt:7;
    u8 m:1;

    u16 seq;

    u32 time;

    u32 ssrc;
};

#endif // __RTPHEADER_H__

// End of File
