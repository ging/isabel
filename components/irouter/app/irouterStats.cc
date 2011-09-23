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
/////////////////////////////////////////////////////////////////////////
//
// $Id: irouterStats.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "irouterStats.hh"

#ifdef __BUILD_FOR_LINUX
#include <netinet/in.h>
#endif

//
// irouterStats_t:
//       bandWidth + statsType + numberOfPkts
//
int stats_t::LIFETIME=10;
int stats_t::STATISTICS_LEN=3*sizeof(int);

#define SQ_LEN 100
#define MAX_SEQ_DIFF 0xff
#define MAX_SEQ_NUM  0xffff

stats_t::stats_t(statsType_e sType)
: statsType(sType),
  dataPktN(0),
  dataBytes(0),
  dataBandwidth(0),
  fecPktN(0),
  fecBytes(0),
  fecBandwidth(0),
  lifeTime(LIFETIME)
{
    gettimeofday(&startTime, NULL);
}

stats_t::~stats_t(void)
{
}

bool
stats_t::isDead(void)
{
    return lifeTime == 0;
}

void
stats_t::computeStats(void)
{
    timeval currTime;

    gettimeofday(&currTime, NULL);

    elapsed= (currTime.tv_sec  - startTime.tv_sec )*1000+
             (currTime.tv_usec - startTime.tv_usec)/1000;

    if (elapsed == 0)
    {
        return;
    }

    dataBandwidth = ((dataBytes*8)/(double)elapsed)*1000;
    fecBandwidth = ((fecBytes*8)/(double)elapsed)*1000;

    if (dataBytes == 0)
    {
        assert((lifeTime > 0) && "No lo he borrado y esta muerto");
        lifeTime--;
    }
    else
    {
        lifeTime= LIFETIME;
    }
}

void
stats_t::reset(void)
{
    gettimeofday(&startTime, NULL);

    dataPktN      = 0;
    dataBytes     = 0;
    dataBandwidth = 0;
    fecPktN       = 0;
    fecBytes      = 0;
    fecBandwidth  = 0;
}

int
stats_t::getDataPktN(void)
{
    return dataPktN;
}

int
stats_t::getDataBytes(void)
{
    return dataBytes;
}

double
stats_t::getDataBandwidth(void)
{
    return dataBandwidth;
}

int
stats_t::getFecPktN(void)
{
    return fecPktN;
}

int
stats_t::getFecBytes(void)
{
    return fecBytes;
}

double
stats_t::getFecBandwidth(void)
{
    return fecBandwidth;
}

long
stats_t::getElapsed(void)
{
    return elapsed;
}

irouterSendStats_t::irouterSendStats_t(void)
: stats_t(SENT),
  discarded(0),
  enqueued(0)
{
}

irouterSendStats_t::~irouterSendStats_t(void)
{
}

void
irouterSendStats_t::reset(void)
{
    stats_t::reset();

    discarded  = 0;
    enqueued   = 0;
}

void
irouterSendStats_t::addSentPkt(int size, u32 time)
{
    dataPktN++;
    dataBytes +=size;
}

void
irouterSendStats_t::addSentParity(int size)
{
    fecPktN++;
    fecBytes +=size;
}

void
irouterSendStats_t::addDiscardedPkt(void)
{
    discarded++;
}

void
irouterSendStats_t::addEnqueuedPkt(int enq)
{
    enqueued = enq;
}

int
irouterSendStats_t::getDiscarded(void)
{
    return discarded;
}

int
irouterSendStats_t::getEnqueued(void)
{
    return enqueued;
}

irouterRecvStats_t::irouterRecvStats_t(void)
: stats_t(RECEIVED),
  recoveredPktN(0),
  lostSQ(0),
  disorder(0),
  duplicated(0)
{
    lostList = new u16[SQ_LEN+1];
    pLostList = lostList;
    memset(lostList,0,SQ_LEN);

    firstPacket = true;
}

irouterRecvStats_t::~irouterRecvStats_t(void)
{
    delete[] lostList;
}

void
irouterRecvStats_t::reset(void)
{
    stats_t::reset();

    recoveredPktN = 0;
    disorder      = 0;
    duplicated    = 0;
}

void
irouterRecvStats_t::insertLost(u16 SQ)
{
    //avanzamos ptr
    pLostList = lostList + ((int)(pLostList-lostList+1))%SQ_LEN;
    *pLostList = SQ;
    lostSQ++;
    return;
}

void
irouterRecvStats_t::checkDuplicated(u16 SQ)
{
    // check duplication
    // looking up the SQ
    // into the lostSQ List
    for (u16 * auxPtr = (lostList + 1);
         auxPtr != lostList;
         auxPtr = lostList + ((int)(auxPtr - lostList+1))%SQ_LEN
        )
    {
        if (SQ == *auxPtr)
        {
            lostSQ--;
            *auxPtr = 0;
            return;
        }
    }

    // if not found in LostSQ list
    // then we've a duplicated pkt
    duplicated++;
}

void
irouterRecvStats_t::addRecvPkt(u32 timestamp, u16 seqnum, int size, u32 time)
{
    dataPktN++;
    dataBytes += size;

    if ( ! firstPacket)
    {
        lastArrival = time;

        int diffSQ = seqnum - maxSeqnum;

        // check if we have a cicle
        if (abs(diffSQ) > MAX_SEQ_DIFF)
        {
           if (diffSQ > 0)
               diffSQ -= MAX_SEQ_NUM;
           else
               diffSQ += MAX_SEQ_NUM;
        }

        // ALL RIGHT if diff SQ == 0
        // else...

        if (diffSQ > 0)
        {
            for (u16 i = maxSeqnum+1; i < seqnum; i++)
            {
                insertLost(i);
            }

            maxSeqnum = seqnum;
            maxTimestamp = timestamp;

        // if we receive a delayed pkt
        }
        else
        {
            if (diffSQ < 1)
            {
                checkDuplicated(seqnum);
                disorder++;
            }
        }

    // if first packet
    }
    else
    {
        maxSeqnum = seqnum;
        maxTimestamp = timestamp;
        firstPacket = false;
        lastArrival = time;
    }
}


void
irouterRecvStats_t::addRecvParity(int size)
{
    fecPktN++;
    fecBytes += size;
}


void
irouterRecvStats_t::addRecoveredPkt(void)
{
    recoveredPktN++;
}


int
irouterRecvStats_t::getRecovered(void)
{
    return recoveredPktN;
}


int
irouterRecvStats_t::getLost(void)
{
    return lostSQ;
}


int
irouterRecvStats_t::getDisorder(void)
{
    return disorder;
}


int
irouterRecvStats_t::getDuplicated(void)
{
    return duplicated;
}


