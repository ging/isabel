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

#include <icf2/notify.hh>

#include <aCodecs/soundutils.h>

#include "mixerbuffer.h"
#include "vumeter.h"
#include "rtpsink.h"

// -----------------------------------------------------------------------------
// MixerBuffer::MixerBuffer
//
// -----------------------------------------------------------------------------
//
MixerBuffer::MixerBuffer(SoundDevice_t *device,
                         dgramSocket_t *vsocket,
                         const char    *vport
                        )
: minOffset(0),
  soundDevice(device),
  playing (false),
  fileReader(NULL),
  delay(DELAY),
  maxDelay(SoundDevice_t::SAMPLE_RATE/1000*SoundDevice_t::BPS*3500)
{
    rtpsink = new RTPSink(MIX_CHANNEL,
                          vsocket,
                          NULL,
                          SoundDevice_t::SAMPLE_RATE,
                          SoundDevice_t::BPS
                         );

    vumeter = new Vumeter(0, 2, vsocket, vport);
    memset(buffer, 0, BUFFER_SIZE);
}


// -----------------------------------------------------------------------------
// MixerBuffer::~MixerBuffer
//
// -----------------------------------------------------------------------------
//
MixerBuffer::~MixerBuffer(void)
{
    stopFile();
    if (playing)
    {
        soundDevice->stopPlaying();
    }

    if (vumeter)
    {
        delete vumeter;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::bind()
// Mirar excepciones. OJO! Puede tardar mucho en fallar.
// -----------------------------------------------------------------------------
//
int
MixerBuffer::bind(char *host, char *port)
{
    return rtpsink->bind(host, port);
}

// -----------------------------------------------------------------------------
// MixerBuffer::unbind()
//
// -----------------------------------------------------------------------------
//
bool
MixerBuffer::unbind(int bindId)
{
    return rtpsink->unbind(bindId);
}

// -----------------------------------------------------------------------------
// MixerBuffer::playFile
// Abre el fichero en modo de lectura (r) binaria (b) optimizado para acceso
// secuencial (S)
// -----------------------------------------------------------------------------
//
bool
MixerBuffer::playFile(const char *name)
{
    stopFile();

    try
    {
        fileReader= new FileReader_t(name);
    }
    catch (const char *e)
    {
        NOTIFY("playFile: error opening file due to %s", e);
        fileReader= NULL; // paranoid
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// MixerBuffer::stopFile
// Cierra el fichero
// -----------------------------------------------------------------------------
//
void
MixerBuffer::stopFile(void)
{
    if (fileReader)
    {
        delete fileReader;
        fileReader= NULL;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::getMinOffset
// devuelve el puntero de lectura + un retraso
// -----------------------------------------------------------------------------
//
int
MixerBuffer::getMinOffset(void)
{
    return minOffset + delay;
}

// -----------------------------------------------------------------------------
// MixerBuffer::setDelay
// establece el delay y limpia el buffer
// -----------------------------------------------------------------------------
//
void
MixerBuffer::setDelay(int del)
{
    if (del < 100 || del > 300)
    {
        NOTIFY("Bad parameter for delay %d\n", del);
        return;
    }

    int n = del / BYTES_PER_BEAT;

    delay = n * BYTES_PER_BEAT;
    memset(buffer, 0, BUFFER_SIZE);
    minOffset = 0;
}

// -----------------------------------------------------------------------------
// MixerBuffer::setMaxDelay
// establece el delay y limpia el buffer
// -----------------------------------------------------------------------------
//
bool
MixerBuffer::setMaxDelay(int del)
{
    if (del < 300 || del > 4000)
    {
        NOTIFY("Bad parameter for max delay %d\n", del);
        return false;
    }

    maxDelay= SoundDevice_t::SAMPLE_RATE / 1000 * SoundDevice_t::BPS * del;

    memset(buffer, 0, BUFFER_SIZE);
    minOffset = 0;
    return true;
}


// -----------------------------------------------------------------------------
// MixerBuffer::isBetweenPointers
// Copia los datos en la posicion que se le diga.
// -----------------------------------------------------------------------------
//
bool
MixerBuffer::isBetweenPointers(unsigned int down,
                               unsigned int up,
                               unsigned int p
                              )
{
    if (down < up)   // down y up en el mismo ciclo
    {
        if (down <= p && p <= up)
        {
            return true;
        }
    } // up esta en el siguiente ciclo de down
    else
    {
        if (p <= up || p >= down)
        {
            return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// MixerBuffer::write
// Copia los datos en la posicion que se le diga.
// -----------------------------------------------------------------------------
//
int
MixerBuffer::write(int offset,
                   const unsigned char *pBuffer,
                   int length,
                   bool force
                  )
{
    // Si estoy reproduciendo un fichero, no escribo nada
    if (fileReader)
    {
        return delay;
    }

    unsigned int realOffset = offset%BUFFER_SIZE;

    // Esta offset entre minOffset y el máximo buffer tolerado (MAX_BUFFER)?
    unsigned int down = minOffset;
    unsigned int up   = (minOffset+maxDelay)%BUFFER_SIZE;
    if ( ! isBetweenPointers(down, up, realOffset))
    {
        //NOTIFY("MB Desincronizado min=%d real=%d \n", minOffset, realOffset);
        if ( ! force)
        {
            NOTIFY("Paquete descartado por MixerBuffer\n");
            return offset;
        }
        realOffset = (minOffset + delay)%BUFFER_SIZE;
    }

    int wrapAround = realOffset + length - BUFFER_SIZE;

    if (wrapAround <= 0)
    {
        //memcpy(buffer + realOffset, pBuffer, length);
        mix(buffer + realOffset, pBuffer, length);
        return (realOffset + length);
    }
    else
    {
        //memcpy(buffer + realOffset, pBuffer, length - wrapAround);
        //memcpy(buffer, pBuffer + length - wrapAround, wrapAround);

        mix(buffer + realOffset, pBuffer, length - wrapAround);
        mix(buffer, pBuffer + length - wrapAround, wrapAround);

        return wrapAround;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::mix
// Supone audio de 16 bits
// -----------------------------------------------------------------------------
//
void
MixerBuffer::mix(void *dest, const void *source, size_t length)
{
    short *des = (short*)dest;
    const short *sou = (const short*)source;

    size_t len = length/2;

    int temp = 0;

    for (size_t i = 0; i < len; i++)
    {
        temp = des[i] + sou[i];
        if (temp > 32767)         // 32767 = MAX_SIGNED_SHORT
            des[i] = 32767;
        else if (temp < -32767)
            des[i] = -32767;
        else
            des[i] = (short int)temp;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::heartBeat
//
// -----------------------------------------------------------------------------
//
void
MixerBuffer::heartBeat(void)
{
    static int beats= 0;

    int length = BYTES_PER_BEAT;

    // Si estoy reproduciendo un fichero
    // lleno el buffer con datos del fichero
    if (fileReader)
    {
        size_t fromFile= fileReader->read(buffer, length);
        if (fromFile != (unsigned int)length)
        {
            NOTIFY("WARNING: FileReader returned %d, requested %d\n",
                   fromFile,
                   length
                  );
        }
        minOffset= 0;
    }

    // Escribo al dispositivo el siguiente grupo de datos
    // y pongo la parte que he escrito a cero
    int wrapAround = minOffset + length - BUFFER_SIZE;

    if (wrapAround <= 0)   // Esta todo seguido
    {
        double power = SoundUtils::evalPower(buffer + minOffset, length);
        vumeter->nextData(power);
        SoundUtils::EchoSuppressor::nextData(power);

        writeAndClean(buffer + minOffset, length, power);

        minOffset = minOffset + length;
    }
    else   // parte al final y parte al principio
    {
        writeAndClean(buffer + minOffset, length - wrapAround, -50);
        writeAndClean(buffer, wrapAround, -50);

        minOffset = wrapAround;
    }

    // Si el offset queda al final lo pongo al principio
    // para que no se envie un write de cero bytes
    // en el siguiente ciclo.
    if (minOffset == BUFFER_SIZE)
    {
        minOffset = 0;
    }

    // Arranco el dispositivo a la segunda (para que tenga datos suficientes)
    if ( ! playing && beats++ == 1)
    {
        soundDevice->startPlaying();
        playing = true;
    }
}

// -----------------------------------------------------------------------------
// MixerBuffer::writeBuffer
//
// -----------------------------------------------------------------------------
//
void
MixerBuffer::writeAndClean(unsigned char *buf, int length, double power)
{
    soundDevice->write(buf, length);
    rtpsink->writeBuffer(buf, length, power);
    memset(buf, 0, length);
}

