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
// $Id: irouterStats.cc 6038 2004-12-20 14:21:24Z sirvent $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
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

stats_t::stats_t(void)
:__dataPktN(0), __dataBandwidth(0), 
 __fecPktN(0), __fecBandwidth(0),
 __lifeTime(LIFETIME), __statsType(NO_TYPE)
{
}


bool
stats_t::isDead(void) 
{
    return (__lifeTime==0);
}

irouterSendStats_t::irouterSendStats_t(void)
:__dataBytes(0), __fecBytes(0) 
{
    __statsType = SENT;
    gettimeofday(&__startTime, NULL);
    __enqueued = 0;
    __discarded = 0;
    __switchTime = 0;
}

irouterSendStats_t::irouterSendStats_t(irouterSendStats_t &other)
{
    __statsType = other.__statsType;
    __startTime = other.__startTime;
    __currTime  = other.__currTime;
    __dataBytes = other.__dataBytes;
    __fecBytes  = other.__fecBytes;
    __dataPktN  = other.__dataPktN;
    __dataBandwidth = other.__dataBandwidth;
    __lifeTime  = other.__lifeTime;
    __enqueued  = other.__enqueued;
    __discarded = other.__discarded;
    __switchTime= other.__switchTime;
}

irouterSendStats_t::~irouterSendStats_t(void)
{
    // printf("BORRADA ::~irouterStats\n");
}

void  
irouterSendStats_t::getStatistics(void) 
{ 
    gettimeofday(&__currTime, NULL);
    long elapsedMilisec= (__currTime.tv_sec -__startTime.tv_sec )*1000+
                         (__currTime.tv_usec-__startTime.tv_usec)/1000;
    if (elapsedMilisec!=0) {
        __dataBandwidth = ((__dataBytes*8)/(double)elapsedMilisec)*1000;
        __fecBandwidth = ((__fecBytes*8)/(double)elapsedMilisec)*1000;
    }

    debugMsg(dbg_App_Normal,
             "getStatistics",
             "period=%ld bytes=%d bw=%lld\n\n",
             elapsedMilisec,
             __dataBytes,
             __dataBandwidth
            );

    if (__dataPktN != 0){
        __switchTime = __switchTime/__dataPktN;
    }

    if (__dataBytes==0) {
        assert((__lifeTime>0) && "No lo he borrado y esta muerto");
        __lifeTime--;
    } else {
        __lifeTime=LIFETIME;
    }
}
irouterRecvStats_t::irouterRecvStats_t(void)
: __dataBytes(0), __fecBytes(0), __recpPktN(0)
{
    __lostList = new u16[SQ_LEN+1];
    __pLostList = __lostList;
    memset(__lostList,0,SQ_LEN);
    __statsType = RECEIVED;
    gettimeofday(&__startTime, NULL);
    __disorder = 0;
    __duplicated = 0;
    __lostSQ = 0;
    __firstPacket = true;
}

irouterRecvStats_t::~irouterRecvStats_t(void)
{
    delete[] __lostList;
}

void
irouterRecvStats_t::reset(void) 
{

    __dataBandwidth=0;
    __fecBandwidth=0;
    __fecBytes=0;
    __dataPktN=0;
    __fecPktN=0;
    __diff = 0;
    __dataBytes = 0;
    gettimeofday(&__startTime, NULL);
}

void  
irouterRecvStats_t::getStatistics(void) 
{ 
    gettimeofday(&__currTime, NULL);
    long elapsedMilisec= (__currTime.tv_sec -__startTime.tv_sec )*1000+
                         (__currTime.tv_usec-__startTime.tv_usec)/1000;
    if (elapsedMilisec!=0) {
        __dataBandwidth = ((__dataBytes*8)/(double)elapsedMilisec)*1000;
        __fecBandwidth = ((__fecBytes*8)/(double)elapsedMilisec)*1000;
    }

    debugMsg(dbg_App_Normal,
             "getStatistics",
             "period=%ld bytes=%d bw=%lld\n\n",
             elapsedMilisec,
             __dataBytes,
             __dataBandwidth
            );

    if (__dataPktN != 0){
        __diff = __diff/__dataPktN;
    }
   
    if (__dataBytes==0) {
        assert((__lifeTime>0) && "No lo he borrado y esta muerto");
        __lifeTime--;
    } else {
        __lifeTime=LIFETIME;
    }
}

void
irouterRecvStats_t::insertLost(u16 SQ)
{
    //avanzamos ptr
    __pLostList = __lostList + ((int)(__pLostList-__lostList+1))%SQ_LEN;
    *__pLostList = SQ;
    __lostSQ++;
    return;
}

void 
irouterRecvStats_t::checkDuplicated(u16 SQ)
{
    
    //check duplication
    //looking up the SQ
    //into the lostSQ List
    for (u16 * auxPtr = (__lostList + 1);
         auxPtr != __lostList;
         auxPtr = __lostList + ((int)(auxPtr-__lostList+1))%SQ_LEN)
         {
             if (SQ == *auxPtr)
             {
                 __lostSQ--;
                 *auxPtr = 0;
                 return;
             }
         }
     //if not found in LostSQ list
     //then we've a duplicated pkt
     __duplicated++;
          
}

void 
irouterRecvStats_t::addRecvPkt(u32 timestamp,
                               u16 seqnum, 
                               int size, 
                               u32 time) 
{
    __dataPktN++;
    __dataBytes+=size; 
    
    debugMsg(dbg_App_Paranoic, "addRecvPkt", 
        "Resultado bytes=%d\n", __dataBytes);
    
    if (!__firstPacket)
    {
        __diff += (time - __lastArrival);
        __lastArrival = time;
        
        int diffSQ = seqnum - __maxSeqnum;
        
        //check if we have a cicle
        if(abs(diffSQ) > MAX_SEQ_DIFF)
	    {
		   if (diffSQ>0)
		       diffSQ-=MAX_SEQ_NUM;
		   else
		       diffSQ+=MAX_SEQ_NUM;
	    }

        // ALL RIGHT if diff SQ == 0
        //else...
        
        if (diffSQ>0)
        {
            for (u16 i = __maxSeqnum+1; i < seqnum; i++)
            {
                insertLost(i);
            } 
            
            __maxSeqnum = seqnum;
            __maxTimestamp = timestamp;
            
        //if we recive a delayed pkt
        }else{
            
            if (diffSQ<1)
            {
                checkDuplicated(seqnum);
                __disorder++;
            }
        }
    
    //if first packet
    }else{
        __maxSeqnum = seqnum;
        __maxTimestamp = timestamp;
        __firstPacket = false;
        __lastArrival = time;
    }
    
}
    


void 
irouterRecvStats_t::addRecvParity(int size) 
{
    __fecPktN++;
    __fecBytes+=size;
}
   
void 
irouterRecvStats_t::addRecpPkt()
{
    __recpPktN++;
}
 
int 
irouterRecvStats_t::getDataBytes()
{
    return __dataBytes;
}

int 
irouterRecvStats_t::getFecBytes()
{
    return __fecBytes;
}

int 
irouterRecvStats_t::getRecuperated()
{
    return __recpPktN;
}

int 
irouterRecvStats_t::getLost()
{
    return __lostSQ;
}

int 
irouterRecvStats_t::getDisorder()
{
    return __disorder;
}

int 
irouterRecvStats_t::getDuplicated()
{
    return __duplicated;
}
 
u32 
irouterRecvStats_t::getJitter()
{
    return __diff;
} 




