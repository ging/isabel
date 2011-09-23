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
// $Id: videoApp.cc 21105 2010-08-26 15:07:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <vCodecs/codecs.h>

#include <icf2/notify.hh>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "ctrlProtocol.hh"
#include "videoApp.hh"

const char *DEFAULT_STATS_PORT= "51000";

const char *DEFAULT_RTP_PORT=   "51017";
const char *DEFAULT_RTCP_PORT=  "51019";

videoApp_t::videoApp_t(int &argc, argv_t &argv)
: application_t(argc, argv)
{
    const char *statsPort= DEFAULT_STATS_PORT;

    const char *rtpPort  = DEFAULT_RTP_PORT;
    const char *rtcpPort = DEFAULT_RTCP_PORT;

    serverSock= NULL;

    srcV4L= true;
    srcWin= true;
    srcShm= false;

    char *videoOutput= NULL;

    bool sendStatsFlag= false;

    enum myOptions
    {
        optDisplay,

        optConfFile,
        optShell,
        optCPort,

        optNoV4L,
        optNoWin,
        optShmGrabber,

        optVideoOutput,

        optRTPPort,
        optRTCPPort,

        optDoStats,
        optStatsPort
    };

    optionDefList_t  opt;
    appParamList_t  *parList;

    opt
        << new optionDef_t("@display"     , optDisplay     )

        << new optionDef_t("@conf"        , optConfFile    )
        << new optionDef_t("shell"        , optShell       )
        << new optionDef_t("@cport"       , optCPort       )

        << new optionDef_t("noV4L"        , optNoV4L       )
        << new optionDef_t("noWin"        , optNoWin       )
        << new optionDef_t("shmSrc"       , optShmGrabber  )

        << new optionDef_t("@vo"          , optVideoOutput )

        << new optionDef_t("@rtpPort"     , optRTPPort     )
        << new optionDef_t("@rtcpPort"    , optRTCPPort    )

        << new optionDef_t("doStats"      , optDoStats     )
        << new optionDef_t("@statsPort"   , optStatsPort   );

    parList= getOpt(opt, argc, argv);

    for ( ; parList->len(); parList->behead())
    {
        switch(parList->head()->parId)
        {
        case optDisplay:
            {
                char *b= (char *)malloc(512);
                sprintf(b, "DISPLAY=%s", parList->head()->parValue);
                putenv(b);
            } break;

        case optConfFile:
            *this<< new videoInterface_t(
                        new confFile_t(parList->head()->parValue)
                    );
            break;
        case optShell:
            *this<< new videoInterface_t(new shellIO_t(0, 1));
            break;
        case optCPort:
            {
                inetAddr_t addr("127.0.0.1", parList->head()->parValue);
                serverSock= new streamSocket_t(addr);
                if (serverSock->listen(addr) < 0)
                {
                    NOTIFY("Error tcp server socket:: system halted!\n");
                    abort();
                }

                *this<< new tcpServer_t<videoInterface_t> (*serverSock);
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

        case optVideoOutput:
            videoOutput = strdup (parList -> head() -> parValue);
            break;

        case optRTPPort:
            rtpPort = strdup (parList -> head() -> parValue);
            break;
        case optRTCPPort:
            rtcpPort = strdup (parList -> head() -> parValue);
            break;

        case optDoStats:
            sendStatsFlag= true;
            break;
        case optStatsPort:
            statsPort = strdup(parList->head()->parValue);
            break;

        default:
            NOTIFY("---Unknown option: `%s', bailing out\n",
                   parList->head()->parId
                  );
            fprintf(stderr,
                    "videoApp_t::videoApp_t():: "
                    "bogus parList received from application_t::getOpt()\n"
                   );
            abort();
        }
    }

    //
    // Initialize vCodecs
    //
    vCodecInit();
    NOTIFY("vCodecs Initialised\n");

    //
    // Initialize Grabbers
    //
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
    registerURLGrabber();
#endif
#ifdef __BUILD_FOR_WINXP
    registerDSGrabber();
#endif
    registerSyntheticGrabber();
    registerFileReader();
    NOTIFY("Grabbers Initialised\n");

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    const char *dpyKind, *dpyOptions;
    // by default, X11 with GLX
    dpyKind= "X11";
    dpyOptions= NULL;
    if ((videoOutput != NULL) && (strlen(videoOutput) > 0))
    {
        if (strcmp(videoOutput, "X11-GLX") == 0)
        {
            dpyKind= "X11";
            dpyOptions= "GLX";
        }
        else if (strcmp(videoOutput, "X11-Shm") == 0)
        {
            dpyKind= "X11";
            dpyOptions= NULL; // without options, MIT Shared Memory
        }
        else
        {
            NOTIFY("-vo accepts \"X11-GLX\" and \"X11-Shm\"\n");
            NOTIFY("Bailing out\n");
            exit(1);
        }
    }
    NOTIFY("Opening display \"%s(%s)\"\n", dpyKind, dpyOptions);
    dpy= createDisplayTask(dpyKind, dpyOptions);
#elif defined(WIN32)
    dpy= createDisplayTask("GDI", NULL);
#else
#error "Please, create display task in your O.S."
#endif

    (*this)<< (task_ref)((abstractTask_t*)dpy);

    channelMgr= new channelMgr_t(this, statsPort);

    (*this)<< channelMgr;

    doStats(sendStatsFlag);

    rtpSession= new videoRTPSession_t(this,
                                      channelMgr,
                                      NULL,
                                      rtpPort,
                                      NULL,
                                      rtcpPort,
                                      VIDEO_RTP_TSU
                                     );

    rtpSession->deliverAnyPacket(true);
}

videoApp_t::~videoApp_t(void)
{
    debugMsg(dbg_App_Paranoic,
             "~videoApp_t",
             "destructor invoked"
            );

    (*this) - (task_ref)((abstractTask_t*)dpy);

    delete channelMgr;
    // delete rtpSession;

    //
    // Release Grabbers
    //
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
    releaseURLGrabber();
#endif
#ifdef __BUILD_FOR_WINXP
    releaseDSGrabber();
#endif
    releaseSyntheticGrabber();
    releaseFileReader();
    NOTIFY("Grabbers Released\n");

    //
    // Release vCodecs
    //
    vCodecRelease();
    NOTIFY("vCodecs Released\n");
}

void
videoApp_t::doStats(bool doIt)
{
    if (doIt)
    {
        NOTIFY("Video's Statistics sending info\n");

        channelMgr->set_period(1000000);
    }
    else
    {
        NOTIFY("Video's Statistics stop sending info\n");

        channelMgr->set_period(0);
    }
}

void
videoApp_t::updateSources(void)
{
#if defined(__BUILD_FOR_LINUX)
    if (srcV4L)
    {
        releaseV4LuGrabber();
        registerV4LuGrabber();
    }
#elif defined(__BUILD_FOR_WINXP)
    releaseDSGrabber();
    registerDSGrabber();
#endif
}

