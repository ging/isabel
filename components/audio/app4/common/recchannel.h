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

#ifndef __RECCHANNEL_H__
#define __RECCHANNEL_H__

#include <icf2/sched.hh>
#include <icf2/sockIO.hh>

#include "channel.h"
#include "toneGenerator.h"
#include "udpsound.h"
#include "sounddevice.h"
#include "fileReader.h"

// FORWARD DECLARATIONS
class Vumeter;
class RTPSink;

// CLASS DECLARATION

/**
* Clase que representa un canal de captura
*/
class RecChannel: public Channel
{
public:

    static const int PREAMP = 0;

    /**
    * Constructor de la clase.
    * @param chId Identificador del canal.
    * @param device dispositivo de donde se leen datos.
    * @param sock Socket donde se escriben loa datos.
    * @param vsock Socket por donde se escriben los vumetros.
    * @param vport Puerto donde se envia el flujo de vumetros.
    * @param vplayport Puerto donde se envia el flujo de vumetros.
    * @param sport Puerto donde se envian las estadisticas.
    */
    RecChannel(u32            chId,
               SoundDevice_t *device,
               UdpSound      *udpsound,
               dgramSocket_t *sock,
               dgramSocket_t *vsock,
               const char    *vport,
               const char    *vplayport,
               const char    *sport
              );

    /**
    * Destructor virtual.
    */
    virtual ~RecChannel(void);

    /**
    * Realiza un bind en el canal.
    * @param host Nombre o IP destino.
    * @param port Numero de puerto destino.
    * @return Identificador del binding.
    */
    int bind(char *host, char *port);

    /**
    * Destruye un binding.
    * @param Identificador del binding.
    * @return True en caso de exito, false en caso contrario.
    */
    bool unbind(int bindId);

    /**
    * Inicia la reproduccion de este canal.
    * @return True en caso de exito, false en caso contrario.
    */
    virtual bool start(void);

    /**
    * Detiene la reproduccion de este canal.
    * @return True en caso de exito, false en caso contrario.
    */
    virtual bool stop(void);

    /**
    * Envia/Deja de enviar un tono de prueba
    * @param active si true, envia el tono, si false deja de enviarlo
    */
    void sendTone(bool activate);

    /**
    * Sustituye la fuente del canal por un fichero.
    * @param name nombre del fichero.
    * @return True en caso de exito, false en caso contrario.
    */
    bool playFile(const char *name);

    /**
    * Detiene la reproduccion del fichero por el canal. Volviendo a
    * la fuente original.
    */
    void stopFile(void);

    /**
    * Establece el codificador del canal.
    * @param format Identificador del codificador.
    * @return True en caso de exito, false en caso contrario.
    */
    bool setCoder(int format);

    /**
    * Establece el limite de potencia que se considera silencio
    * @param limit Nuevo umbral de silencio. Debe ser un numero entre 0 y -50.
    */
    void setSilenceThreshold(double limit);

    /**
    * Establece la ganacia del canal.
    * @param g La nueva ganacia. Un valor en DB entre -10 y 10;
    */
    void setGain(float g);

    /**
    * Metodo que se ejecuta periodicamente en el canal
    * Lee datos de la fuente y los envia por el socket.
    */
    virtual void heartBeat(void);

    // ICF debug
    virtual const char *className(void) const { return "RecChannel"; }

private:

   /**
    * Objeto para enviar audio en RTP
    */
    RTPSink *rtpsink;

    /**
    * Tamaño de los bufferes de captura, codificado y resampleado
    */
    static const int BUFFER_SIZE = MixerBuffer::BYTES_PER_BEAT*2;

   /**
    * Dispositivo de captura de audio.
    */
    SoundDevice_t *soundDevice;

    /**
    * Dispositivo de captura de audio local por udp.
    */
    UdpSound *udpSound;

    /**
    * Socket para enviar los paquetes RTP
    */
    dgramSocket_t *socket;

    /*
    * genera un tono de prueba para enviar
    */
    ToneGenerator_t *toneGen;

    /**
    * Fichero que se esta enviando a la red.
    * Si es NULL se envian los datos capturados
    * desde el dispositivo de captura.
    */
    FileReader_t *fileReader;

    /**
    * Objeto para enviar el flujo de vumetros.
    */
    Vumeter *vumeter;

    /**
    * Ganancia que se aplica al canal.
    */
    float gain;
};

#endif
