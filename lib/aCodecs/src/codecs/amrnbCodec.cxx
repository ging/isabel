
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <opencore-amrnb/interf_enc.h>
#include <opencore-amrnb/interf_dec.h>

#include "codec.h"

#include "amrnbCodec.h"

int amrnbDeleteCoder(void *context);
int amrnbDeleteDecoder(void *context);

int amrnbEncode(void *context,
                unsigned char *inBuff,
                int nSamples,
                unsigned char *outBuff
               );
int amrnbDecode(void *context,
                unsigned char *inBuff,
                int nBytes,
                unsigned char *outBuff
               );


static const unsigned char block_size_nb[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};


struct amrnbEncState_t
{
    void *encstate;
    int dtx;
    enum Mode mode;
};


aCoder_t*
amrnbNewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("noneNewCoder::malloc");
        return NULL;
    }

    amrnbEncState_t *state= (amrnbEncState_t*)malloc(sizeof(amrnbEncState_t));

    if ( ! state )
    {
        free(coder);
        perror("speexNewDecoder::malloc");
        return NULL;
    }

    // encoder
    state->dtx  = 1;
    state->mode = MR122;

    state->encstate= Encoder_Interface_init(0);

    if ( ! state->encstate )
    {
        free(state);
        free(coder);
        printf("amrnbCoder_t::ERROR creating codec!\n");
        return NULL;
    }

    coder->Delete = amrnbDeleteCoder;
    coder->Encode = amrnbEncode;

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
amrnbDeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    amrnbEncState_t *state = (amrnbEncState_t *)coder->state;

    Encoder_Interface_exit(state->encstate);

    free(coder->state);

    free(coder);

    return 1;
}


aDecoder_t*
amrnbNewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("amrnbNewDecoder::malloc");
        return NULL;
    }

    void *state= Decoder_Interface_init();

    if ( ! state)
    {
        free(decoder);
        printf("amrnbDecoder_t::malloc!\n");
        return NULL;
    }

    decoder->Delete = amrnbDeleteDecoder;
    decoder->Decode = amrnbDecode;

    decoder->format = fmt;
    decoder->sampleRate = sampleRate;
    decoder->bytesPerSample = 2;

    decoder->state = state;

    return decoder;
}


int
amrnbDeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    Decoder_Interface_exit(decoder->state);

    free(decoder);

    return 1;
}


int
amrnbEncode(void *context,
            unsigned char *inBuff,
            int nSamples,
            unsigned char *outBuff
           )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    amrnbEncState_t *state = (amrnbEncState_t*)coder->state;

    short *buffer = (short *)inBuff;

    int written= 0;

    for (int n= 0; n < nSamples / 160; n++)
    {
        int len= Encoder_Interface_Encode(state->encstate,
                                          state->mode,
                                          buffer,
                                          outBuff,
                                          0
                                         );
        written += len;
        outBuff += len;
        buffer  += 160;
    }

    return written;
}


int
amrnbDecode(void *context,
          unsigned char *inBuff,
          int nBytes,
          unsigned char *outBuff
         )
{
    aDecoder_t *codec= static_cast<aDecoder_t*>(context);

    void *state = codec->state;

    int decodedLen= 0;

    short *output = (short*)outBuff;

    const unsigned char *amrData= inBuff;

    enum Mode dec_mode;
    int packet_size;

    for (int n= 0; n < nBytes; )
    {
        dec_mode= (Mode)((amrData[0] >> 3) & 0x000F);
        packet_size= block_size_nb[dec_mode];

#if 0
si lo pongo, no se oye
        if (packet_size > nBytes)
        {
            printf("amrnbDecode: amrnb frame too short (%u, should be %u)\n",
                   nBytes,
                   packet_size
                  );
            return -1;
        }
#endif

        Decoder_Interface_Decode(state, amrData, output, 0);

        amrData += packet_size + 1;
        n += packet_size + 1;

        decodedLen += (160 * 2);
        output += 160;
    }

    return decodedLen;
}

