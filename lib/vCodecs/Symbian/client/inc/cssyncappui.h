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


#ifndef __CSSYNCAPPUI_H__
#define __CSSYNCAPPUI_H__

// INCLUDE FILES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CCSSyncAppView;
class CCSSyncDocument;

// CLASS DECLARATION
/**
* CCSSyncAppUi
*  An instance of class CCSSyncAppUi is the UserInterface part of the AVKON
*  application framework for the CSSync example application
*/
class CCSSyncAppUi : public CAknAppUi
    {
    public: // Constructors and destructors

        /**
        * CCSSyncAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi.
        */
        CCSSyncAppUi();

        /**
        * ~CCSSyncAppUi.
        * Destructor.
        */
        virtual ~CCSSyncAppUi();

    public: // Functions from base classes

        /**
        * From CAknAppUi, HandleCommandL.
        * Handles user menu selections.
        * @param aCommand The enumerated code for the option selected.
        */
        void HandleCommandL( TInt aCommand );

    private: // Constructors and destructors

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New functions

        /**
        * SyncDocument.
        * Casts iDocument member from CAknAppUi to CCSSyncDocument.
        */
        CCSSyncDocument* SyncDocument() const;

    private: // Data

        /**
        * iAppView, the application view.
        * Owned by CCSSyncAppUi object.
        */
        CCSSyncAppView* iAppView;
    };


#endif // __CSSYNCAPPUI_H__


// End of File
