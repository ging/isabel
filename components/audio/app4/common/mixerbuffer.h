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
#ifndef __MIXER_BUFFER_H__
#define __MIXER_BUFFER_H__

#include "audioapp.h"
#include "sounddevice.h"
#include "fileReader.h"

// FORWARD DECLARATIONS
class dgramSocket_t;
class Vumeter;
class RTPSink;

// Class Declaration

/**
* Clase que representa el buffer de mezclado de canales.
* Suma el audio de los distintos canales y periodicamente
* envia a la tarjeta audio o silencio si no hay datos.
*/
class MixerBuffer
{
private:

   /**
    * SSRC utilizado para el envio de la mezcla de audio.
    */
    static const int MIX_CHANNEL = 101;

    /**
    * Tamaño del buffer.
    */
    // 2 min to minimize the chance of a channel writing
    // in the buffer after it has looped
    static const unsigned int BUFFER_SIZE =
        SoundDevice_t::SAMPLE_RATE * SoundDevice_t::BPS * 120;

public:
    /**
    * Bytes que se han de enviar a la tarjeta en cada periodo.
    */
    static const unsigned int BYTES_PER_BEAT =
        AudioApp::HB_TIME / 1000 *
        SoundDevice_t::SAMPLE_RATE / 1000 *
        SoundDevice_t::BPS;

    /**
    * Retraso minimo en el que los canales pueden escribir respecto
    * al puntero de lectura.
    */
    static const int DELAY = 4 * BYTES_PER_BEAT; // 200 ms

    /**
    * Buffer de audio.
    */
    unsigned char buffer[BUFFER_SIZE];

    /**
    * Puntero de lectura.
    */
    unsigned int minOffset;

    /**
    * Dispositivo de sonido.
    */
    SoundDevice_t *soundDevice;

    /**
    * Dice si ya se ha ordenado al dispositivo que comience la reproduccion
    */
    bool playing;

    /**
    * Fichero que se esta reproducioendo por la salida local.
    * Si es NULL se reproducen los datos de la red.
    */
    FileReader_t *fileReader;

    /**
    * Objeto para enviar el flujo de vumetros.
    */
    Vumeter *vumeter;

    /**
    * Objeto para enviar el audio a la red sobre RTP.
    */
    RTPSink *rtpsink;

    /**
    * Retraso minimo en el que los canales pueden escribir respecto
    * al puntero de lectura.
    */
    int delay;

    /**
    * Maximo buffer permitido. Despues se resincroniza.
    */
    int maxDelay;

public:

    /**
    * Constructor de la clase.
    */
    MixerBuffer(SoundDevice_t *device,
                dgramSocket_t *vsocket,
                const char *vport
               );

    /**
    * Destructor virtual.
    */
    virtual ~MixerBuffer(void);

    /**
    * Inicia la reproduccion de un fichero por la salida local.
    * @param name Nombre del fichero.
    * @return True en caso de exito, false en caso contrario.
    */
    bool playFile(const char *name);

    /**
    * Detiene la reproduccion del fichero.
    */
    void stopFile(void);

    /**
    * Devuelve el offset minimo de escritura en el buffer.
    */
    int getMinOffset(void);

    /**
    * Establece el delay mínimo. La granularidad del delay es el tiempo de heartbeat.
    * @param del Multiplicador que se aplica al delay.
    */
    void setDelay(int del);

    /**
    * Establece el delay máximo.
    * @param delDelay en ms.
    */
    bool setMaxDelay(int del);

    /**
    * Escribe datos al buffer.
    * @param offset  Offset de escritura dentro del buffer.
    * @param pBuffer Buffer con los datos a escribir.
    * @param length  Numero de bytes a escribir.
    * @param force Si es true escribe el paquete recolocando el puntero si es necesario
    * @return Posicion del puntero de escritura tras escribir los datos.
    *
    */
    int write(int offset, const unsigned char *pBuffer, int length, bool force);

    /**
    * Tarea periodica que se encarga de escribir datos a la tarjeta.
    */
    virtual void heartBeat(void);

   /**
    * Realiza un bind en el canal de mazcla.
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

private:

    /**
    * Escribe un buffer de audio a la saldia del mezclador
    * @param buf Puntero a los datos
    * @param length longitud de los datos
    * @param power Potencia acustica
    */
    void writeAndClean(unsigned char *buf, int length, double power);

    /**
    * Dice si un puntero dentro del buffer esta entre los limites que se le pasan.
    */
    bool isBetweenPointers(unsigned int down, unsigned int up, unsigned int p);

    /**
    * Suma un frame de audio al contenido de un buffer destino
    * @param dest Buffer destino
    * @param source Datos a sumar
    * @param length Longitud de los datos en bytes.
    */
    void mix(void *dest, const void *source, size_t length);

};

#endif

