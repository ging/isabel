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
#ifndef __PA_SOUNDDEVICE_H__
#define __PA_SOUNDDEVICE_H__

#include <portaudio.h>

#include "../common/ringBuffer.h"
#include "../common/sounddevice.h"

#define USE_PLAY_CALLBACK 1
#define USE_REC_CALLBACK 1

class PASoundDevice_t: public virtual SoundDevice_t
{
private:
    //
    // PLAYER
    //
    PaStream *playStream;
    PaStreamParameters playParameters;

    //
    // RECORDER
    //
    PaStream *recStream;
    PaStreamParameters recParameters;

#if USE_PLAY_CALLBACK
    RingBuffer_t playRingBuffer;

    friend int paPlayCallback(const void *inputBuffer,
                              void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData
                             );
#endif

#if USE_REC_CALLBACK
    int accum; // accumulated samples, for sleeping properly

    RingBuffer_t recRingBuffer;

    friend int paRecCallback(const void *inputBuffer,
                             void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData
                            );
#endif

public:

    PASoundDevice_t(void);

    virtual ~PASoundDevice_t(void);

    /**
    * Inicia el proceso de reproduccion
    */
    void startCapture(void);

    /**
    * Detiene el proceso de reproduccion
    */
    void stopCapture(void);

    /**
    * Inicia el proceso de reproduccion
    */
    void startPlaying(void);

    /**
    * Detiene el proceso de reproduccion
    */
    void stopPlaying(void);

    /**
    * Resetea los dispositivos de audio.
    */
    void resetDevices(void);

    /**
    * Copia datos del buffer de captura en Buffer
    * @param pBuffer buffer de salida.
    * @param length Longitud del buffer de salida.
    * @return Numero de bytes leidos.
    */
    int read(unsigned char *pBuffer, int length);

    /**
    * Escribe datos en el buffer de reproduccion.
    * @param pBuffer datos que se quieren mandar a reproducir.
    * @param lenght Longitud del buffer.
    * @return True en casod e exito y falsde en caso de error.
    */
    bool write(const unsigned char *pBuffer, int lenght);

    /**
    * Establece un nuevo dispositivo de reproduccion.
    * @param device String that identifies the sound render device.
    * @return True en caso de exito y false en caso de error.
    */
    bool setPlayDevice(const char *device);

    /**
    * Establece un nuevo dispositivo de captura.
    * @param device String that identifies the sound capture device.
    * @return True en caso de exito y false en caso de error.
    */
    bool setCaptureDevice(const char *device);
};

#endif
