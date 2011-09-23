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
/*
* ============================================================================
*  Name     : CCodecServerSession from Codecsession.h
*  Part of  : SIPHONE
*  Created  : 15.96.2005 by Fernando Escribano
*  Description:
*     Server side session of the codec server
*  Version  :
*  Copyright: 
* ============================================================================
*/



#ifndef __CODECSESSION_H__
#define __CODECSESSION_H__

// INCLUDE FILES
#include <e32base.h>

//INCLUDE vCodecs lib
#include "../../include/vCodecs/codecs.h"
#include "Clientservercommon.h"

// CONSTANTS


// FORWARD DECLARATIONS
class CCodecServer;

// CLASS DECLARATION
/**
* CCodecServerSession.
*  An instance of class CCodecServerSession is created for each client.
*/


class CCodecServerSession : public CSession
    {

	public:   // constants
		static const TInt KInputBufferSize = XDIM*YDIM*BPP;
		static const TInt KOutputBufferSize = XDIM*YDIM*BPP;

    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * @param aClient The Client's thread.
        * @param aServer The server.
        * @return Pointer to created CCodecServerSession object.
        */
        static CCodecServerSession* NewL( RThread& aClient,
                                         CCodecServer& aServer );

        /**
        * NewLC.
        * Two-phased constructor.
        * @param aClient The Client's thread.
        * @param aServer The server.
        * @return Pointer to created CCodecServerSession object.
        */
        static CCodecServerSession* NewLC( RThread& aClient,
                                          CCodecServer& aServer );

        /**
        * ~CCodecServerSession.
        * Destructor.
        */
        virtual ~CCodecServerSession();

    public: // Functions from base classes

        /**
        * From CSession, ServiceL.
        * Service request from client.
        * @param aMessage Message from client
        *                 (containing requested operation and any data).
        */
        void ServiceL( const RMessage& aMessage );

    private: // Constructors and destructors

        /**
        * CCodecServerSession.
        * C++ default constructor.
        * @param aClient The Client's thread.
        * @param aServer The server.
        */
        CCodecServerSession( RThread& aClient, CCodecServer& aServer );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New methods

        /**
        * PanicClient.
        * Causes the client thread to panic.
        * @param aPanic Panic code.
        */
        void PanicClient( TInt aPanic ) const;

        /**
        * EncodeImageL.
        */
        void EncodeImageL();

		/**
		* DecodeImageL
		*/
		void DecodeImageL();

    private: // Data

        /**
        * iServer, reference to the server.
        */
        CCodecServer& iServer;

        /**
		* iInputBuffer
		*/
		TDes8* iInputBuffer;

		/**
		* iOutputBuffer
		*/
		TDes8* iOutputBuffer;
		/**
		* video Coder
		*/
		vCoder_t *   coder;
		/**
		* video Decoder
		*/
		vDecoder_t * decoder;


		
    };

#endif // __CODECSESSION_H__


// End of File
