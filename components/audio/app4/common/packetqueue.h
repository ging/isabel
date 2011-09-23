/*
 * ISABEL: A group collaboration tool for the Internet
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


#ifndef __PACKETQUEUE_H__
#define __PACKETQUEUE_H__

#include <map>
#include <icf2/notify.hh>

// FORWARD DECLARATIONS
class PlayChannel;


typedef std::map< int, unsigned char *> PACKETQUEUE;
typedef std::map< int, int > LENGTHQ;

/**
 * Esta clase se encarga de cuando llega un paquete pasarselo al playchannel si es el siguiente en el flujo o
 * encolarlo si no. Los paquetes se mantienen en la cola hasta que llegan los paquetes desordenados o
 * hasta que ha pasado el tiempo máximo en cola.
 */
class PacketQueue
{
public:

    /**
    * Constructor de la clase.
    */
    PacketQueue(PlayChannel *ch);

    /**
    * Destructor virtual
    */
    virtual ~PacketQueue(void);

    /**
    * Método para procesar paquetes recibidos.
    */
    void packetReceived(const unsigned char *data, int length);

private:

    /**
    * Guarda un paquete en la cola de paquetes
    */
    void enqueuePacket(const unsigned char *data, int length, u16 nseq);

    /**
    * Comprueba si hay que enviar alguno de los paquetes de la cola
    */
    void checkQueue(void);

    /**
    * Borra la cola
    */
    void cleanQueue(void);

    /**
    * Envia el primer paque de la cola
    */
    void sendFirst(void);

    /**
    * Numero maximo de diferencia entre un paquete y otro.
    * Un numero mayor indicaria reseteo del numero de secuencia.
    **/
    static const int MAX_DIFF = 25;

    /**
    * Numero maximo de diferencia en TS entre un paquete y otro.
    * Un numero mayor indicaria reseteo del numero del TS.
    */
    static const int MAX_DIFF_TS = 5000;

    /**
    * Tamaño de la cola de paquetes
    */
    static const int MAX_SIZE = 10;

    /**
    * Canal que reproduce los paquetes de audio
    */
    PlayChannel *channel;

    /**
    * cola de paquets
    */
    PACKETQUEUE queue;

    /**
    * cola con las longitudes de los paquetes
    */
    LENGTHQ lqueue;

    /**
    * Ultimo número de secuencia enviado hacia el playchannel
    */
    u16 lastNseq;

    /**
    * Timestamp del ultimo paquete recibido.
    */
    u32 lastTs;

};

#endif
