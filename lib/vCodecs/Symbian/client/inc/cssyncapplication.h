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
/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __CSSYNC_APPLICATION_H__
#define __CSSYNC_APPLICATION_H__

// INCLUDE FILES
#include <aknapp.h>

// CONSTANTS
// Uid for this application, this should match the one in the mmp file
const TUid KUidCSSyncApp = { 0x10005BC8 };

// CLASS DECLARATION
/**
* CCSSyncApplication
*  An instance of CCSSyncApplication is the application part of the AVKON
*  application framework for the CSSync example application.
*/
class CCSSyncApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CAknApplication, AppDllUid.
        * Returns the application DLL UID value.
        * @return the UID of this Application/Dll.
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CAknApplication, CreateDocumentL.
        * Creates a CApaDocument object and return a pointer to it.
        * @return A pointer to the created document.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __CSSYNC_APPLICATION_H__


// End of File
