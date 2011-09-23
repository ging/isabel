/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __SDL_SOUNDDEVICE_H__
#define __SDL_SOUNDDEVICE_H__

#include <SDL/SDL.h>

#include "../common/ringBuffer.h"
#include "../common/sounddevice.h"

class SDLPlaySoundDevice_t: public virtual PlaySoundDevice_t
{
private:
    RingBuffer_t ringBuffer;

    friend void sdl_audio_callback(void *opaque, Uint8 *stream, int len);

public:

    SDLPlaySoundDevice_t(void);

    virtual ~SDLPlaySoundDevice_t(void);

    /**
    * Inicia el proceso de reproduccion
    */
    virtual void startPlaying(void);

    /**
    * Detiene el proceso de reproduccion
    */
    virtual void stopPlaying(void);

    /**
    * Escribe datos en el buffer de reproduccion.
    * @param pBuffer datos que se quieren mandar a reproducir.
    * @param lenght Longitud del buffer.
    * @return True en casod e exito y falsde en caso de error.
    */
    virtual bool write(unsigned char *pBuffer, int lenght);

    /**
    * Establece un nuevo dispositivo de reproduccion.
    * @param device String that identifies the sound render device.
    * @return True en caso de exito y false en caso de error.
    */
    virtual bool setPlayDevice(const char *device);
};

#endif
