
#include <string.h>
#include <stdlib.h>

#include "codec.h"

#include "amrnbCodec.h"

extern "C"
{
#include "amr/typedef.h"
#include "amr/cnst.h"
#include "amr/n_proc.h"
#include "amr/mode.h"
#include "amr/frame.h"
#include "amr/strfunc.h"
#include "amr/sp_enc.h"
#include "amr/pre_proc.h"
#include "amr/sid_sync.h"
#include "amr/vadname.h"
#include "amr/e_homing.h"
#include "amr/sp_dec.h"
#include "amr/d_homing.h"
}


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


short packed_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};


typedef struct
{
    short dtx;
    enum Mode encMode;
    enum Mode used_mode;
    enum TXFrameType enc_tx_type;
    short enc_reset_flag;
    Speech_Encode_FrameState *speech_encoder_state;
    sid_syncState *sid_state;
}  amrnbEncState_t;

typedef struct
{
    enum Mode decMode;

    Speech_Decode_FrameState *speech_decoder_state;

    unsigned char decoded[4096];

    int  decodedLen;

    enum RXFrameType rx_type;
    enum TXFrameType dec_tx_type;
    short dec_reset_flag;
    short reset_flag_old;
} amrnbDecState_t;


aCoder_t*
amrnbNewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("amrnbNewCoder::malloc");
        return NULL;
    }

    amrnbEncState_t *state = (amrnbEncState_t*)malloc(sizeof(amrnbEncState_t));

    if ( ! state )
    {
        free(coder);
        perror("amrnbNewCoder::malloc");
        return NULL;
    }

    // encoder
    state->encMode= MR122;
    state->dtx    = 1;

    if (Speech_Encode_Frame_init(&state->speech_encoder_state, state->dtx, "encoder")
        || sid_sync_init (&state->sid_state)
       )
    {
        free(state);
        free(coder);
        printf("amrnbNewCoder::ERROR initializing amrnb!\n");
        return NULL;
    }

    coder->Delete= amrnbDeleteCoder;
    coder->Encode= amrnbEncode;

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

    amrnbEncState_t *state= (amrnbEncState_t *)coder->state;

    Speech_Encode_Frame_exit(&state->speech_encoder_state);
    sid_sync_exit(&state->sid_state);

    free(state);
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

    amrnbDecState_t *state = (amrnbDecState_t*)malloc(sizeof(amrnbDecState_t));

    if ( ! state )
    {
        free(decoder);
        perror("amrnbNewDecoder::malloc");
        return NULL;
    }

    state->decMode = (enum Mode)0;
    state->rx_type = (enum RXFrameType)0;
    state->dec_tx_type = (enum TXFrameType)0;
    state->dec_reset_flag = 0;
    state->reset_flag_old = 1;
    
    if (Speech_Decode_Frame_init(&state->speech_decoder_state, "Decoder"))
    {
        free(state);
        free(decoder);
        printf("amrnbNewDecoder::ERROR initializing amrnb!\n");
        return NULL;
    }

    decoder->Delete= amrnbDeleteDecoder;
    decoder->Decode= amrnbDecode;

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

    amrnbDecState_t *state= (amrnbDecState_t *)decoder->state;

    Speech_Decode_Frame_exit(&state->speech_decoder_state);

    free(state);
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
    short    output[4096];

    int encodedLen= 0;

    for (int n = 0 ; n < nSamples / 160 ; n++)
    {
        memset(output, 0, 4096 * sizeof(short));

        /* check for homing frame */
        state->enc_reset_flag = encoder_homing_frame_test(buffer);

        /* encode speech */
        Speech_Encode_Frame(state->speech_encoder_state,
                            state->encMode,
                            buffer,
                            output,
                            &state->used_mode
                           );
        /* include frame type and mode information in serial bitstream */
        sid_sync (state->sid_state,
                  state->used_mode,
                  &state->enc_tx_type
                 );

        int bits = PackBits(state->used_mode,
                            state->encMode,
                            state->enc_tx_type,
                            output,
                            &outBuff[encodedLen]
                           );

        encodedLen += bits;

        buffer += 160;
    }

    return encodedLen;
}


int
amrnbDecode(void *context,
            unsigned char *inBuff,
            int nBytes,
            unsigned char *outBuff
           )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    amrnbDecState_t *state = (amrnbDecState_t*)decoder->state;

    unsigned char *buffer = (unsigned char *)inBuff;

    short unpacked[1024];

    short *output = (short*)outBuff;

    int decodedLen= 0;

    for (int n = 0 ; n < nBytes ; )
    {
        memset(unpacked, 0, 1024 * sizeof(short));

        /* read rest of the frame based on ToC byte */

        unsigned char q  = (buffer[n] >> 2) & 0x01;
        unsigned char ft = (buffer[n] >> 3) & 0x0F;

        n++;

        state->rx_type = UnpackBits(q, ft,&buffer[n],&state->decMode,unpacked);


        if (state->rx_type == RX_NO_DATA)
        {
            state->decMode = state->speech_decoder_state->prev_mode;
        }
        else
        {
            state->speech_decoder_state->prev_mode = state->decMode;
        }

        /* if homed: check if this frame is another homing frame */
        if (state->reset_flag_old == 1)
        {
            /* only check until end of first subframe */
            state->dec_reset_flag = decoder_homing_frame_test_first(unpacked, state->decMode);
        }
        /* produce encoder homing frame if homed & input=decoder homing frame */
        if ((state->dec_reset_flag != 0) && (state->reset_flag_old != 0))
        {
            for (int i = 0; i < L_FRAME; i++)
            {
                output[i] = EHF_MASK;
            }
        }
        else
        {
            /* decode frame */
            Speech_Decode_Frame(state->speech_decoder_state,
                                state->decMode,
                                unpacked,
                                state->rx_type,
                                output
                               );
        }

        /* if not homed: check whether current frame is a homing frame */
        if (state->reset_flag_old == 0)
        {
            /* check whole frame */
            state->dec_reset_flag = decoder_homing_frame_test(unpacked, state->decMode);
        }
        /* reset decoder if current frame is a homing frame */
        if (state->dec_reset_flag != 0)
        {
            Speech_Decode_Frame_reset(state->speech_decoder_state);
        }
        n += packed_size[ft];
        state->reset_flag_old = state->dec_reset_flag;
        decodedLen += L_FRAME*2;
        output += L_FRAME;
    }

    return decodedLen;
}

