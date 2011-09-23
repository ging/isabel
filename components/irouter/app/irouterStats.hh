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
////////////////////////////////////////////////////////////////////////
//
// $Id: irouterStats.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __irouter_stats_hh__
#define __irouter_stats_hh__

#include <stdlib.h>
#include <string.h>

#include <icf2/icfTime.hh>

#include <icf2/general.h>
#include <icf2/sched.hh>
#include <icf2/sockIO.hh>


#include "flow.hh"

typedef enum {
    SENT=1,
    RECEIVED=2,
    LOST=3
} statsType_e;


class stats_t
{
protected:
    statsType_e statsType;

    int     dataPktN;
    int     dataBytes;
    double  dataBandwidth;

    int     fecPktN;
    int     fecBytes;
    double  fecBandwidth;

    int     lifeTime;
    timeval startTime;
    long    elapsed;  // milliseconds from startTime to currTime

    virtual void computeStats(void);

    static int LIFETIME;

public:
    static int STATISTICS_LEN;

    stats_t(statsType_e sType);
    virtual ~stats_t(void);

    bool isDead(void);
    virtual void reset(void);

    int    getDataPktN(void);
    int    getDataBytes(void);
    double getDataBandwidth(void);
    int    getFecPktN(void);
    int    getFecBytes(void);
    double getFecBandwidth(void);
    long   getElapsed(void);

    friend class irouterStatsGatherer_t;
};

class irouterSendStats_t: public stats_t
{
private:
    unsigned int   discarded;
    unsigned int   enqueued;

public:
    irouterSendStats_t(void);

    virtual ~irouterSendStats_t(void);

    void reset(void);

    void addSentPkt(int size, u32 time);
    void addSentParity(int size);
    void addDiscardedPkt(void);
    void addEnqueuedPkt(int enq);

    int getDiscarded(void);
    int getEnqueued(void);

    friend class irouterStatsGatherer_t;
};


class irouterRecvStats_t: public stats_t
{
private:
    bool  firstPacket;
    u32   maxTimestamp;
    u16   maxSeqnum;

    int   recoveredPktN;
    u16   lostSQ;    // Number of lost packets
    u16  *lostList;  // List of lost SQ
    u16  *pLostList; // pointer to work with the SQ list

    u16   disorder;
    u16   duplicated;

    u32   lastArrival;

    void checkDuplicated(u16);
    void insertLost(u16);

public:
    irouterRecvStats_t(void);
    virtual ~irouterRecvStats_t(void);

    void reset(void);

    void addRecvPkt(u32 timestamp, u16 seqnum, int size, u32 time);
    void addRecvParity(int size);
    void addRecoveredPkt(void);

    int getRecovered(void);
    int getLost(void);
    int getDisorder(void);
    int getDuplicated(void);

    friend class irouterStatsGatherer_t;
};

#endif

