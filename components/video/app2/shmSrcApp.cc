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
// $Id: shmSrcApp.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <Isabel_SDK/isabelSDK.hh>

#include "ctrlProtocol.hh"
#include "shmSrcApp.hh"

shmSrcApp_t::shmSrcApp_t(int &argc, argv_t &argv)
: application_t(argc, argv)
{
    int shmId= -1, shmSize= -1;

    serverSock= NULL;

    srcV4L= true;
    srcWin= true;
    srcShm= false;

    enum myOptions
    {
        optConfFile,
        optShell,
        optCPort,

        optNoV4L,
        optNoWin,
        optShmGrabber,

        optShmId,
        optShmSize
    };

    optionDefList_t  opt;
    appParamList_t  *parList;

    opt
        << new optionDef_t("@conf"        , optConfFile   )
        << new optionDef_t("shell"        , optShell      )
        << new optionDef_t("@cport"       , optCPort      )

        << new optionDef_t("noV4L"        , optNoV4L       )
        << new optionDef_t("noWin"        , optNoWin       )
        << new optionDef_t("shmSrc"       , optShmGrabber  )

        << new optionDef_t("@shmId"       , optShmId      )
        << new optionDef_t("@shmSize"     , optShmSize    );

    parList= getOpt(opt, argc, argv);

    for ( ; parList->len(); parList->behead())
    {
        switch(parList->head()->parId)
        {
        case optConfFile:
            *this<< new shmSrcInterface_t(
                        new confFile_t(parList->head()->parValue)
                    );
            break;
        case optShell:
            *this<< new shmSrcInterface_t(new shellIO_t(0, 1));
            break;
        case optCPort:
            {
                inetAddr_t addr("127.0.0.1", parList->head()->parValue);
                serverSock = new streamSocket_t(addr);
                if (serverSock->listen(addr) < 0)
                {
                    NOTIFY("Problems when calling listen() "
                           "for control socket, bailing out"
                          );
                    abort();
                }

                *this<< new tcpServer_t<shmSrcInterface_t> (*serverSock);
            }
            break;

        case optNoV4L:
            srcV4L= false;
            break;

        case optNoWin:
            srcWin= false;
            break;

        case optShmGrabber:
            srcShm= true;
            break;

        case optShmId:
            shmId = atoi (parList -> head() -> parValue);
            break;
        case optShmSize:
            shmSize = atoi (parList -> head() -> parValue);
            break;

        default:
            NOTIFY("---Unknown option: `%s', bailing out\n",
                   parList->head()->parId
                  );
            fprintf(stderr,
                    "shmSrcApp_t::shmSrcApp_t():: "
                    "bogus parList received from application_t::getOpt()\n"
                   );
            abort();
        }
    }

    if ( (shmId <= 0) || (shmSize <= 0) ) {
        NOTIFY("---Invalid options for shmId (%d) or shmSize (%d)\n",
               shmId,
               shmSize
              );
        abort();
    }

    //
    // Initialize Grabbers
    //
    registerSyntheticGrabber();
    registerFileReader();
#ifdef __BUILD_FOR_LINUX
    if (srcV4L)
    {
        registerV4LuGrabber();
    }
    if (srcWin)
    {
        registerWinGrabber();
    }
    if (srcShm)
    {
        registerShmGrabber();
    }
#endif
#ifdef __BUILD_FOR_WINXP
    registerDSGrabber();
#endif
    NOTIFY("Grabbers Initialised\n");

    srcMgr= new srcMgr_t(this, shmId, shmSize);
}

shmSrcApp_t::~shmSrcApp_t(void)
{
    debugMsg(dbg_App_Paranoic, "~shmSrcApp_t", "destructor invoked");

    //
    // Release Grabbers
    //
    releaseSyntheticGrabber();
#ifdef __BUILD_FOR_LINUX
    if (srcV4L)
    {
        releaseV4LuGrabber();
    }
    if (srcWin)
    {
        releaseWinGrabber();
    }
    if (srcShm)
    {
        releaseShmGrabber();
    }
#endif
#ifdef __BUILD_FOR_WINXP
    releaseDSGrabber();
#endif
    NOTIFY("Grabbers Released\n");
}

