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

#include "audioapp.h"
#include "ctrlprotocol.h"

#include "channelmgr.h"
#include "mixerbuffer.h"
#include "rtpheader.h"
#include "udpsound.h"
#include "playchannel.h"

#if defined(__BUILD_FOR_LINUX)
#include "../linux/alsasounddevice.h"
#include "../portaudio/pasounddevice.h"
#elif defined(WIN32)
#include "winsounddevice.h"
#else
#error "Please, implement sound device for your O.S."
#endif

// -----------------------------------------------------------------------------
// AudioApp::AudioApp
// Constructor de la clase. Crea todos los objetos de la aplicacion
// -----------------------------------------------------------------------------
//
AudioApp::AudioApp(int &argc, argv_t &argv)
: application_t(argc, argv),
  autoCreate(false),
  doStats(false)
{
    const char *myHost     = NULL;

    const char *rtpPort    = "51001";
    const char *vuPort     = "51022";
    const char *vuPlayPort = "51021";

#ifdef __BUILD_FOR_LINUX
    const char *capDevice  = "0";
    const char *playDevice = "0";
#endif

    const char *statsPort  = "51000";
    const char *udpPort    = "6789";

    enum myOptions
    {
        optMyHost,     /* source IP for RTP/UDP packets */

        optShell,      /* terminal control              */
        optCtrlPort,   /* tcp control port              */

        optRtpPort,    /* rtp data port                 */
        optVuPort,     /* Rec vumeter data port         */
        optVuPlayPort, /* Play vumeter data port        */
        optStatsPort,  /* Statistics data port          */

        optCapDevice,  /* Dispositivo de captura        */
        optPlayDevice, /* Dispositivo de reproduccion   */

        optAutoCreate  /* auto create play channels     */
    };

    optionDefList_t  opt;
    appParamList_t  *parList;

    opt
        << new optionDef_t("@myHost"     , optMyHost     )

        << new optionDef_t("shell"       , optShell      )
        << new optionDef_t("@cport"      , optCtrlPort   )

        << new optionDef_t("@rtpPort"    , optRtpPort    )
        << new optionDef_t("@vuPort"     , optVuPort     )
        << new optionDef_t("@vuPlayPort" , optVuPlayPort )

        << new optionDef_t("@statsPort"  , optStatsPort  )

#ifdef __BUILD_FOR_LINUX
        << new optionDef_t("@capDevice"  , optCapDevice  )
        << new optionDef_t("@playDevice" , optPlayDevice )
#endif

        << new optionDef_t("autoCreate"  , optAutoCreate );

    parList = getOpt(opt, argc, argv);

    // Parseo de los parametros de la linea de comando
    for ( ; parList->len(); parList->behead())
    {
        switch(parList->head()->parId)
        {
        case optMyHost:
            myHost = strdup(parList->head()->parValue);
            break;

        case optShell:
            *this<< new AudioInterface(new shellIO_t(0, 1));
            break;

        case optCtrlPort:
            {
                inetAddr_t ctrlAddr("127.0.0.1", parList->head()->parValue);
                ctrlSocket= new streamSocket_t(ctrlAddr);
                if (ctrlSocket->listen(ctrlAddr) < 0)
                {
                    NOTIFY("Error tcp server socket:: system halted!\n");
                    abort();
                }

                *this<< new tcpServer_t<AudioInterface>(*ctrlSocket);
            }

            break;

        case optRtpPort:
            rtpPort = strdup(parList->head()->parValue);
            break;

        case optVuPort:
            vuPort = strdup(parList->head()->parValue);
            break;

        case optVuPlayPort:
            vuPlayPort = strdup(parList->head()->parValue);
            break;

        case optStatsPort:
            statsPort = strdup(parList->head()->parValue);
            break;

#ifdef __BUILD_FOR_LINUX
        case optCapDevice:
            capDevice = strdup(parList->head()->parValue);
            break;

        case optPlayDevice:
            playDevice = strdup(parList->head()->parValue);
            break;
#endif

        case optAutoCreate:
            autoCreate = true;
            break;

        default:
            NOTIFY("AudioApp_t:: Usage error\n");
            NOTIFY("AudioApp_t:: Bailing out\n");
            abort();
            break;
        }
    }


    // -------------------------------------
    //  RTP data socket
    // -------------------------------------
#ifdef WIN32
    if ( ! myHost )
    {
        myHost= "0.0.0.0"; // Windows does not accept (NULL,0)
    }
    inetAddr_t  addr1(myHost, rtpPort, serviceAddr_t::DGRAM);
    inetAddr_t  addr2("0.0.0.0", udpPort, serviceAddr_t::DGRAM);
#else
    inetAddr_t  addr1(myHost, rtpPort, serviceAddr_t::DGRAM);
    inetAddr_t  addr2(NULL, udpPort, serviceAddr_t::DGRAM);
#endif

    rtpSocket = new dgramSocket_t(addr1);
    udpSocket = new dgramSocket_t(addr2);

    // -------------------------------------
    //  Vumeter data socket
    // -------------------------------------
    //inetAddr_t  addr2(NULL, "30000", serviceAddr_t::DGRAM);
    //vuSocket = new dgramSocket_t(addr2);

#ifdef __BUILD_FOR_LINUX
    //soundDevice = new AlsaSoundDevice_t(playDevice, capDevice);
    soundDevice = new PASoundDevice_t();
#else
    soundDevice = new WinSoundDevice_t(NULL, NULL);
#endif
    mixer = new MixerBuffer(soundDevice, rtpSocket, vuPlayPort);
    UdpSound *udpSound = new UdpSound(mixer);
    channelMgr = new ChannelManager(soundDevice,
                                    rtpSocket,
                                    rtpSocket,
                                    vuPort,
                                    vuPlayPort,
                                    statsPort,
                                    mixer,
                                    udpSound
                                   );

    AudioTask *task = new AudioTask(HB_TIME);
    task->add_IO(rtpSocket);
    this->insertTask(task);

    udpSound->add_IO(udpSocket);
    this->insertTask(udpSound);
}

// -----------------------------------------------------------------------------
// AudioApp::~AudioApp
// Destructor de la clase.
// -----------------------------------------------------------------------------
//
AudioApp::~AudioApp(void)
{
    if (soundDevice)
    {
        delete soundDevice;
        soundDevice = NULL;
    }

    if (channelMgr)
    {
        delete channelMgr;
        channelMgr = NULL;
    }

    if (mixer)
    {
        delete mixer;
        mixer = NULL;
    }

    if (rtpSocket)
    {
        delete rtpSocket;
        rtpSocket = NULL;
    }

    if (udpSocket)
    {
        delete udpSocket;
        udpSocket = NULL;
    }

/*
    if (vuSocket)
    {
        delete vuSocket;
        vuSocket = NULL;
    }
*/
}

// -----------------------------------------------------------------------------
// AudioApp::AudioTask::hearbeat
// Pasa el evento al canal de envio y al mezclador
// -----------------------------------------------------------------------------
//
void
AudioApp::AudioTask::heartBeat(void)
{
    static int beats = 0;
    beats++;

    AudioApp *app = static_cast<AudioApp*>(this->get_owner());

    // Aviso al recChannel para que envie los datos capturados
    RecChannel *rc = app->channelMgr->getRecChannel();

    if (rc != NULL)
    {
        rc->heartBeat();
    }

    // Aviso al Buffer de mezcla para que mande los datos a reproducir
    app->mixer->heartBeat();

    // Mandar las estadisticas
    if (app->doStats && beats%STATS_TIME == 0)
    {
        app->channelMgr->sendStats();
    }
}

// -----------------------------------------------------------------------------
// AudioApp::AudioTask::IOReady
// Pasa el paquete de datos al canal adecuado o lo descarta si no hay canal
// -----------------------------------------------------------------------------
//
void
AudioApp::AudioTask::IOReady(io_ref& io)
{
    static unsigned char buffer[BUFFER_SIZE];

    int readed = io->read(buffer, BUFFER_SIZE);

    // Le paso el paquete al canal correspondiente
    if (readed > RTPHeader::SIZE)
    {
        RTPHeader *header = reinterpret_cast<RTPHeader*>(buffer);

        AudioApp *app = static_cast<AudioApp*>(this->get_owner());

        Channel *ch= app->channelMgr->getChannel(header->GetSSRC());

        if ( (ch == NULL) && app->autoCreate)
        {
            app->channelMgr->newPlayChannel(header->GetSSRC());
            NOTIFY("Creado canal automatico SSRC=%d\n", header->GetSSRC());

            ch= app->channelMgr->getChannel(header->GetSSRC());
        }

        if (ch != NULL)
        {
            if (ch->getKind() != Channel::PLAY_CHANNEL)
            {
                return;
            }

            PlayChannel *pc= dynamic_cast<PlayChannel*>(ch);

            pc->packetReceived(buffer, readed);
        }
    }
}

// -----------------------------------------------------------------------------
// main
//
// -----------------------------------------------------------------------------
//
int
main(int argc, char *argv[])
{
    AudioApp theApp(argc, argv);
    theApp.run();
}

