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

#include <aCodecs/codecs.h>

#include <icf2/notify.hh>

#include <rtp/RTPPayloads.hh>

#include "rtpsink.h"
#include "statssender.h"

// -----------------------------------------------------------------------------
// RTPSink::RTPSink
// Codec por defecto none-16
// -----------------------------------------------------------------------------
//
RTPSink::RTPSink(u32 chId,
                 dgramSocket_t *sock,
                 StatsSender *st,
                 int rate,
                 int bpsa
                )
: stats(st),
  coder(NULL),
  buffered(0),
  counter(0),
  threshold(DEFAULT_THRESHOLD),
  silenceCounter(0),
  started(false),
  inputRate(rate),
  bps(bpsa),
  socket(sock)
{
    setCoder(NONE_16_FORMAT);
    header.SetSSRC(chId);
    header.SetMarker(1);
}

// -----------------------------------------------------------------------------
// RTPSink::~RTPSink
//
// -----------------------------------------------------------------------------
//
RTPSink::~RTPSink(void)
{
    // Borro la lista de bindings
    while (bindTable.size() > 0)
    {
        inetAddr_t *addr = bindTable.begin()->second;
        bindTable.erase(bindTable.begin());
        delete addr;
    }
}

// -----------------------------------------------------------------------------
// RTPSink::setSilenceThreshold
//
// -----------------------------------------------------------------------------
//
void
RTPSink::setSilenceThreshold(double limit)
{
    threshold = limit;
}

// -----------------------------------------------------------------------------
// RTPSink::bind()
// Mirar excepciones. OJO! Puede tardar mucho en fallar.
// -----------------------------------------------------------------------------
//
int
RTPSink::bind(char *host, char *port)
{
    try
    {
        inetAddr_t *addr = new inetAddr_t(host, port, serviceAddr_t::DGRAM);
        bindTable.insert(BINDINGS::value_type(++counter, addr));
        return counter;
    }
    catch (...)
    {
        return -1;
    }
}

// -----------------------------------------------------------------------------
// RTPSink::unbind()
//
// -----------------------------------------------------------------------------
//
bool
RTPSink::unbind(int bindId)
{
    BINDINGS::iterator it = bindTable.find(bindId);

    if (it != bindTable.end())
    {
        inetAddr_t *addr = it->second;
        bindTable.erase(it);
        delete addr;
        return true;
    }
    else
    {
        return false;
    }
}

// -----------------------------------------------------------------------------
// RTPSink::start()
//
// -----------------------------------------------------------------------------
//
void
RTPSink::start(void)
{
    started = true;
}

// -----------------------------------------------------------------------------
// RTPSink::stop()
//
// -----------------------------------------------------------------------------
//
void
RTPSink::stop(void)
{
    started = false;
}


// -----------------------------------------------------------------------------
// RTPSink::setCoder()
//
// -----------------------------------------------------------------------------
//
bool
RTPSink::setCoder(int format)
{
    aCoder_t *newCoder= aGetCoder(format);

    if (newCoder == NULL)
    {
        NOTIFY("RTPSink::setCoder: could not set format %d\n", format);
        return false;
    }

    if (coder != NULL)
    {
        aDeleteCoder(coder);
    }

    coder= newCoder;

    u8 PT= getPTByFmt(format);
    int MSPerFrame= getMSPerPacketByPT(PT);
    fullRateFrameSize = inputRate * MSPerFrame * bps / 1000;
    samplesPerFrame = aGetCoderRate(coder) * MSPerFrame / 1000;
    frameSize = samplesPerFrame*bps;
    header.SetPayloadType(PT);

    if (stats)
    {
        stats->codecChange(aGetFormatNameById(format));
    }

    return true;
}


// -----------------------------------------------------------------------------
// RTPSink::writeBuffer
//
// -----------------------------------------------------------------------------
//
void
RTPSink::writeBuffer(unsigned char *buff, int length, double power)
{
    if (length <= 0)
    {
        return;
    }

    memcpy(buffer+buffered, buff, length);

    int frames = (buffered+length)/fullRateFrameSize;

    if (power <= threshold)
    {
        silenceCounter++;
    }
    else
    {
        silenceCounter = 0;
    }

    buffered = buffered + length - frames*fullRateFrameSize;

    // Resamplear la parte del buffer de audio que se va a enviar
    // desde el rate de la tarjeta al rate del codec
    resampler.resample(buffer,
                       frames*fullRateFrameSize,
                       inputRate,
                       resampledBuffer,
                       BUFFER_SIZE,
                       aGetCoderRate(coder)
                      );

    for (int i = 0; i < frames; i++)
    {
        // El timestamp se actualiza se envie o no
        header.SetTimestamp(header.GetTimestamp() + samplesPerFrame);

        // si hay que enviar y no esta en silencio pues se envia
        if (started && (silenceCounter < SILENCE_PERIODS_TO_CUT) )
        {
            header.SetSeqNumber(header.GetSeqNumber() + 1);

            memcpy(packet, &header, RTPHeader::SIZE);

            int packetLen = 0;

            // Codificar el buffer
            int nencoded= aEncode(coder,
                                  resampledBuffer + frameSize * i,
                                  frameSize / 2,
                                  encodedBuffer
                                 );
            if (nencoded <= 0)
            {
                NOTIFY("Error al codificar el buffer");
                return ;
            }
            memcpy(packet + RTPHeader::SIZE, encodedBuffer, nencoded);
            packetLen = RTPHeader::SIZE + nencoded;

            // Enviar a todas las direcciones de bind
            BINDINGS::iterator it;
            for (it = bindTable.begin(); it != bindTable.end(); it++)
            {
                inetAddr_t *addr = it->second;
                socket->writeTo(*addr, packet, packetLen);
            }

            if (stats)
            {
                stats->packetSent(packetLen);
            }

            // hasta que no pare de enviar el rtp marker a 0
            header.SetMarker(0);

        }
        else
        {
            // No envío, luego el próximo paquete
            // tiene que llevar el rtp marker a 1
            header.SetMarker(1);
        }
    }

    // Lo leido y no enviado lo copio al principio del buffer
    if (buffered > 0 && frames > 0)
    {
        memcpy(buffer, buffer + frames*fullRateFrameSize, buffered);
    }
}

