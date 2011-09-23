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
// $Id: RTPSender.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_sender_hh__
#define __rtp_sender_hh__

#include <icf2/icfTime.hh>

#include <string.h>
#include <sys/types.h>
#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPRandom.hh>
#include <rtp/RTPStructs.hh>
#include <rtp/bindingHolder.hh>

#include <icf2/item.hh>
#include <icf2/notify.hh>
#include <icf2/ql.hh>



/**
* This class represents an entity which has sent a RTCP RR packet.
*/
class  RRSender_t: public virtual item_t
{
public:
    /**
    * RRSender_t constructor.
    * @param src: RR Sender's SSRC identifier.
    * @param rb: First report block received from RR sender.
    */
    RRSender_t (u32 src, ReportBlock_t *rb);

    /**
    * RRSender_t destructor.
    */
    ~RRSender_t (void);

    /**
    * This method processes a Report block received from a given entity.
    * @param reportBlock: report block to process.
    */
    void processReportBlock (ReportBlock_t *rb);

    const char *className(void) const { return "RRSender_t"; }
    friend class RTPSender_t;

private:
    u32 ssrc;
    ReportBlock_t *reportBlock;
    struct timeval rttime;
    struct timeval rrtime;
};


typedef enum {SENDER, MIXER, TRANSLATOR} Sendertype;

/**
* This class represents a RTP flow which is being sent.
* RTPSender_t stores some dinamic parameters like SSRC identifier,
* sequence number, timestamp, set of bindings,...
*/
class  RTPSender_t: public item_t
{
public:

    /**
    * RTPSender_t constructor.
    * @param src: SSRC identifier.
    * @param CNAMEBUFFER: SDES CNAME item. It will be user@host.
    * @param tsUnit: This parameter is used to calculate the timestamp
    * to set in each RTP packet.
    * @param type: Sender type. It can be a simple sender, a mixer or
    * a translator.
    */
    RTPSender_t (u32 src,
                 const char *CNAMEBuffer,
                 double tsUnit,
                 Sendertype type,
                 dgramSocket_t *rtpSocket
                );

    /**
    * RTPSender_t destructor.
    */
    ~RTPSender_t (void);

    /**
    * Method to assign a binding which RTP packets will be sent to.
    * @param h: IP address of the destiny.
    * @param p: UDP port of the destiny.
    * @returns Binding Identifier.
    */
    bindId_t bind (const char *h, const char *p);

    /**
    * Method to eliminate a binding which RTP packets are being sent to.
    * @param bId: binding Identifier.
    * @returns The result of the elimination.
    */
    bool unbind (bindId_t bId);

    /**
    * Method to show the set of bindings which RTP packets are being sent to.
    * @returns The set of bindings represented by a string of characters,
    * which can be played on the standard output.
    */
    const char *showBindings (void);

    /**
    * This method gets the array of bindings which packets are being sent to.
    * @returns The list of bindings.
    */
    const vector_t<bindDescriptor_t*> *getBindings(void);

    /**
    * This method is used to indicate the RTPSender_t to update dinamic parameters to include in the RTP header.
    * @param ts: Its value indicates if it's necessary to increment the timestamp.
    * @param length: data length in bytes.
    */
    void updateParameters (int length, u16 seqNum, u32 ts);

    /**
    * Returns the SSRC identifier.
    */
    inline u32 getSSRC (void) { return ssrc; }

    /**
    * This method is used to get the present sequence number.
    */
    u16 getSequenceNumber (void) {return sequenceNumber;}

    /**
    * This method is used to get the present timestamp.
    */
    u32 getTimestamp (void) {return timestamp;}

    /**
    * This method is used to get the timestamp unit.
    */
    double getTimestampUnit (void) {return timestampUnit;}

    /**
    * The parameter sentData is used to know if it's necessary to
    * send a RTCP SR packet for this flow in the next RTCP compound packet.
    * This method is used to set its value.
    * @ param sent: Its value will be set to setData parameter.
    */
    inline void setSentData (bool sent) {sentData = sent;}

    /**
    * This method is used to get the value of the parameter sendData.
    */
    inline bool getSentData (void) {return sentData;}

    /**
    * This method is used to know the sender's type.
    */
    inline Sendertype getType (void) {return sendertype;}

    /**
    * This method is used to set a new CSRC identifier.
    * If the sender is a MIXER it has a set of CSRC identifiers
    * which will be included in each RTP packet.
    */
    void setCSRC (u32 csrc);

    /**
    * This method is used to get the list of CSRC identifiers.
    */
    ql_t<u32> *getCSRCList (void);

    /**
    * This method is used to set a SDES item which will be included
    * in SDES information sent in RTCP SDES packets.
    * @param info: SDES item.
    * @param pos: SDES identifier (1..7).
    */
    void setSDESInfo (const char *info, int pos);

    /**
    * This method is used to set a SDES item whose identifier is PRIV (7).
    * @param prefix: SDES PRIV prefix.
    * @param info: SDES PRIV info.
    * @param pos: SDES identifier (7).
    */
    void setSDESInfo (const char *prefix, const char *info, int pos);

    /**
    * This method is used to get a given SDES item set for this RTP flow.
    * @param pos: SDES identifier (1..7).
    */
    const char *getSDESInfo (int pos);

    /**
    * This method is used to process a Report block received
    * from a given entity.
    * @param ssrc: RR sender's SSRC identifier.
    * @param reportBlock: report block to process.
    * @returns TRUE if the report block is the first one from this RR sender,
    * false otherwise.
    */
    bool processReportBlock (u32 ssrc, ReportBlock_t *reportBlock);

    /**
    * This method is used to get the list of entities' identifiers
    * which have sent RTCP RR packets.
    */
    ql_t<u32> *getRRList(void);

    /**
    * Method to show the set of SSRC identifiers of the entities which
    * have sent RTCP RR packets to a given sender.
    * @returns Set of SSRC identifiers represented by a string of
    * characters, which can be played on the standard output.
    */
    const char *showReceivers (void);

    /**
    * Method to show the cumulative number of packets of a determined
    * RTP flow that a given receiver hasn't received and it considers
    * lost packets.
    * @param receiver: SSRC identifier of the entity which have sent
    * RTCP RR packet with the lost packets information.
    * @returns Total number of lost packets represented by a string of
    * charactes, which can be played on the standard output.
    */
    const char *getPacketsLost (u32 receiver);

    /**
    * Method to show the % of packets of a determined RTP flow that a
    * given receiver hasn't received and it considers lost packets.
    * @param receiver: SSRC identifier of the entity which have sent
    *        RTCP RR packet with the % of lost packets information.
    * @returns % of lost packets represented by a string of charactes,
    *        which can be played on the standard output.
    */
    const char *getFractionLost (u32 receiver);

    /**
    * Method to show the round-trip time of the packets of a determined
    * RTP flow, calculated by a given receiver.
    * @param receiver: SSRC identifier of the entity which have sent
    * RTCP RR packet with the round-trip time information.
    * @returns Round-trip time represented by a string of charactes,
    * which can be played on the standard output.
    */
    const char *getRoundTripTime (u32 receiver);

    /**
    * Method to show the interarrival jitter of the packets of a
    * determined RTP flow, calculated by a given receiver.
    * @param receiver: SSRC identifier of the entity which have sent
    * RTCP RR packet with the interarrival jitter information.
    * @returns Interarrival jitter represented by a string of charactes,
    * which can be played on the standard output.
    */
    const char *getJitter (u32 receiver);

    const char *className(void) const { return "RTPSender_t"; }

    friend class RTPStatisticsManager_t;

private:
    RTPRandom_t *random;
    bindingHolder_t *rtpHolder;

    u32 ssrc;
    u32 timestamp, timestampOffset, lastTimestamp;
    u16 sequenceNumber;
    double timestampUnit;

    bool sentData;
    Sendertype sendertype;

    ql_t<u32> *csrcList;

    char *SDESInfo [TYPE_SDES + 1];

    ql_t<RRSender_t *> *rrSenderList;

    struct timeval initialTime;
    bool first;
    int octetCount;
    int packetCount;

    dgramSocket_t *socketPrueba;
};

#endif

