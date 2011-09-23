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


#ifndef __CSSYNCDOCUMENT_H__
#define __CSSYNCDOCUMENT_H__

// INCLUDE FILES
#include <akndoc.h>
#include "CodecServerSession.h"

// FORWARD DECLARATIONS
class CCSSyncAppUi;
class CEikApplication;

// CLASS DECLARATION
/**
* CCSSyncDocument
*  An instance of class CCSSyncDocument is the Document part of the AVKON
*  application framework for the CSSync example application
*/
class CCSSyncDocument : public CAknDocument
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Constructs a CCSSyncDocument for the AVKON application aApp 
        * using two phase construction, and returns a pointer to the 
        * created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCSSyncDocument.
        */
        static CCSSyncDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Constructs a CCSSyncDocument for the AVKON application aApp 
        * using two phase construction, and returns a pointer to the 
        * created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCSSyncDocument.
        */
        static CCSSyncDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CCSSyncDocument.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSSyncDocument();

    public: // New functions

        /**
        * UpdateTime.
        * Asks the time server to update this documents locally stored time.
        */
        void UpdateTime();

        /**
        * Time.
        * Returns the currently stored time.
        * @return The time.
        */
        const TTime& Time() const;

    public: // Functions from base classes

        /**
        * From CAknDocument, CreateAppUiL.
        * Creates a CCSSyncAppUi object and returns a pointer to it.
        * @return A pointer to the created instance of the AppUi created.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors and destructors

        /**
        * CCSSyncDocument.
        * C++ default constructor.
        * Performs the first phase of two phase construction.
        * @param aApp Application creating this document.
        */
        CCSSyncDocument( CEikApplication& aApp );

        /**
        * ConstructL.
        * Performs the second phase construction of a CCSSyncDocument object.
        */
        void ConstructL();

private: // Data

        /**
        * iServer, interface to the Time Server.
        */
        RCodecServerSession iServer;

        /**
        * iTime, the currently displayed time.
        */
        TTime iTime;
};


#endif // __CSSYNCDOCUMENT_H__


// End of File
