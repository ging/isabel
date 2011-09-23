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
/*
-----------------------------------------------------------------------------

    aCodecsTest.cpp - test application source

-----------------------------------------------------------------------------
*/

//  Include Files

#include "aCodecsTest.h"
#include <uikon.hrh>            // command codes
#include <eikenv.h>             // CEikonEnv

#include <codecs.h>

//  Constants

const TUid KUidaCodecsTestApp = { 0x0116C9D3 };    // random temporary UID


//  Member Functions

CApaDocument* CaCodecsTestApplication::CreateDocumentL()
    {
    return new (ELeave) CaCodecsTestDocument(*this);
    }


TUid CaCodecsTestApplication::AppDllUid() const
    {
    return KUidaCodecsTestApp;
    }


CaCodecsTestDocument::CaCodecsTestDocument(CEikApplication& aApp) : CEikDocument(aApp)
    {
    }


CEikAppUi* CaCodecsTestDocument::CreateAppUiL()
    {
    return new (ELeave) CaCodecsTestAppUi;
    }


void CaCodecsTestAppUi::ConstructL(void)
    {
    BaseConstructL();

    }


CaCodecsTestAppUi::~CaCodecsTestAppUi()
    {
 
    }




void CaCodecsTestAppUi::HandleCommandL(TInt aCommand)
    {
    _LIT(KTest1, "Test1: ");
    _LIT(KTest2, "Test2: ");
    TInt ch;

    switch (aCommand)
        {
        case EEikCmdFileOpen: 
            {// Test 1
            Codec * codec = CodecNew(3); //gsm 8KHz

            }
            break;

        case EEikCmdFileOpenAnother:    // Test 2
            
            break;

        case EEikCmdExit:
            Exit();
            break;

        default:
            break;
        }
    }


//  Global Functions

#ifdef __WINS__
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return KErrNone;
    }
#endif


//  Exported Functions

EXPORT_C CApaApplication* NewApplication()
    {
    return new CaCodecsTestApplication;
    }


// End of file
