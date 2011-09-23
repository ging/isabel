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

#ifndef __CHANNEL_H__
#define __CHANNEL_H__


#include <icf2/ql.hh>
#include <icf2/item.hh>

#include <aCodecs/soundutils.h>

#include "statssender.h"

/**
* Clase padre de los distintos tipos de canales.
*/
class Channel : public virtual item_t, public virtual collectible_t
{
public:
    enum channelKind_e
    {
        PLAY_CHANNEL,
        REC_CHANNEL,
        NET_CHANNEL
    };


protected:

    /**
    * Numero empleado cuando se produce un error al hacer un bind.
    */
    static const int BIND_ERROR = -1;

    /**
    * Identificador de canal.
    */
    u32 chId;

    /**
    * Tipo de canal.
    */
    channelKind_e chKind;

    /**
    * Recopilador de estadisticas del canal.
    */
    StatsSender stats;

public:

    /**
    * Constructor de la clase.
    * @param id Identificador del canal.
    * @kind tipo de canal.
    * @param sock Socket donde se escriben loa datos de estadisticas.
    * @param aport Puerto donde se envian los paquetes de estadisticas.
    */
    Channel(u32 id, channelKind_e kind, dgramSocket_t *sock, const char *aport);

    /**
    * Destructor virtual.
    */
    virtual ~Channel(void);

    /**
    * Devuelve el identificador del canal.
    */
    u32 getId(void) const { return chId;}

    /**
    * Devuelve el tipo de canal.
    */
    channelKind_e getKind(void) const { return chKind; }

    /**
    * Inicia el canal. Dependiendo del tipo
    * puede significar una cosa u otra.
    * @return True en caso de exito, false en caso contrario.
    */
    virtual bool start(void) { return false; }

    /**
    * Detiene el canal. Dependiendo del tipo
    * puede significar una cosa u otra.
    * @return True en caso de exito, false en caso contrario.
    */
    virtual bool stop(void) { return false; }

    /**
    * Metodo para enviar las estadisticas
    */
    virtual void sendStats(void) { stats.sendStats(); }

    //
    // icf administrivia
    //
    virtual const char *className(void) const { return "Channel"; }
};

#endif
