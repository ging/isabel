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
#include "CSSyncApplication.h"

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Dll()
// DLL entry point, return that everything is ok
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// NewApplication()
// Create a CSSync application, and return a pointer to it
// -----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return ( static_cast<CApaApplication*>( new CCSSyncApplication ) );
    }

// End of File
