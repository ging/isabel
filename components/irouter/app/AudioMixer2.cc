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

#include "AudioMixer2.hh"
#include "random32.hh"

Mixer_t::Mixer_t(int format, link_t *link)
: PT(255),
  MSPerFrame(0)
{
    srand(time(NULL));

    this->coder = NULL;
    this->link = link;

    ssrc = 0;
    ts = random32();
    sq = random32();
    silenceLevel = -50;
    silencePeriods = 0;
    samplesPerFrame = -1;
    mark = 0;

    setCoder(format);
}

int
Mixer_t::write(RTPHeader *header, const unsigned char *data, int length)
{
    if (samplesPerFrame <= 0)
    {
        NOTIFY("Mixer_t::write: samples per frame = 0!\n");
        return false;
    }

    // CALCULO DEL OFFSET EN EL BUFFER DE MEZCLA
    u32 pkt_ssrc = header->GetSSRC();
    if (ssrc == 0)
    {
        ssrc = pkt_ssrc;
    }

    int tsjump = 0;
    int nsjump = 0;
    PacketCase type = getPacketCase(header->GetSSRC(),
                                    header->GetSeqNumber(),
                                    header->GetTimestamp(),
                                    header->GetPayloadType(),
                                    tsjump,
                                    nsjump
                                   );

    switch (type)
    {
    case DUPLICATE_PACKET:
        break;
    case NEW_FLOW:
        offset[pkt_ssrc] = mixerBuffer.getMinOffset();
        offset[pkt_ssrc] = mixerBuffer.write(offset[pkt_ssrc] + tsjump, data, length, true);
        break;
    case NEXT_PACKET:
        offset[pkt_ssrc] = mixerBuffer.write(offset[pkt_ssrc] + tsjump , data, length, true);
        break;
    case JUMP_PACKET:
        offset[pkt_ssrc] = mixerBuffer.write(offset[pkt_ssrc] + tsjump, data, length, true);
        break;
    case OLD_PACKET:
        mixerBuffer.write(offset[pkt_ssrc] + tsjump, data, length, false);
        break;
    }
    return length;
}

bool
Mixer_t::send(void)
{
    if (coder == NULL)
    {
        return false;
    }

    int bytes_to_process = samplesPerFrame*BPS;
    u8 *mixedBuffer = new u8[bytes_to_process];
    u8 *resampledBuffer = new u8[bytes_to_process];
    u8 *encodedBuffer = new u8[bytes_to_process];

    int nmixed = mixerBuffer.read(mixedBuffer,bytes_to_process);

    if (evalPower(mixedBuffer,nmixed) < silenceLevel)
    {
        silencePeriods++;
        if (silencePeriods == SILENCE_PERIODS_TO_CUT)
        {
            mark = 1;
        }
    }
    else
    {
        silencePeriods = 0;
        mark = 0;
    }

    if (silencePeriods < SILENCE_PERIODS_TO_CUT)
    {
        int nresampled=
            resampler.resample(mixedBuffer,
                               nmixed,
                               SAMPLE_RATE,
                               resampledBuffer,
                               bytes_to_process,
                               aGetCoderRate(coder),
                               true
                              );

        int ncoded = aEncode(coder,
                             resampledBuffer,
                             nresampled / 2,
                             encodedBuffer
                            );

        if (ncoded > 0)
        {
            u8 aux[1500];
            memset(aux,0,1500);

            ts += ncoded/BPS;

            RTPHeader_t *rtpHeader = (RTPHeader_t*)aux;
            rtpHeader->version     = RTP_VERSION;
            rtpHeader->padding     = 0;
            rtpHeader->extension   = 0;
            rtpHeader->cc          = 0;
            rtpHeader->marker      = mark;
            rtpHeader->payloadtype = getPTByFmt(aGetFormat(coder));
            rtpHeader->seqnum      = htons(sq++);
            rtpHeader->timestamp   = htonl(ts);
            rtpHeader->ssrc        = htonl(ssrc);
            memcpy(aux+sizeof(RTPHeader_t), encodedBuffer, ncoded);
            RTPPacket_t pkt(aux,sizeof(RTPHeader_t)+ncoded);

            sharedPkt_t *shPkt = new sharedPkt_t();
            memcpy(shPkt->sharedBuf->data,
                   pkt.getPacket(),
                   pkt.getTotalLength()
                  );
            shPkt->sharedBuf->len= pkt.getTotalLength();
            shPkt->flowId        = audioId;
            shPkt->pktOutLink    = link;
            link->copyPktToTargets(shPkt);

            delete shPkt;
        }
    }

    delete mixedBuffer;
    delete resampledBuffer;
    delete encodedBuffer;

    return true;
}

bool
Mixer_t::setSilenceLevel(int level)
{
    bool ret = false;
    if (level < 0)
    {
        silenceLevel = level;
        ret = true;
    }
    return ret;
}

bool
Mixer_t::setCoder(int format)
{
    aCoder_t *tmp_coder = aGetCoder(format);
    if (tmp_coder == NULL)
    {
        NOTIFY("Mixer_t::setCodec: Error setting codec %s",
               aGetFormatNameById(format)
              );
        return false;
    }

    if (coder)
    {
        aDeleteCoder(coder);
    }
    coder = tmp_coder;

    PT= getPTByFmt(format);
    MSPerFrame= getMSPerPacketByPT(PT);

    samplesPerFrame= Mixer_t::SAMPLE_RATE * MSPerFrame / 1000;
    NOTIFY("Mixer_t[%d]::setCodec: Output codec changed to %s\n",
           ssrc,
           aGetFormatNameById(aGetFormat(coder))
          );

    return true;
}

Mixer_t::PacketCase
Mixer_t::getPacketCase(u32 ssrc,
                       u16 nseq,
                       u32 ts,
                       u8 pt,
                       int& tsjump,
                       int& nsjump
                      )
{
    if (pt != lastPT[ssrc])
    {
        lastPT[ssrc] = pt;
        lastTs[ssrc] = ts;
        lastNseq[ssrc] = nseq;

        return NEW_FLOW;
    }

    int nseqdiff = nseq - lastNseq[ssrc];
    long long int ltsdiff = (long long int)ts - (long long int)lastTs[ssrc];

    // nseq sequence cicle test
    if ( abs(nseqdiff) > ( USHRT_MAX - MAX_DIFF ) )
    {
        NOTIFY("Mixer_t::getPacketCase: canal %d, vuelta del NSeq ns=%d last=%d\n", ssrc, nseq, lastNseq[ssrc]);
        if (nseqdiff > 0)
            nseqdiff-= (USHRT_MAX + 1);
        else
            nseqdiff+= (USHRT_MAX + 1);
    }

    // ts sequence cicle test
    /*
    if (ltsdiff > UINT_MAX - MAX_DIFF*samplesPerFrame)
    {
        NOTIFY("Vuelta del TS ts=%d last=%d\n", ts, lastTs);
        ltsdiff-= (UINT_MAX + samplesPerFrame );
    }
    else if (ltsdiff < - (long long int)(UINT_MAX - MAX_DIFF*samplesPerFrame))
    {
        NOTIFY("Vuelta del TS\n");
        ltsdiff+= (UINT_MAX + samplesPerFrame );
    }*/

    int input_rate = getRateByPT(pt);
    int input_ms   = getMSPerPacketByPT(pt);
    int tsdiff = (int)ltsdiff;
    float ratio = SAMPLE_RATE/input_rate;
    int msjump = (tsdiff*1000)/input_rate;
    tsjump = (msjump - input_ms) * (SAMPLE_RATE/1000) * BPS;
    nsjump = nseqdiff - 1;

    PacketCase result;

    if (abs(tsdiff) > MAX_DIFF*samplesPerFrame || abs(nseqdiff) > MAX_DIFF )
    {
        result = NEW_FLOW;
    }
    else if (nseqdiff > 1)
    {
        result = JUMP_PACKET;
    }
    else if (nseqdiff == 1)
    {
        result = NEXT_PACKET;
    }
    else if (nseqdiff < 0)
    {
        result = OLD_PACKET;
    }
    else if (nseqdiff == 0)
    {
        result = DUPLICATE_PACKET;
    }

    //NOTIFY("Type=%d ts=%d lastTs=%d ns=%d lastns=%d tsjump=%d nsjump=%d\n", result, ts, lastTs[ssrc], nseq, lastNseq[ssrc], tsjump, nsjump);

    if (result != OLD_PACKET)
    {
        lastTs[ssrc] = ts;
        lastNseq[ssrc] = nseq;
    }

    return result;
}

AudioMixer2_t::AudioMixer2_t(unsigned int fmt)
{
    setCoder(fmt);
    setActive(false);
}

AudioMixer2_t::~AudioMixer2_t(void)
{
    for (std::map<link_t *, Mixer_t*>::iterator iter = mixers.begin();
         iter != mixers.end();
         ++iter
        )
    {
        if (iter->second != NULL)
        {
            delete iter->second;
            iter->second = NULL;
        }
    }
    mixers.clear();
}

bool
AudioMixer2_t::setDecoder(int SSRC, int fmt)
{
    if (decoders[SSRC] != NULL && aGetFormat(decoders[SSRC]) == fmt)
    {
        return true;
    }

    if (decoders[SSRC])
    {
        aDeleteDecoder(decoders[SSRC]);
    }

    decoders[SSRC] = aGetDecoder(fmt);
    if (decoders[SSRC] == NULL)
    {
        return false;
    }

    NOTIFY("AudioMixer2_t::setDecoder: Cambio de codec detectado. "
           "SSRC=%d PT=%d.\n",
           SSRC, fmt
          );
    return true;
}

void
AudioMixer2_t::heartBeat(void)
{
    for (std::map<link_t *, Mixer_t*>::iterator iter = mixers.begin();
         iter != mixers.end();
         ++iter
        )
    {
        if (iter->second != NULL)
        {
            iter->second->send();
        }
    }
}

bool
AudioMixer2_t::deliverPkt(RTPPacket_t *pkt, link_t *inLink)
{
    RTPHeader *header = reinterpret_cast<RTPHeader*>(pkt->getPacket());
    int SSRC = header->GetSSRC();
    int fmt = getFmtByPT(header->GetPayloadType());
    if ( ! setDecoder(SSRC, fmt))
    {
        return false;
    }

    int out = 0;

    int ndecoded = aDecode(decoders[SSRC],
                           pkt->getData(),
                           pkt->getDataLength(),
                           decodedBuffer
                          );
    if (ndecoded <= 0)
    {
        return false;
    }

    // resamplear el buffer decodificado
    out = resamplers[SSRC].resample(decodedBuffer,
                                    ndecoded,
                                    aGetDecoderRate(decoders[SSRC]),
                                    resampledBuffer,
                                    RESAMPLE_BUFFER,
                                    Mixer_t::SAMPLE_RATE
                                   );

    if (out > 0)
    {
        for (std::map<link_t *, Mixer_t*>::iterator iter = mixers.begin();
             iter != mixers.end();
             ++iter
            )
        {
            if (iter->first != inLink && iter->second != NULL)
            {
                // escribimos audio en todos los links menos el origen
                iter->second->write(header, resampledBuffer, out);
            }
        }
    }
    return true;
}


bool
AudioMixer2_t::newLink(link_t *link)
{
    bool ret = false;

    if (mixers[link] == NULL)
    {
        mixers[link] = new Mixer_t(codecFmt, link);
        ret = true;
    }

    return ret;
}

bool
AudioMixer2_t::delLink(link_t *link)
{
    if (mixers[link] != NULL)
    {
        delete mixers[link];
        mixers[link] = NULL;
    }

    return true;
}


bool
AudioMixer2_t::setCoder(unsigned fmt)
{
    bool ret = true;

    for (std::map<link_t*, Mixer_t*>::iterator iter = mixers.begin();
         iter != mixers.end();
         ++iter
        )
    {
        if (iter->second != NULL)
        {
            if (iter->second->setCoder(fmt) == false)
            {
                ret = false;
                break;
            }
        }
    }
    if (ret == true)
    {
        codecFmt = fmt;
    }
    else
    {
        NOTIFY("AudioMixer2_t::setCodec: Error setting codec %s",
               aGetFormatNameById(fmt)
              );
    }
    return ret;
}


void
AudioMixer2_t::setActive(bool boolean)
{
    if (boolean)
    {
        std::map<link_t*, Mixer_t*>::iterator iter = mixers.begin();
        if (iter != mixers.end()) // not empty, can it be possible?
        {
            set_period(iter->second->getMSPerFrame()*1000);
        }
    }
    else
    {
        set_period(0);
    }
    active = boolean;
}

bool
AudioMixer2_t::isActive(void)
{
    return active;
}

