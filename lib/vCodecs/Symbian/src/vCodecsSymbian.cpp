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
*  Name     : CvCodecsSymbian from vCodecsSymbian.h
*  Part of  : vCodecsSymbian
*  Created  : 12.04.2005 by 
*  Description:
*     CvCodecsSymbian DLL source
*  Version  :
*  Copyright: 
* ============================================================================
*/

//  Include Files  

#include "vCodecsSymbian.h"	// CvCodecsSymbian
#include "vCodecsSymbian.pan"      	// panic codes



//  Member Functions
__EXPORT CvCodecsSymbian* CvCodecsSymbian::NewLC()
    {
    CvCodecsSymbian* self = new (ELeave) CvCodecsSymbian;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


__EXPORT CvCodecsSymbian* CvCodecsSymbian::NewL()
    {
    CvCodecsSymbian* self = CvCodecsSymbian::NewLC();
    CleanupStack::Pop(self);
    return self;
    }


CvCodecsSymbian::CvCodecsSymbian()
// note, CBase initialises all member variables to zero
    {
    }


void CvCodecsSymbian::ConstructL()
    {
    }


__EXPORT CvCodecsSymbian::~CvCodecsSymbian()
    {
    }


__EXPORT TVersion CvCodecsSymbian::Version() const
    {
    // Version number of example API
    const TInt KMajor = 1;
    const TInt KMinor = 0;
    const TInt KBuild = 1;
    return TVersion(KMajor, KMinor, KBuild);
    }


// End of file
