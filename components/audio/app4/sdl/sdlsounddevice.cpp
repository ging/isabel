////////////////////////////////////////////////////////////////////////
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
#include <stdlib.h>
#include <limits.h>
#include <icf2/notify.hh>

#include "../sdl/sdlsounddevice.h"

static bool SDLinitiated= false;

static bool
initSDL(void)
{
    if (SDLinitiated)
    {
        return true;
    }

    if ( SDL_Init(SDL_INIT_AUDIO) == -1)
    {
        return false;
    }

    SDLinitiated= true;

    return true;
}

static const int SDL_AUDIO_BUFFER_SIZE= 1024;

/* prepare a new audio buffer */
void
sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
    SDLPlaySoundDevice_t *ssd= (SDLPlaySoundDevice_t*)opaque;

    if (len > 0)
    {
        u8 data[len];
        int ret= ssd->ringBuffer.Dequeue((i16*)data, len / 2, false) * 2;

        memcpy(stream, data, ret);
    }
}


// -----------------------------------------------------------------------------
// SDLPlaySoundDevice_t::SDLPlaySoundDevice_t
// Constructor de la clase. Inicializa los dispositivos
// -----------------------------------------------------------------------------
//
SDLPlaySoundDevice_t::SDLPlaySoundDevice_t(void)
: ringBuffer(48000, 24000) // 1 sec, seems excesive
{
    SDL_AudioSpec wanted_spec, spec;

    initSDL();

    wanted_spec.freq= 48000;
    wanted_spec.format= AUDIO_S16SYS;
    wanted_spec.channels= 1;
    wanted_spec.silence= 0;
    wanted_spec.samples= SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.callback= sdl_audio_callback;
    wanted_spec.userdata= this;

    if (SDL_OpenAudio(&wanted_spec, &spec) < 0)
    {
        NOTIFY("SDL_OpenAudio: %s\n", SDL_GetError());
        throw "NO SDL Audio";
    }

    SDL_PauseAudio(0);
}

// -----------------------------------------------------------------------------
// SDLPlaySoundDevice_t::~SDLPlaySoundDevice_t
// Destructor de la clase. Libera los recursos de directsound
// -----------------------------------------------------------------------------
//
SDLPlaySoundDevice_t::~SDLPlaySoundDevice_t(void)
{
    SDL_CloseAudio();
}


// -----------------------------------------------------------------------------
// SDLPlaySoundDevice_t::startPlaying
// 
// -----------------------------------------------------------------------------
//
void
SDLPlaySoundDevice_t::startPlaying(void)
{
    NOTIFY("SDLPlaySoundDevice_t::startPlaying\n");
}

// -----------------------------------------------------------------------------
// SDLPlaySoundDevice_t::stopPlaying
// 
// -----------------------------------------------------------------------------
//
void
SDLPlaySoundDevice_t::stopPlaying(void)
{
    NOTIFY("SDLPlaySoundDevice_t::stopPlaying\n");
}

// -----------------------------------------------------------------------------
// SDLPlaySoundDevice_t::write
// 
// -----------------------------------------------------------------------------
//
bool
SDLPlaySoundDevice_t::write(unsigned char *pBuffer, int length)
{
    return ringBuffer.Enqueue((i16*)pBuffer, length / 2) * 2;
}

// -----------------------------------------------------------------------------
// SDLPlaySoundDevice_t::setPlayDevice
// 
// -----------------------------------------------------------------------------
//
bool
SDLPlaySoundDevice_t::setPlayDevice(const char *device)
{
    return true;
}

