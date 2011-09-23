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
* ============================================================================
*  Name     : vCodecsSymbianDll.cpp 
*  Part of  : vCodecsSymbian
*  Created  : 12.04.2005 by 
*  Description:
*    vCodecsSymbianDll.cpp - main DLL source
*  Version  :
*  Copyright: 
* ============================================================================
*/

//  Include Files  

#include <e32std.h>         // GLDEF_C
#include "vCodecsSymbian.pan"       // panic codes


//  Global Functions

GLDEF_C void Panic(TvCodecsSymbianPanic aPanic)
// Panics the thread with given panic code
    {
    User::Panic(_L("VCODECSSYMBIAN"), aPanic);
    }


//  Exported Functions

EXPORT_C TInt E32Dll(TDllReason /*aReason*/)
// Called when the DLL is loaded and unloaded. Note: have to define
// epoccalldllentrypoints in MMP file to get this called in THUMB.
    {
    return KErrNone;
    }


// End of file  
