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
// $Id: transApp.hh 10650 2007-08-24 13:40:35Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

//Modified version for MCU adapter
//Removed options

#ifndef __trans_app_hh__
#define __trans_app_hh__

#include <string>

using namespace std;

#include <icf2/stdTask.hh>

//#include "clientModule.hh"
#include "flow.hh"
#include "link.hh"
#include "configParam.hh"
#include "mcuSender.hh"
//
//! transApp_t -- application scheduler
//

class transApp_t: public application_t
{
public:

    mcuSender_t *mcuSender;
    irouterParam_t *irouterParam;

    //
    //! dieHard_t -- task to kill application
    //
    /*!
        Take care with this task... kills the irouter...
        I _ONLY_ use this function when debugging with insure/purify
        to get information of memory leaks...
    */
    class dieHard_t: public simpleTask_t
    {
       public:
           //! dieHard_t constructor
           /*
               \param n number of seconds to allow irouter up...
           */
           dieHard_t(int n): simpleTask_t(1000000*n) {};

           //! Callback to be executed after n seconds
           void heartBeat(void)
           {
               NOTIFY("MURIENDO DESCARADAMENTE!!!\n");
               get_owner()->shutdown();
           };
    };


private:
    ql_t<string> linkList;
    int getBwMultiplier(char *trailer);

public:

    streamSocket_t *serverSock;

    linkBinder_ref   linkBinder;
//    dictionary_t<flowId_e, char *> groupDictionary;
    //
    // instance vars
    //

    //! transApp_t constructor
    transApp_t(int &argc, argv_t &argv);

    //! transApp_t constructor
    virtual ~transApp_t(void);

    link_t * define_link(const char *linkName, linkClass_t &linkClass, 
			const bool echoBool, const int bw); 
    void delete_link(const char *linkName);

    //! Create a child process. Execute unattached to terminal.
    void goDaemon(void);

    char const *className(void) const { return "transApp_t"; }
};

#endif

