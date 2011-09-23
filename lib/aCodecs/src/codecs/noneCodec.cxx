#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "noneCodec.h"
#include "codec.h"

int noneDeleteCoder(void *context);
int noneDeleteDecoder(void *context);

int noneEncode(void *context,
               unsigned char *inBuff,
               int nSamples,
               unsigned char *outBuff
              );
int noneDecode(void *context,
               unsigned char *inBuff,
               int nBytes,
               unsigned char *outBuff
              );

aCoder_t *
noneNewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("noneNewCoder::malloc");
        return NULL;
    }

    coder->Delete = noneDeleteCoder;
    coder->Encode = noneEncode;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth = bandwidth;
    coder->ratio = 1.0;
    coder->invRatio = 1;
    coder->bytesPerSample = 2;
    coder->minSamples = 1;
    coder->state = NULL;

    return coder;
}

int
noneDeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    free(coder);

    return 1;
}

aDecoder_t *
noneNewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("noneNewDecoder::malloc");
        return NULL;
    }

    decoder->Delete = noneDeleteDecoder;
    decoder->Decode = noneDecode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;
    decoder->state = NULL;

    return decoder;
}

int
noneDeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    free(decoder);

    return 1;
}

int
noneEncode(void *context,
           unsigned char *inBuff,
           int nSamples,
           unsigned char *outBuff
          )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    memcpy(outBuff, inBuff, nSamples*2);

    return nSamples * 2;
}

int
noneDecode(void *context,
           unsigned char *inBuff,
           int nBytes,
           unsigned char *outBuff
          )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    memcpy(outBuff, inBuff, nBytes);

    return nBytes;
}

