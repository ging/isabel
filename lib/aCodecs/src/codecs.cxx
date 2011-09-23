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

/////////////////////////////////////////////////////////////////////////
//
// $Id: codecs.cxx 22241 2011-04-08 12:30:49Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <aCodecs/codecs.h>

#include "codec.h"

#include "codecs/g711Codec.h"
#include "codecs/gsmCodec.h"
#include "codecs/mp3Codec.h"
#include "codecs/speexCodec.h"
#ifdef WIN32
#include "codecs/WinXP/amrnbCodec.h"
#else
#include "codecs/amrnbCodec.h"
#endif

#include "codecs/noneCodec.h"
#include "codecs/g722Codec.h"
#include "codecs/g726Codec.h"
#include "codecs/g728Codec.h"

typedef struct
{
    const char *name;
    aCoder_t *(*New)(int fmt, int sampleRate, int bandwidth);
    int bandwidth;
    unsigned short int rate;
    int channels;
    unsigned int format;
    audioID_e id;
} aCoderFactory_t;

typedef struct
{
    const char *name;
    aDecoder_t *(*New)(int fmt, int sampleRate);
    int bandwidth;
    unsigned short int rate;
    int channels;
    unsigned int format;
    audioID_e id;
} aDecoderFactory_t;

aCoderFactory_t coderRegister [] =
{
#if 1
    {"gsm-8KHz",    gsmNewCoder, 20,  8000, 1, GSM_8_FORMAT, GSM_FORMAT },
#else
    {"gsm-8KHz",    gsmNewCoder, 20,  8000, 1, GSM_8_FORMAT, GSM_FORMAT },
#endif
    {"gsm-16KHz",   gsmNewCoder, 40, 16000, 1, GSM_16_FORMAT, GSM_FORMAT },
    {"gsm-32KHz",   gsmNewCoder, 60, 32000, 1, GSM_32_FORMAT, GSM_FORMAT },

    {"amr-8KHz",  amrnbNewCoder, 20,  8000, 1, AMR_8_FORMAT,  AMR_FORMAT },
//  {"amr-16KHz", amrwbNewCoder, 40, 16000, 1, AMR_16_FORMAT, AMR_FORMAT },
//  {"amr-32KHz", amrwbNewCoder, 60, 32000, 1, AMR_32_FORMAT, AMR_FORMAT },

    {"none-8KHz",     noneNewCoder, 135,  8000, 1, NONE_8_FORMAT, NONE_FORMAT },
    {"none-16KHz",    noneNewCoder, 271, 16000, 1, NONE_16_FORMAT, NONE_FORMAT },
    {"none-22.05KHz", noneNewCoder, 383, 22050, 1, NONE_22_FORMAT, NONE_FORMAT },
    {"L16-44.1KHz",   noneNewCoder, 746, 44100, 1, NONE_44_FORMAT, NONE_FORMAT },
    {"none-48KHz",    noneNewCoder, 800, 48000, 1, NONE_48_FORMAT, NONE_FORMAT },

    {"g711-alaw-8KHz",     g711NewCoder,  72,  8000, 1, PCMA_8_FORMAT, PCMA_FORMAT},
    {"g711-ulaw-8KHz",     g711NewCoder,  72,  8000, 1, PCMU_8_FORMAT, PCMU_FORMAT},
    {"g711-ulaw-16KHz",    g711NewCoder, 143, 16000, 1, PCMU_16_FORMAT, PCMU_FORMAT},
    {"g711-ulaw-22.05KHz", g711NewCoder, 197, 22050, 1, PCMU_22_FORMAT, PCMU_FORMAT},
    {"g711-ulaw-44.1KHz",  g711NewCoder, 394, 48000, 1, PCMU_44_FORMAT, PCMU_FORMAT},

    {"g722-8KHz",  g722NewCoder, 40,  8000, 1, G722_8_FORMAT, G722_FORMAT},
    {"g722-16KHz", g722NewCoder, 79, 16000, 1, G722_16_FORMAT, G722_FORMAT},
// {"g722-22.05KHz", g722NewCoder, 110, 22050, 1},
// {"g722-44.1KHz", g722NewCoder, 158, 44100, 1}, // Id = 3

    {"g726_24-8KHz",  g726NewCoder, 32,  8000, 1, G726_8_FORMAT, G726_FORMAT},
    {"g726_24-16KHz", g726NewCoder, 63, 16000, 1, G726_16_FORMAT, G726_FORMAT},
// {"g726_24-22.05KHz", g726NewCoder, 87, 22050, 1},
// {"g726_44.1.05KHz", g726NewCoder, 126, 44100, 1}, // Id = 6

// {"g728-8KHz", g728NewCoder, 24, 8000, 1},
// {"g728-16KHz", g728NewCoder, 47, 16000, 1},

    {"mp3-8KHz",    mp3NewCoder,  22,  8000, 1,  MP3_8_FORMAT, MP3_FORMAT },
    {"mp3-16KHz",   mp3NewCoder,  43, 16000, 1, MP3_16_FORMAT, MP3_FORMAT },
    {"mp3-32KHz",   mp3NewCoder,  85, 32000, 1, MP3_32_FORMAT, MP3_FORMAT },
    {"mp3-44.1KHz", mp3NewCoder, 117, 44100, 1, MP3_44_FORMAT, MP3_FORMAT },
    {"mp3-48KHz",   mp3NewCoder, 128, 48000, 1, MP3_48_FORMAT, MP3_FORMAT },

    {"speex-8KHz",  speexNewCoder, 20,  8000, 1,  SPEEX_8_FORMAT, SPEEX_FORMAT},
    {"speex-16KHz", speexNewCoder, 40, 16000, 1, SPEEX_16_FORMAT, SPEEX_FORMAT}
};

aDecoderFactory_t decoderRegister [] =
{
#if 1
    {"gsm-8KHz",    gsmNewDecoder, 20,  8000, 1, GSM_8_FORMAT, GSM_FORMAT },
#else
    {"gsm-8KHz",    gsmNewDecoder, 20,  8000, 1, GSM_8_FORMAT, GSM_FORMAT },
#endif
    {"gsm-16KHz",   gsmNewDecoder, 40, 16000, 1, GSM_16_FORMAT, GSM_FORMAT },
    {"gsm-32KHz",   gsmNewDecoder, 60, 32000, 1, GSM_32_FORMAT, GSM_FORMAT },

    {"amr-8KHz",  amrnbNewDecoder, 20,  8000, 1, AMR_8_FORMAT,  AMR_FORMAT },
//  {"amr-16KHz", amrwbNewDecoder, 40, 16000, 1, AMR_16_FORMAT, AMR_FORMAT },
//  {"amr-32KHz", amrwbNewDecoder, 60, 32000, 1, AMR_32_FORMAT, AMR_FORMAT },

    {"none-8KHz",     noneNewDecoder, 135,  8000, 1, NONE_8_FORMAT, NONE_FORMAT },
    {"none-16KHz",    noneNewDecoder, 271, 16000, 1, NONE_16_FORMAT, NONE_FORMAT },
    {"none-22.05KHz", noneNewDecoder, 383, 22050, 1, NONE_22_FORMAT, NONE_FORMAT },
    {"L16-44.1KHz",   noneNewDecoder, 746, 44100, 1, NONE_44_FORMAT, NONE_FORMAT },
    {"none-48KHz",    noneNewDecoder, 800, 48000, 1, NONE_48_FORMAT, NONE_FORMAT },

    {"g711-alaw-8KHz",     g711NewDecoder,  72,  8000, 1, PCMA_8_FORMAT, PCMA_FORMAT},
    {"g711-ulaw-8KHz",     g711NewDecoder,  72,  8000, 1, PCMU_8_FORMAT, PCMU_FORMAT},
    {"g711-ulaw-16KHz",    g711NewDecoder, 143, 16000, 1, PCMU_16_FORMAT, PCMU_FORMAT},
    {"g711-ulaw-22.05KHz", g711NewDecoder, 197, 22050, 1, PCMU_22_FORMAT, PCMU_FORMAT},
    {"g711-ulaw-44.1KHz",  g711NewDecoder, 394, 48000, 1, PCMU_44_FORMAT, PCMU_FORMAT},

    {"g722-8KHz",  g722NewDecoder, 40,  8000, 1, G722_8_FORMAT, G722_FORMAT},
    {"g722-16KHz", g722NewDecoder, 79, 16000, 1, G722_16_FORMAT, G722_FORMAT},
// {"g722-22.05KHz", g722NewDecoder, 110, 22050, 1},
// {"g722-44.1KHz", g722NewDecoder, 158, 44100, 1}, // Id = 3

    {"g726_24-8KHz",  g726NewDecoder, 32,  8000, 1, G726_8_FORMAT, G726_FORMAT},
    {"g726_24-16KHz", g726NewDecoder, 63, 16000, 1, G726_16_FORMAT, G726_FORMAT},
// {"g726_24-22.05KHz", g726NewDecoder, 87, 22050, 1},
// {"g726_44.1.05KHz", g726NewDecoder, 126, 44100, 1}, // Id = 6

// {"g728-8KHz", g728NewDecoder, 24, 8000, 1},
// {"g728-16KHz", g728NewDecoder, 47, 16000, 1},

    {"mp3-8KHz",    mp3NewDecoder,  22, 8000, 1, MP3_8_FORMAT, MP3_FORMAT },
    {"mp3-16KHz",   mp3NewDecoder,  43, 16000, 1, MP3_16_FORMAT, MP3_FORMAT },
    {"mp3-32KHz",   mp3NewDecoder,  85, 32000, 1, MP3_32_FORMAT, MP3_FORMAT },
    {"mp3-44.1KHz", mp3NewDecoder, 117, 44100, 1, MP3_44_FORMAT, MP3_FORMAT },
    {"mp3-48KHz",   mp3NewDecoder, 128, 48000, 1, MP3_48_FORMAT, MP3_FORMAT },

    {"speex-8KHz",  speexNewDecoder, 20, 8000, 1, SPEEX_8_FORMAT, SPEEX_FORMAT },
    {"speex-16KHz", speexNewDecoder, 40, 16000, 1, SPEEX_16_FORMAT, SPEEX_FORMAT }
};

static const int MAX_AUDIO_CODERS = (sizeof(coderRegister) / sizeof(aCoderFactory_t));
static const int MAX_AUDIO_DECODERS = (sizeof(decoderRegister) / sizeof(aDecoderFactory_t));

///////////////////////////////////////////////////////////////////
// Información de formatos de audio
///////////////////////////////////////////////////////////////////

__EXPORT const char *
aGetFormatNameById(unsigned int format)
{
   for (int i = 0 ; i < MAX_AUDIO_CODERS; i++)
    {
        if (coderRegister[i].format == format)
        {
            return coderRegister[i].name;
        }
    }

    return NULL;
}

__EXPORT const char *
aGetFormatNameByIdRate(audioID_e id, unsigned int rate)
{
   for (int i = 0 ; coderRegister[i].name != NULL; i++)
    {
        if (coderRegister[i].id == id && coderRegister[i].rate == rate)
        {
            return coderRegister[i].name;
        }
    }

    return NULL;
}

__EXPORT int
aGetFormatIdByName(const char *name)
{
   for (int i = 0; i < MAX_AUDIO_CODERS; i++)
   {
       if (strcmp(coderRegister[i].name, name) == 0)
       {
           return coderRegister[i].format;
       }
   }

   return -1;
}

__EXPORT int
aGetBandwidthById(unsigned int format)
{
   for (int i = 0; coderRegister[i].name != NULL; i++)
    {
        if (coderRegister[i].format == format)
        {
            return coderRegister[i].bandwidth;
        }
    }

    return -1;
}


///////////////////////////////////////////////////////////////////
// Gestión de codificadores / descodificadores
///////////////////////////////////////////////////////////////////

__EXPORT int
aGetNumRegisteredCoders(void)
{
    return MAX_AUDIO_CODERS;
}

__EXPORT int
aGetNumRegisteredDecoders(void)
{
    return MAX_AUDIO_DECODERS;
}

__EXPORT int
aGetCodecFmtList(int *buffer, int buffer_size)
{
    int i;

    for (i = 0 ; i < MAX_AUDIO_CODERS; i++)
    {
        if (i >= buffer_size)
        {
            // cannot put into 'buffer', simply count how many Fmts there are
            continue;
        }

        buffer[i]= coderRegister[i].format;
    }

    return i;
}


__EXPORT int
aGetCoderList(char *buffer, int buffer_size)
{
    char *mensaje = new char[1024];

    memset(mensaje,0,1024);

    for (int i = 0; i < MAX_AUDIO_CODERS; i++)
    {
        strcat(mensaje, "{");
        strcat(mensaje, coderRegister[i].name);
        strcat(mensaje, "} ");
    }

    strcat(mensaje, "\n");
    strncpy(buffer, mensaje, buffer_size);

    delete mensaje;

    return 1;
}

///////////////////////////////////////////////////////////////////
// Uso de codificadores / descodificadores
///////////////////////////////////////////////////////////////////

__EXPORT aCoder_t *
aGetCoder(unsigned int format)
{
    for (int i = 0; i < MAX_AUDIO_CODERS; i++)
    {
        if (coderRegister[i].format == format)
        {
            return coderRegister[i].New(format,
                                        coderRegister[i].rate,
                                        coderRegister[i].bandwidth
                                       );
        }
    }

    return NULL;
}

__EXPORT aDecoder_t *
aGetDecoder(unsigned int format)
{
    for (int i = 0; i < MAX_AUDIO_DECODERS; i++)
    {
        if (decoderRegister[i].format == format)
        {
            return decoderRegister[i].New(format, decoderRegister[i].rate);
        }
    }

    return NULL;
}


__EXPORT aCoder_t *
aGetCoder(audioID_e id, int rate)
{
    for (int i = 0 ; i < MAX_AUDIO_CODERS; i++)
    {
        if (coderRegister[i].id == id && coderRegister[i].rate == rate)
        {
            return coderRegister[i].New(coderRegister[i].format,
                                        rate,
                                        coderRegister[i].bandwidth
                                       );
        }
    }

    return NULL;
}

__EXPORT aDecoder_t *
aGetDecoder(audioID_e id, int rate)
{
    for (int i = 0 ; i < MAX_AUDIO_DECODERS; i++)
    {
        if (decoderRegister[i].id == id && decoderRegister[i].rate == rate)
        {
            return decoderRegister[i].New(decoderRegister[i].format, rate);
        }
    }

    return NULL;
}

__EXPORT int
aDeleteCoder(aCoder_t *coder)
{
    return coder->Delete(coder);
}

__EXPORT int
aDeleteDecoder(aDecoder_t *decoder)
{
    return decoder->Delete(decoder);
}

__EXPORT int
aEncode(aCoder_t *coder,
        unsigned char *inBuff,
        int nSamples,
        unsigned char *outBuff
       )
{
    return coder->Encode(coder, inBuff, nSamples, outBuff);
}

__EXPORT int
aDecode(aDecoder_t *decoder,
        unsigned char *inBuff,
        int nBytes,
        unsigned char *outBuff
       )
{
    return decoder->Decode(decoder, inBuff, nBytes, outBuff);
}

///////////////////////////////////////////////////////////////////
// Parametros de codificadores / descodificadores
///////////////////////////////////////////////////////////////////

__EXPORT int
aGetFormat(aCoder_t *coder)
{
    assert (coder != NULL);

    return coder->format;
}

__EXPORT int
aGetFormat(aDecoder_t *decoder)
{
    assert (decoder != NULL);

    return decoder->format;
}

__EXPORT int
aGetCoderRate(aCoder_t *coder)
{
    assert (coder != NULL);

    return coder->sampleRate;
}

__EXPORT int
aGetCoderBandwidth(aCoder_t *coder)
{
    assert (coder != NULL);

    return coder->bandwidth;
}


__EXPORT int
aGetDecoderRate(aDecoder_t *decoder)
{
    assert (decoder != NULL);

    return decoder->sampleRate;
}

