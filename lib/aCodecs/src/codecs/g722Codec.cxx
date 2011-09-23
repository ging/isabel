#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codec.h"

#include "g722Codec.h"

#include "g722/codif.h"
#include "g722/decod.h"

#define G722CODNUMSAMPLES    2

int g722DeleteCoder(void *context);
int g722DeleteDecoder(void *context);

int g722Encode(void *context,
               unsigned char *inBuff,
               int nSamples,
               unsigned char *outBuff
              );
int g722Decode(void *context,
               unsigned char *inBuff,
               int nBytes,
               unsigned char *outBuff
              );

aCoder_t *
g722NewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( !coder )
    {
        perror("g722NewCoder::malloc");
        return NULL;
    }

    G722StateEncoder *state= G722StateEncoderNew();

    if ( ! state )
    {
        free(coder);
        perror("g722NewCoder::malloc");
        return NULL;
    }

    coder->Delete = g722DeleteCoder;
    coder->Encode = g722Encode;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth = bandwidth;
    coder->ratio = 1.0/4.0;
    coder->invRatio = 4;
    coder->bytesPerSample = 2;
    coder->minSamples = 2;

    coder->state = state;

return coder;
}

int
g722DeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    G722StateEncoder *state= (G722StateEncoder *)coder->state;

    free(state);
    free(coder);

    return 1;
}

aDecoder_t *
g722NewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("g722NewDecoder::malloc");
        return NULL;
    }

    G722StateDecoder *state= G722StateDecoderNew();

    if ( ! state )
    {
        free(decoder);
        perror("g722NewDecoder::malloc");
        return NULL;
    }

    decoder->Delete = g722DeleteDecoder;
    decoder->Decode = g722Decode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;

    decoder->state = state;

    return decoder;
}

int
g722DeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    G722StateDecoder *state= (G722StateDecoder *)decoder->state;

    free(state);
    free(decoder);

    return 1;
}

/*
 * Espera un buffer con muestras de 16 bits 16KHz
 * y genera un buffer que ocupa 1/4 del tamaño del original.
 * Comprime cada dos muestras de 16bits 16KHz en un byte. Ratio 4:1
 * Devuelve el tamaño del buffer comprimido
 */
int
g722Encode(void *context,
           unsigned char *inBuff,
           int nSamples,
           unsigned char *outBuff
          )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    G722StateEncoder *state= (G722StateEncoder *)coder->state;

    short int *buffer= (short int *)inBuff;

    int nframes = nSamples / coder->bytesPerSample;

    for (int i = 0 ; i < nframes ; i++)
    {
        short int x1,x2;
        short int xL,xH;

        x1 = *buffer++;
        x2 = *buffer++;

        calcTxQmf(state, &xL, &xH, x1, x2);

        *outBuff++ = codif(state, xL, xH);
    }

    return nSamples * coder->bytesPerSample / coder->invRatio;
}

#define G722MODE 2  /* Puede ser 1, 2 o 3. Por ahora fijo. */

/*
 * Espera un buffer con muestras comprimidas con g722,
 * cada muestra ocupa un byte
 * Genera 4 bytes por cada byte de entrada. Genera formato PCM 16bits 16Khz.
 * Devuelve el tamaño del buffer escrito
 */
int
g722Decode(void *context,
           unsigned char *inBuff,
           int nBytes,
           unsigned char *outBuff
          )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    G722StateDecoder *state= (G722StateDecoder *)decoder->state;

    short int *buffer= (short int *)outBuff;

    for (int i= 0 ; i < nBytes; i++)
    {
        short int x3,x4, salest[2];

        decod(state, *inBuff++, salest, G722MODE);

        calcRxQmf(state, salest[1], salest[0], &x3, &x4);

        *buffer++ = x3;
        *buffer++ = x4;
    }

    // Porque comprime 4:1

    return nBytes * 4;
}

