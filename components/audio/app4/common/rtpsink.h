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

#ifndef __RTPSINK_H__
#define __RTPSINK_H__

#include <map>

#include <icf2/sched.hh>
#include <icf2/sockIO.hh>

#include <aCodecs/codecs.h>
#include <aCodecs/soundutils.h>

#include "rtpheader.h"

// FORWARD DECLARATIONS
class Vumeter;
class StatsSender;

typedef std::map< int, inetAddr_t *> BINDINGS;

// CLASS DECLARATION

/**
* Clase que representa un sumidero RTP de audio.
*/
class RTPSink
{
public:

    /**
    * Constructor de la clase.
    * @param chId Identificador del canal.
    * @param sock Socket donde se escriben loa datos.
    * @param st Objeto de recopicación de estadisticas.
    * @param rate Velocidad de muestreo del audio de entrada.
    * @param bpsa Bytes por muestra del audio de entrada.
    */
    RTPSink(u32 chId, dgramSocket_t *sock, StatsSender *st, int rate, int bpsa);

    /**
    * Destructor virtual.
    */
    virtual ~RTPSink(void);

    /**
    * Establece el limite de potencia que se considera silencio
    * @param limit Nuevo umbral de silencio. Debe ser un numero entre 0 y -50.
    */
    void setSilenceThreshold(double limit);

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
    * Establece el codificador del canal.
    * @param name Nombre del codificador.
    * @return True en caso de exito, false en caso contrario.
    */
    bool setCoder(int format);

    /**
    * Inicia el envio de este canal.
    */
    void start(void);

    /**
    * Detiene el envio de este canal.
    */
    void stop(void);

    /**
    * Escribe datos para enviar.
    * @param buff Buffer con los datos
    * @param length Longitud de los datos escritos
    * @param power Potencia del sonido en el buffer
    */
    void writeBuffer(unsigned char *buff, int length, double power);


    // ICF debug
    virtual const char *className(void) const { return "RTPSink"; }

private:

    /**
    * Tamaño de los bufferes de captura, codificado y resampleado
    */
    static const int BUFFER_SIZE = 32*1024;

    /**
    * Tamaño maximo de un paquete de audio
    */
    static const int MAX_FRAME_SIZE = 3*1024;

    /**
    * Numero de ciclos de captura que tiene que haber silencio para
    * dejar de enviar audio.
    */
    static const int SILENCE_PERIODS_TO_CUT = 30;

    /**
    * Umbral por defecto de lo que se considera silencio.
    */
    static const int DEFAULT_THRESHOLD = -40;

    // Buffers para datos
    unsigned char packet[MAX_FRAME_SIZE];
    unsigned char buffer[BUFFER_SIZE];
    unsigned char resampledBuffer[BUFFER_SIZE];
    unsigned char encodedBuffer[BUFFER_SIZE];
    //

    /**
    * Recopilador de estadisticas del canal.
    */
    StatsSender *stats;

    /**
    * Codificador utilizado en el canal.
    */
    aCoder_t *coder;

    /**
    * Bytes almacenados en el buffer, pendientes de enviar.
    */
    int buffered;

    /**
    * Contador del numero de bindings para devolver identificadore
    * unicos.
    */
    int counter;

    /**
    * Umbral de potencia que se considera silencio.
    */
    double threshold;

    /**
    * Cuenta el numero de ciclos en silencio.
    */
    int silenceCounter;

    /**
    * Indica si el canal esta arrancado o no
    */
    bool started;

    /**
    * Velocidad de muestreo del audio de entrada.
    */
    int inputRate;

    /**
    * Bytes por muestra del audio de entrada.
    */
    int bps;

    // codec properties
    int fullRateFrameSize;
    int frameSize;
    int samplesPerFrame;

    /**
    * Tabla de bindings del canal.
    * Asocia identificadores a direcciones de internet.
    */
    BINDINGS bindTable;

    /**
    * Cabecera RTP
    */
    RTPHeader header;

    /**
    * Objeto para resamplear las muestras.
    */
    SoundUtils::Resampler resampler;

    /**
    * Socket para enviar los paquetes RTP
    */
    dgramSocket_t *socket;
};

#endif
