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

#ifndef __FILE_READED_H__
#define __FILE_READED_H__

extern "C"
{
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <libavutil/samplefmt.h>
#include <libavcodec/audioconvert.h>
#include <libavutil/audioconvert.h>
}

class FileReader_t
{
private:
    char *fileName;

    AVFormatContext *ic;

    int audio_stream;
    AVStream *audio_st;

    // audio decoding
    int aDecodedBufLen;
    unsigned char *aDecodedBuf;

    // audio reformat and resampling
    enum SampleFormat audio_src_fmt;

    AVAudioConvert *reformat_ctx;
    int aReformatedBufLen;         // S16
    unsigned char *aReformatedBuf;

    ReSampleContext *resample_ctx;
    int resamplingRate;            // current resampling rate
    int aResampledBufLen;          // S16, 48000Hz
    unsigned char *aResampledBuf;

    unsigned char *audioBuf; // pointer to proper buffer
    int readyBytes; // bytes that can be used from audioBuf

    // control
    bool loop;

    // output circular buffer
    static const int _MAX_BUFF_LEN= 48*4000; //4000 ms de buffer a 48000 Hz

    i16 *outBuff; // circular buffer
    i16 *pin;
    i16 *pout;
    i16 *pend;

    inline i16 CheckSaturation(i32 sample);
    void ResetBuffer(void);  // buffer empty, start again
    void moveFWBuffer(void); // buffer full, move forward a bit
    int  ReadBuffer (i16* samples, int nsamples); // handles ring buffer
    int  WriteBuffer(i16* samples, int nsamples); // handles ring buffer

    // open/close just the audio component
    int  audio_component_open (unsigned stream_index);
    void audio_component_close(unsigned stream_index);

    int decode_audio_packet(AVPacket *pkt);
    int get_audio(void);

public:

    FileReader_t(const char *fName);

    ~FileReader_t(void);

    int read(unsigned char *dstBuf, int len);

};

#endif
