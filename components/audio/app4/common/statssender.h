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
#ifndef __STATSSENDER_H__
#define __STATSSENDER_H__

// INCLUDES

// FORWARD DECLARATIONS
class dgramSocket_t;
class inetAddr_t;

// CLASS DECLARATION
class StatsSender
{
public:

    /**
    * Constructor de la clase.
    * @param id Identificador del canal.
    * @param sock Socket donde se escriben loa datos.
    * @param aport Puerto donde se envian los paquetes de estadisticas.
    */
    StatsSender(unsigned int id, dgramSocket_t *sock, const char *aport);

    /**
    * Destructor virtual.
    */
    virtual ~StatsSender(void);

    /**
    * Avisa al objeto de que se ha enviado un paquete.
    * @param size Tamaño en bytes del paquete enviado.
    */
    void packetSent(int size);

    /**
    * Avisa al objeto de que se ha recibibo un paquete.
    * @param size Tamaño en bytes del paquete recibido.
    */
    void packetRecv(int size);

    /**
    * Avisa al objeto de que se ha recibido un paquete duplicado.
    */
    void duplicateRecv(void);

    /**
    * Avisa al objeto de que se ha recibido un paquete fuera de orden.
    */
    void disorderedRecv(void);

    /**
    * Avisa al objeto de que se ha recibido y ha sido descartado.
    */
    void packetThrown(void);

    /**
    * Avisa al objeto de que se han detectado paquetes perdidos.
    * @param howmany Numero de paquetes perdidos detectado.
    */
    void packetLost(int howmany);

    /**
    * Avisa al objeto de un cambio en el codec utilizado.
    * @param name Nombre del codec utilizado.
    */
    void codecChange(const char *name);

    /**
    * Metodo que se ejecuta de forma periodica y que envia los paquetes
    * de estadisticas.
    */
    void sendStats(void);

private:

    /**
    * Socket para enviar los paquetes de datos
    */
    dgramSocket_t *socket;

    /**
    * Direccion donde se envian los datos.
    */
    inetAddr_t *addr;

    /**
    * Tamaño del paquete de estadisticas.
    */
    static const int SIZE = 85;

    // Variables para recopilar estadisticas
    unsigned int totalBitsSent;
    unsigned int totalBitsRecs;
    unsigned int totalPacketsRecs;
    unsigned int totalPacketsLost;
    unsigned int totalPacketsDuplicated;
    unsigned int totalPacketsDisordered;
    unsigned int totalPacketsThrown;
    //unsigned int jitter;
    //unsigned int buffering;
    //unsigned int buffered;

    unsigned char codecNameLen;  // 8 bytes longitud de nombre del codec
    char          codecName[30]; // Nombre del codec
    char          buffer[SIZE];

};

#endif

