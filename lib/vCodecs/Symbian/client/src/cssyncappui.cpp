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
#include <eikmenup.h>
#include <avkon.hrh>

#include "CSSync.pan"
#include "CSSyncAppUi.h"
#include "CSSyncAppView.h"
#include "CSSyncDocument.h"
#include "CSSync.hrh"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSSyncAppUi::CCSSyncAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCSSyncAppUi::CCSSyncAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSyncAppUi::ConstructL()
    {
    BaseConstructL();

    iAppView = CCSSyncAppView::NewL( ClientRect(), *SyncDocument() );

    AddToStackL( iAppView );
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::~CCSSyncAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSSyncAppUi::~CCSSyncAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::SyncDocument()
// Casts iDocument member from CAknAppUi to CCSSyncDocument.
// -----------------------------------------------------------------------------
//
CCSSyncDocument* CCSSyncAppUi::SyncDocument() const
    {
    return static_cast<CCSSyncDocument*>( iDocument );
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::HandleCommandL()
// Handles user menu selections.
// -----------------------------------------------------------------------------
//
void CCSSyncAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EDisplayTime:
            // Pull the time from the server
            SyncDocument()->UpdateTime();

            // Get view to display the new time
            iAppView->DrawNow();
            break;

        default:
            User::Panic ( KCSSyncClient, ECSSyncBasicUi );
            break;
        }
    }


// End of File
