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
// $Id: ctrlProtocol.cc 22396 2011-05-22 13:16:08Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <vCodecs/codecs.h>

#include <Isabel_SDK/imageLoader.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "ctrlProtocol.hh"
#include "recchannel.hh"
#include "netchannel.hh"
#include "playchannel.hh"
#include "videoApp.hh"
#include "channelMgr.hh"


binding_t<video_InterfaceDef_t> video_methodBinding[]=
{
    // VIDEO PRIMITIVES

    //
    // channel stuff
    //
    { "video_rec_channel",      &video_InterfaceDef_t::video_rec_channel    },
    { "video_net_channel",      &video_InterfaceDef_t::video_net_channel    },
    { "video_play_channel",     &video_InterfaceDef_t::video_play_channel   },

    { "video_delete_channel",   &video_InterfaceDef_t::video_delete_channel },

    { "video_bind",             &video_InterfaceDef_t::video_bind           },
    { "video_unbind",           &video_InterfaceDef_t::video_unbind         },
    { "video_show_bindings",    &video_InterfaceDef_t::video_show_bindings  },

    { "video_rtcp_bind",        &video_InterfaceDef_t::video_rtcp_bind      },
    { "video_rtcp_unbind",      &video_InterfaceDef_t::video_rtcp_unbind    },
    { "video_rtcp_show_bindings",
                            &video_InterfaceDef_t::video_rtcp_show_bindings },

    { "video_send",             &video_InterfaceDef_t::video_send           },

    { "video_channel_bandwidth",&video_InterfaceDef_t::video_channel_bandwidth},
    { "video_bucket_size",      &video_InterfaceDef_t::video_bucket_size    },

    { "video_set_sdesinfo",     &video_InterfaceDef_t::video_set_sdesinfo   },
    { "video_show_sdesinfo",    &video_InterfaceDef_t::video_show_sdesinfo  },
    { "video_show_receivers",   &video_InterfaceDef_t::video_show_receivers },
    { "video_get_lost",         &video_InterfaceDef_t::video_get_lost       },
    { "video_get_local_lost",   &video_InterfaceDef_t::video_get_local_lost },
    { "video_get_fractionlost", &video_InterfaceDef_t::video_get_fractionlost},
    { "video_get_roundtrip_time",
                            &video_InterfaceDef_t::video_get_roundtrip_time },
    { "video_get_jitter",       &video_InterfaceDef_t::video_get_jitter     },

    { "video_compute_stats",    &video_InterfaceDef_t::video_compute_stats  },

    { "video_map_channel",      &video_InterfaceDef_t::video_map_channel    },
    { "video_unmap_channel",    &video_InterfaceDef_t::video_unmap_channel  },

    { "video_window_title",     &video_InterfaceDef_t::video_window_title   },
    { "video_window_geometry",  &video_InterfaceDef_t::video_window_geometry},
    { "video_window_overrideredirect",
                        &video_InterfaceDef_t::video_window_overrideredirect},

    { "video_get_winid",        &video_InterfaceDef_t::video_get_winid      },

    { "video_do_stats",         &video_InterfaceDef_t::video_do_stats       },

    //
    // general configuration
    //
    { "video_configure_channel",&video_InterfaceDef_t::video_configure_channel},

    //
    // sources
    //
    { "video_update_sources",   &video_InterfaceDef_t::video_update_sources },
    { "video_list_sources",     &video_InterfaceDef_t::video_list_sources   },
    { "video_select_source",    &video_InterfaceDef_t::video_select_source  },
    { "video_current_source",   &video_InterfaceDef_t::video_current_source },
    { "video_avatar",           &video_InterfaceDef_t::video_avatar         },
    { "video_avatar_timeout",   &video_InterfaceDef_t::video_avatar_timeout },

    { "video_frame_rate",       &video_InterfaceDef_t::video_frame_rate     },
    { "video_send_frames",      &video_InterfaceDef_t::video_send_frames    },

    { "video_source_format",    &video_InterfaceDef_t::video_source_format  },
    { "video_grab_size",        &video_InterfaceDef_t::video_grab_size      },

    { "video_list_norms",       &video_InterfaceDef_t::video_list_norms     },
    { "video_set_norm",         &video_InterfaceDef_t::video_set_norm       },
    { "video_get_norm",         &video_InterfaceDef_t::video_get_norm       },
    { "video_list_flickerfreqs",&video_InterfaceDef_t::video_list_flickerfreqs},
    { "video_set_flickerfreq",  &video_InterfaceDef_t::video_set_flickerfreq },
    { "video_get_flickerfreq",  &video_InterfaceDef_t::video_get_flickerfreq },
    { "video_set_saturation",   &video_InterfaceDef_t::video_set_saturation },
    { "video_get_saturation",   &video_InterfaceDef_t::video_get_saturation },
    { "video_set_brightness",   &video_InterfaceDef_t::video_set_brightness },
    { "video_get_brightness",   &video_InterfaceDef_t::video_get_brightness },
    { "video_set_hue",          &video_InterfaceDef_t::video_set_hue        },
    { "video_get_hue",          &video_InterfaceDef_t::video_get_hue        },
    { "video_set_contrast",     &video_InterfaceDef_t::video_set_contrast   },
    { "video_get_contrast",     &video_InterfaceDef_t::video_get_contrast   },

    { "video_set_flip",         &video_InterfaceDef_t::video_set_flip       },
    { "video_get_flip",         &video_InterfaceDef_t::video_get_flip       },
    { "video_set_mirror",       &video_InterfaceDef_t::video_set_mirror     },
    { "video_get_mirror",       &video_InterfaceDef_t::video_get_mirror     },

    { "video_crop",             &video_InterfaceDef_t::video_crop           },
    { "video_get_crop",         &video_InterfaceDef_t::video_get_crop       },

    //
    // codecs
    //
    { "video_list_codecs",      &video_InterfaceDef_t::video_list_codecs    },
    { "video_select_codec",     &video_InterfaceDef_t::video_select_codec   },
    { "video_codec_quality",    &video_InterfaceDef_t::video_codec_quality  },
    { "video_set_h263_mode",    &video_InterfaceDef_t::video_set_h263_mode  },
    { "video_set_mpeg_pbetweeni",
                             &video_InterfaceDef_t::video_set_mpeg_pbetweeni},

    //
    // common stuff
    //
    { "video_query_id",        &video_InterfaceDef_t::video_query_id  },
    { "video_nop",             &video_InterfaceDef_t::video_nop       },
    { "video_bye",             &video_InterfaceDef_t::video_bye       },
    { "video_quit",            &video_InterfaceDef_t::video_quit      },

    // SHARED DISPLAY PRIMITIVIES (to be joined with video's)

    //
    // channel stuff
    //
    { "shdis_create_source_channel", &video_InterfaceDef_t::video_shdisp_channel  },
    { "shdis_create_sink_channel",   &video_InterfaceDef_t::video_play_channel},

    { "shdis_delete_channel",   &video_InterfaceDef_t::video_delete_channel },

    { "shdis_bind",             &video_InterfaceDef_t::video_bind           },
    { "shdis_unbind",           &video_InterfaceDef_t::video_unbind         },
    { "shdis_show_bindings",    &video_InterfaceDef_t::video_show_bindings  },

    { "shdis_rtcp_bind",        &video_InterfaceDef_t::video_rtcp_bind      },
    { "shdis_rtcp_unbind",      &video_InterfaceDef_t::video_rtcp_unbind    },
    { "shdis_rtcp_show_bindings", &video_InterfaceDef_t::video_rtcp_show_bindings },

    { "shdis_send",             &video_InterfaceDef_t::video_send           },

    { "shdis_channel_bandwidth",&video_InterfaceDef_t::video_channel_bandwidth},
    { "shdis_bucket_size",      &video_InterfaceDef_t::video_bucket_size   },

    { "shdis_set_sdesinfo",     &video_InterfaceDef_t::video_set_sdesinfo   },
    { "shdis_show_sdesinfo",    &video_InterfaceDef_t::video_show_sdesinfo  },
    { "shdis_show_receivers",   &video_InterfaceDef_t::video_show_receivers },
    { "shdis_get_lost",         &video_InterfaceDef_t::video_get_lost       },
    { "shdis_get_fractionlost", &video_InterfaceDef_t::video_get_fractionlost },
    { "shdis_get_roundtrip_time", &video_InterfaceDef_t::video_get_roundtrip_time },
    { "shdis_get_jitter",       &video_InterfaceDef_t::video_get_jitter    },

    { "shdis_compute_stats",    &video_InterfaceDef_t::video_compute_stats },

    { "shdis_map_channel",      &video_InterfaceDef_t::video_map_channel   },
    { "shdis_unmap_channel",    &video_InterfaceDef_t::video_unmap_channel },

    { "shdis_window_title",     &video_InterfaceDef_t::video_window_title  },
    { "shdis_window_overrideredirect",  &video_InterfaceDef_t::video_window_overrideredirect },
    { "shdis_follow_size",      &video_InterfaceDef_t::video_follow_size   },

    { "shdis_do_stats",         &video_InterfaceDef_t::video_do_stats        },

    //
    // sources
    //
    { "shdis_list_sources",     &video_InterfaceDef_t::video_list_sources    },
    { "shdis_select_source",    &video_InterfaceDef_t::video_select_source   },
    { "shdis_current_source",   &video_InterfaceDef_t::video_current_source  },

    { "shdis_interactive_select", &video_InterfaceDef_t::video_interactive_select  },
    { "shdis_frame_rate",       &video_InterfaceDef_t::video_frame_rate      },
    { "shdis_select_by_winid",  &video_InterfaceDef_t::video_select_by_winid },

    //
    // codecs
    //
    { "shdis_list_codecs",      &video_InterfaceDef_t::video_list_codecs     },
    { "shdis_select_codec",     &video_InterfaceDef_t::video_select_codec    },
    { "shdis_codec_quality",    &video_InterfaceDef_t::video_codec_quality   },
    { "shdis_set_h263_mode",    &video_InterfaceDef_t::video_set_h263_mode   },
    { "shdis_set_mpeg_pbetweeni", &video_InterfaceDef_t::video_set_mpeg_pbetweeni },

    //
    // common stuf
    //
    { "shdis_query_id",         &video_InterfaceDef_t::video_query_id       },
    { "shdis_nop",              &video_InterfaceDef_t::video_nop            },
    { "shdis_bye",              &video_InterfaceDef_t::video_bye            },
    { "shdis_quit",             &video_InterfaceDef_t::video_quit           },

    { NULL, NULL }
};


//#define APP ((videoApp_t *)((videoInterface_t *)this)->get_owner())
#define APP dynamic_cast<videoApp_t*>(dynamic_cast<videoInterface_t*>(this)->get_owner())


//
// Auxiliar function to concatenate values
//
void
addDesc(char *retVal, char *desc, bool first)
{
    char buf[512];

    // as ',' is the separator, it is substituted by ' '
    char *comma;
    while ((comma = strchr(desc, ',')) != NULL) *comma= ' ';

    if (first)
    {
        sprintf(buf, "%s", desc);
    }
    else
    {
        sprintf(buf, ",%s", desc);
    }
    strcat(retVal, buf);
}


//
// channel stuff
//
const char *
video_InterfaceDef_t::video_rec_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *srcDesc, *inputPort;

    INTER_VRFY_ARGC_range(result, 2, 3);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    if ( ! APP->channelMgr->newRecChannel(chId, "VID"))
    {
        return "ERROR: could not create REC channel\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    if (argc == 3) // set the source
    {
        srcDesc= argv[2];
        inputPort= strchr(srcDesc, ';');

        if (inputPort)
        {
            *inputPort= '\0';
            inputPort++;
        }

        bool res= rc->setSource(srcDesc, inputPort);
    }
    else // set the default source
    {
        bool res= rc->setSource(NULL, NULL);
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_shdisp_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    if ( ! APP->channelMgr->newRecChannel(chId, "SHD"))
    {
        return "ERROR: could not create SHDISP channel\n";
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_net_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *rtpPort = NULL;
    char *rtcpPort= NULL;

    INTER_VRFY_ARGC(result, 4);

    //NOTIFY("%s(%s,%s,%s)\n", argv[0], argv[1], argv[2], argv[3]);

    chId= strtoul(argv[1], NULL, 10);

    rtpPort = argv[2];
    rtcpPort= argv[3];

    if (strlen(rtpPort) == 0)
    {
        return "ERROR: rtpPort is empty\n";
    }

    if (strlen(rtcpPort) == 0)
    {
        return "ERROR: rtcpPort is empty\n";
    }

    if ( ! APP->channelMgr->newNetChannel(chId, rtpPort, rtcpPort, "VID"))
    {
        return "ERROR: could not create NET channel\n";
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_play_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    const char *compId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    if (strcmp(argv[0], "video_play_channel") == 0)
    {
        compId= "VID";
    }
    else if (strcmp(argv[0], "shdis_create_sink_channel") == 0)
    {
        compId= "SHD";
    }
    else
    {
        compId= "UKN";
    }

    if ( ! APP->channelMgr->newPlayChannel(chId, compId))
    {
        return "ERROR: could not create PLAY channel\n";
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_delete_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    if ( ! APP->channelMgr->deleteChannel(chId))
    {
        return "ERROR: bad channel\n";
    }

    //rtpSession
    //   ->tlPostMsg(new RTPMsgDeleteSender_t(chId, "Channel deleted"));

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_bind(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *host, *port;

    INTER_VRFY_ARGC(result, 4);

    //NOTIFY("%s(%s,%s,%s)\n", argv[0], argv[1], argv[2], argv[3]);

    chId= strtoul(argv[1], NULL, 10);
    host= argv[2];
    port= argv[3];

    int x= rtpSession->rtpBind(chId, host, port);

    if (x < 0)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%d\n", x);

    return result;
}


const char *
video_InterfaceDef_t::video_unbind(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int bindId;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    bindId= atoi(argv[2]);

    if ( ! rtpSession->rtpUnbind(chId, bindId))
    {
        return "ERROR: bad channel or invalid binding\n";
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_show_bindings(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    const char *retVal = rtpSession->rtpShowBindings(chId);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_rtcp_bind(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    char *host= argv[1];
    char *port= argv[2];

    int x = rtpSession->rtcpBind(host, port);

    if (x < 0)
    {
        return "ERROR: bad thing\n";
    }

    sprintf(result, "%d\n", x);

    return result;
}


const char *
video_InterfaceDef_t::video_rtcp_unbind(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    int bindId= atoi(argv[1]);

    bool res= rtpSession->rtcpUnbind(bindId);

    if ( ! res)
    {
        return "ERROR: bad thing or invalid binding\n";
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_rtcp_show_bindings(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    const char *retVal= rtpSession->rtcpShowBindings();

    if ( ! retVal)
    {
        return "ERROR: bad thing\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_send(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    bool sendFlag;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId    = strtoul(argv[1], NULL, 10);
    sendFlag= atoi(argv[2]) != 0;

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->sendFlow(sendFlag);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->sendFlow(sendFlag);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is a PLAY channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_set_sdesinfo(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC_range(result, 4, 5);

    //NOTIFY("%s(%s,%s,%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    int identifier = atoi(argv[2]);
    if ((identifier < 2) || (identifier > 8))
        return "ERROR: invalid sdes identifier\n";

    char *info = strdup(argv[3]);

    char *prefix = NULL;
    if (argc > 4)
    {
        if (identifier != 8)
        {
            return "ERROR: prefix not permitted for identifier\n";
        }
        prefix = strdup(argv[4]);
    }

    if (prefix == NULL)
    {
        rtpSession->setSDESInfo (chId, identifier, info);
    }
    else
    {
        rtpSession->setSDESInfo (chId, identifier, info, prefix);
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_show_sdesinfo(int argc, char**argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    const char *retVal= rtpSession->showSDESInfo(chId);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_show_receivers(int argc, char**argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    const char *retVal= rtpSession->showReceivers (chId);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_get_lost(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    u32 receiver;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId    = strtoul(argv[1], NULL, 10);
    receiver= strtoul(argv[2], NULL, 10);

    const char *retVal= rtpSession->getPacketsLost(chId, receiver);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_get_local_lost(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    const char *retVal= rtpSession->getPacketsLost(chId);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_get_fractionlost(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    u32 receiver;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId    = strtoul(argv[1], NULL, 10);
    receiver= strtoul(argv[2], NULL, 10);

    const char *retVal= rtpSession->getFractionLost(chId, receiver);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_get_roundtrip_time(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    u32 receiver;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId    = strtoul(argv[1], NULL, 10);
    receiver= strtoul(argv[2], NULL, 10);

    const char *retVal= rtpSession->getRoundTripTime(chId, receiver);

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_get_jitter(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC_range(result, 2, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    const char *retVal= NULL;

    chId= strtoul(argv[1], NULL, 10);

    if (argc == 2)
    {
        retVal= rtpSession->getJitter(chId);
    }
    else
    {
        if (argc == 3)
        {
            u32 receiver = (u32)atoi (argv[2]);
            retVal= rtpSession->getJitter(chId, receiver);
        }
    }

    if ( ! retVal)
    {
        return "ERROR: bad channel\n";
    }

    sprintf(result, "%s", retVal);

    return result;
}


const char *
video_InterfaceDef_t::video_channel_bandwidth(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    double bandwidth;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId     = strtoul(argv[1], NULL, 10);
    bandwidth= (double)(atof(argv[2]) * 1000);

    if (bandwidth <= 0)
    {
        return "ERROR: invalid bandwitdh, must be greater than 0\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setChannelBandwidth(bandwidth);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->setChannelBandwidth(bandwidth);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is a PLAY channel\n", chId);
        return result;
    }

    return "OK\n";
}


#define MAX_FRACTION (100.0)
const char *
video_InterfaceDef_t::video_compute_stats(int argc, char **argv)
{
    static char result[4096];

    double bandwidth;
    double fraction;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    bandwidth= (double)atof(argv[1]) * 1000;
    fraction = (double)atof(argv[2]);

    if (fraction < 0)
    {
        return "ERROR: invalid fraction\n";
    }

    if (fraction > MAX_FRACTION)
    {
        fraction = MAX_FRACTION;
    }

    rtpSession->computeStatistics(bandwidth, fraction);

    return "OK\n";
}


#define MAX_BUCKET (2*1024)
const char *
video_InterfaceDef_t::video_bucket_size(int argc, char **argv)
{
    static char result[4096];

    int useBucket;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    useBucket= atoi(argv[1]);

    if (useBucket < 0)
    {
        return "ERROR: invalid number\n";
    }

    if (useBucket > MAX_BUCKET)
    {
        useBucket= MAX_BUCKET;
    }

    rtpSession->setMaxPacketSize(useBucket);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_map_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    cr->mapWindow();

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_unmap_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    cr->unmapWindow();

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_window_title(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *acronym, *title;

    INTER_VRFY_ARGC(result, 4);

    //NOTIFY("%s(%s,%s,%s)\n", argv[0], argv[1], argv[2], argv[3]);

    chId   = strtoul(argv[1], NULL, 10);
    acronym= argv[2];
    title  = argv[3];

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    cr->setWindowTitle(acronym, title);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_window_geometry(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *geometry;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId    = strtoul(argv[1], NULL, 10);
    geometry= argv[2];

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    cr->setWindowGeometry(geometry);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_window_overrideredirect(int argc, char **argv)
{
    static char result[4096];

    u32  chId;
    bool ORFlag;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId  = strtoul(argv[1], NULL, 10);
    ORFlag= atoi(argv[2]) != 0;

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    cr->setWindowOverrideRedirect(ORFlag);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_follow_size(int argc, char **argv)
{
    static char result[4096];

    u32  chId;
    bool followFlag;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId      = strtoul(argv[1], NULL, 10);
    followFlag= atoi(argv[2]) != 0;

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::PLAY_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a PLAY channel\n", chId);
        return result;
    }

    playChannel_t *pc= dynamic_cast<playChannel_t *>(cr);

    pc->setFollowSize(followFlag);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_winid(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    u32 winId= cr->getWinId();

    sprintf(result, "%ld\n", winId);

    return result;
}


const char *
video_InterfaceDef_t::video_do_stats(int argc, char **argv)
{
    static char result[4096];

    int doStats;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    doStats= atoi(argv[1]);

    APP->doStats(doStats);

    return "OK\n";
}


//
// general configuration
//
const char *
video_InterfaceDef_t::video_configure_channel(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    float frameRate;
    const char *grabSize;
    u32 lineFmt;
    int quality;
    double bandwidth;

    INTER_VRFY_ARGC(result, 7);

    //NOTIFY("%s(%s,%s,%s,%s,%s,%s)\n",
    //       argv[0], argv[1], argv[2], argv[3],
    //       argv[4], argv[5], argv[6]
    //      );

    chId     = strtoul(argv[1], NULL, 10);
    frameRate= atof(argv[2]);
    grabSize = argv[3];
    try
    {
        lineFmt= vGetFormatIdByName(argv[4]);
    }
    catch (const char *e)
    {
        return "ERROR: unknown format name\n";
    }
    quality  = atoi(argv[5]);
    bandwidth= (double)(atof(argv[6]) * 1000);

    // check grab size
    if (strcmp("SQCIF", grabSize) == 0)
    {
      grabSize= "128x96";
    }
    else if (strcmp("QCIF", grabSize) == 0)
    {
      grabSize= "176x144";
    }
    else if (strcmp("CIF", grabSize) == 0)
    {
      grabSize= "352x288";
    }
    else if (strcmp("4CIF", grabSize) == 0)
    {
      grabSize= "704x576";
    }
    else if (strcmp("16CIF", grabSize) == 0)
    {
      grabSize= "1408x1152";
    }

    unsigned w, h;

    if (sscanf(grabSize, "%ux%u", &w, &h) != 2)
    {
        return "ERROR: grab size expected WxH\n";
    }

    if ((w == 0) || (h == 0))
    {
        return "ERROR: grab size cannot be 0";
    }

    // check frame rate
    if (frameRate <= 0)
    {
        sprintf(result, "ERROR: invalid frame rate [%f]\n", frameRate);
        return result;
    }

    // check quality
    if (quality < 1 || quality > 100)
    {
        return "ERROR: invalid quality, must be an integer between 1 and 100\n";
    }

    // check bandwidth

    if (bandwidth <= 0)
    {
        return "ERROR: invalid bandwitdh, must be greater than 0\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setCodecQuality(quality);
            rc->setChannelBandwidth(bandwidth);
            rc->setFrameRate(frameRate);
            rc->setLineFmt(lineFmt);

            rc->setGrabSize(w, h);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->setCodecQuality(quality);
            nc->setChannelBandwidth(bandwidth);
            nc->setLineFmt(lineFmt);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is a PLAY channel\n", chId);
        return result;
    }

    return "OK\n";
}


//
// sources
//
const char *
video_InterfaceDef_t::video_update_sources(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    APP->updateSources();

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_list_sources(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    if (sfia->size() == 0)
    {
        return "ERROR: no sources available\n";
    }

    memset(result, 0, sizeof(result));

    bool first= true;

    strcat(result,"{NONE,");
    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        char *desc= strdup (sfi->getDescriptor()->getID());

        // got "desc::name::device"
        const char *inputPorts= sfi->getDescriptor()->getInputPorts();

        if (inputPorts == NULL)
        {
            addDesc(result, desc, first);
            first= false;
            free(desc);
            continue;
        }

        char **sources;
        int    numSources;
        char   aSourceDesc[512];

        // inputPorts comes in the form (Composite1,S-Video,Composite3)
        // so numSources is 1 more than real arguments
        sources= buildFormattedArgv(inputPorts, numSources);
        if (numSources == 1)
        {
            // inputPorts is in the form of (), meaning than
            // channel has no name
            sprintf(aSourceDesc, "%s;", desc);
            addDesc(result, aSourceDesc, first);
            first= false;
        }
        else
        {
            for (int j= 1; j < numSources; j++)
            {
                sprintf(aSourceDesc, "%s;%s", desc, sources[j]);
                addDesc(result, aSourceDesc, first);
                first= false;
            }
        }
        free(desc);
    }
    strcat(result,"}\n");

    return result;
}


const char *
video_InterfaceDef_t::video_select_source(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *srcDesc, *inputPort;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    srcDesc= argv[2];
    inputPort= strchr(srcDesc, ';');

    if (inputPort)
    {
        *inputPort= '\0';
        inputPort++;
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    bool res;

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            res= rc->setSource(srcDesc, inputPort);
        }
        break;
    default:
        sprintf(result, "ERROR: invalid channel type\n");
        return result;
    }

    if ( ! res)
    {
        sprintf(result, "ERROR: could not load source\n");
        return result;
    }
    else
    {
        return "OK\n";
    }
}


const char *
video_InterfaceDef_t::video_current_source(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    const char *srcDesc= NULL;
    const char *inputPort = NULL;

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            srcDesc= rc->getSourceDesc();

            if (srcDesc == NULL)
                return "ERROR: no source available\n";

            inputPort = rc->getInputPort();
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] not a source channel\n", chId);
        return result;
    }

    if (inputPort != NULL)
    {
        sprintf(result, "%s;%s\n", srcDesc, inputPort);
    }
    else
    {
        sprintf(result, "%s\n", srcDesc);
    }

    return result;
}


const char *
video_InterfaceDef_t::video_avatar(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    const char *filename= NULL;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    filename= argv[2];

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    image_t *img= loadFile(filename);

    if (img == NULL)
    {
        sprintf(result, "ERROR: could not load \"%s\"\n", filename);
        return result;
    }

    cr->setAvatarImage(img);

    delete img;

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_avatar_timeout(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int lapse;

    INTER_VRFY_ARGC(result, 3);

    NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    lapse= atoi(argv[2]);

    if (lapse < 0)
    {
        sprintf(result, "ERROR: lapse shoult be non-negative[%d]\n", lapse);
        return result;
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    cr->setAvatarTimeout(lapse);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_interactive_select(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    rc->interactiveSelect();

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_select_by_winid(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    u32 winId;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    winId= strtoul(argv[2], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    rc->selectByWinId(winId);

    return "OK\n";
}


//
// for REC_CHANNELS, we use frame rate (frames per second)
// for NET_CHANNELS, we use send frames (send 1 frame out of n)
//
const char *
video_InterfaceDef_t::video_frame_rate(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    float frameRate;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    frameRate= atof(argv[2]);

    if (frameRate <= 0)
    {
        sprintf(result, "ERROR: invalid frame rate [%f]\n", frameRate);
        return result;
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setFrameRate(frameRate);
        }
        break;
    default:
        sprintf(result, "ERROR: [%d] is neither a REC channel\n", chId);
        return result;
    }

    return "OK\n";
}


//
// for REC_CHANNELS, we use frame rate (frames per second)
// for NET_CHANNELS, we use send frames (send 1 frame out of n)
//
const char *
video_InterfaceDef_t::video_send_frames(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int n;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    n   = atoi(argv[2]);

    if (n <= 0)
    {
        sprintf(result, "ERROR: invalid ratio [%d]\n", n);
        return result;
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::NET_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a NET channel\n", chId);
        return result;
    }

    netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

    nc->setSendFrames(n);

    return "OK\n";
}


char const *
video_InterfaceDef_t::video_source_format(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    u32 fmt;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    fmt= vGetFormatIdByName(argv[2]);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    channel_t    *cp= static_cast<channel_t    *>(cr);
    recChannel_t *rc= static_cast<recChannel_t *>(cp);

    if ( ! rc->setSourceFormat(fmt) ) {
        return "ERROR: could not set format\n";
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_grab_size(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    const char *grabSize;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    grabSize= argv[2];

    if (strcmp("SQCIF", grabSize) == 0)
    {
        grabSize= "128x96";
    }
    else if (strcmp("QCIF", grabSize) == 0)
    {
        grabSize= "176x144";
    }
    else if (strcmp("CIF", grabSize) == 0)
    {
        grabSize= "352x288";
    }
    else if (strcmp("4CIF", grabSize) == 0)
    {
        grabSize= "704x576";
    }
    else if (strcmp("16CIF", grabSize) == 0)
    {
        grabSize= "1408x1152";
    }

    unsigned w, h;

    if (sscanf(grabSize, "%ux%u", &w, &h) != 2)
    {
        return "ERROR: grab size expected WxH\n";
    }

    if ((w == 0) || (h == 0))
    {
        return "ERROR: grab size cannot be 0";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    rc->setGrabSize(w, h);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_list_norms(int argc, char **argv)
{
    static char result[4096];

    u32   chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    std::string normsList= rc->getVideoStandardList();

    if ( normsList.empty() )
    {
        return "ERROR: no video norms available\n";
    }

    sprintf(result, "{%s}\n", normsList.c_str());
    return result;
}


const char *
video_InterfaceDef_t::video_set_norm(int argc, char **argv)
{
    static char result[4096];

    u32   chId;
    char *vnorm;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    vnorm= argv[2];

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    if ( ! rc->setVideoStandard(vnorm))
    {
        sprintf(result, "ERROR: cannot set video standard [%s]\n", vnorm);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_norm(int argc, char **argv)
{
    static char result[4096];

    u32   chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    std::string vnorm= rc->getVideoStandard();

    if ( vnorm.empty() )
    {
        return "ERROR: cannot get video norm\n";
    }

    sprintf(result, "%s\n", vnorm.c_str());
    return result;
}


const char *
video_InterfaceDef_t::video_list_flickerfreqs(int argc, char **argv)
{
    static char result[4096];

    u32   chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    std::string flickerfreqsList= rc->getFlickerFreqsList();

    if ( flickerfreqsList.empty() )
    {
        return "ERROR: no frequencies available\n";
    }

    sprintf(result, "{%s}\n", flickerfreqsList.c_str());
    return result;
}


const char *
video_InterfaceDef_t::video_set_flickerfreq(int argc, char **argv)
{
    static char result[4096];

    u32   chId;
    char *vflickerfreq;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    vflickerfreq= argv[2];

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    if ( ! rc->setFlickerFreq(vflickerfreq))
    {
        sprintf(result, "ERROR: cannot set frequency [%s]\n", vflickerfreq);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_flickerfreq(int argc, char **argv)
{
    static char result[4096];

    u32   chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId= strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }
    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    std::string vflickerfreq= rc->getFlickerFreq();

    if ( vflickerfreq.empty() )
    {
        return "ERROR: cannot get frequency\n";
    }

    sprintf(result, "%s\n", vflickerfreq.c_str());
    return result;
}


const char *
video_InterfaceDef_t::video_set_saturation(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int value;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    value= atoi(argv[2]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setVideoSaturation(value);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_saturation(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    int value;

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            value= rc->getVideoSaturation();
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    if (value == -1)
    {
        return "ERROR: cannot get Saturation\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


const char *
video_InterfaceDef_t::video_set_brightness(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int value;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    value= atoi(argv[2]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setVideoBrightness(value);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_brightness(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    int value;

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            value= rc->getVideoBrightness();
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    if (value == -1)
    {
        return "ERROR: cannot get Brightness\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


const char *
video_InterfaceDef_t::video_set_hue(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int value;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    value= atoi(argv[2]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setVideoHue(value);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_hue(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    int value;

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            value= rc->getVideoHue();
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    if (value == -1)
    {
        return "ERROR: cannot get Hue\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}


const char *
video_InterfaceDef_t::video_set_contrast(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int value;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    value= atoi(argv[2]);

    // range [0..100] for user convenience
    if (value < 0 || value > 100)
    {
        return "ERROR: invalid value, must be an integer between 0 and 100\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setVideoContrast(value);
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_contrast(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    int value;

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            value= rc->getVideoContrast();
        }
        break;
    default:
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    if (value == -1)
    {
        return "ERROR: cannot get Contrast\n";
    }

    sprintf(result, "%d\n", value);
    return result;
}

const char *
video_InterfaceDef_t::video_set_flip(int argc, char **argv)
{
    static char result[4096];

    u32  chId;
    bool flip;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    flip = atoi(argv[2]) != 0;

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    rc->setFlipMode(flip);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_flip(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    bool value;

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
            sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
            return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    value= rc->getFlipMode();

    sprintf(result, "%d\n", value ? 1 : 0);
    return result;
}


const char *
video_InterfaceDef_t::video_set_mirror(int argc, char **argv)
{
    static char result[4096];

    u32  chId;
    bool mirror;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    mirror = atoi(argv[2]) != 0;

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    rc->setMirrorMode(mirror);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_mirror(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    bool value;

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
            sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
            return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    value= rc->getMirrorMode();

    sprintf(result, "%d\n", value ? 1 : 0);
    return result;
}


const char *
video_InterfaceDef_t::video_crop(int argc, char **argv)
{
    static char result[4096];

    u32  chId;
    double top, left, bottom, right;

    INTER_VRFY_ARGC(result, 6);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId  = strtoul(argv[1], NULL, 10);
    top   = atof(argv[2]);
    left  = atof(argv[3]);
    bottom= atof(argv[4]);
    right = atof(argv[5]);

    if (    (top < 0.0)    || (top > 100.00)
         || (left < 0.0)   || (left > 100.00)
         || (bottom < 0.0) || (bottom > 100.00)
         || (right < 0.0)  || (right > 100.00)
       )
    {
        return "ERROR: crop values must be with [0.0..100.0]\n";
    }

    if (top + bottom > 100.0)
    {
        return "ERROR: top + bottom > 100.0\n";
    }

    if (left + right > 100.0)
    {
        return "ERROR: left + right > 100.0\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
        sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
        return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    rc->setCrop(top, left, bottom, right);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_get_crop(int argc, char **argv)
{
    static char result[4096];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    //NOTIFY("%s(%s)\n", argv[0], argv[1]);

    chId = strtoul(argv[1], NULL, 10);

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    if (cr->getKind() != channel_t::REC_CHANNEL)
    {
            sprintf(result, "ERROR: channel [%d] is not a REC channel\n", chId);
            return result;
    }

    recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

    double top, left, bottom, right;

    rc->getCrop(top, left, bottom, right);

    sprintf(result, "%3.2f %3.2f %3.2f %3.2f\n", top, left, bottom, right);

    return result;
}


//
// codecs
//
const char *
video_InterfaceDef_t::video_list_codecs(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    int numCoders= vGetNumRegisteredCoders();
    u32 *Fmts= new u32[numCoders];

    int numFmts= vGetCodecFmtList(Fmts, numCoders);

    if (numFmts < 0)
    {
        delete []Fmts;
        return "ERROR: number of codecs below zero\n";
    }

    if (numFmts == 0)
    {
        delete []Fmts;
        return "ERROR: no codecs registered!\n";
    }

    std::string codecs;

    codecs += "{";
    for (int i= 0; i < numFmts; i++)
    {
        u32 fmt= Fmts[i];


        codecs += vGetFormatNameById(fmt);

        if (i < numFmts - 1)   // more to add
        {
            codecs += ",";
        }
    }
    codecs += "}\n";

    delete []Fmts;

    sprintf(result, "%s", codecs.c_str());

    return result;
}


const char *
video_InterfaceDef_t::video_select_codec(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    u32 lineFmt;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    try
    {
        lineFmt= vGetFormatIdByName(argv[2]);
    }
    catch (const char *e)
    {
        return "ERROR: unknown format name\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return false;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setLineFmt(lineFmt);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->setLineFmt(lineFmt);
        }
        break;
    default:
        sprintf(result, "ERROR: [%d] is a PLAY channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_codec_quality(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int quality;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    quality= atoi(argv[2]);

    if (quality < 1 || quality > 100)
    {
        return "ERROR: invalid quality, must be an integer between 1 and 100\n";
    }

    channel_t *cr= APP->channelMgr->getChannel(chId);
    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setCodecQuality(quality);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->setCodecQuality(quality);
        }
        break;
    default:
        sprintf(result, "ERROR: [%d] is a PLAY channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_set_h263_mode(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    char *factorStr;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId= strtoul(argv[1], NULL, 10);
    factorStr= argv[2];

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setH263Mode(factorStr);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->setH263Mode(factorStr);
        }
        break;
    default:
        sprintf(result, "ERROR: [%d] is a PLAY channel\n", chId);
        return result;
    }

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_set_mpeg_pbetweeni(int argc, char **argv)
{
    static char result[4096];

    u32 chId;
    int pbeti;

    INTER_VRFY_ARGC(result, 3);

    //NOTIFY("%s(%s,%s)\n", argv[0], argv[1], argv[2]);

    chId = strtoul(argv[1], NULL, 10);
    pbeti= atoi(argv[2])+1;

    channel_t *cr= APP->channelMgr->getChannel(chId);

    if (cr == NULL)
    {
        sprintf(result, "ERROR: channel [%d] does not exist\n", chId);
        return result;
    }

    switch (cr->getKind())
    {
    case channel_t::REC_CHANNEL:
        {
            recChannel_t *rc= dynamic_cast<recChannel_t *>(cr);

            rc->setMpegPBetweenI(pbeti);
        }
        break;
    case channel_t::NET_CHANNEL:
        {
            netChannel_t *nc= dynamic_cast<netChannel_t *>(cr);

            nc->setMpegPBetweenI(pbeti);
        }
        break;
    default:
        sprintf(result, "ERROR: [%d] is not a PLAY channel\n", chId);
        return result;
    }


    return "OK\n";
}


//
// common stuff
//
const char *
video_InterfaceDef_t::video_query_id(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    sprintf(result, "0x%x\n", interID);
    return result;
}


const char *
video_InterfaceDef_t::video_nop(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    return "OK\n";
}


const char *
video_InterfaceDef_t::video_bye(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    (*APP)-(dynamic_cast<videoInterface_t*>(this));
    return (const char *)-1;
}


const char *
video_InterfaceDef_t::video_quit(int argc, char **argv)
{
    static char result[4096];

    INTER_VRFY_ARGC(result, 1);

    //NOTIFY("%s()\n", argv[0]);

    APP->channelMgr->deleteAllChannels();
    APP->shutdown();

    rtpSession = NULL;
    return (const char *)-1;
}


