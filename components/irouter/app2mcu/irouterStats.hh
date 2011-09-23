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
// $Id: irouterStats.hh 6038 2004-12-20 14:21:24Z sirvent $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#ifndef __irouter__stats__hh__
#define __irouter__stats__hh__

#include <icf2/hostArch.h>

#ifdef __BUILD_FOR_LINUX
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include <icf2/icfTime.hh>
#endif

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/smartReference.hh>
#include <icf2/sched.hh>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>


#include "flow.hh"

typedef enum {
    NO_TYPE=0,
    SENT=1,
    RECEIVED=2,
    LOST=3
} statsType_e;



class stats_t: public virtual item_t,
               public virtual collectible_t
{
    protected:
        int            __dataPktN;
        double         __dataBandwidth;
        
        int            __fecPktN;
        double         __fecBandwidth;

        int            __lifeTime;
        statsType_e    __statsType;
        timeval        __currTime;

        virtual void reset()=0;
        virtual void getStatistics()=0; 

        static int LIFETIME;

    public:
        static int STATISTICS_LEN;

        stats_t(void); 
        virtual ~stats_t(void) {}

        bool isDead(void);

        const timeval &getCurrTime(void) const { return __currTime; }

        double getDataBandwidth(void) { return __dataBandwidth;}
        int    getDataPktN(void)      { return __dataPktN;}
        double getFecBandwidth(void)  { return __fecBandwidth;}
        int    getFecPktN(void)       { return __fecPktN;}

        statsType_e getType(void)     { return __statsType;}

    friend class smartReference_t<stats_t>;
    friend class irouterStatsGatherer_t;
};
typedef smartReference_t<stats_t> stats_ref;

class irouterSendStats_t: public stats_t
{
    private:
        unsigned int            __dataBytes;
        unsigned int            __fecBytes;
        unsigned int            __discarded;
        unsigned int            __enqueued;
        struct timeval __startTime;
        u32            __switchTime;

        void  getStatistics(void); 

    public:
        irouterSendStats_t(void);
        irouterSendStats_t(irouterSendStats_t &other);

        virtual ~irouterSendStats_t(void);

        void reset(void) {
            gettimeofday(&__startTime, NULL);
            debugMsg(dbg_App_Normal, "reset", "SEC=%d MICROSEC=%d\n\n\n",
                     __startTime.tv_sec ,__startTime.tv_usec);
            __dataPktN=0;
            __fecPktN=0; 
            __dataBytes = 0; 
            __fecBytes = 0; 
            __switchTime = 0;
        }

        void addSentPkt(int size, u32 time) {
            __dataPktN++; 
            __dataBytes+=size;
            __switchTime += time;
        }

        void addSentParity(int size) {
            __fecPktN++;
            __fecBytes+=size;
        }

        void addDiscardedPkt(void)   { __discarded++;    }
        void addEnqueuedPkt(int enq) { __enqueued = enq; }

        int getDataBytes(void)  { return __dataBytes;  }
        int getFecBytes(void)   { return __fecBytes;   }
        int getDiscarded(void)  { return __discarded;  }
        int getEnqueued(void)   { return __enqueued;   }
        u32 getSwitchTime(void) { return __switchTime; }

        const char *className(void) const { return "irouterSendStats_t";}

    friend class smartReference_t<irouterSendStats_t>;
    friend class irouterStatsGatherer_t;
};
typedef smartReference_t<irouterSendStats_t> irouterSendStats_ref;


class irouterRecvStats_t: public stats_t
{
    private:
        unsigned int            __dataBytes;
        unsigned int            __fecBytes;
        struct timeval __startTime;

        bool           __firstPacket;
        u32            __maxTimestamp;
        u16            __maxSeqnum;
        
        int            __recpPktN;
        u16            __lostSQ; //Number of lost packets
        u16 *          __lostList; //List of lost SQ
        u16 *          __pLostList; //pointer to work with the SQ list

        u16            __disorder;
        u16            __duplicated;

        u32            __lastArrival;
        u32            __diff;

        virtual void getStatistics(void);
        void checkDuplicated(u16);
        void insertLost(u16);

    public:
        irouterRecvStats_t(void);
        virtual ~irouterRecvStats_t(void);

        void reset(void);

        void addRecvPkt(u32 timestamp, u16 seqnum, int size, u32 time);
        void addRecvParity(int size);
        void addRecpPkt(void);
        int getDataBytes(void);
        int getFecBytes(void);
        int getRecuperated(void);
        int getLost(void);
        int getDisorder(void);
        int getDuplicated(void);
        u32 getJitter(void);

        const char *className(void) const { return "irouterRecvStats_t";}

        friend class irouterStatsGatherer_t;
        friend class smartReference_t<irouterRecvStats_t>;
};

typedef smartReference_t<irouterRecvStats_t> irouterRecvStats_ref;
typedef ql_t<irouterRecvStats_ref> irouterRecvStatsList_t;
typedef smartReference_t<irouterRecvStatsList_t> irouterrecvStatsList_ref;

#endif
