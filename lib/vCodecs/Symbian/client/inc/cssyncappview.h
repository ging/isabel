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


#ifndef __CSSYNCAPPVIEW_H__
#define __CSSYNCAPPVIEW_H__

// INCLUDE FILES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CCSSyncDocument;

// CLASS DECLARATION
/**
* CCSAsyncAppView
*  An instance of the Application View object for the CSSync
*  example application.
*/
class CCSSyncAppView : public CCoeControl
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Creates a CCSSyncAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @param aDocument The document to read time data from.
        * @return A pointer to the created instance of CCSSyncAppView.
        */
        static CCSSyncAppView* NewL( const TRect& aRect,
                                     CCSSyncDocument& aDocument );

        /**
        * NewLC.
        * Two-phased constructor.
        * Creates a CCSSyncAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @param aDocument The document to read time data from.
        * @return A pointer to the created instance of CCSSyncAppView.
        */
        static CCSSyncAppView* NewLC( const TRect& aRect,
                                      CCSSyncDocument& aDocument );

        /**
        * ~CCSSyncAppView.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSSyncAppView();

    public:  // Functions from base classes

        /**
        * From CCoeControl, Draw.
        * Draws this CCSSyncAppView to the screen.
        * @param aRect The rectangle of this view that needs updating.
        */
        void Draw( const TRect& aRect ) const;

    private: // Constructors and destructors

        /**
        * CCSSyncAppView.
        * C++ default constructor.
        * Performs the first phase of two phase construction.
        * @param aDocument The document to read time data from.
        */
        CCSSyncAppView( CCSSyncDocument& aDocument );

        /**
        * ConstructL.
        * Performs the second phase construction of a CCSSyncAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL( const TRect& aRect );

    private: // Data

        /**
        * iDocument, the application document.
        */
        CCSSyncDocument& iDocument;
    };


#endif // __CSSYNCAPPVIEW_H__


// End of File
