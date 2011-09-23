#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "codec.h"

#include "g711Codec.h"

#include "g711/g711Imp.h"

int g711DeleteCoder(void *context);
int g711DeleteDecoder(void *context);

/////////////////////////////////////////////////////
////////////   g711 alaw     ////////////////////////
/////////////////////////////////////////////////////
int g711_alawEncode(void *context,
                    unsigned char *inBuff,
                    int nSamples,
                    unsigned char *outBuff
                   );
int g711_alawDecode(void *context,
                    unsigned char *inBuff,
                    int nBytes,
                    unsigned char *outBuff
                   );
/////////////////////////////////////////////////////
////////////   g711 ulaw     ////////////////////////
/////////////////////////////////////////////////////
int g711_ulawEncode(void *context,
                    unsigned char *inBuff,
                    int nSamples,
                    unsigned char *outBuff
                   );
int g711_ulawDecode(void *context,
                    unsigned char *inBuff,
                    int nBytes,
                    unsigned char *outBuff
                   );


aCoder_t *
g711NewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("g711NewCoder::malloc");
        return NULL;
    }

    coder->Delete = g711DeleteCoder;

    switch (fmt)
    {
    case PCMA_8_FORMAT:
        coder->Encode = g711_alawEncode;

        break;

    case PCMU_8_FORMAT:
    case PCMU_16_FORMAT:
    case PCMU_22_FORMAT:
    case PCMU_44_FORMAT:
        coder->Encode = g711_ulawEncode;

        break;
    default:
        perror("g711NewCoder: cannot handle format format\n");
        free(coder);
        return NULL;
    }

    coder->format = fmt;
    coder->sampleRate= sampleRate;
    coder->bandwidth = bandwidth;
    coder->ratio = 1.0/2.0;
    coder->invRatio = 2;
    coder->bytesPerSample = 2;
    coder->minSamples = 1;
    coder->state = NULL;

    return coder;
}

int
g711DeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    free(coder);

    return 1;
}

aDecoder_t *
g711NewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("g711NewDecoder::malloc");
        return NULL;
    }

    decoder->Delete = g711DeleteDecoder;

    switch (fmt)
    {
    case PCMA_8_FORMAT:
        decoder->Decode = g711_alawDecode;

        break;

    case PCMU_8_FORMAT:
        decoder->Decode = g711_ulawDecode;

        break;
    case PCMU_16_FORMAT:
    case PCMU_22_FORMAT:
    case PCMU_44_FORMAT:
        perror("g711NewDecoder: G711.1 NOT IMPLEMENTED, FIX ME!");
        decoder->Decode = g711_ulawDecode;

        break;
    default:
        perror("g711NewDecoder: cannot handle format format\n");
        free(decoder);
        return NULL;
    }

    decoder->format = fmt;
    decoder->sampleRate= sampleRate;
    decoder->bytesPerSample = 2;
    decoder->state = NULL;

    return decoder;
}

int
g711DeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    free(decoder);

    return 1;
}

int
g711_alawEncode(void *context,
                unsigned char *inBuff,
                int nSamples,
                unsigned char *outBuff
               )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    short int *buffer= (short int *)inBuff;

    for (int i = 0 ; i < nSamples ; i++)
    {
        outBuff[i] = linear2alaw(buffer[i]);
    }

    return nSamples; // one byte per sample
}


int
g711_alawDecode(void *context,
                unsigned char *inBuff,
                int nBytes,
                unsigned char *outBuff
               )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    short int *buffer= (short int *)outBuff;

    for (int i = 0 ; i < nBytes ; i++)
    {
        buffer[i] = alaw2linear(inBuff[i]);
    }

    return nBytes * 2;
}

int
g711_ulawEncode(void *context,
                unsigned char *inBuff,
                int nSamples,
                unsigned char *outBuff
               )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    short int *buffer= (short int *)inBuff;

    for (int i = 0 ; i < nSamples ; i++)
    {
        outBuff[i] = linear2ulaw(buffer[i]);
    }

    return nSamples; // one byte per sample
}


int
g711_ulawDecode(void *context,
                unsigned char *inBuff,
                int nBytes,
                unsigned char *outBuff
               )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    short int *buffer= (short int *)outBuff;

    for (int i = 0 ; i < nBytes ; i++)
    {
        buffer[i] = ulaw2linear(inBuff[i]);
    }

    return nBytes * 2;
}

