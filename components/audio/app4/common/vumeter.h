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

#ifndef __VUMETER_H__
#define __VUMETER_H__

#include "rtpheader.h"

// FORWARD DECLARATIONS
class dgramSocket_t;
class inetAddr_t;

// CLASS DECLARATION

/**
* Clase que se encarga del envio de los vumetros
*/
class Vumeter
{

public:

    /**
    * Constructor de la clase.
    * @param achId Identificador del canal.
    * @param abeats Cada cuantos ciclos debe enviar un paquete
    * @param sock Socket donde se escriben loa datos.
    * @param aport Puerto donde se envian los paquetes de vumetros.
    * @param anotherport Otro puerto donde enviar los paquetes de vumetros.
    */
    Vumeter(u32 achId,
            int abeats,
            dgramSocket_t *sock,
            const char *aport,
            const char *anotherport = NULL
           );

    /**
    * Destructor virtual.
    */
    virtual ~Vumeter(void);

    /**
    * Metodo para pasar datos de potencia para enviar.
    * Cuando se llama a este método n veces (n=beats) se produce el
    * envio de un paquete de datos del vumetro.
    * @param power Potencia del audio capturado.
    */
    void nextData(double power);

private:

    /**
    * Payload type del flujo de vumetros.
    */
    static const u8 VUMETER_PT = 22;

    /**
    * Maximo numero de datos que se almacenan
    */
    static const int MAX_BEATS = 10;

    /**
    * Tamaño del paquete de vumetros.
    */
    static const int PACKET_SIZE = RTPHeader::SIZE + 8;

    /*
    * Identificador del canal.
    */
    u32 chId;

    /**
    * Cada cuantos ciclos debe enviar un paquete.
    */
    int beats;

    /**
    * Contador del numero de bindings para devolver identificadore
    * unicos.
    */
    int counter;

    /**
    * Cabecera RTP
    */
    RTPHeader header;

    /**
    * Socket para enviar los paquetes RTP
    */
    dgramSocket_t *socket;

    /**
    * Direccion donde se envian los datos.
    */
    inetAddr_t *addr;

    /**
    * Direccion donde se envian los datos.
    */
    inetAddr_t *addr2;

    /**
    * Array para ir almacenado los datos antes de enviar el paquete
    */
    double data[MAX_BEATS];
};

#endif
