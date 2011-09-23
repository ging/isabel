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
*  Name     : vCodecsSymbian.h
*  Part of  : vCodecsSymbian
*  Created  : 12.04.2005 by 
*  Description:
*     vCodecsSymbian.h - CvCodecsSymbian class header
*  Version  :
*  Copyright: 
* ============================================================================
*/


// This file defines the API for vCodecsSymbian.dll

#ifndef __VCODECSSYMBIAN_H__
#define __VCODECSSYMBIAN_H__


//  Include Files

#include <e32base.h>    // CBase
#include <e32std.h>     // TBuf
#ifdef __SYMBIAN32__
#include "../../include/vCodecs/codec.h"
#else
#include <vCodecs/codec.h>
#endif


class CvCodecsSymbian : public CBase
    {
    public:     // new functions
        __IMPORT static CvCodecsSymbian* NewL();
        __IMPORT static CvCodecsSymbian* NewLC();
        __IMPORT ~CvCodecsSymbian();

    public:     // new functions, example API
        __IMPORT TVersion Version() const;

    private:    // new functions
        CvCodecsSymbian();
        void ConstructL();

    };


#endif  // __VCODECSSYMBIAN_H__

// End of file
