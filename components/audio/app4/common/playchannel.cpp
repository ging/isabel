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

#include <limits.h>

#include <icf2/notify.hh>

#include <rtp/RTPPayloads.hh>

#include "playchannel.h"
#include "mixerbuffer.h"
#include "rtpheader.h"
#include "packetqueue.h"

// -----------------------------------------------------------------------------
// PlayChannel::PlayChannel
//
// -----------------------------------------------------------------------------
//
PlayChannel::PlayChannel(u32            chId,
                         MixerBuffer   *mbuffer,
                         dgramSocket_t *ssock,
                         const char    *sport
                        )
: Channel(chId, Channel::PLAY_CHANNEL, ssock, sport),
  decoder(NULL),
  started(true),
  firstInBurst(true),
  offset(0),
  lastTs(0),
  lastNseq(0),
  lastPT(255),
  mixerBuffer(mbuffer)
{
    queue= new PacketQueue(this);
}

// -----------------------------------------------------------------------------
// PlayChannel::~PlayChannel
//
// -----------------------------------------------------------------------------
//
PlayChannel::~PlayChannel(void)
{
    delete queue;
}


// -----------------------------------------------------------------------------
// PlayChannel::start()
// Por ahora no se usa
// -----------------------------------------------------------------------------
//
bool
PlayChannel::start(void)
{
    started = true;
    return true;
}

// -----------------------------------------------------------------------------
// PlayChannel::stop
// Por ahora no se usa
// -----------------------------------------------------------------------------
//
bool
PlayChannel::stop(void)
{
    started = false;
    return true;
}

// -----------------------------------------------------------------------------
// PlayChannel::packetReceived
// Este metodo es usado cuando llega un paquete de la red. Simplemente
// se lo pasa a la cola de paquetes
// -----------------------------------------------------------------------------
//
void
PlayChannel::packetReceived(const unsigned char *data, int length)
{
    queue->packetReceived(data, length);
}

// -----------------------------------------------------------------------------
// PlayChannel::packetReceived
// Este metodo es usado por la cola que se encarga de ordenar
// -----------------------------------------------------------------------------
//
void
PlayChannel::packetReceived2(const unsigned char *data, int length)
{
    if ( ! started)
    {
        return;
    }

    stats.packetRecv((unsigned int)length);

    const RTPHeader *header = reinterpret_cast<const RTPHeader*>(data);

    if ( ! setDecoder(header->GetPayloadType()))
    {
        return;
    }

    int out = 0;

    // CALCULO DEL OFFSET EN EL BUFFER DE MEZCLA
    int tsjump = 0;
    int nsjump = 0;
    PacketCase type = getPacketCase(header->GetSeqNumber(),
                                    header->GetTimestamp(),
                                    header->GetPayloadType(),
                                    tsjump,
                                    nsjump
                                   );

    // Decodificar el buffer
    if (type != DUPLICATE_PACKET)
    {
        int ndecoded= aDecode(decoder,
                              (unsigned char*)(data + RTPHeader::SIZE),
                              length - RTPHeader::SIZE,
                              decodedBuffer
                             );
        if (ndecoded <= 0)
        {
            //NOTIFY("Error al decodificar el buffer");
            return;
        }

        // resamplear el buffer decodificado
        out= resampler.resample(decodedBuffer,
                                ndecoded,
                                aGetDecoderRate(decoder),
                                resampledBuffer,
                                BUFFER_SIZE,
                                SoundDevice_t::SAMPLE_RATE,
                                type == NEXT_PACKET
                               );
    }

    switch (type)
    {
    case DUPLICATE_PACKET:
        stats.duplicateRecv();
        break;
    case NEW_FLOW:
        offset = mixerBuffer->getMinOffset();
        offset = mixerBuffer->write(offset + tsjump, resampledBuffer, out, true);
        break;
    case NEXT_PACKET:
        offset = mixerBuffer->write(offset + tsjump , resampledBuffer, out, true);
        break;
    case JUMP_PACKET:
        stats.packetLost(nsjump);
        offset = mixerBuffer->write(offset + tsjump, resampledBuffer, out, true);
        break;
    case OLD_PACKET:
        stats.disorderedRecv();
        mixerBuffer->write(offset + tsjump, resampledBuffer, out, false);
        break;
    }
    //NOTIFY(" Offset = %d\n" , offset);
}

// -----------------------------------------------------------------------------
// PlayChannel::setCodec
// Cambia el descodificador si es necesario
// -----------------------------------------------------------------------------
//
PlayChannel::PacketCase PlayChannel::getPacketCase(u16 nseq, u32 ts, u8 pt, int& tsjump, int& nsjump)
{
    if (pt != lastPT)
    {
        lastPT = pt;
        lastTs = ts;
        lastNseq = nseq;
        return PlayChannel::NEW_FLOW;
    }

    int nseqdiff = nseq - lastNseq;
    long long int ltsdiff = (long long int)ts - (long long int)lastTs;

    // nseq sequence cicle test
    if ( abs(nseqdiff) > ( USHRT_MAX - MAX_DIFF ) )
    {
        NOTIFY("Vuelta del NSeq ns=%d last=%d\n", nseq, lastNseq);
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
    }
    */

    int tsdiff = (int)ltsdiff;
    //float ratio = (float)SoundDevice::SAMPLE_RATE/(float)codec->getRate();
    int msjump = (tsdiff*1000) / aGetDecoderRate(decoder);
    tsjump = (msjump - getMSPerPacketByPT(pt)) * (SoundDevice_t::SAMPLE_RATE/1000) * SoundDevice_t::BPS;
    //tsjump = (tsdiff - samplesPerFrame)*SoundDevice::BPS * ratio;
    nsjump = nseqdiff - 1;

    PlayChannel::PacketCase result; 

    if (abs(tsdiff) > MAX_DIFF*samplesPerFrame || abs(nseqdiff) > MAX_DIFF )
    {
        result =  PlayChannel::NEW_FLOW;
    }
    else if (nseqdiff > 1)
    {
        result = PlayChannel::JUMP_PACKET;
    }
    else if (nseqdiff == 1)
    {
        result = PlayChannel::NEXT_PACKET;
    }
    else if (nseqdiff < 0)
    {
        result = PlayChannel::OLD_PACKET;
    }
    else if (nseqdiff == 0)
    {
        result = PlayChannel::DUPLICATE_PACKET;
    }

    //NOTIFY("Type=%d ts=%d lastTs=%d ns=%d lastns=%d tsjump=%d nsjump=%d", result, ts, lastTs, nseq, lastNseq, tsjump, nsjump);

    if (result != PlayChannel::OLD_PACKET)
    {
        lastTs = ts;
        lastNseq = nseq;
    }

    return result;
}

// -----------------------------------------------------------------------------
// PlayChannel::setDecoder
// Cambia el codec si es necesario
// -----------------------------------------------------------------------------
//
bool
PlayChannel::setDecoder(u8 pt)
{
    if (pt == lastPT && decoder != NULL)
    {
        return true;
    }

    aDecoder_t *newdecoder= aGetDecoder(getFmtByPT(pt));
    if (newdecoder == NULL)
    {
        return false;
    }

    if (decoder != NULL)
    {
        aDeleteDecoder(decoder);
    }

    decoder= newdecoder;

    samplesPerFrame = getRateByPT(pt) * getMSPerPacketByPT(pt) / 1000;

    stats.codecChange(aGetFormatNameById(aGetFormat(decoder)));

    NOTIFY("Cambio de descodificador detectado. "
           "Canal=%d. PT=%d. MSPerFrame=%d. Rate=%d.\n",
           chId,
           pt,
           getMSPerPacketByPT(pt),
           getRateByPT(pt)
          );

    return true;
}

