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
// $Id: controlProtocol.cc 8516 2006-05-18 09:40:30Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>

#ifdef __BUILD_FOR_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include "controlProtocol.h"
#include "sessionManager.h"
#include "returnCode.h"

binding_t<mcu_InterfaceDef_t> mcu_methodBinding[]=
{
    // MCU PRIMITIVES

    // session
    { "create_session",         &mcu_InterfaceDef_t::create_session    },
    { "remove_session",         &mcu_InterfaceDef_t::remove_session    },

    // participants
    { "new_participant",        &mcu_InterfaceDef_t::new_participant       },
    { "remove_participant",     &mcu_InterfaceDef_t::remove_participant    },
    { "configure_participant",  &mcu_InterfaceDef_t::configure_participant },
    { "get_participants",       &mcu_InterfaceDef_t::get_participants      },
    { "bind_rtp",               &mcu_InterfaceDef_t::bind_rtp              },
    { "unbind_rtp",             &mcu_InterfaceDef_t::unbind_rtp            },
    { "bind_rtcp",              &mcu_InterfaceDef_t::bind_rtcp             },
    { "unbind_rtcp",            &mcu_InterfaceDef_t::unbind_rtcp           },

    // codecs
    { "get_codecs",            &mcu_InterfaceDef_t::get_codecs       },
    { "get_video_codecs",      &mcu_InterfaceDef_t::get_video_codecs },
    { "get_audio_codecs",      &mcu_InterfaceDef_t::get_audio_codecs },

    // flows
    { "receive",                &mcu_InterfaceDef_t::receive            },
    { "discard",                &mcu_InterfaceDef_t::discard            },
    { "receive_video_mode",     &mcu_InterfaceDef_t::receive_video_mode },
    { "receive_video",          &mcu_InterfaceDef_t::receive_video      },
    { "discard_video",          &mcu_InterfaceDef_t::discard_video      },
    { "receive_audio_mode",     &mcu_InterfaceDef_t::receive_audio_mode },
    { "receive_audio",          &mcu_InterfaceDef_t::receive_audio      },
    { "discard_audio",          &mcu_InterfaceDef_t::discard_audio      },

    // DTE interface (IF3 : MCU <-- DTE)
    { "get_audio_losses",            &mcu_InterfaceDef_t::get_audio_losses },
    { "get_video_losses",            &mcu_InterfaceDef_t::get_video_losses },
    { "get_losses",                  &mcu_InterfaceDef_t::get_losses       },
    { "configure_participant_audio", &mcu_InterfaceDef_t::configure_participant_audio },
    { "configure_participant_video", &mcu_InterfaceDef_t::configure_participant_video },

    // system
    { "query_id",               &mcu_InterfaceDef_t::query_id  },
    { "nop",                    &mcu_InterfaceDef_t::nop       },
    { "bye",                    &mcu_InterfaceDef_t::bye       },
    { "quit",                   &mcu_InterfaceDef_t::quit      },

    { NULL, NULL }
};

//---------COMMANDS BODY-----------------//

char const *
mcu_InterfaceDef_t::create_session(int argc, char **argv)
{
    static char result[4096];
    if (argc != 2)
    {
        return "ERROR: Syntax:: create_session(n_session)\n";
    }

    unsigned int n_session = 0;
    n_session= atoi(argv[1]);

    HRESULT hr= APP->createSession(n_session);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::remove_session(int argc, char **argv)
{
    static char result[4096];
    if (argc != 2)
    {
        return "ERROR: Syntax:: remove_session(n_session)\n";
    }

    unsigned int n_session;
    n_session= atoi(argv[1]);

    HRESULT hr= APP->removeSession(n_session);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::new_participant(int argc, char **argv)
{
    static char result[4096];
    if (argc < 4)
    {
        return "ERROR: Syntax:: new_participant(n_session,ip,rx_port1[,rx_port2,...])\n";
    }

    u16 n_session;
    u16 ID_part;

    ql_t<u16> portList;

    struct addrinfo* addrInfo1 = NULL;
    struct addrinfo* addrInfo2 = NULL;
    getaddrinfo(argv[2], NULL, NULL, &addrInfo1);
    addrInfo2 = addrInfo1;

    n_session = atoi(argv[1]);

/*
    if (getenv("MCU_USE_ONLY_IPv4") &&
        addrInfo2->ai_family != AF_INET)
    {
        while (addrInfo2->ai_next)
        {
            addrInfo2 = addrInfo2->ai_next;
            if (addrInfo2->ai_family == AF_INET)
            {
                break;
            }
        }

        if (addrInfo2->ai_family != AF_INET)
        {
            NOTIFY("controlProtocol_t::new_participant::can't get IPv4 addr for %s. Aborting...\n",
                   argv[2]);
            abort();
        }
    }
*/
    if ( ! addrInfo2)
    {
         sprintf(result,"%s\n",getMessageByCode(E_IP));
         return result;
    }

    for(u16 i= 3; i < argc; i++)
    {
        u16 port = atoi(argv[i]);
        portList << port;
    }
    sockaddr_storage IP;
    switch(addrInfo2->ai_family)
    {
    case AF_INET:
        memcpy(&IP,addrInfo2->ai_addr,sizeof(sockaddr_in));
        break;
    case AF_INET6:
        memcpy(&IP,addrInfo2->ai_addr,sizeof(sockaddr_in6));
        break;
    default:
        return "ERROR: unknown protocol family";
    }
    HRESULT hr = APP->newParticipant(n_session,
                                     IP,
                                     portList,
                                     &ID_part);

    freeaddrinfo(addrInfo1);

    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%s: %d\n", getMessageByCode(hr), ID_part);
    }

    return result;
}


char const *
mcu_InterfaceDef_t::remove_participant(int argc, char **argv)
{
    static char result[4096];

    if (argc != 3)
    {
        return "ERROR: Syntax:: new_participant(n_session,id_part)\n";
    }

    u16 n_session = atoi(argv[1]);
    u16 ID_part   = atoi(argv[2]);

    HRESULT hr = APP->removeParticipant(n_session, ID_part);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::configure_participant(int argc, char **argv)
{
    static char result[4096];
    if (argc != 7)
    {
        return "ERROR: Syntax:: configure_participant(n_session,id_part,PT,ALIVE,FEC,BW)\n";
    }

    u16 n_session = atoi(argv[1]);
    u16 ID_part   = atoi(argv[2]);
    u8  PT        = atoi(argv[3]);
    bool alive    = atoi(argv[4]);
    u8  FEC       = atoi(argv[5]);
    u32 BW        = atoi(argv[6]);

    HRESULT hr =
        APP->configureParticipant(n_session, ID_part, alive, PT, FEC, BW);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::get_participants(int argc, char **argv)
{
    static char result[4096];

    if (argc != 2)
    {
        return "ERROR: Syntax:: get_participants(n_session)\n";
    }

    u16 n_session = atoi(argv[1]);
    char *participants = new char[1024];
    HRESULT hr = APP->getParticipants(n_session, &participants);

    if (SUCCEEDED(hr))
    {
        sprintf(result, "%s: %s\n", getMessageByCode(hr), participants);
    }
    else
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    delete[] participants;
    return result;
}

char const *
mcu_InterfaceDef_t::bind_rtp(int argc, char **argv)
{
    static char result[4096];

    if (argc != 7)
    {
        return "ERROR: Syntax:: bind_rtp(n_session,id_part,PT,remoteRTPport,localRTPport,localRTCPport)\n";
    }

    u16 n_session     = atoi(argv[1]);
    u16 ID_part       = atoi(argv[2]);
    u8  PT            = atoi(argv[3]);
    u16 remoteRTPport = atoi(argv[4]);
    u16 localRTPport  = atoi(argv[5]);
    u16 localRTCPport = atoi(argv[6]);

    HRESULT hr = APP->bindRtp(n_session,
                              ID_part,
                              PT,
                              remoteRTPport,
                              localRTPport,
                              localRTCPport
                             );

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::unbind_rtp(int argc, char **argv)
{
    static char result[4096];
    if (argc != 4)
    {
        return "ERROR: Syntax:: unbind_rtp(n_session,id_part,PT)\n";
    }

    u16 n_session = atoi(argv[1]);
    u16 ID_part   = atoi(argv[2]);
    u8  PT        = atoi(argv[3]);

    HRESULT hr = APP->unbindRtp(n_session, ID_part, PT);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::bind_rtcp(int argc, char **argv)
{
    static char result[4096];
    if (argc != 6)
    {
        return "ERROR: Syntax:: bind_rtcp(n_session,id_part,localRTPport,remoteRTCPport,localRTCPport)\n";
    }

    u16 n_session      = atoi(argv[1]);
    u16 ID_part        = atoi(argv[2]);
    u16 localRTPport   = atoi(argv[3]);
    u16 remoteRTCPport = atoi(argv[4]);
    u16 localRTCPport  = atoi(argv[5]);

    HRESULT hr = APP->bindRtcp(n_session,
                               ID_part,
                               localRTPport,
                               remoteRTCPport,
                               localRTCPport
                              );

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::unbind_rtcp(int argc, char **argv)
{
    if (argc != 4)
    {
        return "ERROR: Syntax:: unbind_rtcp(n_session,id_part,localRTPport,remoteRTCPport)\n";
    }

    return "ERROR: Not Implemented\n";
}

char const *
mcu_InterfaceDef_t::get_codecs(int argc, char **argv)
{
    static char result[4096];
    if (argc != 1)
    {
        return "ERROR: Syntax:: get_codecs()\n";
    }

    char codecs[1024];
    HRESULT hr = APP->getCodecs(codecs);

    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%s: %s\n", getMessageByCode(hr), codecs);
    }
    return result;
}

char const *
mcu_InterfaceDef_t::get_video_codecs(int argc, char **argv)
{
    static char result[4096];
    if (argc != 1)
    {
        return "ERROR: Syntax:: get_video_codecs()\n";
    }

    char codecs[1024];
    HRESULT hr = APP->getVideoCodecs(codecs);

    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%s: %s\n", getMessageByCode(hr), codecs);
    }
    return result;
}

char const *
mcu_InterfaceDef_t::get_audio_codecs(int argc, char **argv)
{
    static char result[4096];
    if (argc != 1)
    {
        return "ERROR: Syntax:: get_audio_codecs()\n";
    }

    char codecs[1024];
    HRESULT hr = APP->getAudioCodecs(codecs);

    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%s: %s\n", getMessageByCode(hr), codecs);
    }
    return result;
}

char const *
mcu_InterfaceDef_t::receive(int argc, char **argv)
{
    static char result[4096];
    if (argc < 5 ||argc > 6)
    {
        return "ERROR: Syntax:: receive(n_session,id_part_rx,id_part_tx,PT)\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part_rx = atoi(argv[2]);
    u16 ID_part_tx = atoi(argv[3]);
    u8  PT         = atoi(argv[4]);
    u32 SSRC = 0;
    if (argc == 6)
    {
        SSRC = atoi(argv[5]);
    }
    HRESULT hr = APP->receive(n_session, ID_part_rx, ID_part_tx, PT, SSRC);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::discard(int argc, char **argv)
{
    static char result[4096];
    if (argc != 5)
    {
        return "ERROR: Syntax:: discard(n_session,id_part_rx,id_part_tx,PT)\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part_rx = atoi(argv[2]);
    u16 ID_part_tx = atoi(argv[3]);
    u8  PT         = atoi(argv[4]);

    HRESULT hr = APP->discard(n_session, ID_part_rx, ID_part_tx, PT);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::receive_video_mode(int argc, char **argv)
{
    static char result[4096];

    if (argc < 4 || argc > 12)
    {
        return "ERROR: Syntax:: receive_video_mode(n_session,id_part,mode[,PT,BW,FR,Q,Width,Height,SSRC])\n";
    }

    u16 n_session= atoi(argv[1]);
    u16 ID_part  = atoi(argv[2]);
    int PT     = argc >  4 ? atoi(argv[4])  : -1;
    u32 BW     = argc >  5 ? atoi(argv[5])  : 750000;
    u8  FR     = argc >  6 ? atoi(argv[6])  : 30;
    u8  Q      = argc >  7 ? atoi(argv[7])  : 0;
    u16 Width  = argc >  8 ? atoi(argv[8])  : 352;
    u16 Height = argc >  9 ? atoi(argv[9])  : 288;
    u32 SSRC   = argc > 10 ? atoi(argv[10]) : 0;

    videoMode_e mode;

    if (strcmp("SWITCH_MODE", argv[3]) == 0)
    {
        if (PT > 0) {
            mode = VIDEO_TRANS_MODE;
        } else {
            mode = VIDEO_SWITCH_MODE;
        }
    }
    else if (strcmp("GRID_MODE", argv[3]) == 0)
    {
        mode = VIDEO_GRID_MODE;
        if (argc < 11)
        {
            return "ERROR: in GRID mode must be specified PT,BW,FR,Q,Width,Height,SSRC,[gridMode]\n";
        }
    }
    else if (strcmp("GRID2_MODE", argv[3]) == 0)
    {
        mode = VIDEO_GRID2_MODE;
        if (argc < 11)
        {
            return "ERROR: in GRID2 mode must be specified PT,BW,FR,Q,Width,Height,SSRC,[gridMode]\n";
        }
    }
    else
    {
        return "ERROR: unknown mode\n";
    }
    
    gridMode_e gridMode = GRID_AUTO;

    if (argc > 11)
    {
        if (strcmp(argv[11],"GRID_BORDER") == 0)
        {
            gridMode = GRID_BORDER;
        }
        else if (strcmp(argv[11],"GRID_BORDER_TR") == 0)
        {
            gridMode = GRID_BORDER_TR;
        }
        else if (strcmp(argv[11],"GRID_PIP") == 0)
        {
            gridMode = GRID_PIP;
        }
        else if (strcmp(argv[11],"GRID_CIRCLE") == 0)
        {
            gridMode = GRID_CIRCLE;
        }
        else if (strcmp(argv[11],"GRID_AUTO") == 0)
        {
            gridMode = GRID_AUTO;
        }
        else
        {
            return "ERROR: unknown GRID type\n";
        }
    }

    HRESULT hr = APP->receiveVideoMode(n_session,
                                       ID_part,
                                       mode,
                                       PT,
                                       BW,
                                       FR,
                                       Q,
                                       Width,
                                       Height,
                                       SSRC,
                                       gridMode
                                      );

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::receive_video(int argc, char **argv)
{
    static char result[4096];

    if (argc < 4 || argc > 11)
    {
        return "ERROR: Syntax:: receive_video(n_session,id_part_rx,id_part_tx[,PT,BW,FR,Q,Width,Height,SSRC])\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part_rx = atoi(argv[2]);
    u16 ID_part_tx = atoi(argv[3]);
    int PT         = argc >  4 ? atoi(argv[4])  : -1;
    u32 BW         = argc >  5 ? atoi(argv[5])  : 0;
    u8  FR         = argc >  6 ? atoi(argv[6])  : 0;
    u8  Q          = argc >  7 ? atoi(argv[7])  : 0;
    u16 Width      = argc >  8 ? atoi(argv[8])  : 0;
    u16 Height     = argc >  9 ? atoi(argv[9])  : 0;
    u32 SSRC       = argc > 10 ? atoi(argv[10]) : 0;

    HRESULT hr = APP->receiveVideo(n_session,
                                   ID_part_rx,
                                   ID_part_tx,
                                   PT,
                                   BW,
                                   FR,
                                   Q,
                                   Width,
                                   Height,
                                   SSRC
                                  );

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

char const *
mcu_InterfaceDef_t::discard_video(int argc, char **argv)
{
    static char result[4096];

    if (argc != 4)
    {
        return "ERROR: Syntax:: discard_video(n_session,id_part_rx,id_part_tx)\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part_rx = atoi(argv[2]);
    u16 ID_part_tx = atoi(argv[3]);

    HRESULT hr = APP->discardVideo(n_session, ID_part_rx, ID_part_tx);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}


char const *
mcu_InterfaceDef_t::receive_audio_mode(int argc, char **argv)
{
    static char result[4096];
    if (argc < 4 || argc > 6)
    {
        return "ERROR: Syntax:: receive_audio_mode(n_session,id_part,mode[,PT,SL])\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part = atoi(argv[2]);

    audioMode_e mode;

    if (strcmp("MIX_MODE", argv[3]) == 0)
    {
        mode = AUDIO_MIXER_MODE;
    }
    else if (strcmp("SWITCH_MODE", argv[3]) == 0)
    {
        mode = AUDIO_SWITCH_MODE;
    }
    else
    {
        return "ERROR: unknown mode\n";
    }


    int PT   = argc >= 5 ? atoi(argv[4]) : -1;
    int SL   = argc == 6 ? atoi(argv[5]) : -100;

    HRESULT hr = APP->receiveAudioMode(n_session, ID_part, mode, PT, SL);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}


char const *
mcu_InterfaceDef_t::receive_audio(int argc, char **argv)
{
    static char result[4096];

    if (argc < 4 || argc > 5)
    {
        return "ERROR: Syntax:: receive_audio(n_session,id_part_rx,id_part_tx)\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part_rx = atoi(argv[2]);
    u16 ID_part_tx = atoi(argv[3]);
    u32 SSRC = 0;
    if (argc == 5)
    {
        SSRC = atoi(argv[4]);
    }

    HRESULT hr = APP->receiveAudio(n_session, ID_part_rx, ID_part_tx, SSRC);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}


char const *
mcu_InterfaceDef_t::discard_audio(int argc, char **argv)
{
    static char result[4096];
    if (argc != 4)
    {
        return "ERROR: Syntax:: discard_audio(n_session,id_part_rx,id_part_tx)\n";
    }
    u16 n_session  = atoi(argv[1]);
    u16 ID_part_rx = atoi(argv[2]);
    u16 ID_part_tx = atoi(argv[3]);

    HRESULT hr = APP->discardAudio(n_session, ID_part_rx, ID_part_tx);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}

// DTE interface

char const *
mcu_InterfaceDef_t::get_audio_losses(int argc, char **argv)
{
    static char result[4096];
    if (argc != 3)
    {
        return "ERROR: Syntax:: get_audio_losses(n_session, id_part)\n";
    }
    u16 n_session  = atoi(argv[1]);
    u16 ID_part    = atoi(argv[2]);
    double losses  = 0.0;

    HRESULT hr = APP->getAudioLosses(n_session, ID_part, &losses);
    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%f\n", losses);
    }
    return result;
}

char const *
mcu_InterfaceDef_t::get_video_losses(int argc, char **argv)
{
    static char result[4096];
    if (argc != 3)
    {
        return "ERROR: Syntax:: get_video_losses(n_session, id_part)\n";
    }
    u16 n_session  = atoi(argv[1]);
    u16 ID_part    = atoi(argv[2]);
    double losses  = 0.0;

    HRESULT hr = APP->getVideoLosses(n_session,ID_part,&losses);
    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%f\n", losses);
    }
    return result;
}

char const *
mcu_InterfaceDef_t::get_losses(int argc, char **argv)
{
    static char result[4096];
    if (argc != 3)
    {
        return "ERROR: Syntax:: get_losses(n_session, id_part)\n";
    }
    u16 n_session  = atoi(argv[1]);
    u16 ID_part    = atoi(argv[2]);
    double losses  = 0.0;

    HRESULT hr = APP->getLosses(n_session, ID_part, &losses);
    if (FAILED(hr))
    {
        sprintf(result, "%s\n", getMessageByCode(hr));
    }
    else
    {
        sprintf(result, "%f\n", losses);
    }
    return result;
}

char const *
mcu_InterfaceDef_t::configure_participant_audio(int argc, char **argv)
{
    static char result[4096];
    if (argc != 5)
    {
        return "ERROR: Syntax:: configure_participant_audio(n_session,id_part,oldPT,newPT)\n";
    }

    u16 n_session  = atoi(argv[1]);
    u16 ID_part    = atoi(argv[2]);
    u8  oldPT      = atoi(argv[3]);
    u8  newPT      = atoi(argv[4]);

    HRESULT hr = APP->configAudioPart(n_session, ID_part, oldPT, newPT);

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}


char const *
mcu_InterfaceDef_t::configure_participant_video(int argc, char **argv)
{
    static char result[4096];
    if (argc < 7 || argc > 9)
    {
        return "ERROR: Syntax:: configure_participant_video(n_session,id_part,oldPT,newPT,CBR|VBR,[[BW]|[FR,Q,Geom]])\n";
    }

    HRESULT hr = E_NOT_IMPLEMENTED;

    sprintf(result, "%s\n", getMessageByCode(hr));
    return result;
}


//
// common stuff
//
char const *
mcu_InterfaceDef_t::query_id(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    sprintf(result, "0x%x\n", interID);
    return result;
}

char const *
mcu_InterfaceDef_t::nop(int argc, char **argv)
{
    return "OK\n";
}


char const *
mcu_InterfaceDef_t::bye(int argc, char **argv)
{
    if (argc != 1)
    {
        return "ERROR: Syntax:: bye()\n";
    }

    return "ERROR: Not Implemented\n";
}


char const *
mcu_InterfaceDef_t::quit(int argc, char **argv)
{
    if (argc != 1)
    {
        return "ERROR: Syntax:: quit()\n";
    }
    exit(0);
    return "OK\n";
}

