#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "codec.h"

#include "speexCodec.h"

#ifdef WIN32
#include "WinXP/speex/speex.h"
#else
#include <speex/speex.h>
#endif

#define MAX_BYTES 1000

#define QUALITY 9

int speexDeleteCoder(void *context);
int speexDeleteDecoder(void *context);

int speexEncode(void *context,
                unsigned char *inBuff,
                int nSamples,
                unsigned char *outBuff
               );
int speexDecode(void *context,
                unsigned char *inBuff,
                int nBytes,
                unsigned char *outBuff
               );

typedef struct
{
    SpeexBits enc_bits;
    void *encoder_state;
} speexEncState_t;

typedef struct
{
    SpeexBits dec_bits;
    void *decoder_state;
    int frame_size;
    int samplesInFrame; // poner el de verdad?
} speexDecState_t;

aCoder_t *
speexNewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("speexNewCoder::malloc");
        return NULL;
    }

    speexEncState_t *state= (speexEncState_t *)malloc(sizeof(speexEncState_t));

    if ( ! state )
    {
        free(coder);
        perror("speexNewCoder::malloc");
        return NULL;
    }

    // Init the encoder
    int quality = QUALITY;
    const SpeexMode *mode;
    // if sample_rate <= 8000
    //     mode= &speex_nb_mode
    // else if sample_rate <= 16000
    //     mode= &speex_nb_mode
    // else
    //     mode= &speex_uwb_mode
    switch (fmt)
    {
    case SPEEX_8_FORMAT:
        mode= &speex_nb_mode;
        break;
    case SPEEX_16_FORMAT:
        mode= &speex_wb_mode;
        break;
    default:
        mode= &speex_uwb_mode;
        break;
    }

    speex_bits_init(&state->enc_bits);

    state->encoder_state = speex_encoder_init(mode);
    if ( ! state->encoder_state)
    {
        speex_bits_destroy(&state->enc_bits);

        free(state);
        free(coder);
        perror("speexNewCoder::speex_encoder_init failed");
        return NULL;
    }
    speex_encoder_ctl(state->encoder_state, SPEEX_SET_QUALITY, &quality);

    coder->Delete = speexDeleteCoder;
    coder->Encode = speexEncode;

    coder->format = fmt;
    coder->sampleRate = sampleRate;
    coder->bandwidth = bandwidth;
    switch (fmt)
    {
    case SPEEX_8_FORMAT:
        coder->ratio = 1.0f/10.0f; // PONER EL DE VERDAD
        coder->invRatio = 10;    // PONER EL DE VERDAD
        coder->minSamples = 160; // 20 msec
        break;
    case SPEEX_16_FORMAT:
        coder->ratio = 1.0f/5.0f; // PONER EL DE VERDAD
        coder->invRatio = 5;    // PONER EL DE VERDAD
        coder->minSamples = 320; // 20 msec
        break;
    default:
        perror("speexNewCoder: cannot handle format format\n");

        speex_bits_destroy(&state->enc_bits);
        speex_encoder_destroy(state->encoder_state);

        free(state);
        free(coder);

        return NULL;
    }
    coder->bytesPerSample = 2;

    coder->state = state;

    return coder;
}

int
speexDeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    speexEncState_t *state= (speexEncState_t*)coder->state;

    speex_bits_destroy(&state->enc_bits);

    speex_encoder_destroy(state->encoder_state);

    free(state);
    free(coder);

    return 1;
}

aDecoder_t*
speexNewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("speexNewDecoder::malloc");
        return NULL;
    }

    speexDecState_t *state= (speexDecState_t *)malloc(sizeof(speexDecState_t));

    if ( ! state )
    {
        free(decoder);
        perror("speexNewDecoder::malloc");
        return NULL;
    }

    const SpeexMode *mode;
    // if sample_rate <= 8000
    //     mode= &speex_nb_mode
    // else if sample_rate <= 16000
    //     mode= &speex_nb_mode
    // else
    //     mode= &speex_uwb_mode
    switch (fmt)
    {
    case SPEEX_8_FORMAT:
        mode= &speex_nb_mode;
        break;
    case SPEEX_16_FORMAT:
        mode= &speex_wb_mode;
        break;
    default:
        mode= &speex_uwb_mode;
        break;
    }

    speex_bits_init(&state->dec_bits);

    state->decoder_state = speex_decoder_init(mode);
    if ( ! state->decoder_state)
    {
        speex_bits_destroy(&state->dec_bits);
        free(state);
        free(decoder);
        perror("speexNewDecoder::speex_decoder_init failed");
        return NULL;
    }

    speex_decoder_ctl(state->decoder_state,
                      SPEEX_GET_FRAME_SIZE,
                      &state->frame_size
                     );

    switch (fmt)
    {
    case SPEEX_8_FORMAT:
        state->samplesInFrame = 160; // 20 msec
        break;
    case SPEEX_16_FORMAT:
        state->samplesInFrame = 320; // 20 msec
        break;
    default:
        perror("speexNewDecoder: cannot handle format format\n");

        speex_bits_destroy(&state->dec_bits);
        speex_decoder_destroy(state->decoder_state);

        free(state);
        free(decoder);

        return NULL;
    }

    decoder->Delete = speexDeleteDecoder;
    decoder->Decode = speexDecode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;

    decoder->state = state;

    return decoder;
}

int
speexDeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    speexDecState_t *state= (speexDecState_t*)decoder->state;

    speex_bits_destroy(&state->dec_bits);

    speex_decoder_destroy(state->decoder_state);

    free(state);
    free(decoder);

    return 1;
}

int
speexEncode(void *context,
            unsigned char *inBuff,
            int nSamples,
            unsigned char *outBuff
           )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    int encoded = 0;
    speexEncState_t *state = (speexEncState_t *)coder->state;

    if (nSamples == coder->minSamples)
    {
        speex_bits_reset(&state->enc_bits);
        speex_encode_int(state->encoder_state,(short*)inBuff, &state->enc_bits);
        encoded = speex_bits_write(&state->enc_bits, (char*)outBuff, MAX_BYTES);
    }
    else
    {
        printf("speexEncode: Need exactly %d samples\n", coder->minSamples);
    }

    return encoded;
}

int
speexDecode(void *context,
            unsigned char *inBuff,
            int nBytes,
            unsigned char *outBuff
           )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    speexDecState_t *state = (speexDecState_t *)decoder->state;

    speex_bits_read_from(&state->dec_bits, (char*)inBuff, nBytes);

    int res = speex_decode_int(state->decoder_state,
                               &state->dec_bits,
                               (short*)outBuff
                              );

    if (res < 0)
    {
        return 0;
    }
    else
    {
        // repasar
        return state->samplesInFrame * decoder->bytesPerSample;
    }
}

