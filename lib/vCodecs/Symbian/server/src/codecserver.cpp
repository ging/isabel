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


// INCLUDE FILES
#include <e32svr.h>
#include <e32math.h>

#include "CodecServer.h"
#include "ClientServerCommon.h"
#include "CodecSession.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCodecServer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCodecServer* CCodecServer::NewL()
    {		
    CCodecServer* CodecServer = CCodecServer::NewLC();
    CleanupStack::Pop( CodecServer );
    return CodecServer;
    }

// -----------------------------------------------------------------------------
// CCodecServer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCodecServer* CCodecServer::NewLC()
    {
    CCodecServer* CodecServer = new ( ELeave ) CCodecServer( EPriorityLow );
    CleanupStack::PushL( CodecServer );
    CodecServer->ConstructL();
    return CodecServer;
    }

// -----------------------------------------------------------------------------
// CCodecServer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCodecServer::ConstructL()
    {
    StartL( KCodecServerName );
    }

// -----------------------------------------------------------------------------
// CCodecServer::CCodecServer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCodecServer::CCodecServer( TInt aPriority )
: CServer( aPriority )
    {
    // Implementation not required
    }

// -----------------------------------------------------------------------------
// CCodecServer::NewSessionL()
// Creates a Codec server session.
// -----------------------------------------------------------------------------
//
CSharableSession* CCodecServer::NewSessionL( const TVersion& aVersion ) const
    {
    // Check we are the right version
    if ( !User::QueryVersionSupported( TVersion( KCodecServMajorVersionNumber,
                                                 KCodecServMinorVersionNumber,
                                                 KCodecServBuildVersionNumber ),
                                       aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    // Make new session
    RThread client = Message().Client();
    return CCodecServerSession::NewL( client,
                                     *const_cast<CCodecServer*> ( this ) );
    }

// -----------------------------------------------------------------------------
// CCodecServer::IncrementSessions()
// Increments the count of the active sessions for this server.
// -----------------------------------------------------------------------------
//
void CCodecServer::IncrementSessions()
    {
    iSessionCount++;
    }

// -----------------------------------------------------------------------------
// CCodecServer::DecrementSessions()
// Decrements the count of the active sessions for this server.
// -----------------------------------------------------------------------------
//
void CCodecServer::DecrementSessions()
    {
    iSessionCount--;
    if ( iSessionCount <= 0 )
        {
        CActiveScheduler::Stop();
        }
    }

// -----------------------------------------------------------------------------
// CCodecServer::RunError()
// Processes any errors.
// -----------------------------------------------------------------------------
//
TInt CCodecServer::RunError( TInt aError )
    {
    if ( aError == KErrBadDescriptor )
        {
        // A bad descriptor error implies a badly programmed client,
        // so panic it; otherwise report the error to the client
        PanicClient( Message(), EBadDescriptor );
        }
    else
        {
        Message().Complete( aError );
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    return KErrNone;    // Handled the error fully
    }

// -----------------------------------------------------------------------------
// CCodecServer::PanicClient()
// Panics the client.
// -----------------------------------------------------------------------------
//
void CCodecServer::PanicClient( const RMessage& aMessage, TCodecServPanic aPanic )
    {
    aMessage.Panic( KCSCodecServer, aPanic );
    }

// -----------------------------------------------------------------------------
// CCodecServer::PanicServer()
// Panics the server.
// -----------------------------------------------------------------------------
//
void CCodecServer::PanicServer( TCodecServPanic aPanic )
    {
    User::Panic( KCSCodecServer, aPanic );
    }

// -----------------------------------------------------------------------------
// CCodecServer::ThreadFunctionL()
// Second stage startup for the server thread.
// -----------------------------------------------------------------------------
//
void CCodecServer::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Construct our server
    CCodecServer::NewLC();    // Anonymous

    RSemaphore semaphore;
    User::LeaveIfError( semaphore.OpenGlobal( KCodecServerSemaphoreName ) );

    // Semaphore opened ok
    semaphore.Signal();
    semaphore.Close();

    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2, activeScheduler ); //Anonymous CCodecServer
    }

// -----------------------------------------------------------------------------
// CCodecServer::ThreadFunction()
// Main function for the server thread.
// -----------------------------------------------------------------------------
//
TInt CCodecServer::ThreadFunction( TAny* /*aNone*/ )
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !( cleanupStack ) )
        {
        PanicServer( ECreateTrapCleanup );
        }

    TRAPD( err, ThreadFunctionL() );
    if ( err != KErrNone )
        {
        PanicServer( ESrvCreateServer );
        }

    delete cleanupStack;
    cleanupStack = NULL;

    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
#ifdef __WINS__
	
// -----------------------------------------------------------------------------
// WinsMain()
// Returns a pointer to CCodecServer::ThreadFunction.
// -----------------------------------------------------------------------------
//
IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
    {
    return reinterpret_cast<TInt>( &CCodecServer::ThreadFunction );
    }

// -----------------------------------------------------------------------------
// E32Dll()
// Entry point function for Symbian Apps.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }

#else  // __ARMI__

// -----------------------------------------------------------------------------
// E32Main()
// Provides the API for the operating system to start the executable.
// Returns the address of the function to be called.
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CCodecServer::ThreadFunction( NULL );
    }

#endif

// End of File
