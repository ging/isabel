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

#ifndef __PLAYCHANNEL_H__
#define __PLAYCHANNEL_H__

#include <icf2/sched.hh>

#include <aCodecs/codecs.h>

#include "channel.h"
#include "sounddevice.h"



// FORWARD DECLARATIONS
class MixerBuffer;
class PacketQueue;

// CLASS DECLARATION

/**
* Clase que representa un canal de reproduccion.
*/
class PlayChannel: public Channel
{
private:
    aDecoder_t *decoder;

public:

    /**
    * Constructor de la clase.
    * @param chId Identificador del canal.
    * @param mbuffer Buffer donde el canal escribe sus datos.
    * @param ssock Socket por donde se escriben las estadisticas.
    * @param sport Puerto donde se envian las estadisticas.
    */
    PlayChannel(u32            chId,
                MixerBuffer   *mbuffer,
                dgramSocket_t *ssock,
                const char    *sport
               );

    /**
    * Destructor virtual.
    */
    virtual ~PlayChannel(void);

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
    * Metodo que procesa un paquete de audio.
    * @param data Buffer con los datos.
    * @param length longitud del buffer de datos.
    */
    void packetReceived(const unsigned char *data, int length);

    /**
    * Metodo que procesa un paquete de audio.
    * @param data Buffer con los datos.
    * @param length longitud del buffer de datos.
    */
    void packetReceived2(const unsigned char *data, int length);


    // ICF debug
    virtual const char *className(void) const { return "PlayChannel"; }

private:

    typedef enum
    {
        NEW_FLOW,
        NEXT_PACKET,
        JUMP_PACKET,
        OLD_PACKET,
        DUPLICATE_PACKET
    } PacketCase;

    /**
    * Tamaño de los bufferes usados para el audio resampleado
    * y el audio decodificado.
    */
    static const int BUFFER_SIZE =
        SoundDevice_t::SAMPLE_RATE * 300 * SoundDevice_t::BPS / 1000;
  
    /**
    * Numero maximo de diferencia entre un paquete y otro. Un numero mayor indicaria reseteo
    * del numero de secuencia.
    **/
    static const int MAX_DIFF = 20;

    /**
    * Establece los valores dependientes del descodificador.
    * @param PT Payload type del descodificador
    * @return True si existe el descodificador y false si no.
    */
    bool setDecoder(u8 PT);

private:

    /**
    * Dice si el canal esta activo o no.
    */
    bool started;

    /**
    * Dice si el paquete que acaba de llegar es el primero
    * de una supuesta rafaga.
    */
    bool firstInBurst;

    /**
    * Offset de escritura en el buffer de salida.
    */
    int offset;

    /**
    * Timestamp del ultimo paquete recibido.
    */
    u32 lastTs;

    /**
    * Numero de secuencia del ultimo paquete recibido.
    */
    u16 lastNseq;

    /**
    * PayloadType del ultimo paquete recibido.
    */
    u8 lastPT;

    /**
    * Sample Rate del ultimo paquete recibido.
    */
    u8 sampleRate;

    /**
    * Numero de muestras por frame. Es lo que se se incrementa
    * el TS en cada paquete si van seguidos.
    */
    int samplesPerFrame;

    /**
    * Buffer para escribir los datos llegados.
    */
    MixerBuffer *mixerBuffer;

    /**
    * Cola para ordenar paquetes
    */
    PacketQueue *queue;

    /**
    * Objeto empleado para resamplear audio.
    */
    SoundUtils::Resampler resampler;

    /**
    * Buffer donde se escriben los datos una vez resampleados
    */
    unsigned char resampledBuffer[BUFFER_SIZE];

    /**
    * Buffer donde se escriben los datos una vez decodificados
    */
    unsigned char decodedBuffer[BUFFER_SIZE];

    /**
    * Analiza el tipo de paquete que ha llegado
    * @param nseq Numero de secuencia
    * @param ts Timestamp
    * @param pt Payload type
    * @param tsjump Parametro de salida con el valor del salto en el ts (en bytes al rate de la tarjeta)
    * @param nsjump Parametro de salida con el valor del salto en el ns
    * @return el caso ante el que nos encontramos
    */
    PacketCase getPacketCase(u16 nseq, u32 ts, u8 pt, int &tsjump, int &nsjump);
};

#endif

