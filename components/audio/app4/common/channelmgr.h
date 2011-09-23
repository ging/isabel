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

#ifndef __CHANNEL_MANAGER__H__
#define __CHANNEL_MANAGER__H__

#include <map>
#include <icf2/notify.hh>

#include "mixerbuffer.h"
#include "channel.h"
#include "recchannel.h"
#include "udpsound.h"

// FORWARD DECLARATIONS
class SoundDevice;
class dgramSocket_t;

typedef std::map< u32, Channel* > CHANNEL_MAP;

// CLASS DECLARATION

/**
* Gestor de canales de la aplicacion.
* Permite la creacion y destruccion de los canales.
*/
class ChannelManager
{
private:

    /**
    * Numero usado para indicar que no hay RecChannel.
    */
    static const u32 NO_CHANNEL = 0;

    /**
    * Tabla que identificadores con los canales.
    */
    CHANNEL_MAP chTable;

    /**
    * Identificador del RecChannel.
    */
    u32 recChannel;

    /**
    * Dispositivo de captura/reproduccion
    */
    SoundDevice_t *soundDevice;

    /**
    * Buffer de mezcla
    */
    MixerBuffer *buffer;

   /**
    * Recepcion local de sonido por udp
    */
    UdpSound *udpSound;

    /**
    * Socket de datos.
    */
    dgramSocket_t *rtpSocket;

    /**
    * Socket de vumetros.
    */
    dgramSocket_t *vuSocket;

    /**
    * Puerto para enviar los vumetros.
    */
    const char *vuPort;

    /**
    * Puerto para enviar los vumetros locales.
    */
    const char *vuPlayPort;

    /**
    * Puerto para enviar las estadisticas.
    */
    const char *statsPort;

public:


    /**
    * Constructor de la clase.
    */
    ChannelManager(SoundDevice_t *device,
                   dgramSocket_t *socket,
                   dgramSocket_t *vsocket,
                   const char    *vport,
                   const char    *vplayport,
                   const char    *sport,
                   MixerBuffer   *mbuffer,
                   UdpSound      *udp
                  );

    /**
    * Destructor virtual.
    */
    virtual ~ChannelManager(void);

    /**
    * Crea un canal de envio.
    * @param chId Identificador del canal.
    * @return True si se crea el canal con exito, false en caso contrario.
    */
    bool newRecChannel(u32 chId);

    /**
    * Crea un canal de reproduccion.
    * @param chId Identificador del canal.
    * @return True si se crea el canal con exito, false en caso contrario.
    */
    bool newPlayChannel(u32 chId);

    //bool newNetChannel(u32 chId, char *rtpPort, char *rtcpPort, char *compID);

    /**
    * Elimina un canal.
    * @param chId Identificador del canal.
    * @return True en casod e exito, false en caso contrario.
    */
    bool deleteChannel(u32 chId);

    /**
    * Ordena al buffer de mezcla que reproduzca un fichero por la salida.
    * @param name Nombre del fichero.
    * @return True en caso de exito y false en caso contrario.
    */
    inline bool playOutputFile(const char *name) { return buffer->playFile(name); }

    /**
    * Detiene la reproduccion del fichero por la salida local.
    */
    inline void stopOutputFile(void) { buffer->stopFile(); }

    /**
    * Obtiene un canal.
    * @param chId Identificador del canal
    * @return Un Channel* que hay que comprobar si es valido (puede ser NULL)
    * 
    */
    Channel *getChannel(u32 chId);

    /**
    * Obtiene el canal de envio.
    * @return Un Channel* que hay que comprobar si es valido (puede ser NULL)
    */
    RecChannel *getRecChannel(void);

    /**
    * Manda las estadisticas de todos los canales
    */
    void sendStats(void);
};

#endif
