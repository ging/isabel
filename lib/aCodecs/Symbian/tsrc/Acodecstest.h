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
-----------------------------------------------------------------------------

    aCodecsTest.h - test application header

-----------------------------------------------------------------------------
*/

#ifndef __ACODECSTEST_H__
#define __ACODECSTEST_H__

//  Include Files

#include <eikapp.h>                 // CEikApplication
#include <eikdoc.h>                 // CEikDocument
#include <eikappui.h>               // CEikAppUi


//  Forward Declarations

class CaCodecs;


//  Class Definitions

class CaCodecsTestApplication : public CEikApplication
    {
    private:    // from CEikApplication
        CApaDocument* CreateDocumentL();

    private:    // from CApaApplication (via CEikApplication)
        TUid AppDllUid() const;
    };


class CaCodecsTestDocument : public CEikDocument
    {
    public:     // new functions
        CaCodecsTestDocument(CEikApplication& aApp);

    private:    // from CEikDocument
        CEikAppUi* CreateAppUiL();
    };


class CaCodecsTestAppUi : public CEikAppUi
    {
    public:     // new functions
        void ConstructL(void);
        ~CaCodecsTestAppUi();
        void ShowInfoL(const TDesC& aDes1, const TDesC& aDes2);

    private:    // from CEikAppUi
        void HandleCommandL(TInt aCommand);

    };


#endif  // __ACODECSTEST_H__

// End of file
