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

#ifndef __AUDIO_APP_H__
#define __AUDIO_APP_H__

#include <icf2/stdTask.hh>
#include <icf2/task.hh>

#include "sounddevice.h"

class MixerBuffer;
class udpSound;
class ChannelManager;

/**
* Audio Application Scheduler.
*/
class AudioApp: public application_t
{
public:
    /**
    * Tiempo entre envios de audio.
    */
    static const int HB_TIME = 60000;

    /**
    * Numero de ciclos entre cada envio de estadisticas.
    */
    static const int STATS_TIME = 1000000/HB_TIME;

    /**
    * Auto create play channels when receiving audio (MARTE mode)
    * Default = false
    */
    bool autoCreate;

    /**
    * Indica si se envian o no estadisticas.
    */
    bool doStats;

private:

    /**
    * Tamaño del buffer de recepcion de paquetes
    */
    static const int BUFFER_SIZE = 2048;

    /**
    * Socket de control.
    */
    streamSocket_t *ctrlSocket;

    /**
    * Socket de datos rtp.
    */
    dgramSocket_t *rtpSocket;

    /**
    * Socket de datos udp.
    */
    dgramSocket_t *udpSocket;

    /**
    * Socket de vumetros.
    */
    //dgramSocket_t *vuSocket;


public:

    /**
    * Constructor de la clase.
    */
    AudioApp (int &argc, argv_t &argv);

    /**
    * Destructor virtual.
    */
    virtual ~AudioApp(void);

    // ICF Debug
    virtual const char *className(void) const { return "AudioApp"; };

public:

    /**
    * Gestor de canales de audio.
    */
    ChannelManager *channelMgr;

    /**
    * Buffer para mezclar los distintos canales de audio.
    */
    MixerBuffer *mixer;

    /**
    * Interfaz con el dispositivo de captura y reproduccion.
    */
    SoundDevice_t *soundDevice;

private:

    class AudioTask;
    friend class AudioTask;

    /**
    * Tarea principal de la aplicacion. Tiene un evento cada cierto tiempo
    * que le pasa al canal de envio y cuando recibe paquetes se los pasa
    * a los canales de reproduccion.
    */
    class AudioTask : public simpleTask_t
    {
    private:
        /**
        * Periodo entre eventos.
        */
        int period;

    public:

        /**
        * Constructor de la tarea
        * @param theApp Puntero al objeto application.
        * @param n Periodo entre eventos.
        */
        AudioTask(int n)
        : period(n)
        {
            set_period(period);
        };

        /**
        * Metodo que se ejecuta periodicamente.
        * Se encarga de enviar el audio capturado.
        */
        virtual void heartBeat(void);

        /**
        * Metodo que se ejecuta cuando se recibe
        * un paquete por el puerto de datos
        */
        virtual void IOReady(io_ref& io);

        virtual const char *className(void) const { return "AudioTask"; }
    };

};

#endif // __AUDIO_APP_H__
