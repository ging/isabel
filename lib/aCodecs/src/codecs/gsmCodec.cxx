
#include <string.h>
#include <stdlib.h>

#include "codec.h"

#include "gsmCodec.h"

#ifdef WIN32
#include "WinXP/gsm/gsm.h"
#else
#include <gsm/gsm.h>
#endif


int gsmDeleteCoder(void *context);
int gsmDeleteDecoder(void *context);

int gsmEncode(void *context,
              unsigned char *inBuff,
              int nSamples,
              unsigned char *outBuff
             );
int gsmDecode(void *context,
              unsigned char *inBuff,
              int nBytes,
              unsigned char *outBuff
             );

typedef struct
{
   struct gsm_state *encoder;
   struct gsm_state *decoder;
} gsmState;

aCoder_t *
gsmNewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("gsmNewCoder::malloc");
        return NULL;
    }

    struct gsm_state *state= gsm_create();

    if ( ! state )
    {
        free(coder);
        perror("gsmNewCoder::malloc");
        return NULL;
    }

    coder->Delete = gsmDeleteCoder;
    coder->Encode = gsmEncode;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth = bandwidth;
    coder->ratio = 160/33; // 160 muestras de 13bits en 33 bytes
    coder->invRatio = 5; // realmente 4.84
    coder->bytesPerSample = 2;
    coder->minSamples = 160;

    coder->state = state;

    return coder;
}

int
gsmDeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    struct gsm_state *state= (struct gsm_state*)coder->state;

    gsm_destroy(state);

    free(coder);

    return 1;
}

aDecoder_t *
gsmNewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("gsmNewDecoder::malloc");
        return NULL;
    }

    struct gsm_state *state= gsm_create();

    if ( ! state )
    {
        free(decoder);
        perror("gsmNewDecoder::malloc");
        return NULL;
    }

    decoder->Delete = gsmDeleteDecoder;
    decoder->Decode = gsmDecode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;

    decoder->state = state;

    return decoder;
}

int
gsmDeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    struct gsm_state *state= (struct gsm_state*)decoder->state;

    gsm_destroy(state);

    free(decoder);

    return 1;
}

int
gsmEncode(void *context,
          unsigned char *inBuff,
          int nSamples,
          unsigned char *outBuff
         )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    struct gsm_state *state= (struct gsm_state*)coder->state;

    short *buffer = (short *)inBuff;

#ifdef WAV49
    // Microsoft format WAV49

    gsm_encode(state, buffer, outBuff);
    gsm_encode(state, buffer + 160, outBuff + 32);

    return 65;

#else  // the other format

    for (int n = 0 ; n < nSamples/160 ; n++)
    {
        gsm_encode(state, buffer + n * 160, outBuff + n * 33);
    }

    return nSamples / 160 * 33;
#endif
}

int
gsmDecode(void *context,
          unsigned char *inBuff,
          int nBytes,
          unsigned char *outBuff
         )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    struct gsm_state *state= (struct gsm_state*)decoder->state;

    short *buffer = (short int *)outBuff;

    if (nBytes == 65) // decoding microsoft format
    {
#ifdef WIN32
        gsm_decode_wav49(state, inBuff, buffer);
        gsm_decode_wav49(state, inBuff + 33, buffer + 160);
#else
        gsm_decode(state, inBuff, buffer);
        gsm_decode(state, inBuff + 33, buffer + 160);
#endif

        return 160 * 2 * 2;

    }
    else
    {  // the other format

        for (int n = 0 ; n < nBytes / 33 ; n++ )
        {
            gsm_decode(state, inBuff + n * 33, buffer + n * 160);
        }
    }

    return nBytes / 33 * 160 * decoder->bytesPerSample;
}

