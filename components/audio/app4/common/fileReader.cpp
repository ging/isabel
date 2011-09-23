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

//
// heavily based on ffplay.cc of Fabrice Bellard, used as documentation
// and tutorial of the ffmpeg library
//

#include <stdlib.h>
#include <string.h>

#include <icf2/notify.hh>

#include "fileReader.h"

#define SAVE_TO_FILE 0

#if SAVE_TO_FILE
FILE *out= NULL;
#endif

static bool AVInitializedFlag= false;

//
// executed just once
//
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

FileReader_t::FileReader_t(const char *fName)
: fileName(NULL),
  aDecodedBuf(NULL),
  aReformatedBuf(NULL),
  aResampledBuf(NULL),
  readyBytes(0),
  loop(true)
{
    int ret;
    int st_index[AVMEDIA_TYPE_NB];

#if SAVE_TO_FILE
    out= fopen ("/tmp/kk.raw", "w");
#endif

    outBuff= (i16*)malloc(_MAX_BUFF_LEN * sizeof(i16));
    if ( ! outBuff )
    {
        NOTIFY("FileReader_t:: out of memory\n");
        throw "Out of memory";
    }

    // circular buffer
    ResetBuffer();

    AVinit();

    AVFormatParameters params, *ap = &params;

    if (! fName || !strlen(fName))
    {
        throw "NO FILENAME";
    }

    fileName= strdup(fName); //av_open_input_file does not dup it!

    audio_src_fmt= SAMPLE_FMT_S16;
    reformat_ctx= NULL;
    resample_ctx= NULL;

    // audio decoded
    aDecodedBufLen= AVCODEC_MAX_AUDIO_FRAME_SIZE;
    aDecodedBuf = new u8[aDecodedBufLen];

    // audio reformated and resampled
    aReformatedBufLen= AVCODEC_MAX_AUDIO_FRAME_SIZE;
    aReformatedBuf = new u8[aReformatedBufLen];
    aResampledBufLen= AVCODEC_MAX_AUDIO_FRAME_SIZE;
    aResampledBuf = new u8[aResampledBufLen];
    resamplingRate= -1; // invalid

    ic = avformat_alloc_context();

    memset(st_index, -1, sizeof(st_index));
#if 0
    memset(st_best_packet_count, -1, sizeof(st_best_packet_count));
#endif
    audio_stream = -1;

    memset(ap, 0, sizeof(*ap));

    ap->prealloced_context = 1;
    ap->width = 0;
    ap->height= 0;
#if __STDC_VERSION__ >= 199901L
    ap->time_base= (AVRational){1, 25};
#else
    AVRational defTB= {1, 25};
    ap->time_base= defTB;
#endif
    ap->pix_fmt = PIX_FMT_NONE;

    //set_context_opts(ic, avformat_opts, AV_OPT_FLAG_DECODING_PARAM);

    ret = av_open_input_file(&ic, fileName, NULL, 0, NULL); //ap);
    if (ret != 0)
    {
        char errbuf[128];
        const char *errbuf_ptr = errbuf;

        if (av_strerror(ret, errbuf, sizeof(errbuf)) < 0)
            errbuf_ptr = strerror(AVUNERROR(ret));

        NOTIFY("%s: %s\n", fileName, errbuf_ptr);

        goto fail;
    }

    ret = av_find_stream_info(ic);
    if (ret < 0)
    {
        char errbuf[128];
        const char *errbuf_ptr = errbuf;

        if (av_strerror(ret, errbuf, sizeof(errbuf)) < 0)
            errbuf_ptr = strerror(AVUNERROR(ret));

        NOTIFY("%s: %s\n", fileName, errbuf_ptr);

        NOTIFY("FileReader_t:: could not find codec parameters\n");
        goto fail;
    }

    for (unsigned i = 0; i < ic->nb_streams; i++)
    {
        AVStream *st= ic->streams[i];
        AVCodecContext *avctx = st->codec;
        ic->streams[i]->discard = AVDISCARD_ALL;
        if (avctx->codec_type >= AVMEDIA_TYPE_NB)
        {
            continue;
        }

        switch (avctx->codec_type)
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
        audio_component_open(st_index[AVMEDIA_TYPE_AUDIO]);
    }

    if (audio_stream < 0)
    {
        NOTIFY("%s: could not open codecs\n", fileName);
        goto fail;
    }

    return; // avoid fail:

fail:
    if (ic)
    {
        av_close_input_file(ic);
    }
    if (this->fileName)
    {
        free(this->fileName);
    }

    if (aDecodedBuf)
        delete []aDecodedBuf;
    if (aReformatedBuf)
        delete []aReformatedBuf;
    if (aResampledBuf)
        delete []aResampledBuf;

    if (outBuff)
        free(outBuff);

    throw "Could not open FileReader_t";
}

FileReader_t::~FileReader_t(void)
{
#if SAVE_TO_FILE
    fclose(out);
    out = NULL;
#endif

    /* close each stream */
    if (audio_stream >= 0)
        audio_component_close(audio_stream);

    if (ic)
    {
        av_close_input_file(ic);
        ic = NULL; /* safety */
    }

    free(fileName);
    fileName= NULL;

    delete []aDecodedBuf;
    delete []aReformatedBuf;
    delete []aResampledBuf;

    free(outBuff);
}


int
FileReader_t::decode_audio_packet(AVPacket *pkt)
{
    AVCodecContext *dec= audio_st->codec;
    int outSize, len1;

    AVPacket inPkt;
    inPkt.data= pkt->data;
    inPkt.size= pkt->size;

    while (inPkt.size > 0)
    {
        outSize= aDecodedBufLen;
        len1= avcodec_decode_audio3(dec,
                                    (short *)aDecodedBuf,
                                    &outSize,
                                    &inPkt
                                   );
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
        audioBuf= aDecodedBuf;

        if (dec->sample_fmt != audio_src_fmt)
        {
            if (reformat_ctx)
            {
                av_audio_convert_free(reformat_ctx);
            }
            reformat_ctx=
                av_audio_convert_alloc(SAMPLE_FMT_S16,
                                       dec->channels,
                                       dec->sample_fmt,
                                       dec->channels,
                                       NULL,
                                       0
                                      );
            if ( ! reformat_ctx)
            {
                NOTIFY("Cannot convert %s sample format to %s sample format\n",
                       av_get_sample_fmt_name(dec->sample_fmt),
                       av_get_sample_fmt_name(SAMPLE_FMT_S16)
                      );
                    break;
            }
            NOTIFY("Audio convert from %s sample format to %s sample format\n",
                   av_get_sample_fmt_name(dec->sample_fmt),
                   av_get_sample_fmt_name(SAMPLE_FMT_S16)
                  );
            audio_src_fmt= dec->sample_fmt;
        }
        if (reformat_ctx)
        {
            const void *ibuf[6]= {aDecodedBuf};
            void *obuf[6]= {aResampledBuf};
            int istride[6]= {av_get_bits_per_sample_fmt(dec->sample_fmt)/8};
            int ostride[6]= {2};
            int len= outSize / istride[0];
            if (av_audio_convert(reformat_ctx,
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
            audioBuf= aResampledBuf;
            outSize= len*2;
        }

        if ((dec->sample_rate != 48000) || (dec->channels > 1))
        {
            if (resample_ctx && (resamplingRate != dec->sample_rate))
            {
                // current resampler not valid
                audio_resample_close(resample_ctx);
                resample_ctx= NULL;
            }
            if ( ! resample_ctx)
            {
                NOTIFY("AUDIO: creating resampler %d -> %d, %d channels\n",
                       dec->sample_rate,
                       48000,
                       dec->channels
                      );
                resample_ctx= av_audio_resample_init(1,
                                                     dec->channels,
                                                     48000,
                                                     dec->sample_rate,
                                                     SAMPLE_FMT_S16,
                                                     SAMPLE_FMT_S16,
                                                     16, 10, 0, 0.8
                                                    );
            }
            if ( ! resample_ctx)
            {
                NOTIFY("AUDIO: cannot resample audio, bailing out\n");
                abort();
            }
            resamplingRate= dec->sample_rate;

            outSize= audio_resample(resample_ctx,
                                    (short*)aResampledBuf,
                                    (short*)audioBuf,
                                    outSize / (2 * dec->channels)
                                   );

            audioBuf= aResampledBuf;
            outSize *= 2;
        }

        return outSize;
    }

    return -1;
}

int
FileReader_t::get_audio(void)
{
    int ret;

    AVPacket pkt1, *pkt = &pkt1;

    av_init_packet(pkt);

    // read until audio data is readed or error ocurred
    // if EOF and 'loop'= true, try to rewind the file
    while (true)
    {
        ret= av_read_frame(ic, pkt);

        if (ret == 0)
        {
            if (pkt->stream_index == audio_stream)
            {
                ret= decode_audio_packet(pkt);
                av_free_packet(pkt);
                break;
            }
            else
            {
                // do nothing with "other" streams, videos, subtitles,...
                av_free_packet(pkt);
                continue;
            }
        }

        //if ((ret == AVERROR_EOF) && loop) // rewind stream
        if (loop) // rewind stream
        {
            NOTIFY("REWIND %s %s\n", ic->iformat->name, fileName);
            ret= av_seek_frame(ic, audio_stream, 0, AVSEEK_FLAG_BACKWARD);
            if (ret < 0)
            {
                NOTIFY("ERROR: get_audio rewinding %s %s after EOF\n",
                       ic->iformat->name,
                       fileName
                      );
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (ret < 0) // sure it is < 0!!
    {
        char errbuf[128];
        const char *errbuf_ptr = errbuf;

        if (av_strerror(ret, errbuf, sizeof(errbuf)) < 0)
            errbuf_ptr = strerror(AVUNERROR(ret));

        NOTIFY("ERROR: get_audio: %s: %s\n", fileName, errbuf_ptr);
    }

    return ret;
}

i16
FileReader_t::CheckSaturation(i32 sample)
{
    if (sample > 0x7fff)
    {
        sample = 0x7fff;
    }
    else if (sample < -0x7fff)
    {
        sample = -0x7fff;
    }
    return (i16)sample;
}

void
FileReader_t::ResetBuffer(void)
{
    pout = outBuff;
    pin  = outBuff; // + MAX_BUFF_LEN / 2;
    pend = outBuff + _MAX_BUFF_LEN;
    memset(outBuff, 0, _MAX_BUFF_LEN);
}

void
FileReader_t::moveFWBuffer(void)
{
NOTIFY("moveFWBuffer\n");
    //pout += 48 * 100; // 100mS
    pout = pin - _MAX_BUFF_LEN;
    if (pout < outBuff)
    {
        pout += _MAX_BUFF_LEN;
    }
}

int
FileReader_t::ReadBuffer(i16* samples, int nsamples)
{
    int n = 0;
    int remain = pend - pout;
    if (remain > nsamples)
    {
        for (int i= 0; i < nsamples; ++i)
        {
            u32 sample = *samples + *pout;
            *samples= CheckSaturation(sample);
            pout++; n++; samples++;
            if (pout == pin)
            {
                ResetBuffer();
                break;
            }
        }
    }
    else
    {
        for (int i= 0; i < remain; ++i)
        {
            u32 sample = *samples + *pout;
            *samples = CheckSaturation(sample);
            pout++; nsamples--; samples++; n++;
            if (pout == pin)
            {
                ResetBuffer();
                break;
            }
        }
        pout = outBuff;
        n += ReadBuffer(samples, nsamples);
    }
    return n;
}

int
FileReader_t::WriteBuffer(i16* samples, int nsamples)
{
    int n = 0;

    int remain = pend - pin;
    if (remain > nsamples)
    {
        for (int i= 0; i < nsamples; ++i)
        {
            *pin++ = *samples++;
            n++;
            if (pin == pout)
            {
                moveFWBuffer();
                break;
            }
        }
    }
    else
    {
        for (int i= 0; i < remain; ++i)
        {
            *pin++ = *samples++;
            nsamples--;
            n++;
            if (pin == pout)
            {
                moveFWBuffer();
                break;
            }
        }
        pin  = outBuff;
        n += WriteBuffer(samples, nsamples);
    }

    return n;
}

int
FileReader_t::read(unsigned char *dstBuff, int len)
{
    int readed, copied;

//NOTIFY("read begin: readyBytes= %d, requested= %d\n", readyBytes, len);
    while (readyBytes < len)
    {
        readed= get_audio();
        if (readed < 0)
        {
            NOTIFY("read: get_audio returned %d\n", readed);
            return readed;
        }

        if (readed == 0)
            break;

        // write to output buffer
        copied= WriteBuffer((i16*)audioBuf, readed / 2);

#if SAVE_TO_FILE
    fwrite(pin, 1, readed, out);
#endif

        if (copied != (readed / 2))
            NOTIFY("WriteBuffer: weird result, copied=%d, readed=%d\n",
                   copied,
                   readed
                  );

        readyBytes+= readed;
//NOTIFY("readed= %d\n", readed);
    }
//NOTIFY("while end: readyBytes= %d\n", readyBytes);

    int toCopy = readyBytes < len ? readyBytes : len;

    int ret= ReadBuffer((i16*)dstBuff, toCopy / 2);
    readyBytes -= (ret * 2);
//NOTIFY("read end: readyBytes= %d\n", readyBytes);
//NOTIFY("Devuelvo %d, toCopy= %d (requested %d)\n", ret, toCopy, len);

    return ret * 2;
}

/* open a given stream. Return 0 if OK */
int
FileReader_t::audio_component_open(unsigned stream_index)
{
    AVCodecContext *avctx;
    AVCodec *codec;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
    {
        return -1;
    }
    avctx = ic->streams[stream_index]->codec;

    /* prepare audio output */
    if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        if (avctx->channels > 0) {
            avctx->request_channels = FFMIN(2, avctx->channels);
        } else {
            avctx->request_channels = 2;
        }
    }

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
    avctx->error_recognition= FF_ER_CAREFUL;
    avctx->error_concealment= 3;
    avctx->thread_count= 1;

    //set_context_opts(avctx, avcodec_opts[avctx->codec_type], 0);

    if (!codec ||
        avcodec_open(avctx, codec) < 0)
        return -1;

    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
    switch (avctx->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        audio_stream = stream_index;
        audio_st = ic->streams[stream_index];

        break;
    default:
        break;
    }
    return 0;
}

void
FileReader_t::audio_component_close(unsigned stream_index)
{
    AVCodecContext *avctx;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return;
    avctx = ic->streams[stream_index]->codec;

    switch(avctx->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        if (reformat_ctx)
        {
            av_audio_convert_free(reformat_ctx);
        }
        if (resample_ctx)
        {
            audio_resample_close(resample_ctx);
        }
        reformat_ctx= NULL;
        resample_ctx= NULL;

        break;
    case AVMEDIA_TYPE_VIDEO:

        break;
    default:
        break;
    }

    ic->streams[stream_index]->discard = AVDISCARD_ALL;
    avcodec_close(avctx);
    switch(avctx->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        audio_st = NULL;
        audio_stream = -1;

        break;
    default:
        break;
    }
}

