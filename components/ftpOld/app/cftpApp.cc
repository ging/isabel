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
// $Id: cftpApp.cc 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <icf2/strOps.hh>

#include "ctrlProtocol.h"
#include "cftpApp.h"
#include "fchannelMgr.h"

cftpApp_t::cftpApp_t(int &argc, argv_t &argv): application_t(argc, argv)
{

    enum myOptions { optNotifyFile, optPort, optRTPPort, optRTCPPort, optConf, optShell, optWatch};

    optionDefList_t opt;
    appParamList_t * parList;

    opt
       << new optionDef_t("@notify"   , optNotifyFile )
       << new optionDef_t("@c"        , optPort       )
       << new optionDef_t("@rtpPort"  , optRTPPort    )
       << new optionDef_t("@rtcpPort" , optRTCPPort   )
       << new optionDef_t("@conf"     , optConf       )
       << new optionDef_t("shell"     , optShell      );

    parList= getOpt(opt, argc, argv);

    const char *ctrlPort= NULL;  // default value
    const char *rtpPort = "51013";  // default value
    const char *rtcpPort= "51015";  // default value

    for ( ; parList->len(); parList->behead()) {
        switch(parList->head()->parId) {

        case optNotifyFile:
            setNotifyFile(parList->head()->parValue);
            break;

        case optPort:
            ctrlPort =strdup(parList->head()->parValue);
            break;

        case optRTPPort:
            rtpPort= strdup(parList->head()->parValue);
            break;

        case optRTCPPort:
            rtcpPort= strdup(parList->head()->parValue);
            break;

        case optConf: {
            io_ref io = new confFile_t(parList->head()->parValue);
            *this << new cftp_Interface(io);
        }   break;

        case optShell: {
            io_ref io = new shellIO_t(0, 1);
            *this << new cftp_Interface(io);
        }    break;

        default:
            // se deberia lanzar una excepcion
            NOTIFY("cftpApp_t::cftpApp_t(): "
                   "bogus parList received from application_t::getOpt()\n");
            NOTIFY("cftpApp_t::cftpApp_t(): Bailing out\n");
            abort();

            break;
        }
    }


    // creamos la tarea y la insertamos

    inetAddr_t ctrlAddr("127.0.0.1", ctrlPort, serviceAddr_t::STREAM);
    streamSocket_t *streamSock = new streamSocket_t(ctrlAddr);

    if (streamSock->listen(ctrlAddr) <0) {
        NOTIFY("Error tcp server ctrl socket:: System halted!\n");
        abort();
    }

    *this << new tcpServer_t<cftp_Interface>(*streamSock);

    theChanMgr   = new fchannelMgr_t ();

    rtpSession= new cftpRTPSession_t(this,
                                     NULL,
                                     rtpPort,
                                     NULL,
                                     rtcpPort,
                                     FTP_RTP_TSU
                                    );

    // Printing control port
    printf("%d\n", streamSock->localPort);

    //item_t::setDebugLevel(100);
    //item_t::debugFile=fopen( "/tmp/FTP.txt", "w");

}


cftpApp_t::~cftpApp_t(void)
{
    //delete  theChanMgr ;
}

