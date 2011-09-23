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

#include <string.h>

#include <icf2/sockIO.hh>
#include <icf2/notify.hh>

#include "statssender.h"

// -----------------------------------------------------------------------------
// StatsSender::StatsSender
//
// -----------------------------------------------------------------------------
//
StatsSender::StatsSender(unsigned int id,
                         dgramSocket_t *sock,
                         const char *aport
                        )
: socket(sock),
  totalBitsSent(0),
  totalBitsRecs(0),
  totalPacketsRecs(0),
  totalPacketsLost(0),
  totalPacketsDuplicated(0),
  totalPacketsDisordered(0),
  totalPacketsThrown(0)
{
    addr = new inetAddr_t("127.0.0.1", aport, serviceAddr_t::DGRAM);

    u8  tmp8;
    u32 tmp32;
    memset(buffer, 0, SIZE);

    // Type DATA=1, 0=CONTROL
    tmp8 = 1;
    memcpy(buffer, &tmp8, 1);

    // Componente
    memcpy(buffer+1, "AUD", 3);

    // Identificador de canal
    tmp32 = htonl(id);
    memcpy(buffer+4, &tmp32, 4);

    //Inicializo el nombre del codec
    codecChange("Unknown");
}

// -----------------------------------------------------------------------------
// StatsSender::~StatsSender
//
// -----------------------------------------------------------------------------
//
StatsSender::~StatsSender(void)
{
    if (addr)
        delete addr;
    addr = NULL;
}

// -----------------------------------------------------------------------------
// StatsSender::packetSent
//
// -----------------------------------------------------------------------------
//
void StatsSender::packetSent(int size)
{
    totalBitsSent += size*8;
}

// -----------------------------------------------------------------------------
// StatsSender::packetRecv
//
// -----------------------------------------------------------------------------
//
void StatsSender::packetRecv(int size)
{
    totalBitsRecs += size*8;
    totalPacketsRecs++;
}

// -----------------------------------------------------------------------------
// StatsSender::duplicateRecv
//
// -----------------------------------------------------------------------------
//
void StatsSender::duplicateRecv()
{
    totalPacketsDuplicated++;
}

// -----------------------------------------------------------------------------
// StatsSender::disorderedRecv
//
// -----------------------------------------------------------------------------
//
void StatsSender::disorderedRecv()
{
    totalPacketsDisordered++;
}

// -----------------------------------------------------------------------------
// StatsSender::packetThrown
//
// -----------------------------------------------------------------------------
//
void StatsSender::packetThrown()
{
    totalPacketsThrown++;
}

// -----------------------------------------------------------------------------
// StatsSender::packetLost
//
// -----------------------------------------------------------------------------
//
void StatsSender::packetLost(int howmany)
{
    totalPacketsLost+=howmany;
}

// -----------------------------------------------------------------------------
// StatsSender::codecChange
//
// -----------------------------------------------------------------------------
//
void StatsSender::codecChange(const char* name)
{
    codecNameLen = strlen(name);
    if (codecNameLen < 30)
    {
        strcpy(codecName, name);
    }
    else
    {
        NOTIFY("Codec name too long for stats: %s \n", name);
        strcpy(codecName, "Name too long");
    }
}
// -----------------------------------------------------------------------------
// StatsSender::sendStats
//
// -----------------------------------------------------------------------------
//
void StatsSender::sendStats()
{

    u32 temp = htonl(totalBitsSent*100);
    memcpy(buffer+8, &temp, 4);

    temp = htonl(totalBitsRecs*100);
    memcpy(buffer+12, &temp, 4);

    temp = htonl(totalPacketsRecs);
    memcpy(buffer+16, &temp, 4);

    temp = htonl(totalPacketsLost);
    memcpy(buffer+20, &temp, 4);

    temp = htonl(totalPacketsDuplicated);
    memcpy(buffer+24, &temp, 4);

    temp = htonl(totalPacketsDisordered);
    memcpy(buffer+28, &temp, 4);

    temp = htonl(totalPacketsThrown);
    memcpy(buffer+32, &temp, 4);

    memcpy(buffer+48, &codecNameLen, 1);
    memcpy(buffer+49, codecName, codecNameLen);

    socket->writeTo(*addr, buffer, SIZE);


    totalBitsSent = 0;
    totalBitsRecs = 0;
    /*
    totalPacketsRecs = 0;
    totalPacketsLost = 0;
    totalPacketsDuplicated = 0;
    totalPacketsDisordered = 0;
    totalPacketsThrown = 0;
    */
}
