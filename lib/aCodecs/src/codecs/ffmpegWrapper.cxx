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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "codec.h"

#include "ffmpegWrapper.h"

extern "C"
{
#include <libavcodec/avcodec.h>
}

// function declaration

int ffmpegDeleteCoder(void *context);
int ffmpegDeleteDecoder(void *context);

int ffmpegEncode(void          *context,
                 unsigned char *inBuff,
                 int            nSamples,
                 unsigned char *outBuff
                );
int ffmpegDecode(void          *context,
                 unsigned char *inBuff,
                 int            nBytes,
                 unsigned char *outBuff
                );


int
sampleRateByFmt(int fmt)
{
    switch (fmt)
    {
    case MP3_8_FORMAT : return  8000;
    case MP3_16_FORMAT: return 16000;
    case MP3_32_FORMAT: return 32000;
    case MP3_44_FORMAT: return 44100;
    case MP3_48_FORMAT: return 48000;
    }
    throw "sampleRateBytFmt: cannot handle format";
}

int
bitRateByFmt(int fmt)
{
    switch (fmt)
    {
    case MP3_8_FORMAT : return  22000;
    case MP3_16_FORMAT: return  43000;
    case MP3_32_FORMAT: return  85000;
    case MP3_44_FORMAT: return 117000;
    case MP3_48_FORMAT: return 128000;
    }
    throw "bitRateBytFmt: cannot handle format";
}

enum CodecID
fmt2ffmpegCoderID(int fmt)
{
    switch (fmt)
    {
    case MP3_8_FORMAT :
    case MP3_16_FORMAT:
    case MP3_32_FORMAT:
    case MP3_44_FORMAT:
    case MP3_48_FORMAT: return CODEC_ID_MP3;
    }
    throw "fmt2ffmpegCoderID: cannot handle format";
}

enum CodecID
fmt2ffmpegDecoderID(int fmt)
{
    switch (fmt)
    {
    case MP3_8_FORMAT :
    case MP3_16_FORMAT:
    case MP3_32_FORMAT:
    case MP3_44_FORMAT:
    case MP3_48_FORMAT: return CODEC_ID_MP3;
    }
    throw "fmt2ffmpegDecoderID: cannot handle format";
}


//---------------------------------------------
// Functions
//---------------------------------------------

static bool ffmpegInitialized= false;

int
FFMPEG_global_init(void)
{
    if (ffmpegInitialized) return 0;

    ffmpegInitialized= true;

    printf("Using LIBAVCODEC VERSION= %d\n", LIBAVCODEC_VERSION_INT);

    // initialize libavcodec
    avcodec_init();

    //av_log_set_level(AV_LOG_ERROR);
    //av_log_set_level(AV_LOG_QUIET);
    av_log_set_level(AV_LOG_DEBUG);
    //printf("Es conveniente quitar log_set_level\n");

    // register ffmpeg encoders and decoders
    avcodec_register_all();

    return 0;
}

aCoder_t *
ffmpegNewCoder(int fmt, int sampleRate, int bandwidth)
{
    aCoder_t *coder= (aCoder_t *)malloc(sizeof(aCoder_t));

    if ( ! coder )
    {
        perror("ffmpegNewCoder::malloc");
        return NULL;
    }

    FFMPEG_global_init();

    enum CodecID ffmpegID= fmt2ffmpegCoderID(fmt);

    AVCodec *avcodec= NULL;
    AVCodecContext *avcodecCtx= NULL;

    /* find the audio encoder */
    avcodec = avcodec_find_encoder(ffmpegID);
    if ( ! avcodec)
    {
        printf("ffmpegNewCoder: coder not found for %d!!\n", ffmpegID);
        return NULL;
    }

    avcodecCtx = avcodec_alloc_context();
    if ( ! avcodecCtx)
    {
        printf("ffmpegNewCoder: run out of memory\n");
        return NULL;
    }

    /* put sample parameters */
    avcodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    avcodecCtx->bit_rate   = bitRateByFmt(fmt);     // fix it!
    avcodecCtx->sample_rate= sampleRateByFmt(fmt); //fix it!
    avcodecCtx->channels   = 1;

    /* open it */
    if (avcodec_open(avcodecCtx, avcodec) < 0)
    {
        printf("ffmpegNewCoder: could not open coder for %d\n", ffmpegID);
        abort();
    }

    coder->Delete= ffmpegDeleteCoder;
    coder->Encode= ffmpegEncode;

    coder->format = fmt;
    coder->ratio = 0.17;
    coder->invRatio = 1;
    coder->bytesPerSample = 2;
    coder->minSamples = 1152;
    coder->state = (void*)avcodecCtx;

    return coder;
}

int
ffmpegDeleteCoder(void *context)
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    AVCodecContext *state= static_cast<AVCodecContext *>(coder->state);

    avcodec_close(state);
    av_free(state);

    free(coder);

    return 1;
}

aDecoder_t *
ffmpegNewDecoder(int fmt, int sampleRate)
{
    aDecoder_t *decoder= (aDecoder_t *)malloc(sizeof(aDecoder_t));

    if ( ! decoder )
    {
        perror("ffmpegNewDecoder::malloc");
        return NULL;
    }

    FFMPEG_global_init();

    enum CodecID ffmpegID= fmt2ffmpegCoderID(fmt);

    AVCodec *avcodec= NULL;
    AVCodecContext *avcodecCtx= NULL;

    /* find the audio decoder */
    avcodec = avcodec_find_decoder(ffmpegID);
    if ( ! avcodec)
    {
        char errorMsg[1024];
        sprintf(errorMsg,
                "ffmpegNewDecoder: decoder not found for %d!!\n",
                ffmpegID
               );
        perror(errorMsg);
        return NULL;
    }

    avcodecCtx = avcodec_alloc_context();
    if ( ! avcodecCtx)
    {
        perror("ffmpegNewDecoder: run out of memory\n");
        return NULL;
    }

    /* put sample parameters */
    avcodecCtx->bit_rate   = bitRateByFmt(fmt);    // fix it!
    avcodecCtx->sample_rate= sampleRateByFmt(fmt); //fix it!
    avcodecCtx->channels   = 1;

    /* open it */
    if (avcodec_open(avcodecCtx, avcodec) < 0)
    {
        fprintf(stderr,
                "FFMPEG_new_decoder: could not open decoder for %d\n",
                ffmpegID
               );
        return NULL;
    }

    decoder->Delete= ffmpegDeleteDecoder;
    decoder->Decode= ffmpegDecode;

    decoder->format = fmt;
    decoder->bytesPerSample = 2;
    decoder->state = (void*)avcodecCtx;

    return decoder;
}

int
ffmpegDeleteDecoder(void *context)
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    AVCodecContext *state= static_cast<AVCodecContext *>(decoder->state);

    avcodec_close(state);
    av_free(state);

    free(decoder);

    return 1;
}

int
ffmpegEncode(void          *context,
             unsigned char *inBuff,
             int            nSamples,
             unsigned char *outBuff
            )
{
    aCoder_t *coder= static_cast<aCoder_t*>(context);

    AVCodecContext *state= static_cast<AVCodecContext *>(coder->state);

    int outSize= -1;

    /* encode the audio samples */
    outSize = avcodec_encode_audio(state,
                                   outBuff,
                                   nSamples * 2,
                                   (short*)inBuff
                                  );

    return outSize;
}

int
ffmpegDecode(void          *context,
             unsigned char *inBuff,
             int            nBytes,
             unsigned char *outBuff
            )
{
    aDecoder_t *decoder= static_cast<aDecoder_t*>(context);

    AVCodecContext *state= static_cast<AVCodecContext *>(decoder->state);

    AVPacket avpkt1, *avpkt= &avpkt1;

    av_init_packet(avpkt);

    avpkt->data = inBuff;
    avpkt->size = nBytes;

    int outSize, decSize= 0, len= -1;

    int16_t *decBuff= (int16_t*)malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

    if ( ! decBuff )
    {
        perror("ffmpegDecode: malloc\n");
        return -1;
    }

    while (avpkt->size > 0)
    {
        outSize= AVCODEC_MAX_AUDIO_FRAME_SIZE;
        len = avcodec_decode_audio3(state,
                                    decBuff,
                                    &outSize,
                                    avpkt
                                   );
        if (len < 0)
        {
            perror("ffmpegDecode: Error while decoding frame\n");
            free(decBuff);
            return -1;
        }

        avpkt->size -= len;
        avpkt->data += len;

        if (outSize <= 0)
        {
            continue;
        }

        memcpy(outBuff, decBuff, outSize);
        outBuff+= outSize;
        decSize+= outSize;
    }

    free(decBuff);

    if ( outSize <= 0)
    {
        perror("ffmpegDecoder: Error due to wrong size\n");
        return -1;
    }

    return decSize;
}

