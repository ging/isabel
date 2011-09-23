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
#ifndef __SOUNDDEVICE_H__
#define __SOUNDDEVICE_H__

class SoundDevice_t
{
public:

    static const int SAMPLE_RATE = 48000;
    static const int BPS = 2;
    static const int REC_BUF_SIZE = SAMPLE_RATE * BPS; // 1 segundo
    static const int PLAY_BUF_SIZE = SAMPLE_RATE * 2 * BPS; // 2 segundos

    /**
    * Inicia el proceso de captura
    */
    virtual void startCapture(void)= 0;

    /**
    * Detiene el proceso de captura
    */
    virtual void stopCapture(void)= 0;

    /**
    * Inicia el proceso de reproduccion
    */
    virtual void startPlaying(void)= 0;

    /**
    * Detiene el proceso de reproduccion
    */
    virtual void stopPlaying(void)= 0;

    /**
    * Resetea los dispositivos de audio.
    */
    virtual void resetDevices(void)= 0;

    /**
    * Copia datos del buffer de captura en Buffer
    * @param pBuffer buffer de salida.
    * @param length Longitud del buffer de salida.
    * @return Numero de bytes leidos.
    */
    virtual int read(unsigned char *pBuffer, int length)= 0;

    /**
    * Escribe datos en el buffer de reproduccion.
    * @param pBuffer datos que se quieren mandar a reproducir.
    * @param lenght Longitud del buffer.
    * @return True en caso de exito y false en caso de error.
    */
    virtual bool write(const unsigned char *pBuffer, int lenght)= 0;

    /**
    * Establece un nuevo dispositivo de captura.
    * @param device String that identifies the sound capture device.
    * @return True en caso de exito y false en caso de error.
    */
    virtual bool setCaptureDevice(const char *device)= 0;

    /**
    * Establece un nuevo dispositivo de reproduccion.
    * @param device String that identifies the sound render device.
    * @return True en caso de exito y false en caso de error.
    */
    virtual bool setPlayDevice(const char *device)= 0;
};

#endif
