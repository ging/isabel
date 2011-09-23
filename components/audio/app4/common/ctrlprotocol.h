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

#ifndef __AUDIO_CTRL_PROTOCOL_H__
#define __AUDIO_CTRL_PROTOCOL_H__

#include <icf2/item.hh>
#include <icf2/stdTask.hh>

/**
* Implementacion del interfaz de control.
*/
class AudioInterfaceDef
{

public:

    AudioInterfaceDef() { }

    char const *audio_nop   (int, char **);          // Estoy vivo?
    char const *audio_quit  (int, char **);          // Mata el demonio
    char const *audio_bye   (int, char **);          // Un usuario se desconecta

    char const *audio_assign_channel (int, char **); // Crea el canal de envio local 
    char const *audio_new_channel (int, char **);    // Crea el canal de reproduccion
    char const *audio_delete_channel(int, char **);  // Borra un canal

    char const *audio_set_threshold(int, char **);   // Establece el umbral de silencio
    char const *audio_set_input_gain(int, char **);  // Establece la ganacia del canal de entrada

    char const *audio_bind(int, char **);            // Establece la direccion donde se envia audio
    char const *audio_unbind(int, char **);          // Establece la direccion donde se envia audi

    char const *audio_start_sending(int, char **);   // Inicia el envio del audio local
    char const *audio_stop_sending(int, char **);    // Detiene el envio del audio local

    char const *audio_query_codecs(int, char **);    // Devuelve una lista de los codecs
    char const *audio_set_codec(int, char **);       // Establece el codec de audio a utilizar
    char const *audio_get_codec_bw(int, char **);    // Obtiene BW de un codec

    char const *audio_play_output_file(int, char**); // Reproduce un fichero por la salida local
    char const *audio_stop_output_file(int, char**); // Detiene la reproduccion del fichero por la salida local

    char const *audio_play_input_file(int, char**);  // Reproduce un fichero hacia la red
    char const *audio_stop_input_file(int, char**);  // Detiene la reproduccion del fichero hacia la red

    char const *audio_tone_generator(int, char**);       // Reproduce o detiene un tono hacia la red (contenido en un fichero)
    char const *audio_local_tone_generator(int, char**); // Reproduce o detiene un tono hacia la salida local (contenido en un fichero)

	char const *audio_set_echo_suppressor(int, char**);  //  Establece el modo de funcionamiento del supresor de echo
	char const *audio_set_echo_threshold(int, char**);   //  Establece el modo de funcionamiento del supresor de echo
	char const *audio_set_echo_window(int, char**);      //  Establece el modo de funcionamiento del supresor de echo

    char const *audio_set_cag(int, char**);             //  Establece el modo de funcionamiento del cag
	char const *audio_set_cag_threshold(int, char**);   //  Establece el modo de funcionamiento del cag
	char const *audio_set_cag_window(int, char**);      //  Establece el modo de funcionamiento del cag

	char const *audio_do_stats(int, char**);              //  Activa o desactiva el envio de estadisticas

	char const *audio_bind_mix(int, char**);              //  Activa el envio de la mezcla de audio por RTP

	char const *audio_capture_device(int, char**);        //  Seleccion del dispositivo de captura
	char const *audio_play_device(int, char**);           //  Seleccion del dispositivo de reproduccion
	char const *audio_reset_devices(int, char**);           //  Reset de los dispositivos

	char const *audio_set_buffering(int, char**);         //  Tamaño del buffer de recepción

};

/**
* Array que relaciona nombres con funciones.
*/
extern binding_t<AudioInterfaceDef> audioMethodBinding[];


/**
* Tarea del interfaz del control del demonio de audio.
* Conjuro de templarte nivel 7 o mas.
*/
class AudioInterface: public interface_t <AudioInterfaceDef, 
    audioMethodBinding>
{
public:
    AudioInterface(const io_ref &i)
        :interface_t<AudioInterfaceDef, audioMethodBinding>(i)
    { };

};

#endif

