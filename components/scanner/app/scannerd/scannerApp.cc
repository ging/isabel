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
#include <stdio.h>

#include <icf/general.h>
#include <icf/io.h>
#include <icf/sockIO.h>
#include <icf/task.h>
#include <icf/sched.h>
#include <icf/stdTask.h>


#include "scannerApp.hh"
#include "ctrlProtocol.hh"


//
// application
//
scannerApp_t::scannerApp_t(int &argc, argv_t &argv)
    : application_t(argc, argv)
{
    char isaDir[256] = "/opt/DITIsabel/libexec";
    enum myOptions {
        optConfig,
        optShell,
        optControlPort,
        optIsaDir
    };


    optionDefList_t opt;
    appParamList_t  *par;

    opt
        << new optionDef_t("@conf"     , optConfig)
        << new optionDef_t("@cfg"      , optConfig)
        << new optionDef_t("shell"     , optShell)
        << new optionDef_t("@libExecDir", optIsaDir)
        << new optionDef_t("@cport"    , optControlPort);

    par= getOpt(opt, argc, argv);

    serverSock= NULL;

    for(; par->len(); (*par)--) {
        switch(par->head()->parId) {
            case optIsaDir:
                strcpy( isaDir, par->head()->parValue);
                break;
            case optConfig:
                *this<< new scanner_Interface(
                            *new confFile_t(par->head()->parValue)
                        );
                break;

            case optShell:
                *this<< new scanner_Interface(
                            *new shellIO_t(0,1)
                        );
                break;

            case optControlPort:
                if(par->head()->parValue[0]== '/') {
                    unlink(par->head()->parValue);
                    *this<< new tcpServer_t<scanner_Interface>(
                                *new streamSocket_t(
                                        *new unixAddr_t(par->head()->parValue),
                                        true
                                )
                            );
                } else {
                    *this<< new tcpServer_t<scanner_Interface>(
                                *(serverSock= new streamSocket_t(
                                        *new inetAddr_t(NULL, par->head()->parValue)
                                ))
                            );
                }
                break;
        }
    }
    // Create scanner task, and check if it could open the scanner
    theScanner= new scannerTask_t( isaDir);
    if( !theScanner->scannerOK) {
        fprintf( stderr,
                 "Could not open scanner device, check permissions\n");
        exit( 1);
    }
    *this<< theScanner;
}

scannerApp_t::~scannerApp_t(void)
{
  debugMsg(dbg_App_Normal, "~scannerApp_t", "llamando destructor");
//  delete theScanner;

  return;
}

