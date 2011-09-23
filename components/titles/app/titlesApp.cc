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
// $Id: titlesApp.cc 20764 2010-07-06 10:13:50Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include <X11/X.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/io.hh>
#include <icf2/sockIO.hh>
#include <icf2/task.hh>
#include <icf2/sched.hh>
#include <icf2/stdTask.hh>

#include "titlesApp.hh"

//
// application
//
titlesApp_t::titlesApp_t(int &argc, argv_t &argv)
: application_t(argc, argv)
{
    enum myOptions
    {
        optDisplay,

        optConfFile,
        optShell,
        optCPort,

        optColor
    };

    serverSock= NULL;

    ql_t<char*> colorNames;

    Display *dpy;

    optionDefList_t  opt;
    appParamList_t  *parList;

    opt
        << new optionDef_t("@display"     , optDisplay     )

        << new optionDef_t("@conf"        , optConfFile    )
        << new optionDef_t("shell"        , optShell       )
        << new optionDef_t("@cport"       , optCPort       )

        << new optionDef_t("@color"       , optColor       )
        ;

    parList= getOpt(opt, argc, argv);

    for(; parList->len(); parList->behead()) {
        switch(parList->head()->parId) {
        case optDisplay:
            {
                char *b= (char *)malloc(512);
                sprintf(b, "DISPLAY=%s", parList->head()->parValue);
                putenv(b);
            } break;

        case optConfFile:
            *this<< new titles_Interface_t(
                        new confFile_t(parList->head()->parValue)
                    );
            break;

        case optShell:
            *this<< new titles_Interface_t( new shellIO_t(0,1) );
            break;

        case optCPort:
            {
                inetAddr_t addr("127.0.0.1", parList->head()->parValue);
                serverSock = new streamSocket_t(addr);
                if (serverSock->listen(addr) < 0) {
                    NOTIFY("Problems when calling listen() "
                           "for control socket, bailing out"
                          );
                    abort();
                }

                *this<< new tcpServer_t<titles_Interface_t>(*serverSock);
            }
            break;

        case optColor:
            colorNames << parList->head()->parValue;
            break;

        default:
            NOTIFY("---Unknown option: `%s', bailing out\n",
                   parList->head()->parId
                  );
            fprintf(stderr,
                    "titlesApp_t::titlesApp_t():: "
                    "bogus parList received from application_t::getOpt()\n"
                   );
            abort();
        }
    }

    char *dpyName;
    if ((dpyName= getenv("DISPLAY")) == NULL)
    {
        fprintf(stderr, "%s: cannot get DISPLAY env. var.\n", className());
        exit(1);
    }

    if ((dpy= XOpenDisplay(dpyName)) == NULL)
    {
        fprintf(stderr, "%s: XOpenDisplay failed.\n", className());
        exit(1);
    }

    //
    // Reserving colors
    //
    for (ql_t<char*>::iterator_t i= colorNames.begin();
         i!= colorNames.end();
         i++
        )
    {
        char *colorName= static_cast<char*>(i);

        reservedColors << (new Color_t(dpy, colorName));
    }

    //
    // Constructing hero
    //
    theTxtWinMgr= new txtWinMgr_t(dpy);
}

titlesApp_t::~titlesApp_t(void)
{
    debugMsg(dbg_App_Normal, "~titlesApp_t", "destructor invoked");

    while (reservedColors.len() > 0)
    {
        Color_t *color= reservedColors.head();
        reservedColors.behead();
        delete color;
    }

    delete theTxtWinMgr;
}

