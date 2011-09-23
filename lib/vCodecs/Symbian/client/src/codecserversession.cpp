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
/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES
#include <e32math.h>
#include <aknnotewrappers.h> 

#include "ClientServerCommon.h"
#include "CodecServerSession.h"

// FUNCTION PROTOTYPES
static TInt StartServer();
static TInt CreateServerProcess();

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// RCodecServerSession::RCodecServerSession()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RCodecServerSession::RCodecServerSession()
: RSessionBase()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RCodecServerSession::Connect()
// Connects to the server and create a session.
// -----------------------------------------------------------------------------
//
TInt RCodecServerSession::Connect()
    {
    TInt error = ::StartServer();

    if ( KErrNone == error )
        {
        error = CreateSession( KCodecServerName,
                               Version(),
                               KDefaultMessageSlots );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RCodecServerSession::Version()
// Gets the version number.
// -----------------------------------------------------------------------------
//
TVersion RCodecServerSession::Version() const
    {
    return( TVersion( KCodecServMajorVersionNumber,
                      KCodecServMinorVersionNumber,
                      KCodecServBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RCodecServerSession::RequestEncode()
// Issues a request to the Codec  server.
// -----------------------------------------------------------------------------
//

_LIT(KNAME, "nombre");
void RCodecServerSession::RequestEncode( TDesC8& aInput, TDes8& aOutput ) const
    {
    TAny* messageParameters[KMaxMessageArguments];

    // Set the first message parameter to point to the descriptor
    messageParameters[0] = static_cast<TAny*>( &aInput );

    // Set the second message parameter to point to the descriptor
    messageParameters[1] = static_cast<TAny*>( &aOutput );

    // This call waits for the server to complete the request before
    // proceeding. When it returns, the image will be in aOutput
    TInt j = SendReceive( ECodecServEncode, &messageParameters[0] );

	/*
	if (i!=KErrNone)
		{

		error.AppendNum(i);
        CAknInformationNote* informationNote;

        informationNote = new ( ELeave ) CAknInformationNote;

        // Show the information Note with
        // textResource loaded with StringLoader.
        informationNote->ExecuteLD( error );		
		}
	*/
    } 


// -----------------------------------------------------------------------------
// RCodecServerSession::RequestDecode()
// Issues a request to the Codec  server.
// -----------------------------------------------------------------------------
//
void RCodecServerSession::RequestDecode( TDesC8& aInput, TDes8& aOutput ) const
    {
    TAny* messageParameters[KMaxMessageArguments];

    // Set the first message parameter to point to the descriptor
    messageParameters[0] = static_cast<TAny*>( &aInput );

    // Set the second message parameter to point to the descriptor
    messageParameters[1] = static_cast<TAny*>( &aOutput );

    // This call waits for the server to complete the request before
    // proceeding. When it returns, the image will be in aOutput
    SendReceive( ECodecServDecode, &messageParameters[0] );
    }


// ============================= OTHER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// StartServer()
// Starts the server if it is not already running
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    TInt result;

    TFindServer findCodecServer( KCodecServerName );
    TFullName name;

    result = findCodecServer.Next( name );
    if ( result == KErrNone )
        {
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphore;
    result = semaphore.CreateGlobal( KCodecServerSemaphoreName, 0 );
    if ( result != KErrNone )
        {
        return  result;
        }

    result = CreateServerProcess();
    if ( result != KErrNone )
        {
        return  result;
        }

    semaphore.Wait();
    semaphore.Close();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CreateServerProcess()
// Creates a server process
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    TInt result;

    const TUidType serverUid( KNullUid, KNullUid, KServerUid3 );

#ifdef __WINS__

    RLibrary lib;
    result = lib.Load( KCodecServerFilename, serverUid );
    if ( result != KErrNone )
        {
        return  result;
        }

    //  Get the WinsMain function
    TLibraryFunction functionWinsMain = lib.Lookup( 1 );

    //  Call it and cast the result to a thread function
    TThreadFunction serverThreadFunction =
        reinterpret_cast<TThreadFunction>( functionWinsMain() );

    TName threadName( KCodecServerName );

    // Append a random number to make it unique
    threadName.AppendNum( Math::Random(), EHex );

    RThread server;

    // Create new server thread
    result = server.Create( threadName,
                            serverThreadFunction, // Thread's main function
                            KDefaultStackSize,
                            NULL,
                            &lib,
                            NULL,
                            KServerMinHeapSize,
                            KServerMaxHeapSize,
                            EOwnerProcess );

    lib.Close();    // If successful, server thread has handle to library now

    if ( result != KErrNone )
        {
        return  result;
        }

    server.SetPriority( EPriorityLess );


#else

    RProcess server;

    _LIT( KStrEmpty,"" );
    result = server.Create( KCodecServerFilename, KStrEmpty, serverUid );

	server.Rename(KNAME);

    if ( result != KErrNone )
        {
        return  result;
        }

#endif

    server.Resume();
    server.Close();

    return  KErrNone;
    }


// End of File
