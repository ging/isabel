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
// $Id: RTPSource.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_source_hh__
#define __rtp_source_hh__

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPStructs.hh>
#include <rtp/RTPPacket.hh>

#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>

class RTPStatisticsManager_t;

/**
* This class represents the statistics store for a given RTP flow which is being received.
* This statistics are used to generate the correct parameters to include into RTCP RR packets.
*/
class  RTPSourceStatistics_t: public item_t
{
public:

    /**
    * RTPSourceStatistics_t constructor.
    */
    RTPSourceStatistics_t (void);

    /**
    * RTPSourceStatistics_t destructor.
    */
    ~RTPSourceStatistics_t (void);

    unsigned int maxSeq;
    unsigned long cycles;
    unsigned long baseSeq;
    unsigned long badSeq;

    unsigned long packetsReceived;
    unsigned long expectedPrior;
    unsigned long receivedPrior;
    unsigned long jitter;
    struct timeval tsTime;
    u32 timestamp;

    bool srreceived;
    struct timeval srtime;
    SenderInfo_t *senderInfo;

    struct timeval rttime;

    /**
    * This method is used to initialize the statistics for a given RTP flow.
    * @param seqnum: first sequence number received from a given RTP flow.
    * @param tstamp: first timestamp received from a given RTP flow.
    */
    void initStatistics (u16 seqnum, u32 tstamp);

    /**
    * This method is used to calculate the interarrival jitter between packets with different timestamp.
    */
    void computeJitter (u32 tstamp, double tsunit);

    virtual const char *className(void) const { return "RTPSourceStatistics_t"; }

};


/**
* This class represents a RTP flow which is being received.
*/
class RTPSource_t: public item_t
{
public:

    /**
    * RTPSource_t constructor.
    * @param id: RTP flow's SSRC identifier.
    * @param tsUnit: This parameter is used to compute the interarrival jitter.
    */
    RTPSource_t (u32 id, double tsUnit);

    /**
    * RTPSource_t destructor.
    */
    ~RTPSource_t (void);

    /**
    * Returns the SSRC identifier.
    */
    inline u32 getSSRC (void) { return ssrc; }

    /**
    * Returns if 2 RTP packets have been received from this RTP flow
    * in an order way.
    */
    bool getProving (void);

    /**
    * checks if some RTP packet has been received from this RTP flow.
    */
    inline bool getPacketReceived (void) { return packetReceived; }

    /**
    * Instant of time in which last RTP packet has been received
    * from this RTP flow.
    */
    unsigned long getLastPacketTime (void);

    /**
    * Process a RTP packet from a given RTP flow.
    * @param seqnum: RTP packet's sequence number. It can be used to know
    *                if the packet arrives in order.
    * @param tstamp: RTP packet's timestamp.
    * @param list: set of CSRC identifiers.
    */
    int processPacket (u16 seqnum, u32 tstamp, ql_t<u32>* list);

    /**
    * Calculates the statistics using the parameters present in RTP header
    * @seqnum: RTP packet's sequence number.
    * @tstamp: RTP packet's timestamp.
    */
    int updateStatistics (u16 seqnum, u32 tstamp);

    /**
    * Process a Sender Info block corresponding to a given RTP flow.
    * @param senderInfo: sender info block carried into a RTCP SR packet.
    */
    void processSenderInfo (SenderInfo_t *senderInfo);

    /**
    * Process a set of SDES items for a given RTP flow which is not being mixed.
    * @param list: set of SDES items to store.
    */
    void processSDES (ql_t<SDESItem_t *> *list);

    /**
    * Process a set of SDES items for a given RTP flow which is being mixed.
    * @param csrc: CSRC identifier.
    * @param list: set of SDES items to store.
    */
    void processSDES (u32 csrc, ql_t<SDESItem_t *> *list);

    /**
    * Process a RTCP BYE packet.
    * It will be used when the RTP flow is being mixed by a given mixer.
    * @param ssrc: RTP flow's identifier.
    */
    void processBYE (u32 ssrc);

    /**
    * This method is called when a RTP packet from a given mixer is received.
    * The set of CSRC identifiers are checked in order to know
    * if some of them have changed.
    * @param csrcList: list of CSRC identifiers.
    */
    void updateCSRC (ql_t<u32> *csrcList);
    inline void setCSRC(void){isaCSRC = true;}

    /**
    * Returns the list of CSRC identifiers of a given mixed RTP flow
    * which is being received.
    */
    ql_t<u32> *getCSRCList(void);

    /**
    * Returns a given SDES item.
    * It is intended to get an SDES item received from each contributing
    * sources which a mixer is mixing.
    * @param csrc: contributing source SSRC identifier.
    * @param identifier: SDES identifier (1..7).
    */
    const char *getSDESItem (u32 csrc, u8 identifier);

    /**
    * Method to show source description information which is carried
    * by RTCP SDES packet.
    * A RTP session can use this method to know the source
    * description information received for a given RTP flow.
    * @returns SDES information represented by a string of
    * characters, which can be played on the standard output.
    */
    char *getSDESItems (void);
    char *getJitter (void);
    char *getPacketsLost (void);

    virtual const char *className(void) const { return "RTPSource_t"; }

    friend class RTPStatisticsManager_t;

private:
    RTPSourceStatistics_t *statistics;
    u32 ssrc;
    double timestampUnit;
    struct timeval lastPacketTime;

    int proving;
    bool packetReceived;
    bool isaCSRC;

    char **sdesInfo;
    dictionary_t<u32, char **> *sdesInfoDict;

    void processSDESItem (ql_t<SDESItem_t *> *list, SDESItem_t *item);
};

#endif

