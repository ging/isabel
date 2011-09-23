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
// $Id: link.cc 10635 2007-08-22 17:32:16Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>

//#include "output.hh"
#include "transApp.hh"
#include "link.hh"

#ifdef __BUILD_FOR_LINUX
#include <netdb.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <arpa/inet.h>
#endif


// link_t -- a network link as seen by end user

link_t::link_t(transApp_t *app,
               linkClass_e lc,
               const bool  echoBool,
               const int   bandwidth,
               bool        typeFlag
              )
: myApp(app),
  lclass(lc),
  type(typeFlag),
  shouldEcho(echoBool)
{
}


link_t::~link_t(void)
{
    debugMsg(dbg_App_Paranoic, "~link", "No hay link!\n");
}



//
// linkBinder_t stuff
//
dictionary_t<string, link_t *>  linkBinder_t::linkDict(MAX_FLOW);

linkBinder_t::linkBinder_t(transApp_t *app)
{
    myApp= app;
}


linkBinder_t::~linkBinder_t(void)
{
    debugMsg(dbg_App_Paranoic, "~linkBinder_t", "Destroying linkBinder_t\n");
}


link_t *
linkBinder_t::newLink(const char *nam,
                      link_t::linkClass_e c,
                      const bool echoMode,
                      const int bw,
                      bool type
                     )
{
    link_t *theLink= linkDict.lookUp(nam);

    if (theLink)
    {  // exists!!
        return theLink;
    }

    debugMsg(dbg_App_Normal, "newLink", "LinkName=[%s]\n", nam);

    theLink= new link_t(myApp, c, echoMode, bw, type);
/*    if (myApp->audioMixer.isValid())
    {
        newLinkMsg_ref linkMsg = new newLinkMsg_t(theLink);
        myApp->depacketizer->newLink(linkMsg);
    }
*/
    linkDict.insert(nam, theLink);

    return theLink;
}


bool
linkBinder_t::deleteLink(const char *name)
{
    link_t *theLink= linkDict.lookUp(name);

    if( ! theLink)
    {
        return false;
    }

    linkDict.remove(name);
    delete theLink;

    return true;
}


link_t *
linkBinder_t::lookUp(const char *name)
{
    return linkDict.lookUp(name);
}



