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
// $Id: main.cc 20617 2010-06-17 13:02:35Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>

#include "videoApp.hh"

videoApp_t *theApp;

int
main(int argc, char *argv[])
{
    try
    {
        theApp= new videoApp_t(argc, argv);

        theApp->run();
    } catch (char const *e)
    {
        NOTIFY("terminate after throwing exception: \"%s\"\n", e);
    }

    return 0; // keeps compiler happy
}

