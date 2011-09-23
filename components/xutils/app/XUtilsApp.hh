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
// $Id: XUtilsApp.hh 20653 2010-06-23 13:20:32Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __XUTILS_APP_HH__
#define __XUTILS_APP_HH__

#include <map>

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/stdTask.hh>

#include "XConn.hh"

class xutilsApp_t: public application_t
{
private:
    streamSocket_t *serverSock;

public:

    std::map<int, XConn_t*> theXConns;

    xutilsApp_t (int &argc, argv_t &argv);
  
    //
    // no se por que pero da un internal error si no lo
    // pongo
    //
    virtual ~xutilsApp_t(void);

    virtual const char *className(void) const { return "xutilsApp_t"; };

    friend int main (int, char**);
};

#endif

