/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
#ifndef __MIXER_BUFFER_H__
#define __MIXER_BUFFER_H__

// Class Declaration

/**
* Clase que representa el buffer de mezclado de canales.
* Suma el audio de los distintos canales y periodicamente
* envia a la tarjeta audio o silencio si no hay datos.
*/
class MixerBuffer {
private:
    /**
    * Tamaño del buffer.
    */
    static const unsigned int BUFFER_SIZE = 48000*2*2; //2 segundos a maxima calidad

    static const int delay = 48 * 1000; //250 ms.

    /**
    * Buffer de audio.
    */
    unsigned char buffer[BUFFER_SIZE];

    /**
    * Puntero de lectura.
    */
    unsigned int minOffset;
    

public:

    /**
    * Constructor de la clase.
    */
    MixerBuffer(void);

    /**
    * Destructor virtual.
    */
    virtual ~MixerBuffer();

    /**
    * Devuelve el offset minimo de escritura en el buffer.
    */
    int getMinOffset();

	/**
	* Establece el delay mínimo. La granularidad del delay es el tiempo de heartbeat.
	* @param del Multiplicador que se aplica al delay.
	*/
	void setDelay(int del);

    /**
    * Escribe datos al buffer.
    * @param offset  Offset de escritura dentro del buffer.
    * @param pBuffer Buffer con los datos a escribir.
    * @param length  Numero de bytes a escribir.
	* @param force Si es true escribe el paquete recolocando el puntero si es necesario
    * @return Posicion del puntero de escritura tras escribir los datos.
	*
    */
	int write(int offset, const unsigned char* pBuffer, int length, bool force);

    int read(unsigned char* pBuffer,int length);
  
private:

	/**
	* Escribe un buffer de audio a la saldia del mezclador
	* @param buf Puntero a los datos
	* @param length longitud de los datos
    * @param power Potencia acustica
	*/
	void writeAndClean(unsigned char * obuf, unsigned char* buf, int length);

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
    void mix(void* dest, const void* source, size_t length);

};

#endif // __MIXER_BUFFER__

