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
// $Id: ffWrapper.hh 22102 2011-03-08 17:34:56Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __ff_wrapper_hh__
#define __ff_wrapper_hh__

#include <pthread.h>

#include <icf2/sockIO.hh>

//#define USA_BUFFER_VIDEO

#include "imageBuf.hh"

extern "C"
{
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#if LIBAVCODEC_VERSION_INT < ((52<<16)+(25<<8)+0)
#include "audioconvert.h"
#elif LIBAVCODEC_VERSION_INT < ((52<<16)+(109<<8)+0)
#include <libavcore/samplefmt.h>
#include <libavcodec/audioconvert.h>
#include <libavcore/audioconvert.h>
#else
#include <libavutil/samplefmt.h>
#include <libavcodec/audioconvert.h>
#include <libavutil/audioconvert.h>
#endif
}

typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} PacketQueue;

typedef struct
{
    AVFormatContext *ic;

    int audio_stream;
    AVStream *audio_st;
    PacketQueue audioq;
    pthread_t audio_tid;

    int video_stream;
    AVStream *video_st;
    PacketQueue videoq;
    pthread_t video_tid;

    char *streamName;

    pthread_t decodeThrID;
#ifdef USA_BUFFER_VIDEO
    imageBuf_t imgBuf;
#else
    pthread_mutex_t frame_mutex;
#endif
    bool stopRequested;

    dgramSocket_t *audioSocket;

    // audio decoding
    int aDecodedBufLen;
    unsigned char *aDecodedBuf;

    // audio reformat and resampling
    enum SampleFormat audio_src_fmt;
//#if LIBAVCODEC_VERSION_INT > ((52<<16)+(0<<8)+0)
    AVAudioConvert *reformat_ctx;
//#else
//#endif
    int aReformatedBufLen;         // S16
    unsigned char *aReformatedBuf;

    ReSampleContext *resample_ctx;
    int resamplingRate;            // current resampling rate
    int aResampledBufLen;          // S16, 48000Hz
    unsigned char *aResampledBuf;

    unsigned char *audioBuf; // pointer to proper buffer

    // video
    int width, height;
    int vrawBufSize;
    unsigned char *vrawBuf; // decoded video frame

    int64_t video_pts;

    // control
    bool loop;
} URLStreamState_t;

void AVinit(void); // executed just once

URLStreamState_t *URLStreamOpen(const char *streamName, dgramSocket_t *audUDP);

void URLStreamClose(URLStreamState_t *urlss);

#endif
