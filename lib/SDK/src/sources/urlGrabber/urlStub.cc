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
/////////////////////////////////////////////////////////////////////////
//
// $Id: urlStub.cc 20999 2010-07-30 15:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "urlGrabber.hh"

// ----------------------------------------------------------
//
// Initialization/ Shutdown
//

int
registerURLGrabber(void)
{
    NOTIFY("%s: starting up\n", URL_ID);

    //
    // start registration
    //

    sourceDescriptor_ref desc= new urlGrabberDescriptor_t;
    sourceFactory_ref    fact= new urlGrabberFactory_t;


    if ( ! desc.isValid())
    {
        NOTIFY("---Module internal error\n");
        NOTIFY("---Bailing out\n");

        return -1;
    }
    if ( ! fact.isValid())
    {
        NOTIFY("---Unable to create factory\n");
        NOTIFY("---Bailing out\n");

        return -1;
    }

    if (registerSourceFactory(desc, fact))
    {
        NOTIFY("+++Registered grabber '%s'\n", desc->getID());
    }
    else
    {
        NOTIFY("---Unable to register grabber '%s'\n", desc->getID());
        NOTIFY("---Bailing out\n");

        return -1;
    }

    return 0;
}

void
releaseURLGrabber(void)
{
    NOTIFY("%s: shutting down\n", URL_ID);
}

