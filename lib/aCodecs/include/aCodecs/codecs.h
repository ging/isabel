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
/////////////////////////////////////////////////////////////////////////
//
// $Id: codecs.h 22241 2011-04-08 12:30:49Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __acodecs_codecs_H__
#define __acodecs_codecs_H__

// forget them!
#undef __IMPORT
#undef __EXPORT

#ifdef _USRDLL
#define __IMPORT  __declspec(dllexport)
#define __EXPORT  __declspec(dllexport)
#else
#ifdef _IMPORT
#define __IMPORT  __declspec(dllimport)
#define __EXPORT  __declspec(dllexport)
#else
#define __IMPORT
#define __EXPORT
#endif
#endif

#ifdef __SYMBIAN32__
#undef __IMPORT
#undef __EXPORT
#define __IMPORT  IMPORT_C
#define __EXPORT  EXPORT_C
#endif

typedef struct aCoder_t aCoder_t;
typedef struct aDecoder_t aDecoder_t;


// formatos completos de audio (formato + rate)
const unsigned short GSM_8_FORMAT    =   3;
const unsigned short GSM_16_FORMAT   = 107;
const unsigned short GSM_32_FORMAT   = 115;

const unsigned short AMR_8_FORMAT    = 116;
const unsigned short AMR_16_FORMAT   = 117;
const unsigned short AMR_32_FORMAT   = 121;

const unsigned short NONE_8_FORMAT   = 100;
const unsigned short NONE_16_FORMAT  = 102;
const unsigned short NONE_22_FORMAT  = 108;
const unsigned short NONE_44_FORMAT  =  11;
const unsigned short NONE_48_FORMAT  = 119;

const unsigned short PCMA_8_FORMAT   =   8;
const unsigned short PCMU_8_FORMAT   =   0;
const unsigned short PCMU_16_FORMAT  = 103;
const unsigned short PCMU_22_FORMAT  = 109;
const unsigned short PCMU_44_FORMAT  = 112;

const unsigned short G722_8_FORMAT   =   9;
const unsigned short G722_16_FORMAT  = 104;

const unsigned short G726_8_FORMAT   = 101;
const unsigned short G726_16_FORMAT  = 105;

const unsigned short MP3_8_FORMAT    =  14;
const unsigned short MP3_16_FORMAT   =  15;
const unsigned short MP3_32_FORMAT   =  16;
const unsigned short MP3_44_FORMAT   =  17;
const unsigned short MP3_48_FORMAT   =  18;

const unsigned short SPEEX_8_FORMAT  = 123;
const unsigned short SPEEX_16_FORMAT = 124;

enum audioID_e
{
    NONE_FORMAT   = 0x00000001,
    G726_FORMAT   = 0x00000064,
    G722_FORMAT   = 0x00000065,
    PCMA_FORMAT   = 0x00000006,
    PCMU_FORMAT   = 0x00000007,
    GSM_FORMAT    = 0x00000031,
    MP3_FORMAT    = 0x00000055,
    AMR_FORMAT    = 0x726d6173, //FCC('samr');
    SPEEX_FORMAT  = 0x00000123
};

///////////////////////////////////////////////////////////////////
// Información de formatos de audio
///////////////////////////////////////////////////////////////////

// Devuelve un nombre a partir del formato completo
__IMPORT const char *aGetFormatNameById(unsigned int format);

// Devuelve un nombre a partir del identificadr y el rate
__IMPORT const char *aGetFormatNameByIdRate(audioID_e id, unsigned int rate);

// Devuelve un formato completo para un nombre (completo) de codec,
// en la forma "amr-18KHz", "none-22.05KHz", etc.
// Si el nombre no esta registrado devuelve -1
__IMPORT int aGetFormatIdByName(const char *name);

// Acceso a parametros de preconfiguracion de codecs
__IMPORT int aGetBandwidthById(unsigned int format);

///////////////////////////////////////////////////////////////////
// Gestión de codificadores / descodificadores
///////////////////////////////////////////////////////////////////

// Devuelve el numero de codificacores de audio disponibles
__IMPORT int aGetNumRegisteredCoders(void);

// Devuelve el numero de descodificadores de audio disponibles
__IMPORT int aGetNumRegisteredDecoders(void);

__IMPORT int aGetCodecFmtList(int *buffer, int buffer_size);

// Crea una cadena con los nombres de los codificadores
// entre llaves {c1} {c2}...\n
__IMPORT int aGetCoderList(char *buffer, int buffer_size);

///////////////////////////////////////////////////////////////////
// Uso de codificadores / descodificadores
///////////////////////////////////////////////////////////////////

// Devuelve un codificador a partir del formato completo
__IMPORT aCoder_t *aGetCoder(unsigned int format);

// Devuelve un codificador a partir del identificador del formato y el rate
__IMPORT aCoder_t *aGetCoder(audioID_e format, int rate);

// Devuelve un descodificador a partir del formato completo
__IMPORT aDecoder_t *aGetDecoder(unsigned int format);

// Devuelve un descodificador a partir del identificador del formato y el rate
__IMPORT aDecoder_t *aGetDecoder(audioID_e format, int rate);

// Libera los recursos de un codificador
__IMPORT int aDeleteCoder(aCoder_t *coder);

// Libera los recursos de un descodificador
__IMPORT int aDeleteDecoder(aDecoder_t *decoder);

// codifica 'nSamples' muestras de 'inBuff' y las deja en 'outBuff'
// devuelve el numero de bytes generados,
// < 0 en caso de error
__IMPORT int aEncode(aCoder_t *coder,
                     unsigned char *inBuff,
                     int nSamples,
                     unsigned char *outBuff
                    );

// descodifica 'nBytes' de 'inBuff' y las deja en 'outBuff'
// devuelve el numero de muestras generadas,
// < 0 en caso de error
__IMPORT int aDecode(aDecoder_t *decoder,
                     unsigned char *inBuff,
                     int nBytes,
                     unsigned char *outBuff
                    );

///////////////////////////////////////////////////////////////////
// Parametros de codificadores / descodificadores
///////////////////////////////////////////////////////////////////

__IMPORT int aGetFormat(aCoder_t *coder);
__IMPORT int aGetFormat(aDecoder_t *decoder);

__IMPORT int aGetCoderRate(aCoder_t *coder);
__IMPORT int aGetCoderBandwidth(aCoder_t *coder);

__IMPORT int aGetDecoderRate(aDecoder_t *decoder);

#endif

