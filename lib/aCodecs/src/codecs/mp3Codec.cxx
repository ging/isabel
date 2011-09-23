#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codec.h"

#include "mp3Codec.h"

#ifdef WIN32
#include "mp3/config.h"
#endif

#include "mp3/lame.h"

int mp3DeleteCoder(void *context);
int mp3DeleteDecoder(void *context);

int mp3Encode(void *context,
              unsigned char *inBuff,
              int nSamples,
              unsigned char *outBuff
             );
int mp3Decode(void *context,
              unsigned char *inBuff,
              int nBytes,
              unsigned char *outBuff
             );


typedef struct
{ 
    MPSTR mp;
    short decodedL[96000];
    short decodedR[96000];
} mp3DecState_t;

aCoder_t*
mp3NewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("mp3NewCodec::malloc");
        return NULL;
    }

    lame_global_flags *state= lame_init();

    if ( ! state )
    {
        free(coder);
        perror("mp3NewCoder::malloc");
        return NULL;
    }

    lame_set_num_channels(state, 1);
    lame_set_mode(state, MONO);
    lame_set_in_samplerate(state, sampleRate);
    lame_set_out_samplerate(state, sampleRate);
    lame_set_VBR(state, vbr_off);
    lame_set_brate(state, bandwidth); // in Kbps
    lame_set_VBR_min_bitrate_kbps(state, lame_get_brate(state));
    lame_set_quality(state, 2); // good algorithm
    lame_set_bWriteVbrTag(state, 0);
    lame_set_disable_reservoir(state, 1);

    if (lame_init_params(state) < 0)
    {
        lame_close(state);
        free(coder);
        perror("mp3NewCoder::improper parameters");
        return NULL;
    }

    int frameSize= lame_get_framesize(state);
    int bw= lame_get_brate(state);

    printf("LAME MP3: open at %dKbps [%d bytes per pkt]\n", bw, frameSize);

    coder->Delete = mp3DeleteCoder;
    coder->Encode = mp3Encode;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth  = bandwidth;
    coder->ratio = 0.17;
    coder->invRatio = 1;
    coder->bytesPerSample = 2;
    coder->minSamples = lame_get_framesize(state);

    coder->state = state;

    return coder;
}

int
mp3DeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    lame_global_flags *state = (lame_global_flags *)coder->state;

    lame_close(state);

    free(coder);

    return 1;
}

aDecoder_t*
mp3NewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("mp3NewDecoder::malloc");
        return NULL;
    }

    mp3DecState_t *state= (mp3DecState_t *)malloc(sizeof(mp3DecState_t));

    if ( ! state )
    {
        free(decoder);
        perror("mp3NewDecoder::malloc");
        return NULL;
    }

    lame_decode_init(&state->mp);

    decoder->Delete = mp3DeleteDecoder;
    decoder->Decode = mp3Decode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;

    decoder->state = state;

    return decoder;
}

int
mp3DeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    mp3DecState_t *state= (mp3DecState_t*)decoder->state;

    lame_decode_exit(&state->mp);

    free(decoder->state);

    free(decoder);

    return 1;
}

int
mp3Encode(void *context,
          unsigned char *inBuff,
          int nSamples,
          unsigned char *outBuff
         )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    lame_global_flags *state = (lame_global_flags *)coder->state;

    int ret = 0;

    if (nSamples % coder->minSamples == 0)
    {
        ret = lame_encode_buffer(state,
                                 (short *)inBuff, // MONO, by now
                                 NULL, //(short *)inBuff,
                                 nSamples,
                                 outBuff,
                                 nSamples * 2
                                );
    }
    else
    {
        printf("mp3Encode: Need exactly %d samples\n", coder->minSamples);
    }

    return ret;
}

int
mp3Decode(void *context,
          unsigned char *inBuff,
          int nBytes,
          unsigned char *outBuff
         )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    mp3DecState_t *state= (mp3DecState_t*)decoder->state;

    int samples= lame_decode(&state->mp, inBuff, nBytes, state->decodedL, state->decodedR);

    if (samples <= 0)
    {
        return samples;
    }

    memcpy(outBuff, state->decodedL, samples * 2);

    return samples * 2;
}

