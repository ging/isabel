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

#include "channelmgr.h"
#include "recchannel.h"
#include "playchannel.h"


// -----------------------------------------------------------------------------
// ChannelManager::ChannelManager
//
// -----------------------------------------------------------------------------
//
ChannelManager::ChannelManager(SoundDevice_t *device,
                               dgramSocket_t *socket,
                               dgramSocket_t *vsocket,
                               const char    *vport,
                               const char    *vplayport,
                               const char    *sport,
                               MixerBuffer   *mbuffer,
                               UdpSound      *udp
                              )
: recChannel(NO_CHANNEL),
  soundDevice(device),
  buffer(mbuffer),
  udpSound(udp),
  rtpSocket(socket),
  vuSocket(vsocket),
  vuPort(vport),
  vuPlayPort(vplayport),
  statsPort(sport)
{
}

// -----------------------------------------------------------------------------
// ChannelManager::~ChannelManager
//
// -----------------------------------------------------------------------------
//
ChannelManager::~ChannelManager(void)
{
}


// -----------------------------------------------------------------------------
// ChannelManager::getChannel
// La referencia devuelta podria no ser valida (NULL)
// -----------------------------------------------------------------------------
//
Channel*
ChannelManager::getChannel(u32 chId)
{
    CHANNEL_MAP::iterator it = chTable.find(chId);

    return (it != chTable.end()) ? it->second : NULL;
}

// -----------------------------------------------------------------------------
// La referencia devuelta podria no ser valida (NULL)
//
// -----------------------------------------------------------------------------
//
RecChannel*
ChannelManager::getRecChannel(void)
{
    Channel *ch= getChannel(recChannel);

    if (ch == NULL)
    {
        return NULL;
    }

    return dynamic_cast<RecChannel*>(ch);
}

// -----------------------------------------------------------------------------
// ChannelManager::newRecChannel
//
// -----------------------------------------------------------------------------
//
bool
ChannelManager::newRecChannel(u32 chId)
{
    // Si el identificador es NO_CHANNEL(0) falla
    if (chId == NO_CHANNEL)
    {
        return false;
    }

    // Si el canal existe falla
    if (getChannel(chId) != NULL)
    {
        return false;
    }

    // Si ya hay un canal de envio falla
    if (recChannel != NO_CHANNEL)
    {
        return false;
    }

    Channel *ch = new RecChannel(chId,
                                 soundDevice,
                                 udpSound,
                                 rtpSocket,
                                 vuSocket,
                                 vuPort,
                                 vuPlayPort,
                                 statsPort
                                );

    chTable.insert(CHANNEL_MAP::value_type(chId,ch));
    recChannel = chId;

    return true;
}


// -----------------------------------------------------------------------------
// ChannelManager::newPlayChannel
//
// -----------------------------------------------------------------------------
//
bool
ChannelManager::newPlayChannel(u32 chId)
{
    // Si el identificador es NO_CHANNEL(0) falla
    if (chId == NO_CHANNEL)
    {
        return false;
    }

    // Si ya existe falla
    if (getChannel(chId) != NULL)
    {
        return false;
    }

    Channel *ch = new PlayChannel(chId, buffer, vuSocket, statsPort);
    chTable.insert(CHANNEL_MAP::value_type(chId,ch));

    return true;
}


// -----------------------------------------------------------------------------
// ChannelManager::deleteChannel
//
// -----------------------------------------------------------------------------
//
bool
ChannelManager::deleteChannel(u32 chId)
{
    Channel *ch = getChannel(chId);

    if (ch == NULL)
    {
        return false;
    }

    chTable.erase(chId);

    delete ch;

    // Si se estaba borrando en canal de envio
    // actualizo el valor de recChannel
    if (chId == recChannel)
    {
        recChannel = NO_CHANNEL;
    }

    return true;
}

// -----------------------------------------------------------------------------
// ChannelManager::getChannelList
//
// -----------------------------------------------------------------------------
//
void
ChannelManager::sendStats(void)
{
    CHANNEL_MAP::iterator it;

    for (it = chTable.begin(); it != chTable.end(); it++)
    {
        it->second->sendStats();
    }
}

