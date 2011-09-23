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

#include <aCodecs/codecs.h>

#include <icf2/notify.hh>

#include <rtp/RTPPayloads.hh>

#include "ctrlprotocol.h"
#include "audioapp.h"
#include "channelmgr.h"
#include "recchannel.h"

#define APP static_cast<AudioApp*>(static_cast<AudioInterface*>(this)->get_owner())

binding_t<AudioInterfaceDef> audioMethodBinding[]=
{
    {"audio_nop",               &AudioInterfaceDef::audio_nop},
    {"audio_ready",             &AudioInterfaceDef::audio_nop},
    {"audio_bye",               &AudioInterfaceDef::audio_bye},
    {"audio_quit",              &AudioInterfaceDef::audio_quit},

    {"audio_assign_channel",    &AudioInterfaceDef::audio_assign_channel},
    {"audio_new_channel",       &AudioInterfaceDef::audio_new_channel},
    {"audio_delete_channel",    &AudioInterfaceDef::audio_delete_channel},

    {"audio_set_silence_level", &AudioInterfaceDef::audio_set_threshold},
    {"audio_set_input_gain",    &AudioInterfaceDef::audio_set_input_gain},

    {"audio_bind",              &AudioInterfaceDef::audio_bind},
    {"audio_unbind",            &AudioInterfaceDef::audio_unbind},

    {"audio_start_sending",     &AudioInterfaceDef::audio_start_sending},
    {"audio_stop_sending",      &AudioInterfaceDef::audio_stop_sending},

    {"audio_set_codec",         &AudioInterfaceDef::audio_set_codec},
    {"audio_query_codecs",      &AudioInterfaceDef::audio_query_codecs},
    {"audio_get_codec_bw",      &AudioInterfaceDef::audio_get_codec_bw},

    {"audio_play_file",         &AudioInterfaceDef::audio_play_output_file},
    {"audio_stop_file",         &AudioInterfaceDef::audio_stop_output_file},
    {"audio_send_file",         &AudioInterfaceDef::audio_play_input_file},
    {"audio_stop_send_file",    &AudioInterfaceDef::audio_stop_input_file},

    {"audio_tone_generator",        &AudioInterfaceDef::audio_tone_generator},
    {"audio_local_tone_generator",  &AudioInterfaceDef::audio_local_tone_generator},

    {"audio_set_echo_suppresor",    &AudioInterfaceDef::audio_set_echo_suppressor},
    {"audio_set_echo_window",       &AudioInterfaceDef::audio_set_echo_window},
    {"audio_set_echo_threshold",    &AudioInterfaceDef::audio_set_echo_threshold},

    {"audio_set_cag",               &AudioInterfaceDef::audio_set_cag},
    {"audio_set_cag_window",        &AudioInterfaceDef::audio_set_cag_window},
    {"audio_set_cag_threshold",     &AudioInterfaceDef::audio_set_cag_threshold},


    {"audio_do_stats",              &AudioInterfaceDef::audio_do_stats},
    {"audio_bind_mix",              &AudioInterfaceDef::audio_bind_mix},

    {"audio_capture_device",        &AudioInterfaceDef::audio_capture_device},
    {"audio_play_device",           &AudioInterfaceDef::audio_play_device},
    {"audio_reset_devices",         &AudioInterfaceDef::audio_reset_devices},

    {"audio_set_buffering",         &AudioInterfaceDef::audio_set_buffering},

    {NULL,                    NULL}
};


// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_nop 
// 
// -----------------------------------------------------------------------------
//
char const * 
AudioInterfaceDef::audio_nop (int argc, char **argv)
{
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_bye 
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_bye (int argc, char **argv)
{
    (*APP)-(static_cast<AudioInterface*>(this));
    return reinterpret_cast<char *>(-1);
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_quit
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_quit (int argc, char **argv)
{
    APP->shutdown();
    return reinterpret_cast<char *>(-1);
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_assign_channel
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_assign_channel (int argc, char **argv)
{
    static char result[512];

    u32 chId;
    INTER_VRFY_ARGC(result, 2);

    chId = strtoul(argv[1], NULL, 10);

    if (chId <= 0)
    {
        return "ERROR: bad channel number\n";
    }

    if ( ! APP->channelMgr->newRecChannel(chId))
    {
        return "ERROR: Unable to create REC channel\n";
    }

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_new_channel
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_new_channel (int argc, char **argv)
{
    static char result[512];

    u32 chId;
    INTER_VRFY_ARGC(result, 2);

    chId = strtoul(argv[1], NULL, 10);

    if (chId <= 0) {
        return "ERROR: bad channel number\n";
    }

    if( ! APP->channelMgr->newPlayChannel(chId)) {
        return "ERROR: channel already in use\n";
    }

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_bind 
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_bind(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 3);

    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    int bindId= rc->bind(argv[1], argv[2]);
    if (bindId > 0)
    {
        sprintf(result, "%d\n", bindId);
        return result;
    }

    return "ERROR: could not bind, bad address?\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_unbind
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_unbind (int argc, char **argv)
{
    static char result[512];

    int bindId;
    INTER_VRFY_ARGC(result, 2);

    bindId = strtoul(argv[1], NULL, 10);

    RecChannel *rc= APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    if ( ! rc->unbind(bindId) )
    {
        return "ERROR: could not delete bind\n";
    }

    return "OK\n";
}


// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_delete_channel
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_delete_channel(int argc, char **argv)
{
    static char result[512];

    u32 chId;

    INTER_VRFY_ARGC(result, 2);

    chId = strtoul(argv[1], NULL, 10);

    if ( ! APP->channelMgr->deleteChannel(chId))
    {
        return "ERROR: bad channel\n";
    }

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_start_sending 
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_start_sending(int argc, char **argv)
{
    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    rc->start();

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_stop_sending
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_stop_sending(int argc, char **argv)
{
    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    rc->stop();

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_play_output_file
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_play_output_file(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    if ( ! APP->channelMgr->playOutputFile(argv[1]))
    {
        return "ERROR: Bad file name\n";
    }

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_stop_output_file
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_stop_output_file(int argc, char **argv)
{
    APP->channelMgr->stopOutputFile();

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_play_input_file
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_play_input_file(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    if ( ! rc->playFile(argv[1]))
    {
        return "ERROR: Bad file name\n";
    }

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_stop_input_file
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_stop_input_file(int argc, char **argv)
{
    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    rc->stopFile();

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_tone_generator
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_tone_generator(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    bool active = atoi(argv[1]) != 0;

    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No RECchannel\n";
    }

    rc->sendTone(active);

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_local_tone_generator
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_local_tone_generator(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    const char *file = "/usr/local/isabel/test/audio_test.mp3";

    int active = atoi(argv[1]);

    if (active)
    {
        if (APP->channelMgr->playOutputFile(file))
        {
            return "OK\n";
        }
        else
        {
            return "ERROR: Bad file name\n";
        }
    }
    else
    {
        APP->channelMgr->stopOutputFile();
        return "OK\n";
    }
}


// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_codec
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_codec(int argc, char **argv)
{
    static char result[512];

    INTER_VRFY_ARGC(result, 2);

    int fmt= aGetFormatIdByName(argv[1]);

    if (fmt < 0)
    {
        return "ERROR: Unknown codec descriptor\n";
    }

    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    if ( ! rc->setCoder(fmt))
    {
        return "ERROR: No such codec\n";
    }

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_query_codecs
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_query_codecs (int argc, char **argv)
{
    static char result[1024];

    aGetCoderList(result, 1024);

    return result;
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_get_codec_bw
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_get_codec_bw(int argc, char **argv)
{
    static char result[512];

    INTER_VRFY_ARGC(result, 2);

    int format= aGetFormatIdByName(argv[1]);

    if (format == -1)
    {
        return "ERROR: no such audio format\n";
    }

    sprintf(result, "%d\n", aGetBandwidthById(format));

    return result;
}


// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_threshold
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_threshold(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level > 0 || level < -50)
    {
        return "ERROR: invalid argument\n";
    }

    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    rc->setSilenceThreshold(level);

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_input_gain
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_input_gain(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int gain = atoi(argv[1]);
    if (gain < -50 || gain > 50) {
        return "ERROR: invalid argument\n";
    }

    float dbs = gain/1.0f;


    RecChannel *rc = APP->channelMgr->getRecChannel();

    if (rc == NULL)
    {
        return "ERROR: No REC channel\n";
    }

    rc->setGain(dbs);

    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_echo_suppressor
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_echo_suppressor(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level < 0 || level > 100) {
        return "ERROR: invalid argument\n";
    }

    SoundUtils::EchoSuppressor::level = level;
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_echo_threshold
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_echo_threshold(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level > 0 || level < -50) {
        return "ERROR: invalid argument\n";
    }

    SoundUtils::EchoSuppressor::echo_threshold = level;
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_echo_window
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_echo_window(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level < 2 || level > SoundUtils::EchoSuppressor::MAX_ECHO_WINDOW) {
        return "ERROR: invalid argument\n";
    }

    SoundUtils::EchoSuppressor::echo_window = level;
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_cag
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_cag(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level < 0 || level > 100) {
        return "ERROR: invalid argument\n";
    }

    SoundUtils::CAG::level = level;
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_cag_threshold
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_cag_threshold(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level > 0 || level < -50) {
        return "ERROR: invalid argument\n";
    }

    SoundUtils::CAG::cag_threshold = level;
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_cag_window
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_cag_window(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int level = atoi(argv[1]);
    if (level < 2 || level > SoundUtils::CAG::MAX_CAG_WINDOW) {
        return "ERROR: invalid argument\n";
    }

    SoundUtils::CAG::cag_window = level;
    return "OK\n";
}


// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_do_stats
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_do_stats(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int active = atoi(argv[1]);
    if (active) {
        APP->doStats = true;

    }
    else {
        APP->doStats = false;
    }
    return "OK\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_bind_mix 
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_bind_mix (int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 3);

    int bindId = APP->mixer->bind(argv[1], argv[2]);
    if (bindId > 0)
    {
        sprintf(result, "%d\n", bindId);
        return result;
    }
    return "ERROR: Bad address\n";
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_capture_device
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_capture_device(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    if ( ! APP->soundDevice->setCaptureDevice(argv[1]))
    {
        return "ERROR: Bad device\n";
    }
    else
    {       
        return "OK\n";
    }
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_play_device
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_play_device(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    if ( ! APP->soundDevice->setPlayDevice(argv[1]))
    {
        return "ERROR: Bad device\n";
    }
    else
    {       
        return "OK\n";
    }
}

// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_reset_devices
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_reset_devices(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 1);

    APP->soundDevice->resetDevices();

    return "OK\n";
}


// -----------------------------------------------------------------------------
// AudioInterfaceDef::audio_set_buffering
// 
// -----------------------------------------------------------------------------
//
char const *
AudioInterfaceDef::audio_set_buffering(int argc, char **argv)
{
    static char result[512];
    INTER_VRFY_ARGC(result, 2);

    int buf = atoi(argv[1]);
    
	if ( ! APP->mixer->setMaxDelay(buf))
    {
        return "ERROR: invalid argument\n";
    }
    else
    {       
        return "OK\n";
    }

}
