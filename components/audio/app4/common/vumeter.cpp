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
#include "vumeter.h"

#include <icf2/notify.hh>
#include <icf2/sockIO.hh>

#include <math.h>

// -----------------------------------------------------------------------------
// Vumeter::Vumeter
//
// -----------------------------------------------------------------------------
//
Vumeter::Vumeter(u32            achId,
                 int            abeats,
                 dgramSocket_t *sock,
                 const char    *aport,
                 const char    *anotherport
                )
: chId(achId),
  beats(abeats),
  counter(0),
  socket(sock),
  addr2(NULL)
{
    header.SetSSRC(chId);
    header.SetPayloadType(VUMETER_PT);

    addr = new inetAddr_t("127.0.0.1", aport, serviceAddr_t::DGRAM);

    if (anotherport)
        addr2 = new inetAddr_t("127.0.0.1", anotherport, serviceAddr_t::DGRAM);
}

// -----------------------------------------------------------------------------
// Vumeter::~Vumeter
//
// -----------------------------------------------------------------------------
//
Vumeter::~Vumeter(void)
{
    if (addr)
        delete addr;
    if (addr2)
        delete addr2;
}

// -----------------------------------------------------------------------------
// Vumeter::nextData
//
// -----------------------------------------------------------------------------
//
void
Vumeter::nextData(double power)
{
    static unsigned char packet[PACKET_SIZE];

    // Almaceno el dato
    data[counter++] = power;

    // Si hay suficientes datos se envia un paquete
    if (counter >= beats)
    {
        double media = 0;
        for (int i = 0; i < counter; i++)
        {
            media += pow(10.0, data[i]/10);
        }
        media = media/counter;
        media = 10*log10(media);

        media = media*2+100; // Para que el valor quede entre 0 y 100;

        if (media > 100) media = 100;
        if (media < 0) media = 0;
        if (media > 2 && media < 35) media = 35;

        char result = (char)media;

        header.SetSeqNumber(header.GetSeqNumber() + 1);
        header.SetTimestamp(header.GetTimestamp() + 1);

        memcpy(packet, &header, RTPHeader::SIZE);
        memcpy(packet+RTPHeader::SIZE, ((char*)&header) + 8, 4);
        memcpy(packet+RTPHeader::SIZE+4, &result, 1);

        socket->writeTo(*addr, packet, PACKET_SIZE);

        if (addr2)
        {
            socket->writeTo(*addr2, packet, PACKET_SIZE);
        }

        counter = 0;
    }
}

