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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "aviReader.hh"
#include "imageSource.hh"

//
// our source descriptor -- yeah!
//
static const char *FILE_ID= "File Source";

//
// there is JUST one descriptor, althouth there are many sources:
// for movies, for images, for directories, ...
//
class fileReaderDescriptor_t: public sourceDescriptor_t
{
public:
    fileReaderDescriptor_t(void) { }

public:
    const char *getID         (void) const { return FILE_ID; }
    const char *getInputPorts (void) const { return NULL;    }
};

//
// our source factory-- yeah! yeah! yeah!!
//
class fileReaderFactory_t: public sourceFactory_t
{
private:
    static sourceFactory_ref singleFactory;

    fileReaderFactory_t(void) { }

    ~fileReaderFactory_t(void) { }

public:
    static sourceFactory_ref createFactory(void)
    {
        if(singleFactory.isValid())
            return singleFactory;

        fileReaderFactory_t *f= new fileReaderFactory_t;

        singleFactory= f;

        return singleFactory;
    }

    source_ref createSource(sourceDescriptor_ref desc,
                            const char *inPort
                           )
    {
        try // avi reader
        {
            return new aviReader_t(desc, inPort);
        }
        catch (const char *)
        {
            // not an avi file
        }

        try // image loader
        {
            return new imageSource_t(desc, inPort);
        }
        catch (const char *)
        {
            // unrecognizeble image
        }

        return NULL;
    }

    friend class smartReference_t<fileReaderFactory_t>;
};

sourceFactory_ref fileReaderFactory_t::singleFactory;

//
// housekeeping funtions
//

int
registerFileReader(void)
{
    NOTIFY("%s: starting up\n", FILE_ID);

    //
    // start registration
    //

    sourceDescriptor_ref desc= new fileReaderDescriptor_t;
    sourceFactory_ref    fact= fileReaderFactory_t::createFactory();


    if ( ! desc.isValid())
    {
        NOTIFY("---Module internal error\n");
        NOTIFY("---Bailing out\n");

        return -1;
    }
    if ( ! fact.isValid())
    {
        NOTIFY("---Cannot build movie factory\n");
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
releaseFileReader(void)
{
    NOTIFY("%s: shutting down\n", FILE_ID);
}

