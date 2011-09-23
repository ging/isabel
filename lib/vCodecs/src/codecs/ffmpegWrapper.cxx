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
// $Id: ffmpegWrapper.cxx 22262 2011-04-27 12:33:01Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ffmpegWrapper.h"

extern "C"
{
#include <libavcodec/avcodec.h>

// older versions have AVPacket in avformat.h
// newer versions have AVPackert in avcoded.h
#if LIBAVCODEC_VERSION_INT < ((52<<16)+(23<<8)+0)
#include <libavformat/avformat.h>
#else
// nothing, AVPacket is defined in avcodec.h
#endif
}


enum CodecID
fmt2ffmpegCoderID(u32 fmt)
{
    switch (fmt)
    {
    case H263_FORMAT : return CODEC_ID_H263;
    case H263P_FORMAT: return CODEC_ID_H263P;
    case H264_FORMAT : return CODEC_ID_H264;
    case MPEG1_FORMAT: return CODEC_ID_MPEG1VIDEO;
    case MPEG2_FORMAT: return CODEC_ID_MPEG2VIDEO;
    case MPEG4_FORMAT: return CODEC_ID_MPEG4;
    case XVID_FORMAT : return CODEC_ID_XVID;
    }
    throw "fmt2ffmpegCoderID: cannot handle format";
}

enum CodecID
fmt2ffmpegDecoderID(u32 fmt)
{
    switch (fmt)
    {
    case H263_FORMAT :
    case H263P_FORMAT: return CODEC_ID_H263; // both for H263 & H263+
    case H264_FORMAT : return CODEC_ID_H264;
    case MPEG1_FORMAT:
    case MPEG2_FORMAT: return CODEC_ID_MPEG2VIDEO; // MPEG-1/2
    case MPEG4_FORMAT: return CODEC_ID_MPEG4;
    case XVID_FORMAT : return CODEC_ID_XVID;
    }
    throw "fmt2ffmpegDecoderID: cannot handle format";
}


// FFMPEG structures

struct FFMPEGenv_t
{
    AVCodecContext *c;
    AVFrame *picture;
};

// function declaration

int
FFMPEG_encode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            );

void
FFMPEG_delete_coder(void *context);

int
FFMPEG_decode(void           *context,
             unsigned char  *inBuff,
             int             inBuffLen,
             unsigned char  *outBuff,
             int             outBuffLen
            );

void
FFMPEG_delete_decoder(void *context);

void
FFMPEG_get_coder_params(void *context, vCoderArgs_t *params);

void
FFMPEG_get_decoder_params(void *context, vDecoderArgs_t *params);


//---------------------------------------------
// Local functions declaration
//---------------------------------------------
int
FFMPEG_encode(unsigned char *inBuff,
             unsigned int   inBuffLen,
             unsigned char *outBuff,
             unsigned int   outBuffLen
            );

int
FFMPEG_decode(unsigned char *inBuff,
             unsigned int   inBuffLen,
             unsigned char *outBuff,
             unsigned int   outBuffLen
            );

//---------------------------------------------
// Functions
//---------------------------------------------

int
FFMPEG_global_init(void)
{
    printf("Using LIBAVCODEC VERSION= %d\n", LIBAVCODEC_VERSION_INT);

    // initialize libavcodec
    avcodec_init();

    //av_log_set_level(AV_LOG_ERROR);
    av_log_set_level(AV_LOG_QUIET);
    //av_log_set_level(AV_LOG_DEBUG);
    //printf("Es conveniente quitar log_set_level\n");

    // register ffmpeg encoders and decoders
    avcodec_register_all();

    return 0;
}

vCoder_t *
FFMPEG_new_coder(u32 fmt, vCoderArgs_t *params)
{
    assert ( (params->frameRate > 0)
            &&
            "FFMPEG_new_coder: frame rate must be positive"
           );

    assert ( (params->width % 16 == 0 && params->height % 16 == 0)
            &&
            "FFMPEG_new_coder::geometry must be %%16"
           );

    enum CodecID ffmpegID= fmt2ffmpegCoderID(fmt);

    vCoder_t *FFMPEGCoder = new vCoder_t;

    AVCodec *coder;
    AVCodecContext *c;

    /* find the video encoder */
    coder = avcodec_find_encoder(ffmpegID);
    if ( ! coder)
    {
        printf("FFMPEG_new_coder: coder not found for %d!!\n", ffmpegID);
        return NULL;
    }

    c = avcodec_alloc_context();
    if ( ! c)
    {
        printf("FFMPEG_new_coder: run out of memory\n");
        return NULL;
    }

    /* put sample parameters */
    c->bit_rate = params->bitRate;
    c->bit_rate_tolerance = 1.1 * c->bit_rate / params->frameRate;
    c->rc_max_rate = c->bit_rate * 2;

    if (params->frameRate >= 1.0)
    {
        c->rc_buffer_size = params->bitRate; // 1 second stored, in bps
    }
    else
    {
        c->rc_buffer_size = 1.1 * params->bitRate / params->frameRate;
    }
    c->rc_buffer_aggressivity = 1.0;

    /* resolution must be a multiple of two */
    c->width = params->width;
    c->height= params->height;

    /* image pixel format */
    c->pix_fmt = PIX_FMT_YUV420P;

    /* frames per second */
    c->time_base= (AVRational){1000, 1000*params->frameRate};

    /* emit one intra frame every n frames */
    c->gop_size = params->maxInter;

    /* does not emit B frames */
    c->max_b_frames= 0;

    /* frame skip comparison function */
    //c->frame_skip_cmp = FF_CMP_DCTMAX;

    /* motion estimation method (epzs) */
    c->me_method= ME_EPZS;

    /* open it */
    if (avcodec_open(c, coder) < 0)
    {
        printf("FFMPEG_new_coder: could not open coder for %d\n", ffmpegID);
        abort();
    }

    // frame for coding pictures
    AVFrame *picture;
    picture= avcodec_alloc_frame();
    if ( ! picture)
    {
        printf("FFMPEG_new_coder: run out of memory\n");
        abort();
    }

    // environment with FFMPEG's codec and picture
    FFMPEGenv_t *ffmpegEnv= new FFMPEGenv_t();
    ffmpegEnv->c= c;
    ffmpegEnv->picture= picture;

    FFMPEGCoder->encoderContext = (void*)ffmpegEnv;
    FFMPEGCoder->Delete         = FFMPEG_delete_coder;
    FFMPEGCoder->Encode         = FFMPEG_encode;
    FFMPEGCoder->GetParams      = FFMPEG_get_coder_params;
    FFMPEGCoder->format         = fmt;

    // accepts only YUV420P
    params->format= I420P_FORMAT;

    memcpy(&FFMPEGCoder->params, params, sizeof(vCoderArgs_t));

    return FFMPEGCoder;
}

void
FFMPEG_delete_coder(void *context)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);
    FFMPEGenv_t *ffmpegEnv= static_cast<FFMPEGenv_t *>(coder->encoderContext);

    if (ffmpegEnv->c)
    {
        avcodec_close(ffmpegEnv->c);
        av_free(ffmpegEnv->c);
    }
    if (ffmpegEnv->picture)
    {
        av_free(ffmpegEnv->picture);
    }
    delete ffmpegEnv;
    delete coder;
}

vDecoder_t *
FFMPEG_new_decoder(u32 fmt, vDecoderArgs_t *params)
{
    vDecoder_t *FFMPEGDecoder = new vDecoder_t;

    AVCodec *decoder;
    AVCodecContext *c;

    enum CodecID ffmpegID= fmt2ffmpegDecoderID(fmt);

    /* find the video decoder */
    decoder = avcodec_find_decoder(ffmpegID);
    if ( ! decoder)
    {
        fprintf(stderr,
                "FFMPEG_new_decoder: decoder not found for %d!!\n",
                ffmpegID
               );
        return NULL;
    }

    c = avcodec_alloc_context();
    if ( ! c)
    {
        fprintf(stderr, "FFMPEG_new_decoder: run out of memory\n");
        return NULL;
    }

    /* resolution must be a multiple of two */
    c->width = params->width;
    c->height= params->height;

    // Inform the codec that we can handle truncated bitstreams -- i.e.,
    // bitstreams where frame boundaries can fall in the middle of packets
    //if(decoder->capabilities & CODEC_CAP_TRUNCATED)
    //    c->flags|=CODEC_FLAG_TRUNCATED;

    /* open it */
    if (avcodec_open(c, decoder) < 0)
    {
        fprintf(stderr,
                "FFMPEG_new_decoder: could not open decoder for %d\n",
                ffmpegID
               );
        return NULL;
    }

    // picture for decoding frames
    AVFrame *picture = avcodec_alloc_frame();
    if ( ! picture)
    {
        fprintf(stderr, "FFMPEG_new_decoder: run out of memory\n");
        return NULL;
    }

    // environment with FFMPEG's codec and picture
    FFMPEGenv_t *ffmpegEnv= new FFMPEGenv_t();
    ffmpegEnv->c= c;
    ffmpegEnv->picture= picture;

    FFMPEGDecoder->decoderContext = (void*)ffmpegEnv;
    FFMPEGDecoder->Delete         = FFMPEG_delete_decoder;
    FFMPEGDecoder->Decode         = FFMPEG_decode;
    FFMPEGDecoder->GetParams      = FFMPEG_get_decoder_params;
    FFMPEGDecoder->format         = fmt;

    // returns only YUV420P
    params->format= I420P_FORMAT;

    memcpy(&FFMPEGDecoder->params, params, sizeof(vDecoderArgs_t));

    return FFMPEGDecoder;
}

void
FFMPEG_delete_decoder(void *context)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);
    FFMPEGenv_t *ffmpegEnv= static_cast<FFMPEGenv_t *>(decoder->decoderContext);

    if (ffmpegEnv->c)
    {
        avcodec_close(ffmpegEnv->c);
        av_free(ffmpegEnv->c);
    }
    if (ffmpegEnv->picture)
    {
        av_free(ffmpegEnv->picture);
    }
    delete ffmpegEnv;
    delete decoder;
}

int
FFMPEG_encode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            )
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);
    FFMPEGenv_t *ffmpegEnv= static_cast<FFMPEGenv_t *>(coder->encoderContext);

    int retSize= -1;
    int size = coder->params.width * coder->params.height;

    {
        ffmpegEnv->picture->pts = AV_NOPTS_VALUE;
        ffmpegEnv->picture->data[0] = inBuff;
        ffmpegEnv->picture->data[1] = inBuff + size;
        ffmpegEnv->picture->data[2] = inBuff + size + size / 4;
        ffmpegEnv->picture->linesize[0] = coder->params.width;
        ffmpegEnv->picture->linesize[1] = coder->params.width / 2;
        ffmpegEnv->picture->linesize[2] = coder->params.width / 2;

        /* encode the image */
        retSize = avcodec_encode_video(ffmpegEnv->c,
                                       outBuff,
                                       outBuffLen,
                                       ffmpegEnv->picture
                                      );
    }

    return retSize;
}

int
FFMPEG_decode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            )
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);
    FFMPEGenv_t *ffmpegEnv= static_cast<FFMPEGenv_t *>(decoder->decoderContext);

    AVPacket avpkt;

    avpkt.data = inBuff;
    avpkt.size = inBuffLen;

    int got_picture;

    /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
       and this is the only method to use them because you cannot
       know the compressed data size before analysing it.

       BUT some other codecs (msmpeg4, mpeg4) are inherently frame
       based, so you must call them with all the data for one
       frame exactly. You must also initialize 'width' and
       'height' before initializing them. */

    /* NOTE2: some codecs allow the raw parameters (frame size,
       sample rate) to be changed at any frame. We handle this, so
       you should also take care of it */

    /* here, we use a stream based decoder (mpeg1video), so we
       feed decoder and see if it could decode a frame */

    int len;
    while (avpkt.size > 0)
    {
#if LIBAVCODEC_VERSION_INT < ((52<<16)+(26<<8)+0)
        len = avcodec_decode_video(ffmpegEnv->c,
                                   ffmpegEnv->picture,
                                   &got_picture,
                                   avpkt.data,
                                   avpkt.size
                                  );
#else
        len = avcodec_decode_video2(ffmpegEnv->c,
                                    ffmpegEnv->picture,
                                    &got_picture,
                                    &avpkt
                                   );
#endif
        if (len < 0)
        {
            fprintf(stderr, "FFMPEG_decode: Error while decoding frame\n");
            return -1;
        }
        if (got_picture)
        {
            goto decoding;
        }
        avpkt.size -= len;
        avpkt.data += len;
    }

#if 0
    /* some codecs, such as MPEG, transmit the I and P frame with a
       latency of one frame. You must do the following to have a
       chance to get the last frame of the video
    */
    len= avcodec_decode_video(ffmpegEnv->c,
                              ffmpegEnv->picture,
                              &got_picture,
                              NULL,
                              0
                             );
#endif

    if ( ! got_picture)
    {
        fprintf(stderr, "FFMPEG_decode: Error due to !got_picture\n");
        return -1;
    }

decoding:

    int outSize= ffmpegEnv->c->height*ffmpegEnv->c->width;

    if ( ! outSize )
    {
        fprintf(stderr, "FFMPEG_decode: Error due to wrong size\n");
        return -1;
    }

    if (    (ffmpegEnv->c->width  != decoder->params.width)
         || (ffmpegEnv->c->height != decoder->params.height)
       )
    {
        decoder->params.width  = ffmpegEnv->c->width;
        decoder->params.height = ffmpegEnv->c->height;
    }

    if (outBuffLen < (outSize * 3/2) )
    {
        return outSize*3/2;
    }

    unsigned char *lum   = outBuff;
    unsigned char *cromU = outBuff + outSize;
    unsigned char *cromV = outBuff + outSize + outSize / 4;

#if 0
    memcpy(lum,   ffmpegEnv->picture->data[0], outSize);
    memcpy(cromU, ffmpegEnv->picture->data[1], outSize/4);
    memcpy(cromV, ffmpegEnv->picture->data[2], outSize/4);
#else
    // wrap lines
    unsigned char *src= NULL;
    int src_linesize, dst_linesize;

    src_linesize= ffmpegEnv->picture->linesize[0];
    dst_linesize= ffmpegEnv->c->width;
    src= ffmpegEnv->picture->data[0];
    for (int i= ffmpegEnv->c->height; i > 0; i--)
    {
        memcpy(lum, src, dst_linesize);
        lum += dst_linesize;
        src += src_linesize;
    }

    src_linesize= ffmpegEnv->picture->linesize[1];
    dst_linesize= ffmpegEnv->c->width / 2;
    src= ffmpegEnv->picture->data[1];
    for (int i= ffmpegEnv->c->height / 2; i > 0; i--)
    {
        memcpy(cromU, src, dst_linesize);
        cromU += dst_linesize;
        src   += src_linesize;
    }

    src_linesize= ffmpegEnv->picture->linesize[2];
    dst_linesize= ffmpegEnv->c->width / 2;
    src= ffmpegEnv->picture->data[2];
    for (int i= ffmpegEnv->c->height / 2; i > 0; i--)
    {
        memcpy(cromV, src, dst_linesize);
        cromV += dst_linesize;
        src   += src_linesize;
    }
#endif

    return outSize*3/2;
}

void
FFMPEG_get_coder_params(void *context, vCoderArgs_t *params)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    memcpy(params, &coder->params, sizeof(vCoderArgs_t));
}

void
FFMPEG_get_decoder_params(void *context, vDecoderArgs_t *params)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    memcpy(params, &decoder->params, sizeof(vDecoderArgs_t));
}

