#include <stdlib.h>
#include <stdio.h>

#include "codec.h"

#include "g728Codec.h"

#include "g728/state.h"
#include "g728/prototyp.h"

//Posfiltro desactivado 1 para activarlo
#define POSTFILTER 1

int g728DeleteCoder(void *context);
int g728DeleteDecoder(void *context);

int g728Encode(void *context,
               unsigned char *inBuff,
               int nSamples,
               unsigned char *outBuff
              );
int g728Decode(void *context,
               unsigned char *inBuff,
               int nBytes,
               unsigned char *outBuff
              );

aCoder_t *
g728NewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("g728NewCoder::malloc");
        return NULL;
    }

    LDCELP_STATE *state= (LDCELP_STATE *)malloc(sizeof(LDCELP_STATE));

    if ( ! state )
    {
        free(coder);
        perror("g728NewCoder::malloc");
        return NULL;
    }

    init_encoder(state);

    coder->Delete = g728DeleteCoder;
    coder->Encode = g728Encode;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth = bandwidth;
    coder->ratio = 1.0/8.0;
    coder->invRatio = 8;
    coder->bytesPerSample = 2;
    coder->minSamples = 20;

    coder->state = state;

    return coder;
}

int
g728DeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    free(coder->state);
    free(coder);

    return 1;
}

aDecoder_t *
g728NewCodec(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("g728NewDecoder::malloc");
        return NULL;
    }

    LDCELP_STATE *state= (LDCELP_STATE *)malloc(sizeof(LDCELP_STATE));

    if ( ! state )
    {
        free(decoder);
        perror("g728NewDecoder::malloc");
        return NULL;
    }

    init_decoder(state, POSTFILTER);

    decoder->Delete = g728DeleteDecoder;
    decoder->Decode = g728Decode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;

    decoder->state = state;

    return decoder;
}

int
g728DeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    free(decoder->state);
    free(decoder);

    return 1;
}

/* Comprime tramas de 40 bytes en tramas de 40bits=5bytes.
 * Por lo que espera nSamples multiplo de 20 ya que nSamples
 * es nBytes/2
 * Devuelve el tamaño del buffer comprimido
 */
int
g728Encode(void *context,
           unsigned char *inBuff,
           int nSamples,
           unsigned char *outBuff
          )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    unsigned int i, j, nFrames, n;
    double speech[IDIM], rscale;

    LDCELP_STATE *state= (LDCELP_STATE *)coder->state;
    short int *buffer= (short int *)inBuff;

    unsigned char aux[8]; //8bytes * 5 bitesbajos = 40 bites => 40 / 10 = 4 tramas de 5samples = 20samples*2bytes/sample = 40bytes

    rscale = 0.128;
    nFrames = nSamples / 20;
    if (nSamples % 20)
    {
        perror("g728Encode::Numero de muestras incorrecta");
        nSamples = nSamples / 40;
    }

    for ( n = 0 ; n < nFrames ; n++)
    {
        // Primero comprime 5*4=20muestras=40bytes=320 bits en 40 bits,
        // repartidos en los 5bits de menos peso de aux = 8bytes
        for ( i = 0 ; i < 4 ; i++ )
        {
            unsigned int ix;

            // Este bucle comprime 5muestras=10bytes=80bits en 10bits
            for ( j = 0 ; j < IDIM ; j++ )
            {
                 speech[j] = rscale*(double)(*buffer++);
            }
            ix = encode_vector(state, speech);
            adapt_encoder(state);
            NEXT_FFASE(state);
            ix = ix & 0x03FF;             // Solo valen los 10 bits menos significativos
            aux[i*2] = (ix & 0x1F);
            aux[i*2+1] = (ix >> 5) & 0x1F;
        }
        // Pasar los 40 bits de 8 bytes a 5 bytes=8*5=40.
        // 5bajos del 0 + 3bajos del 1
        outBuff[0] = (aux[0]&0x1F) | ((aux[1]&0x07)<<5);
        // 2altos del 1 + 5bajos del 2 + 1bajo del 3
        outBuff[1] = ((aux[1]>>3)&0x03) | ((aux[2]&0x17)<<2) | ((aux[3]&0x01)<<7);
        // 4altos del 3 + 4bajos del 4
        outBuff[2] = ((aux[3]>>1)&0x0F) | ((aux[4]&0x0F)<<4);
        // 1alto del 4 + 5bajos del 5 + 2bajos del 6
        outBuff[3] = ((aux[4]>>4)&0x01) | ((aux[5]&0x1F)<<1) | ((aux[6]&0x3)<<6);
        // 3altos del 6 + 5bajos del 7
        outBuff[4] = ((aux[6]>>2)&0x07) | ((aux[7]&0x1F)<<3);
        outBuff += 5;
    }

    return nSamples * coder->bytesPerSample / coder->invRatio;
}

/* Devuelve el tamaño del buffer escrito */
int
g728Decode(void *context,
           unsigned char *inBuff,
           int nBytes,
           unsigned char *outBuff
          )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    unsigned int i, j, nFrames, n;
    double speech[IDIM], rscale;
    unsigned char aux[8];

    LDCELP_STATE *state= (LDCELP_STATE *)decoder->state;

    short int *buffer= (short int *)outBuff;

    rscale = 0.125;
    nFrames = nBytes/5;
    if (nBytes % 5)
    {
        perror("g728Decode::Numero de bytes incorrecto");
    }

    /////////////// SOLO PARA USAR LDST dentro del bucle
    for ( n = 0 ; n < nFrames ; n++ )
    {
        // Desensamblo cada 5 bytes en 8 bytes que descomprimo
        // en 40bytes=20muestras
        // 5bajos de 0
        aux[0] = inBuff[0]&0x01F;
        // 3altos de 0 + 2bajos de 1
        aux[1] = ((inBuff[0]>>5)&0x07) | ((inBuff[1]&0x3)<<3);
        // 5medios de 1
        aux[2] = ((inBuff[1]>>2)&0x1F);
        // 1alto de 1 + 4bajos de 2
        aux[3] = ((inBuff[1]>>7)&0x01) | ((inBuff[2]&0x0F)<<1);
        // 4altos de 2 + 1bajo de 3
        aux[4] = ((inBuff[2]>>4)&0x0F) | ((inBuff[3]&0x01)<<4);
        // 5medios de 3
        aux[5] = ((inBuff[3]>>1)&0x1F);
        // 2altos de 3 + 3bajos de 4
        aux[6] = ((inBuff[3]>>6)&0x03) | ((inBuff[4]&0x07)<<2);
        // 5altos de 4
        aux[7] = ((inBuff[4]>>3)&0x1F);

        // Descomprimo 8bytes con sus 5bitslsb en
        // 20 muestras = 4vueltas * 5bytes/vuelta; 5muestras
        // cada 10bits => 10bytes=80bits
        for ( i = 0 ; i < 4 ; i++ )
        {
            unsigned int ix;

            ix = (aux[i*2] & 0x1F) | (aux[i*2+1]<<5);
            decode_vector(state, ix, speech);
            if (state->postfilter)
            {
                postfilt(state, speech, speech);
            }
            adapt_decoder(state);
            NEXT_FFASE(state);
            for ( j = 0 ; j < IDIM ; j++ )
            {
                double xx;

                xx = speech[j]/rscale;
                if ( xx > 0.0 )
                {
                   if ( xx > 32767.0 ) xx = 32767.0;
                   else xx += 0.5;
                }
                else
                {
                   if ( xx < -32767.0 ) xx = -32767.0;
                   else xx -= 0.5;
                }
                *buffer++ = (short int)xx;
            }
        }
        inBuff += 5;
    }

    return nBytes * 8;
}


