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
/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __CodecSERVERSESSION_H__
#define __CodecSERVERSESSION_H__

// INCLUDE FILES
#include <e32base.h>

// CONSTANTS
// Number of message slots to reserve for this client server session.
// Since we only communicate synchronously here, we never have any
// outstanding asynchronous requests.
static const TUint KDefaultMessageSlots = 0;

// Server UID
const TUid KServerUid3 = { 0x10005BD9 };

_LIT( KCodecServerFilename, "CodecServer" );

#ifdef __WINS__
static const TUint KServerMinHeapSize = 0x200000; // 2*1024K
static const TUint KServerMaxHeapSize = 0x200000; // 2*1024K
#endif

// CLASS DECLARATION
/**
* RCodecServerSession
*  This class provides the client-side interface to the server session
*/
class RCodecServerSession : public RSessionBase
    {
    public: // Constructors and destructors

        /**
        * RCodecServerSession.
        * Constructs the object.
        */
        RCodecServerSession();

    public: // New functions

        /**
        * Connect.
        * Connects to the server and create a session.
        * @return Error code.
        */
        TInt Connect();

        /**
        * Version.
        * Gets the version number.
        * @return The version.
        */
        TVersion Version() const;

        /**
        * RequestEncode.
        * Issues a request to the server to encode an image
        * @param aInput Descriptor with the image to be encoded
		* @param aOutput Descriptor where the encoded image will be written.
        */
        void RequestEncode( TDesC8& aInput, TDes8& aOutput ) const;

        /**
        * RequestDecode.
        * Issues a request to the server to decode an image
        * @param aInput Descriptor with the image to be decoded
		* @param aOutput Descriptor where the decoded image will be written.
        */
        void RequestDecode( TDesC8& aInput, TDes8& aOutput ) const;

    };

#endif // __CodecSERVERSESSION_H__


// End of File
