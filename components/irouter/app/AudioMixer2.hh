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
#ifndef __AUDIO_MIXER_2_HH__
#define __AUDIO_MIXER_2_HH__

#include <map>
#include <string>

#include <aCodecs/codecs.h>

#include <icf2/stdTask.hh>

#include <rtp/RTPPacket.hh>

#include "link.hh"
#include "mixerbuffer.hh"
#include "rtpheader.hh"
#include "SoundUtils.hh"

class Mixer_t
{
public:
    static const int SAMPLE_RATE = 48000;

    typedef enum
    {
        NEW_FLOW,
        NEXT_PACKET,
        JUMP_PACKET,
        OLD_PACKET,
        DUPLICATE_PACKET
    } PacketCase;

private:

    aCoder_t *coder;
    u8 PT; // cached
    int MSPerFrame; // cached

    static const int SILENCE_PERIODS_TO_CUT = 0xffff;
    static const int BPS = 2;
    static const int MAX_DIFF = 10;

    int samplesPerFrame;
    int silenceLevel;
    int silencePeriods;

    Resampler resampler;

    std::map<u32,u32> lastTs;
    std::map<u32,u16> lastNseq;
    std::map<u32,u8>  lastPT;
    std::map<u32,int> offset;

    u32 ssrc, ts;
    u16 sq;
    u8  mark;

    link_t *link;
    MixerBuffer mixerBuffer;

    PacketCase getPacketCase(u32 ssrc, u16 nseq, u32 ts, u8 pt, int &tsjump, int &nsjump);

public:

    Mixer_t(int format, link_t *link);
    int write(RTPHeader *header, const unsigned char *data, int length);
    bool send(void);

    bool setCoder(int format);
    bool setSilenceLevel(int level);

    inline int getRate(void) { return coder == NULL ? 0 : aGetCoderRate(coder); }
    inline int getMSPerFrame(void) { return MSPerFrame; }
    inline link_t *getLink(void) { return link; }
};

class AudioMixer2_t: public simpleTask_t
{
private:

    static const int DECODER_BUFFER = 96000;
    static const int RESAMPLE_BUFFER = DECODER_BUFFER*2;

    u8 decodedBuffer[DECODER_BUFFER];
    u8 resampledBuffer[RESAMPLE_BUFFER];

    int codecFmt;

    std::map<int, Resampler>    resamplers;
    std::map<int, aDecoder_t*>  decoders;
    std::map<link_t*, Mixer_t*> mixers;

    bool active;

    bool setDecoder(int SSRC, int PT);

public:

    AudioMixer2_t(unsigned int fmt);
    virtual ~AudioMixer2_t(void);

    bool deliverPkt(RTPPacket_t *pkt, link_t *inLink);
    void heartBeat(void);
    bool newLink(link_t *link);
    bool delLink(link_t *link);

    bool setCoder(unsigned int newFmt);
    void setActive(bool boolean);
    bool isActive(void);

    friend class smartReference_t<AudioMixer2_t>;
};

typedef smartReference_t<AudioMixer2_t> AudioMixer2_ref;

#endif

