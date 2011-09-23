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

#include "recchannel.h"
#include "sounddevice.h"
#include "vumeter.h"
#include "rtpsink.h"

// -----------------------------------------------------------------------------
// RecChannel::RecChannel
// Codec por defecto none-16
// -----------------------------------------------------------------------------
//
RecChannel::RecChannel(u32            chId,
                       SoundDevice_t *device,
                       UdpSound      *udpsound,
                       dgramSocket_t *sock,
                       dgramSocket_t *vsock,
                       const char    *vport,
                       const char    *vplayport,
                       const char    *sport
                      )
: Channel(chId, Channel::REC_CHANNEL, vsock, sport),
  soundDevice(device),
  udpSound(udpsound),
  socket(sock),
  toneGen(NULL),
  fileReader(NULL),
  gain(0)
{
    rtpsink= new RTPSink(chId,
                         sock,
                         &stats,
                         SoundDevice_t::SAMPLE_RATE,
                         SoundDevice_t::BPS
                        );

    vumeter = new Vumeter(chId, 5, vsock, vport, vplayport);

    setCoder(NONE_16_FORMAT);

    soundDevice->startCapture();
}

// -----------------------------------------------------------------------------
// RecChannel::~RecChannel
//
// -----------------------------------------------------------------------------
//
RecChannel::~RecChannel(void)
{
    soundDevice->stopCapture();
    stopFile();

    if (vumeter)
        delete vumeter;

    if (rtpsink)
        delete rtpsink;

}

// -----------------------------------------------------------------------------
// RecChannel::bind()
// Mirar excepciones. OJO! Puede tardar mucho en fallar.
// -----------------------------------------------------------------------------
//
int
RecChannel::bind(char *host, char *port)
{
    return rtpsink->bind(host, port);
}

// -----------------------------------------------------------------------------
// RecChannel::unbind()
//
// -----------------------------------------------------------------------------
//
bool
RecChannel::unbind(int bindId)
{
    return rtpsink->unbind(bindId);
}

// -----------------------------------------------------------------------------
// RecChannel::start()
// Inicia la captura desde la tarjeta
// -----------------------------------------------------------------------------
//
bool
RecChannel::start(void)
{
    //soundDevice->startCapture();
    soundDevice->resetDevices();

    rtpsink->start();
    return true;
}

// -----------------------------------------------------------------------------
// RecChannel::stop()
// Detiene la captura: TODO: Para vumetros no se puede detener
// -----------------------------------------------------------------------------
//
bool
RecChannel::stop(void)
{
    //soundDevice->stopCapture();
    rtpsink->stop();
    return true;
}


// Envia/Deja de enviar un tono de prueba
// @param active si true, envia el tono, si false deja de enviarlo
//
void
RecChannel::sendTone(bool activate)
{
    if (activate)
    {
        if ( ! toneGen)
        {
            toneGen= new ToneGenerator_t(700, 48000);
        }
    }
    else
    {
        if ( toneGen)
        {
            delete toneGen;
            toneGen= NULL;
        }
    }
}

// -----------------------------------------------------------------------------
// RecChannel::playFile()
// Abre el fichero en modo de lectura (r) binaria (b) optimizado para acceso
// secuencial (S)
// -----------------------------------------------------------------------------
//
bool
RecChannel::playFile(const char *name)
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
// RecChannel::stopFile()
// Cierra el fichero
// -----------------------------------------------------------------------------
//
void
RecChannel::stopFile(void)
{
    if (fileReader)
    {
        delete fileReader;
        fileReader= NULL;
    }
}

// -----------------------------------------------------------------------------
// RecChannel::setCoder()
//
// -----------------------------------------------------------------------------
//
bool
RecChannel::setCoder(int format)
{
    return rtpsink->setCoder(format);
}

// -----------------------------------------------------------------------------
// RecChannel::setSilenceThreshold
//
// -----------------------------------------------------------------------------
//
void
RecChannel::setSilenceThreshold(double limit)
{
    rtpsink->setSilenceThreshold(limit);
}

// -----------------------------------------------------------------------------
// RecChannel::setGain()
//
// -----------------------------------------------------------------------------
//
void
RecChannel::setGain(float g)
{
    gain= g;
}

// -----------------------------------------------------------------------------
// RecChannel::heartBeat()
// Tiene varios static por lo que solo puede haber uno por aplicacion
// -----------------------------------------------------------------------------
//
void
RecChannel::heartBeat(void)
{
    static unsigned char buffer[BUFFER_SIZE];

    int readed= soundDevice->read(buffer, MixerBuffer::BYTES_PER_BEAT);

    if (readed <= 0)
    {
        return ;
    }

    // si estoy reproduciendo un fichero
    // sustituyo los datos por los del fichero
    if (fileReader)
    {
        size_t fromFile= fileReader->read(buffer, readed);
        if (fromFile != (unsigned int)readed)
        {
            NOTIFY("WARNING: FileReader returned %d, requested %d\n",
                   fromFile,
                   readed
                  );
        }
    }

    // Mezclo con la entrada de udp local de sonido
    udpSound->read(buffer, readed);

    // Test tone overrides recorded sound
    if (toneGen)
    {
        toneGen->read(buffer, readed);
    }

    // Calculo la potencia
    double power = SoundUtils::evalPower(buffer, readed);
    SoundUtils::CAG::nextData(power);

    // Aplico la ganancia
    float echo= SoundUtils::EchoSuppressor::getAttenuation();
    float cag = SoundUtils::CAG::getAttenuation();

    if (echo <= -10) cag = 0;

    SoundUtils::bufferAtenuator(buffer, readed, PREAMP+gain+echo+cag);

    // Vemos si es necesario o no enviar estimando la potencia de audio
    // y cortando tras un periodo de silencio prolongado
    power = SoundUtils::evalPower(buffer, readed);
    vumeter->nextData(power);

    rtpsink->writeBuffer(buffer, readed, power);
}

