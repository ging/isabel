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
// $Id: winGrabberStub.cc 9308 2006-11-28 17:15:11Z gabriel $
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

#include "winGrabber.hh"

// ----------------------------------------------------------
//
// Source descriptor
//
static const char *WIN_ID= "Window grabber";

class winGrabberDescriptor_t: public sourceDescriptor_t
{
public:
    winGrabberDescriptor_t(void) { }

    const char *getID         (void) const { return WIN_ID; }
    const char *getInputPorts (void) const { return NULL;   }
};

// ----------------------------------------------------------
//
// Source Factory
//

class winGrabberFactory_t: public sourceFactory_t
{
public:

    winGrabberFactory_t(void)  {}
    virtual ~winGrabberFactory_t(void) {}

    source_ref createSource(sourceDescriptor_ref sdr,
                            const char *inPort= NULL
                           )
    {
        return new windowGrabber_t(sdr);
    }

};

// ----------------------------------------------------------
//
// Initialization/ Shutdown
//

int
registerWinGrabber(void)
{
    NOTIFY("%s: starting up\n", WIN_ID);

    sourceDescriptor_ref desc= new winGrabberDescriptor_t;
    sourceFactory_ref    fact= new winGrabberFactory_t;

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
releaseWinGrabber(void)
{
    NOTIFY("%s: shutting down\n", WIN_ID);
}

