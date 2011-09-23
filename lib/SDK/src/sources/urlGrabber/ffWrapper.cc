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
// $Id: ffWrapper.cc 22300 2011-05-06 11:43:55Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

//
// heavily based on ffplay.cc of Fabrice Bellard, used as documentation
// and tutorial of the ffmpeg library
//

#include <stdlib.h>
#include <string.h>

#include <icf2/notify.hh>

#include "ffWrapper.hh"

const int MAX_QUEUE_SIZE   = (15 * 512 * 1024);

// hack for older versions of ffmpeg
#ifndef AVMEDIA_TYPE_VIDEO
#define AVMEDIA_TYPE_VIDEO CODEC_TYPE_VIDEO
#endif
#ifndef AVMEDIA_TYPE_AUDIO
#define AVMEDIA_TYPE_AUDIO CODEC_TYPE_AUDIO
#endif
#ifndef AVMEDIA_TYPE_NB
#define AVMEDIA_TYPE_NB CODEC_TYPE_NB
#endif

// hack for lack of alloc/free of av_audio_convert
#if LIBAVCODEC_VERSION_INT < ((52<<16)+(25<<8)+0)
extern "C"
{
// this code comes from audioconvert.c, included as not all libavcodec
// version contains it
struct AVAudioConvert {
    int in_channels, out_channels;
    int fmt_pair;
};

#ifndef SAMPLE_FMT_NB
#define SAMPLE_FMT_NB 5
#endif

typedef struct SampleFmtInfo {
    const char *name;
    int bits;
} SampleFmtInfo;

/** this table gives more information about formats */
static const SampleFmtInfo sample_fmt_info[SAMPLE_FMT_NB] = {
     {   "u8",   8 },
     {  "s16",  16 },
     {  "s24",  24 },
     {  "s32",  32 },
     {  "flt",  32 }
};

//static const SampleFmtInfo sample_fmt_info[SAMPLE_FMT_NB] = {
//    [SAMPLE_FMT_U8]  = { .name = "u8",  .bits = 8 },
//    [SAMPLE_FMT_S16] = { .name = "s16", .bits = 16 },
//    [SAMPLE_FMT_S24] = { .name = "s24", .bits = 24 },
//    [SAMPLE_FMT_S32] = { .name = "s32", .bits = 32 },
//    [SAMPLE_FMT_FLT] = { .name = "flt", .bits = 32 }
//};

const char *avcodec_get_sample_fmt_name(int sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= SAMPLE_FMT_NB)
        return NULL;
    return sample_fmt_info[sample_fmt].name;
}

AVAudioConvert *av_audio_convert_alloc(enum SampleFormat out_fmt, int out_channels,
                                       enum SampleFormat in_fmt, int in_channels,
                                       const float *matrix, int flags)
{
    AVAudioConvert *ctx;
    if (in_channels!=out_channels)
        return NULL;  /* FIXME: not supported */
    ctx = (AVAudioConvert*)av_malloc(sizeof(AVAudioConvert));
    if (!ctx)
        return NULL;
    ctx->in_channels = in_channels;
    ctx->out_channels = out_channels;
    ctx->fmt_pair = out_fmt + SAMPLE_FMT_NB*in_fmt;
    return ctx;
}

void av_audio_convert_free(AVAudioConvert *ctx)
{
    av_free(ctx);
}
}
#endif

//
/* BEGIN packet queue handling */
static void packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

static void packet_queue_flush(PacketQueue *q)
{
    AVPacketList *pkt, *pkt1;

    pthread_mutex_lock(&q->mutex);
    for(pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->nb_packets = 0;
    q->size = 0;
    pthread_mutex_unlock(&q->mutex);
}

static void packet_queue_end(PacketQueue *q)
{
    packet_queue_flush(q);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt1;

    /* duplicate the packet */
    if (av_dup_packet(pkt) < 0)
        return -1;

    pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;


    pthread_mutex_lock(&q->mutex);

    if (!q->last_pkt)

        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;
    q->nb_packets++;
    q->size += pkt1->pkt.size + sizeof(*pkt1);
    /* XXX: should duplicate packet data in DV case */
    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

static void packet_queue_abort(PacketQueue *q)
{
    pthread_mutex_lock(&q->mutex);

    q->abort_request = 1;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    pthread_mutex_lock(&q->mutex);

    for(;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            q->size -= pkt1->pkt.size + sizeof(*pkt1);
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}
//
/* END packet queue handling */


int
get_audio_frame(URLStreamState_t *is, AVPacket *pkt)
{
    AVCodecContext *dec= is->audio_st->codec;
    int outSize, len1= -1;

    AVPacket inPkt;
    inPkt.data= pkt->data;
    inPkt.size= pkt->size;

    while (inPkt.size > 0)
    {
        outSize= is->aDecodedBufLen;
#if LIBAVCODEC_VERSION_INT < ((52 << 16)+(25<<8)+0)
        len1= avcodec_decode_audio2(dec,
                                    (int16_t *)is->aDecodedBuf,
                                    &outSize,
                                    inPkt.data,
                                    inPkt.size
                                   );
#else
        len1= avcodec_decode_audio3(dec,
                                    (short *)is->aDecodedBuf,
                                    &outSize,
                                    &inPkt
                                   );
#endif
        if (len1 < 0)
        {
            return -1;
        }

        inPkt.size -= len1;
        inPkt.data += len1;

        if (outSize <= 0)
        {
            continue;
        }

        if (dec->sample_fmt != is->audio_src_fmt)
        {
            if (is->reformat_ctx)
            {
                av_audio_convert_free(is->reformat_ctx);
            }
            is->reformat_ctx=
                av_audio_convert_alloc(SAMPLE_FMT_S16,
                                       1,
                                       dec->sample_fmt,
                                       1,
                                       NULL,
                                       0
                                      );
            if ( ! is->reformat_ctx)
            {
                NOTIFY("Cannot convert %s sample format to %s sample format\n",
#if LIBAVCODEC_VERSION_INT < ((52 << 16)+(94<<8)+3)
                       avcodec_get_sample_fmt_name(dec->sample_fmt),
                       avcodec_get_sample_fmt_name(SAMPLE_FMT_S16)
#else
                       av_get_sample_fmt_name(dec->sample_fmt),
                       av_get_sample_fmt_name(SAMPLE_FMT_S16)
#endif
                      );
                    break;
            }
            NOTIFY("Audio convert from %s sample format to %s sample format\n",
#if LIBAVCODEC_VERSION_INT < ((52 << 16)+(94<<8)+3)
                   avcodec_get_sample_fmt_name(dec->sample_fmt),
                   avcodec_get_sample_fmt_name(SAMPLE_FMT_S16)
#else
                   av_get_sample_fmt_name(dec->sample_fmt),
                   av_get_sample_fmt_name(SAMPLE_FMT_S16)
#endif
                  );
            is->audio_src_fmt= dec->sample_fmt;
        }
        if (is->reformat_ctx)
        {
            const void *ibuf[6]= {is->aDecodedBuf};
            void *obuf[6]= {is->aResampledBuf};
#if LIBAVCODEC_VERSION_INT < ((52 << 16)+(30<<8)+2)
            int istride[6]= {av_get_bits_per_sample(dec->codec_id)/8};
#elif LIBAVCODEC_VERSION_INT < ((52 << 16)+(108<<8)+3)
            int istride[6]= {av_get_bits_per_sample_format(dec->sample_fmt)/8};
#else
            int istride[6]= {av_get_bits_per_sample_fmt(dec->sample_fmt)/8};
#endif
            int ostride[6]= {2};
            int len= outSize/istride[0];
            if (av_audio_convert(is->reformat_ctx,
                                 obuf,
                                 ostride,
                                 ibuf,
                                 istride,
                                 len
                                ) < 0
               )
            {
                NOTIFY("av_audio_convert() failed\n");
                break;
            }
            is->audioBuf= is->aResampledBuf;
            outSize= len*2;
        }
        else
        {
            is->audioBuf= is->aDecodedBuf;
        }

        int numAudChannels= dec->channels;
        if (dec->sample_rate != 48000)
        {
            if (is->resample_ctx && (is->resamplingRate != dec->sample_rate))
            {
                // current resampler not valid
                audio_resample_close(is->resample_ctx);
                is->resample_ctx= NULL;
            }
            if ( ! is->resample_ctx)
            {
                NOTIFY("URL Grabber: creating resampling audio %d -> %d\n",
                       dec->sample_rate,
                       48000
                      );
#if LIBAVCODEC_VERSION_INT < ((52<<16)+(15<<8)+0)
                is->resample_ctx= audio_resample_init(1,
                                                      numAudChannels,
                                                      48000,
                                                      dec->sample_rate
                                                     );
#else
                is->resample_ctx= av_audio_resample_init(1,
                                                         numAudChannels,
                                                         48000,
                                                         dec->sample_rate,
                                                         SAMPLE_FMT_S16,
                                                         SAMPLE_FMT_S16,
                                                         16, 10, 0, 0.8
                                                        );
#endif
            }
            if ( ! is->resample_ctx)
            {
                NOTIFY("URL Grabber: cannot resample audio, bailing out\n");
                abort();
            }
            is->resamplingRate= dec->sample_rate;

            outSize= audio_resample(is->resample_ctx,
                                    (short*)is->aResampledBuf,
                                    (short*)is->audioBuf,
                                    outSize / 2 // / (2 * numAudChannels)
                                   );

            is->audioBuf= is->aResampledBuf;
            outSize *= 2;
        }

        return outSize;
    }

    return -1;
}

int
get_video_frame(URLStreamState_t *is, AVFrame *frame, AVPacket *pkt)
{
    int len, got_picture;
    int thisPktPts= pkt->pts;

#if LIBAVCODEC_VERSION_INT > ((52<<16)+(0<<8)+0)
    is->video_st->codec->reordered_opaque= pkt->pts;
#endif

#if LIBAVCODEC_VERSION_INT < ((52<<16)+(26<<8)+0)
    len = avcodec_decode_video(is->video_st->codec,
                               frame,
                               &got_picture,
                               pkt->data,
                               pkt->size
                              );
#else
    len = avcodec_decode_video2(is->video_st->codec,
                                frame,
                                &got_picture,
                                pkt
                               );
#endif

    if (len < 0)
    {
        return -1;
    }

    if (got_picture)
    {
        if (thisPktPts != 0)
        {
            is->video_pts= thisPktPts;
//NOTIFY("pkt->pts= %lu, ", thisPktPts);
//NOTIFY("ahora? video_pts= %lu\n", is->video_pts);
        }
        else
        {
            double frame_delay;
            frame_delay= av_q2d(is->video_st->codec->time_base);
            frame_delay += frame->repeat_pict * (frame_delay * 0.5);

            //is->video_pts += av_q2d(is->video_st->time_base);
            //is->video_pts += frame_delay;
            //is->video_pts += is->video_st->codec->time_base.den;
//NOTIFY("incrementa %f, video_pts= %lu\n", frame_delay, is->video_pts);
        }

        return 0;
    }

    return -2; // simply, the frame is not yet available
}

void
copy_video_frame(URLStreamState_t *is, AVFrame *frame)
{
    is->width = is->video_st->codec->width;
    is->height= is->video_st->codec->height;

    // realloc vrawBuf, if neeeded
    int npixels= is->width * is->height;
    int size= npixels * 3 / 2;
    if ( size > is->vrawBufSize)
    {
        free(is->vrawBuf);
        is->vrawBufSize= 2 * size;
        is->vrawBuf= (unsigned char *)malloc(is->vrawBufSize);
        if (! is->vrawBuf)
        {
            NOTIFY("copy_video_frame: run out of memory\n");
            abort();
        }
    }

    unsigned char *lum   = is->vrawBuf;
    unsigned char *cromU = is->vrawBuf + npixels;
    unsigned char *cromV = is->vrawBuf + npixels + npixels / 4;

#if 0
    memcpy(lum,   frame->data[0], npixels);
    memcpy(cromU, frame->data[1], npixels / 4);
    memcpy(cromV, frame->data[2], npixels / 4);
#else
    // wrap lines
    int wrap= frame->linesize[0];
    for (int i= 0; i < is->height; i++)
    {
        memcpy(lum + i * is->width,
               frame->data[0] + i * wrap,
               is->width
              );
    }

    wrap= frame->linesize[1];
    for (int i= 0; i < is->height / 2; i++)
    {
        memcpy(cromU + i * is->width / 2,
               frame->data[1] + i * wrap,
               is->width / 2
              );
    }

    wrap= frame->linesize[2];
    for (int i= 0; i < is->height / 2; i++)
    {
        memcpy(cromV + i * is->width / 2,
               frame->data[2] + i * wrap,
               is->width / 2
              );
    }
#endif

#ifdef USA_BUFFER_VIDEO
    image_t *newImage= new image_t(is->vrawBuf,
                                   npixels * 3 / 2,
                                   I420P_FORMAT,
                                   is->width,
                                   is->height,
                                   is->video_pts
                                  );

    is->imgBuf.put(newImage);
#endif
}

const int STREAM_LAPSE= 50000; // 50 mS
void*
audio_thread(void *arg)
{
    URLStreamState_t *is= (URLStreamState_t*)arg;
    AVPacket pkt1, *pkt = &pkt1;
    // timeToSend may be negative, for large packets
    int timeToSend= STREAM_LAPSE;

    int64_t past, now;

    past= av_gettime();

    for (;;)
    {
        while (timeToSend > 0)
        {
            if (packet_queue_get(&is->audioq, pkt, 1) < 0)
            {
                goto endAudio;
            }

            int n= get_audio_frame(is, pkt);
            if (n > 0) // got it!
            {
                u8 *data = is->audioBuf;
                int len = n;
                while (len > 0)
                {
                    int pkt_len = 900;
                    if (len < pkt_len)
                    {
                        pkt_len = len;
                    }
                    is->audioSocket->write(data, pkt_len);
                    data += pkt_len;
                    len  -= pkt_len;
                }

                int playTime= (1000.0 * n / 2) / 48;
                timeToSend -= playTime;
            }
        }

        if (strcmp(is->ic->iformat->name, "rtsp") == 0)
        {
            // do not sleep in non buffered streams nor networked ones
            timeToSend += STREAM_LAPSE;
            continue;
        }

        usleep(STREAM_LAPSE);

        now= av_gettime();

        timeToSend += (now - past);
        past = now;
    }

endAudio:

    return 0;
}

void*
video_thread(void *arg)
{
    URLStreamState_t *is= (URLStreamState_t*)arg;
    AVPacket pkt1, *pkt = &pkt1;

    AVFrame *frame= avcodec_alloc_frame();

    // timeToSend may be negative, for large packets
    int timeToSend= STREAM_LAPSE;

    int64_t past, now;

    double tbf= 0;

    // compute time between frames
    if (is->video_st->r_frame_rate.den && is->video_st->r_frame_rate.num)
    {
        tbf= 1 / av_q2d(is->video_st->r_frame_rate);
    }

#if LIBAVFORMAT_VERSION_INT >= ((52 << 16)+(41<<8)+0)
    if (is->video_st->avg_frame_rate.den && is->video_st->avg_frame_rate.num)
    {
        tbf = 1 / av_q2d(is->video_st->r_frame_rate);
    }
#endif

    past= av_gettime();

    for (;;)
    {
        while (timeToSend > 0)
        {
            if (packet_queue_get(&is->videoq, pkt, 1) < 0)
            {
                goto endVideo;
            }

            int ret= get_video_frame(is, frame, pkt);
            if (ret == 0) // got frame!
            {
#ifndef USA_BUFFER_VIDEO
                pthread_mutex_lock(&is->frame_mutex);
#endif
                copy_video_frame(is, frame);
#ifndef USA_BUFFER_VIDEO
                pthread_mutex_unlock(&is->frame_mutex);
#endif

                int playTime= (int)(1000000 * tbf); // tbf may be 0

                timeToSend -= playTime;
            }
        }

        if (strcmp(is->ic->iformat->name, "rtsp") == 0)
        {
            // do not sleep in non buffered streams, as networked ones
            timeToSend += STREAM_LAPSE;
            continue;
        }

        usleep(STREAM_LAPSE);

        now= av_gettime();

        timeToSend += (now - past);
        past = now;
    }

endVideo:

    av_free(frame);
    return 0;
}

#ifndef AVERROR_EOF
#include <errno.h>
#define AVERROR_EOF AVERROR(EPIPE)
#endif

void*
decode_thread(void *arg)
{
    URLStreamState_t *is= (URLStreamState_t*)arg;

    int ret;
    bool eof= false;
    AVPacket pkt1, *pkt = &pkt1;

    for (;;)
    {
        if (is->stopRequested)
        {
            break;
        }
        if (eof)
        {
            //NOTIFY("%s %s EOF\n", is->ic->iformat->name, is->streamName);
            if (is->loop)
            {
                ret = av_seek_frame(is->ic, -1, 0, AVSEEK_FLAG_BYTE);
                if (ret < 0)
                {
                    NOTIFY("error while seeking %s %s after EOF\n",
                           is->ic->iformat->name,
                           is->streamName
                          );
                    break;
                }
                eof= false;
                continue;
            }
            break;
        }

        if (is->audioq.size + is->videoq.size > MAX_QUEUE_SIZE)
        {
//NOTIFY("MAX_QUEUE_SIZE, going to sleep\n");
            usleep(200000); // 20 ms
            continue;
        }

        av_read_play(is->ic);

#if LIBAVFORMAT_VERSION_INT < ((52<<16)+(0<<8)+0)
        if (url_feof(&is->ic->pb))
#else
        if (url_feof(is->ic->pb))
#endif
        {
            eof= true;
            continue;
        }

        ret = av_read_frame(is->ic, pkt);
        if (ret < 0)
        {
            if (ret == AVERROR_EOF)
            {
                eof= true;
                continue;
            }
#if LIBAVFORMAT_VERSION_INT < ((52<<16)+(0<<8)+0)
            if (url_ferror(&is->ic->pb))
#else
            if (url_ferror(is->ic->pb))
#endif
            {
                NOTIFY("%s %s URL error\n", is->ic->iformat->name, is->streamName);
                break;
            }
            NOTIFY("%s %s other error (ret<0)\n", is->ic->iformat->name, is->streamName);
            continue;
        }

        if (pkt->stream_index == is->audio_stream )
        {
            packet_queue_put(&is->audioq, pkt);
        }
        else if (pkt->stream_index == is->video_stream )
        {
            packet_queue_put(&is->videoq, pkt);
        }
        else
        {
            // do something with "other" stream, subtitles?
            av_free_packet(pkt);
        }
    }
    return 0;
}

/* open a given stream. Return 0 if OK */
int
stream_component_open(URLStreamState_t *is, unsigned stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;
    AVCodec *codec;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
    {
        return -1;
    }
    avctx = ic->streams[stream_index]->codec;

    /* prepare audio output */
#if LIBAVCODEC_VERSION_INT < ((51<<16)+(42<<8)+0)
    // do not prepare audio :-(
#elif LIBAVCODEC_VERSION_MAJOR < 53
    if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        if (avctx->channels > 0) {
            avctx->request_channels = FFMIN(2, avctx->channels);
        } else {
            avctx->request_channels = 2;
        }
    }
#else
    // use request_channel_layout, but how?
#endif

    codec = avcodec_find_decoder(avctx->codec_id);
    avctx->debug_mv = 0;
    avctx->debug = 0;
    avctx->workaround_bugs = 1;
    avctx->lowres = 0;
    //if(lowres) avctx->flags |= CODEC_FLAG_EMU_EDGE;
    avctx->idct_algo= FF_IDCT_AUTO;
    //if(fast) avctx->flags2 |= CODEC_FLAG2_FAST;
    avctx->skip_frame= AVDISCARD_DEFAULT;
    avctx->skip_idct= AVDISCARD_DEFAULT;
    avctx->skip_loop_filter= AVDISCARD_DEFAULT;
#if LIBAVCODEC_VERSION_INT > ((52<<16)+(0<<8)+0)
    avctx->error_recognition= FF_ER_CAREFUL;
#endif
    avctx->error_concealment= 3;
    avctx->thread_count= 1;

    //set_context_opts(avctx, avcodec_opts[avctx->codec_type], 0);

    if (!codec ||
        avcodec_open(avctx, codec) < 0)
        return -1;

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch(avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        is->audio_stream = stream_index;
        is->audio_st = ic->streams[stream_index];

        packet_queue_init(&is->audioq);
        pthread_create(&is->audio_tid, NULL, audio_thread, (void*)is);

        break;
    case AVMEDIA_TYPE_VIDEO:
        is->video_stream = stream_index;
        is->video_st = ic->streams[stream_index];

#ifndef USA_BUFFER_VIDEO
        pthread_mutex_init(&is->frame_mutex, NULL);
#endif

        packet_queue_init(&is->videoq);
        pthread_create(&is->video_tid, NULL, video_thread, (void*)is);
        break;
    default:
        break;
    }
    return 0;
}

void
stream_component_close(URLStreamState_t *is, unsigned stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return;
    avctx = ic->streams[stream_index]->codec;

    switch(avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        packet_queue_abort(&is->audioq);

        pthread_join(is->audio_tid, NULL);

        packet_queue_end(&is->audioq);

        if (is->reformat_ctx)
        {
            av_audio_convert_free(is->reformat_ctx);
        }
        if (is->resample_ctx)
        {
            audio_resample_close(is->resample_ctx);
        }
        is->reformat_ctx= NULL;
        is->resample_ctx= NULL;

        break;
    case AVMEDIA_TYPE_VIDEO:
        packet_queue_abort(&is->videoq);

        pthread_join(is->video_tid, NULL);

        packet_queue_end(&is->videoq);

        break;
    default:
        break;
    }

    ic->streams[stream_index]->discard = AVDISCARD_ALL;
    avcodec_close(avctx);
    switch(avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        is->audio_st = NULL;
        is->audio_stream = -1;

        break;
    case AVMEDIA_TYPE_VIDEO:
        is->video_st = NULL;
        is->video_stream = -1;
        break;
    default:
        break;
    }
}

URLStreamState_t *
URLStreamOpen(const char *streamName, dgramSocket_t *audUDP)
{
    URLStreamState_t *is;

    AVFormatContext *ic;
    int err;
    int st_index[AVMEDIA_TYPE_NB];
#if 0
    int st_best_packet_count[AVMEDIA_TYPE_NB];
#endif
    AVFormatParameters params, *ap = &params;

    if (! streamName || !strlen(streamName))
    {
        return NULL;
    }

    is = (URLStreamState_t*)av_mallocz(sizeof(URLStreamState_t));
    if (!is)
        return NULL;

    // dup it, I do not trust AVFormatContext->filename, as
    // it holds only the pointer, and it does not dup memory
    is->streamName= strdup(streamName);

    is->stopRequested= false;
    is->loop= true;

    is->audio_src_fmt= SAMPLE_FMT_S16;
    is->reformat_ctx= NULL;
    is->resample_ctx= NULL;

    //
    is->audioSocket = audUDP;

    // audio decoded
    is->aDecodedBufLen= AVCODEC_MAX_AUDIO_FRAME_SIZE;
    is->aDecodedBuf = new u8[is->aDecodedBufLen];

    // audio reformated and resampled
    is->aReformatedBufLen= AVCODEC_MAX_AUDIO_FRAME_SIZE;
    is->aReformatedBuf = new u8[is->aReformatedBufLen];
    is->aResampledBufLen= AVCODEC_MAX_AUDIO_FRAME_SIZE;
    is->aResampledBuf = new u8[is->aResampledBufLen];
    is->resamplingRate= -1; // invalid

    // video decoded
    is->width= is->height= 0;
    is->vrawBufSize= 1024 * 768 * 4;
    is->vrawBuf= (unsigned char*)malloc(is->vrawBufSize);


#if LIBAVFORMAT_VERSION_INT < ((52<<16)+(26<<8)+0)
    ic = av_alloc_format_context();
#else
    ic = avformat_alloc_context();
#endif

    memset(st_index, -1, sizeof(st_index));
#if 0
    memset(st_best_packet_count, -1, sizeof(st_best_packet_count));
#endif
    is->video_stream = -1;
    is->audio_stream = -1;

    memset(ap, 0, sizeof(*ap));

    ap->prealloced_context = 1;
    ap->width = 0;
    ap->height= 0;
    ap->time_base= (AVRational){1, 25};
    ap->pix_fmt = PIX_FMT_NONE;

    //set_context_opts(ic, avformat_opts, AV_OPT_FLAG_DECODING_PARAM);

    err = av_open_input_file(&ic, is->streamName, NULL, 0, ap);
    if (err < 0) {
        char errbuf[128];
        const char *errbuf_ptr = errbuf;

#ifdef av_strerror
        if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
            errbuf_ptr = strerror(AVUNERROR(err));
#else
            errbuf_ptr = "UNKNOWN ERROR";
#endif
        NOTIFY("%s: %s\n", is->streamName, errbuf_ptr);

        goto fail;
    }
    is->ic = ic;

    err = av_find_stream_info(ic);
    if (err < 0)
    {
        NOTIFY("%s: could not find codec parameters\n", is->streamName);
        goto fail;
    }

#if LIBAVFORMAT_VERSION_INT > ((52<<16)+(0<<8)+0)
    // maybe this has no sense and can be removed
    if(ic->pb)
        ic->pb->eof_reached= 0; //FIXME hack, ffplay maybe should not use url_feof() to test for the end
#endif

    for (unsigned i = 0; i < ic->nb_streams; i++)
    {
        AVStream *st= ic->streams[i];
        AVCodecContext *avctx = st->codec;
        ic->streams[i]->discard = AVDISCARD_ALL;
        if (avctx->codec_type >= AVMEDIA_TYPE_NB)
        {
            continue;
        }

#if 0
        if(st_best_packet_count[avctx->codec_type] >= st->codec_info_nb_frames)
            continue;
        st_best_packet_count[avctx->codec_type]= st->codec_info_nb_frames;
#endif

        switch(avctx->codec_type)
        {
        case AVMEDIA_TYPE_AUDIO:
            st_index[AVMEDIA_TYPE_AUDIO] = i;
            break;
        case AVMEDIA_TYPE_VIDEO:
            st_index[avctx->codec_type] = i;
            break;
        default:
            break;
        }
    }

    /* open the streams */
    if (st_index[AVMEDIA_TYPE_AUDIO] >= 0)
    {
        stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
    }

    if (st_index[AVMEDIA_TYPE_VIDEO] >= 0)
    {
        stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
    }

    if (is->video_stream < 0 && is->audio_stream < 0)
    {
        NOTIFY("%s: could not open codecs\n", is->streamName);
        goto fail;
    }

    // is->streamName == is->ic->filename
    NOTIFY("URLStreamOpen: opened stream \"%s\"\n", is->streamName);

    // start the thread

    pthread_create(&is->decodeThrID, NULL, decode_thread, (void*)is);

    return is;

fail:
    av_free(is);
    return NULL;
}

void
URLStreamClose(URLStreamState_t *is)
{
    is->stopRequested= true;
    pthread_join(is->decodeThrID, NULL);

    /* close each stream */
    if (is->audio_stream >= 0)
        stream_component_close(is, is->audio_stream);
    if (is->video_stream >= 0)
        stream_component_close(is, is->video_stream);
    if (is->ic)
    {
        av_close_input_file(is->ic);
        is->ic = NULL; /* safety */
    }

    free(is->streamName);
    is->streamName= NULL;

    delete []is->aDecodedBuf;
    delete []is->aReformatedBuf;
    delete []is->aResampledBuf;

    free(is->vrawBuf);

#ifndef USA_BUFFER_VIDEO
    pthread_mutex_destroy(&is->frame_mutex);
#endif

    av_free(is);
}

static bool AVInitializedFlag= false;

void
AVinit(void)
{
    if (AVInitializedFlag)
    {
        return;
    }

    AVInitializedFlag= true;

    // register all codecs, protocols
    avcodec_register_all();
    av_register_all();

    //for (int i= 0; i < AVMEDIA_TYPE_NB; i++)
    //{
    //    avcodec_opts[i]= avcodec_alloc_context2(i);
    //}
    //avformat_opts = avformat_alloc_context();
}

