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
#ifndef __POINTERAPP_HH__
#define __POINTERAPP_HH__


#include <icf/general.h>
#include <icf/stdTask.h>


#include "scanner.hh"
#include "ctrlProtocol.hh"


struct scannerApp_t: public application_t
{
private:
    streamSocket_t *serverSock;

public:

    scannerTask_t *theScanner;

    scannerApp_t(int &argc, argv_t &argv);
    virtual ~scannerApp_t(void);

    virtual const char *className(void) const { return "scannerApp_t"; };

    friend main(int, char**);
};



#endif
