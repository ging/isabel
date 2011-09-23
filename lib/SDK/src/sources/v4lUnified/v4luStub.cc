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
// $Id: v4luStub.cc 20996 2010-07-30 10:43:32Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "v4luGrabber.hh"

int
registerV4LuGrabber(void)
{
    NOTIFY("%s: starting up\n", V4LU_NAME);

    //
    // start registration
    //
    sourceFactory_ref fact= v4luGrabberFactory_t::createFactory();

    if ( ! fact.isValid())
    {
        NOTIFY("---Hardware not found or insufficients rights\n");
        NOTIFY("---Bailing out\n");

        return -1;
    }

    srcDescList_ref sdlr= new ql_t<sourceDescriptor_ref>;

    for (unsigned i= 0; i < v4luGrabberFactory_t::getNumDevices(); i++)
    {
        VideoHandler_t *devStat = v4luGrabberFactory_t::getVideoHandler(i);

        const char *inputPorts= devStat->getInputPorts();

        sourceDescriptor_ref desc=
            new v4luGrabberDescriptor_t(devStat->getID(), inputPorts);

        if ( ! desc.isValid())
        {
            NOTIFY("---Module internal error\n");
            NOTIFY("---Bailing out\n");

            return -1;
        }

        sdlr->insert(desc);
    }

    for (ql_t<sourceDescriptor_ref>::iterator_t i= sdlr->begin();
         i != sdlr->end();
         i++
        )
    {
        sourceDescriptor_ref desc= i;

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
    }

    return 0;
}

void
releaseV4LuGrabber(void)
{
    for (unsigned i= 0; i < v4luGrabberFactory_t::getNumDevices(); i++)
    {
        VideoHandler_t *devStat = v4luGrabberFactory_t::getVideoHandler(i);

        if (releaseSourceFactory(devStat->getID()))
        {
            NOTIFY("+++Released grabber '%s'\n", devStat->getName());
        }
        else
        {
            NOTIFY("---Unable to release grabber '%s'\n", devStat->getName());
        }
    }

    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();
    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);
        NOTIFY("%d %s\n", i, sfi->getDescriptor()->getID());
    }
    NOTIFY("%s: shutting down\n", V4LU_NAME);
}

