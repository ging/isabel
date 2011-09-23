/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
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

#include "packetqueue.h"
#include "playchannel.h"
#include "rtpheader.h"



// -----------------------------------------------------------------------------
// PacketQueue::PacketQueue
//
// -----------------------------------------------------------------------------
//
PacketQueue::PacketQueue(PlayChannel *ch)
: channel(ch), lastNseq(0), lastTs(0)
{
}


// -----------------------------------------------------------------------------
// PacketQueue::~PacketQueue
//
// -----------------------------------------------------------------------------
//
PacketQueue::~PacketQueue(void)
{
    cleanQueue();
}

// -----------------------------------------------------------------------------
// PacketQueue::packetReceived
//
// -----------------------------------------------------------------------------
//
void PacketQueue::packetReceived(const unsigned char *data, int length)
{
    //channel->packetReceived2(data, length);
    //return;

    const RTPHeader *header = reinterpret_cast<const RTPHeader*>(data);
    u16 nseq = header->GetSeqNumber();
    u32 ts = header->GetTimestamp();

    long long int ltsdiff = (long long int)ts - (long long int)lastTs;
    int tsdiff = (int)ltsdiff;
    int nseqdiff = nseq - lastNseq;
/*
    // nseq sequence cicle test
    if ( abs(nseqdiff) > ( USHRT_MAX - MAX_DIFF ) )
    {
        NOTIFY("Vuelta del NSeq ns=%d last=%d\n", nseq, lastNseq);
        if (nseqdiff > 0)
            nseqdiff-= (USHRT_MAX + 1);
        else
            nseqdiff+= (USHRT_MAX + 1);
    }
*/
    if (abs(tsdiff) > MAX_DIFF_TS || abs(nseqdiff) > MAX_DIFF )
    {
        // new flow, process and clean queue
        channel->packetReceived2(data, length);
        lastNseq = nseq;
        lastTs = ts;
        cleanQueue();
    }
    else if (nseqdiff > 1)
    {
        // Jump in nseq, enqueue
        enqueuePacket(data, length, nseq);
        checkQueue();
    }
    else if (nseqdiff == 1)
    {
        // next packet, process
        channel->packetReceived2(data, length);
        lastNseq = nseq;
        lastTs = ts;
        checkQueue();
    }
    else if (nseqdiff < 0)
    {
        // old packet, discard?
        // stats?
    }
    else if (nseqdiff == 0)
    {
        //duplicate packet, process (for stats)?
        //channel->packetReceived2(data, length);
    }
}

// -----------------------------------------------------------------------------
// PacketQueue::enqueuePacket
//
// -----------------------------------------------------------------------------
//
void
PacketQueue::enqueuePacket(const unsigned char *data, int length, u16 nseq)
{
    unsigned char *buf = new unsigned char[length];
    memcpy(buf, data, length);
    queue.insert(PACKETQUEUE::value_type(nseq, buf));
    lqueue.insert(LENGTHQ::value_type(nseq, length));
}

// -----------------------------------------------------------------------------
// PacketQueue::checkQueue
//
// -----------------------------------------------------------------------------
//
void
PacketQueue::checkQueue(void)
{
    // Max size reached, send first
    if (queue.size() >= MAX_SIZE)
    {
        sendFirst();
    }
    // recorrer la cola para ver si hay paquetes que pueden ser enviados
    while (queue.size() > 0)
    {
        if (queue.begin()->first == lastNseq + 1)
        {
            sendFirst();
        }
        else
        {
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// PacketQueue::cleanQueue
//
// -----------------------------------------------------------------------------
//
void
PacketQueue::cleanQueue(void)
{
    // vaciar el mapa
    while (queue.size() > 0)
    {
        unsigned char *data = queue.begin()->second;
        queue.erase(queue.begin());
        delete[] data;
    }
    lqueue.clear();
}

// -----------------------------------------------------------------------------
// PacketQueue::sendFirst
//
// -----------------------------------------------------------------------------
//
void
PacketQueue::sendFirst(void)
{
    unsigned char *data = queue.begin()->second;
    int length = lqueue.begin()->second;

    const RTPHeader *header = reinterpret_cast<const RTPHeader*>(data);
    lastNseq = queue.begin()->first;
    lastTs = header->GetTimestamp();

    channel->packetReceived2(data, length);
    queue.erase(queue.begin());
    lqueue.erase(lqueue.begin());

    delete []data;
}

