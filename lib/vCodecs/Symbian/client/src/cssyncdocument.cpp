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
#include <f32file.h>

#include "CSSyncAppUi.h"
#include "CSSyncDocument.h"
#include "Clientservercommon.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSSyncDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSyncDocument* CCSSyncDocument::NewL( CEikApplication& aApp )
    {
    CCSSyncDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSyncDocument* CCSSyncDocument::NewLC( CEikApplication& aApp )
    {
    CCSSyncDocument* self = new ( ELeave ) CCSSyncDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSyncDocument::ConstructL()
    {
    User::LeaveIfError( iServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::CCSSyncDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCSSyncDocument::CCSSyncDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::~CCSSyncDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSSyncDocument::~CCSSyncDocument()
    {
    iServer.Close();
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::CreateAppUiL()
// Creates a CCSAsyncAppUi object and return a pointer to it.
// -----------------------------------------------------------------------------
//
CEikAppUi* CCSSyncDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    return ( static_cast<CEikAppUi*>( new ( ELeave ) CCSSyncAppUi() ) );
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::UpdateTime()
// Asks the time server to update this documents locally stored time.
// -----------------------------------------------------------------------------
//
void CCSSyncDocument::UpdateTime()
    {

	TDes8* input = new TBuf8<XDIM*YDIM*BPP>;
	TDes8* outputC = new TBuf8<XDIM*YDIM*BPP>;
	TDes8* outputD = new TBuf8<XDIM*YDIM*BPP>;

	RFs fileServer;
	fileServer.Connect();
	
	char hdr[18];

	hdr[0]  = 0; /* ID length */
	hdr[1]  = 0; /* Color map type */
	hdr[2]  = (BPP>1)?2:3; /* Uncompressed true color (2) or greymap (3) */
	hdr[3]  = 0; /* Color map specification (not used) */
	hdr[4]  = 0; /* Color map specification (not used) */
	hdr[5]  = 0; /* Color map specification (not used) */
	hdr[6]  = 0; /* Color map specification (not used) */
	hdr[7]  = 0; /* Color map specification (not used) */
	hdr[8]  = 0; /* LSB X origin */
	hdr[9]  = 0; /* MSB X origin */
	hdr[10] = 0; /* LSB Y origin */
	hdr[11] = 0; /* MSB Y origin */
	hdr[12] = (XDIM>>0)&0xff; /* LSB Width */
	hdr[13] = (XDIM>>8)&0xff; /* MSB Width */
	if (BPP > 1) {
		hdr[14] = (YDIM>>0)&0xff; /* LSB Height */
		hdr[15] = (YDIM>>8)&0xff; /* MSB Height */
	} else {
		hdr[14] = ((YDIM*3)>>1)&0xff; /* LSB Height */
		hdr[15] = ((YDIM*3)>>9)&0xff; /* MSB Height */
	}
	hdr[16] = BPP*8;
	hdr[17] = 0x00 | (1<<5) /* Up to down */ | (0<<4); /* Image descriptor */
		
	TPtr8 headerDes((TUint8*)hdr,18,18);

	for (TInt im = 0; im <20; im++)
		{
		input->Zero();
		outputC->Zero();
		outputD->Zero();

		input->Fill('a',XDIM*YDIM*BPP);

		for (int i = 0; i <XDIM*YDIM*BPP; i+=3)
			{
			if (i >(XDIM*BPP*10+im*XDIM*5) && i < (XDIM*BPP*50+im*XDIM*5))
				{
				(*input)[i] = 200;	
				(*input)[i+1] = 0;
				(*input)[i+2] = 0;
				}
			}

		iServer.RequestEncode( *input, *outputC );
		iServer.RequestDecode( *outputC, *outputD );
		RFile file;
		TBuf<30> name;
		name.Append(_L("c:\\test"));
		name.AppendNum(im);
		name.Append(_L(".tga"));

		file.Replace(fileServer, name, EFileWrite);

		file.Write(headerDes);
		file.Write(*outputD);
		file.Flush();
		file.Close();
		}
	fileServer.Close();

	delete input;
	delete outputC;
	delete outputD;
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::Time()
// Returns the currently stored time.
// -----------------------------------------------------------------------------
//
const TTime& CCSSyncDocument::Time() const
    {
    return iTime;
    }


// End of File
