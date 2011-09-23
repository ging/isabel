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
*  Name     : CCodecServer from Codecserver.h
*  Part of  : SIPHONE
*  Created  : 15.09.2005 by Fernando Escribano
*  Description:
*     Codec server main class
*  Version  :
*  Copyright: 
* ============================================================================
*/



#ifndef __CODECSERVER_H__
#define __CODECSERVER_H__

// INCLUDE FILES
#include <e32base.h>
#include "CodecServer.pan"

// CLASS DECLARATION
/**
* CCodecServer.
*  An instance of class CCodecServer is the main server class
*  for the codec server application
*/
class CCodecServer : public CServer
    {

    public : // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * @return Pointer to created CCodecServer object.
        */
        static CCodecServer* NewL();

        /**
        * NewLC.
        * Two-phased constructor.
        * @return Pointer to created CCodecServer object.
        */
        static CCodecServer* NewLC();

    public: // New functions

        /**
        * ThreadFunction.
        * Main function for the server thread.
        * @param aNone Not used.
        * @return Error code.
        */
        static TInt ThreadFunction( TAny* aNone );

        /**
        * IncrementSessions.
        * Increments the count of the active sessions for this server.
        */
        void IncrementSessions();

        /**
        * DecrementSessions.
        * Decrements the count of the active sessions for this server.
        * If no more sessions are in use the server terminates.
        */
        void DecrementSessions();

    protected: // Functions from base classes

        /**
        * From CActive, RunError.
        * Processes any errors.
        * @param aError The leave code reported.
        * @result return KErrNone if leave is handled.
        */
        TInt RunError( TInt aError );

    private: // Constructors and destructors

        /**
        * CCodecServer.
        * C++ default constructor.
        * @param aPriority priority for this thread.
        */
        CCodecServer( TInt aPriority );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New methods

        /**
        * PanicClient.
        * Panics the client.
        * @param aMessage The message channel to the client.
        * @param aReason The reason code for the panic.
        */
        static void PanicClient( const RMessage& aMessage,
                                 TCodecServPanic aReason );

        /**
        * PanicServer.
        * Panics the server.
        * @param aReason the reason code for the panic.
        */
        static void PanicServer( TCodecServPanic aReason );

        /**
        * ThreadFunctionL.
        * Second stage startup for the server thread.
        */
        static void ThreadFunctionL();

    private: // Functions from base classes

        /**
        * From CServer, NewSessionL.
        * Create a codec server session.
        * @param aVersion The client version.
        * @return Pointer to new session.
        */
        CSharableSession* NewSessionL( const TVersion& aVersion ) const;

    private: // Data

        /**
        * iSessionCount, the number of open sessions.
        */
        TInt iSessionCount;
    };


#endif // __CODECSERVER_H__


// End of File
