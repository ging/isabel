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
#include <f32file.h>

#include "CodecSession.h"
#include "ClientServerCommon.h"
#include "CodecServer.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCodecServerSession::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCodecServerSession* CCodecServerSession::NewL( RThread& aClient,
                                              CCodecServer& aServer )
    {
    CCodecServerSession* self = CCodecServerSession::NewLC( aClient, aServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCodecServerSession* CCodecServerSession::NewLC( RThread& aClient,
                                               CCodecServer& aServer )
    {
    CCodecServerSession* self = new ( ELeave ) CCodecServerSession( aClient,
                                                                  aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCodecServerSession::ConstructL()
    {
    iServer.IncrementSessions();

	iInputBuffer = new (ELeave) TBuf8<KInputBufferSize>();
	iOutputBuffer = new (ELeave) TBuf8<KOutputBufferSize>();

	//inititialize vCodec library
	vCodecInit();

	//create codec and decoder
	vCodecArgs Cparams;
	
	Cparams.bitRate   = 10000; //output codec bitrate
	Cparams.frameRate = 30;   //expected fps, (used to calc Bitrate)
	Cparams.height    = YDIM;  
	Cparams.width     = XDIM;
	Cparams.format    = RGB24_FORMAT; //wanted input format	                             
	Cparams.quality   = 50; //Quality depends of codec presets

	codec = vGetCodecByPT(H263_PT).newCodec(&Cparams);

	vDecoderArgs Dparams;
	Dparams.height  = 0;  //not needed, decoders get it from frames
	Dparams.width   = 0;  //not needed, decoders get it from frames
	Dparams.format  = RGB24_FORMAT; //wanted output format

	decoder = vGetDecoderByPT(H263_PT).newDecoder(&Dparams);
	
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::CCodecServerSession()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCodecServerSession::CCodecServerSession( RThread& aClient, CCodecServer& aServer )
: CSession( aClient ), iServer( aServer )
    {
    // Implementation not required
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::~CCodecServerSession()
// Destructor.
// -----------------------------------------------------------------------------
//
CCodecServerSession::~CCodecServerSession()
    {
	//delete codec and decoder
	vDeleteCodec(codec);
	vDeleteDecoder(decoder);
	//release vCodec library
	vCodecRelease();
    iServer.DecrementSessions();
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::ServiceL()
// Service request from client.
// -----------------------------------------------------------------------------
//
void CCodecServerSession::ServiceL( const RMessage& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case ECodecServEncode :
            EncodeImageL();
            break;
		case ECodecServDecode :
			DecodeImageL();
			break;

        default:
            PanicClient( EBadRequest );
            break;
        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::EncodeImageL()
// Called as a result of the client requesting the encoding of an image
// -----------------------------------------------------------------------------
//

void CCodecServerSession::EncodeImageL()
    {
	//Read the inputImage into the iInputBuffer
	Message().ReadL(Message().Ptr0(), *iInputBuffer);

	RFs fileServer;
	RFile file;

	fileServer.Connect();
	TBuf<30> name;
	name.Append(_L("c:\\log.txt"));
	file.Replace(fileServer, name, EFileWrite);
	TBuf8<200> log;
	log.Append(_L("Encode: Leidos "));
	log.AppendNum(iInputBuffer->Length());
	log.Append(_L(" bytes \n"));
	file.Write(log);

	//Encode the image
	TInt n = vEncode(codec,
	   	               (unsigned char *)iInputBuffer->Ptr(),
					   iInputBuffer->Length(),
					   (unsigned char *)iOutputBuffer->Ptr(),
					   iOutputBuffer->MaxLength());
	if (n<0) n=0;
	iOutputBuffer->SetLength(n);

	TBuf8<200> log2;
	log2.Append(_L("Encode: codificados "));
	log2.AppendNum(n);
	log2.Append(_L(" bytes \n"));
	file.Write(log2);
	file.Flush();
	file.Close();
	fileServer.Close();

	
	// Write the result to the client
	Message().WriteL(Message().Ptr1(), *iOutputBuffer);
    }

// -----------------------------------------------------------------------------
// CCodecServerSession::DecodeImageL()
// Called as a result of the client requesting the decoding of an image
// -----------------------------------------------------------------------------
//

void CCodecServerSession::DecodeImageL()
    {
	//Read the inputImage into the iInputBuffer
	Message().ReadL(Message().Ptr0(), *iInputBuffer);

	RFs fileServer;
	RFile file;

	fileServer.Connect();
	TBuf<30> name;
	name.Append(_L("c:\\log2.txt"));
	file.Replace(fileServer, name, EFileWrite);
	TBuf8<200> log;
	log.Append(_L("Decoder: Leidos "));
	log.AppendNum(iInputBuffer->Length());
	log.Append(_L(" bytes \n"));
	file.Write(log);

	//Decode the image
	TInt n = vDecode(decoder,
		               (unsigned char *)iInputBuffer->Ptr(),
					   iInputBuffer->Length(),
					   (unsigned char *)iOutputBuffer->Ptr(),
					   iOutputBuffer->MaxLength());
	if (n<0) n=0;
	iOutputBuffer->SetLength(n);
	TBuf8<200> log2;
	log2.Append(_L("Decoder: decodificados "));
	log2.AppendNum(n);
	log2.Append(_L(" bytes \n"));
	file.Write(log2);
	file.Flush();
	file.Close();
	fileServer.Close();

	// Write the result to the client
	Message().WriteL(Message().Ptr1(), *iOutputBuffer);

    }



// -----------------------------------------------------------------------------
// CCodecServerSession::PanicClient()
// Causes the client thread to panic.
// -----------------------------------------------------------------------------
//
void CCodecServerSession::PanicClient( TInt aPanic ) const
    {
    Panic( KCSCodecServer,aPanic ); // Note: this panics the client thread,
                                   // not server
    }

// End of File
