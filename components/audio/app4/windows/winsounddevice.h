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
#ifndef __WIN_SOUNDDEVICE_H__
#define __WIN_SOUNDDEVICE_H__

#include "sounddevice.h"

#include <tchar.h>
#include <Dsound.h>

class WinSoundDevice_t: public SoundDevice_t
{
public:

    /**
    * Constructor de la clase.
    * @param renderDevice GUID that identifies the sound render device
    * @param captureDevice GUID that identifies the sound capture device
    */
    WinSoundDevice_t(void *renderDevice, void *captureDevice);

    /**
    * Destructor de la clase.
    */
    virtual ~WinSoundDevice_t(void);

    /**
    * Inicializa los buferes de captura y reproduccion.
    * @param renderDevice GUID that identifies the sound render device.
    * @param captureDevice GUID that identifies the sound capture device.
    * @param aec Activate Acoustic echo cancellation.
    */
    bool SoundDeviceInit(void *renderDevice, void *captureDevice, bool aec);

public:

    /**
    * Inicia el proceso de captura
    */
    void startCapture(void);

    /**
    * Detiene el proceso de captura
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
    * Copia datos del buffer de captura en pBuffer
    * @param pBuffer direccion del buffer de salida.
    * @param length Longitud del buffer de salida.
    * @return Numero de bytes leidos.
    */
    int read(unsigned char *pBuffer, int length);

    /**
    * Escribe datos en el buffer de reproduccion.
    * @param pBuffer Datos que se quieren mandar a reproducir.
    * @param lenght Longitud del buffer.
    * @return True en caso de exito y false en caso de error.
    */
    bool write(const unsigned char *pBuffer, int lenght);

    /**
     * Establece un nuevo dispositivo de captura.
     * @param device GUID that identifies the sound capture device.
     * @return True en caso de exito y false en caso de error.
     */
    bool setCaptureDevice(const char *device);

    /**
     * Establece un nuevo dispositivo de reproduccion.
     * @param device GUID that identifies the sound render device.
     * @return True en caso de exito y false en caso de error.
     */
    bool setPlayDevice(const char *device);

private:

    /**
    * Interfaz con el dispositivo fullduplex.
    */
    LPDIRECTSOUNDFULLDUPLEX pDSFullDuplex;

    /**
    * Buffer de captura.
    */
    LPDIRECTSOUNDCAPTUREBUFFER8 pDSCaptureBuffer;

    /**
    * Buffer de escritura.
    */
    LPDIRECTSOUNDBUFFER8 pDSBuffer;

};

#endif
