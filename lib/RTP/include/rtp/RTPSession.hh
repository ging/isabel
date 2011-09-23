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
// $Id: RTPSession.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_session_hh__
#define __rtp_session_hh__

#include <string.h>

#ifdef __BUILD_FOR_LINUX
#include <pwd.h>
#endif

#include <icf2/general.h>
#include <icf2/smartReference.hh>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>
#include <icf2/lockSupport.hh>
#include <icf2/sched.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPRandom.hh>
#include <rtp/RTPStructs.hh>
#include <rtp/RTPStatisticsManager.hh>
#include <rtp/RTPPacket.hh>
#include <rtp/RTPSource.hh>

class RTPReceiver_t;
class RTCPReceiver_t;
class RTPFlowsReceived_t;
class RTPTimeOutChecker_t;

class RTPSession_t: public virtual item_t, public virtual collectible_t
{
public:

    /**
    * RTPSession_t constructor.
    * @param sched: scheduler in which RTP and RTCP receiver tasks will be inserted.
    * @param rtpHost: IP address.
    * @param rtpPort: UDP port in which RTP packets will be received.
    * @param rtcpHost: IP address.
    * @param rtcpPort: UDP port in which RTCP compound packets will be received.
    * @param tsUnit: parameter for calculating timestamp increment: timestamp increment = (time difference)/tsUnit.
    * @see RTPMsgSetBandwidth_t
    * @see RTPMsgSetRtcpFragment_t
    * @see RTPMsgSetMaxPacketSize_t
    */
    RTPSession_t (sched_t    *sched,
                  const char *rtpHost,
                  const char *rtpPort,
                  const char *rtcpHost,
                  const char *rtcpPort,
                  double      tsUnit
                 );

    /**
    * RTPSession_t destructor.
    */
    virtual ~RTPSession_t (void);

    /**
    * Method called by RTPSession_t when a RTP packet arrives.
    * This method must be redefined by derived classes.
    * @param packet: RTP packet which provides the interface for getting RTP header fields and data.
    * @see RTPPacket_t
    */
    virtual void receivedData (RTPPacket_t* packet) = 0;

    /**
    * Method called by RTPSession_t when a RTCP BYE packet is received from a given data source.
    * This method must be redefined by derived classes.
    * @param ssrc: RTP data flow Identifier.
    * @param reason: The parameter reason is carried in the packet, and RTPSession_t delivers it to the application.
    */
    virtual void eliminateReceiver (u32 ssrc, const char *reason) = 0;

    /**
    * Method to make the session to deliver any packet to the aplication.
    * @param deliver: If true packets are delivered.
    */
    void deliverAnyPacket(bool deliver);

    /**
    * Method to eliminate the tasks which listen in the sockets.
    * @param sched: scheduler which contains the tasks listeningin the sockets.
    */
    void killSockets(void);

    /**
    * Method to assign a RTPSender_t object to a given data flow in order
    * to form RTP packets.
    * When an application wants to send a data flow using RTP/RTCP library,
    * it must assign several dinamic parameters to be included
    * in RTP headers (SSRC, first sequence number, first timestamp).
    * These parameters will be stored in a RTPSender_t object which
    * will be created with this method.
    * @param ssrc: RTP flow Identifier. It can be generated randomly or not.
    * @see RTPRandom_t
    */
    void assignSender (u32 ssrc);

    /**
    * Method to assign a RTPSender_t object to a given mixed data flow
    * in order to form RTP packets.
    * When an application wants to send a mixed data flow using
    * RTP/RTCP library, it must assign several dinamic parameters to be
    * included in RTP headers (SSRC, first sequence number, first timestamp).
    * These parameters will be stored in a RTPSender_t object
    * which will be created with this method.
    * @param ssrc: RTP flow Identifier. It can be generated randomly or not.
    * @see RTPRandom_t
    */
    void assignMixer (u32 ssrc);

    /**
    * Method to add a contributing source identifier to the RTP header of
    * a mixed RTP flow.
    * The set of CSRC identifiers is stored in the RTPSender_t object
    * combined to the RTP mixed flow. The maximum number of CSRC is 16.
    * @param ssrc: Mixed RTP flow identifier.
    * @param csrc: Contributing Source Identifier.
    */
    void addCsrc (u32 ssrc, u32 csrc);

    /**
    * Method to assign a RTPSender_t object to a given translated data
    * flow in order to form RTP packets.
    * When an application wants to send a translated data flow using
    * RTP/RTCP library, it must assign several dinamic parameters to be
    * included in RTP headers (SSRC, first sequence number, first timestamp).
    * These parameters will be stored in a RTPSender_t object
    * which will be created with this method.
    * @param ssrc: RTP flow Identifier. It can be generated randomly or not.
    * @see RTPRandom_t
    */
    void assignTranslator (u32 ssrc);

    /**
    * Method to eliminate a RTPSender_t object combined to a given data flow.
    * When an application wants to give up sending a given data flow, it can call this method to send a RTCP BYE packet for the
    * corresponding RTP flow.
    * @param ssrc: RTP flow identifier.
    * @param reason: Reason to eliminate the RTP flow, which will be included in the RTCP BYE packet.
    */
    void deleteSender (u32 ssrc, const char *reason);

    /**
    * Method to create a RTPSource_t object in order to receive RTP packets.
    * @param ssrc: RTP flow Identifier.
    * @see RTPRandom_t
    */
    void newFlow (u32 ssrc);

    /**
    * Method to delete a RTPSource_t object in order to give up receiving RTP packets.
    * @param ssrc: RTP flow Identifier.
    * @see RTPRandom_t
    */
    void deleteFlow (u32 ssrc);

    /**
    * Method to assign a binding which packets of a given RTP flow will be sent to.
    * Each RTP flow has a set of bindings which RTP packets will be sent to.
    * @param ssrc: RTP flow identifier.
    * @param host: IP address of the destiny.
    * @param port: UDP port of the destiny.
    * @returns Binding Identifier.
    */
    bindId_t rtpBind (u32 ssrc, const char *host, const char *port);

    /**
    * Method to eliminate a binding which packets of a given RTP flow are being sent to.
    * @param ssrc: RTP flow identifier.
    * @param bId: binding Identifier.
    * @returns The result of the elimination.
    */
    bool rtpUnbind (u32 ssrc, bindId_t bId);

    /**
    * Method to show the set of bindings wich packets of a given RTP flow are being sent to.
    * @param ssrc: RTP flow Identifier.
    * @returns The set of bindings represented by a string of characters, which can be played on the standard output.
    */
    const char *rtpShowBindings (u32 ssrc);

    /**
    * Method to assign a binding which RTCP compound packets will be sent to.
    * @param host: IP address of the destiny.
    * @param port: UDP port of the destiny.
    * @returns Binding Identifier.
    */
    bindId_t rtcpBind (char *host, char *port);

    /**
    * Method to eliminate a binding which RTCP compound packets are being sent to.
    * @param bId: binding Identifier.
    * @returns The result of the elimination.
    */
    bool rtcpUnbind (bindId_t bId);

    /**
    * Method to show the set of bindings wich RTCP componund packets are being sent to.
    * @returns The set of bindings represented by a string of characters, which can be played on the standard output.
    */
    const char *rtcpShowBindings (void);

    /**
    * Method to get the maximum size of data to include in a RTP packet.
    * The RTP header is not fixed because of extensions and contributing sources identifiers can exist or not. This method permits the
    * application to know the maximum size of data which a RTP packet of a given RTP flow can carried.
    * @param ssrc: RTP flow identifier.
    * @param extLength: If the application wants include an extension in the RTP header, this parameters must indicate its size in octets.
    * @returns Maximum number of octets destined to data in a RTP packet for this RTP flow.
    */
    int getMaxPacketSize (u32 ssrc, u16 extLength);

    /**
    * Method to send data using RTP/RTCP library.
    * @param ssrc: RTP flow identifier.
    * @param data: Data to include in the RTP packet.
    * @param length: Data length.
    * @param pt: It indicates the payload type.
    * @param marker: It indicates the value of marker bit of the RTP header.
    * @param incrementTS: It indicates if timestamp of the packet must be incremented respect the previous one.
    * @param extId: Extension header identifier.
    * @param extData: Extension header data.
    * @param extLength: Extension header data length.
    */
    double sendData (u32 ssrc,
                     u8 *data,
                     int length,
                     u8 pt,
                     bool marker,
                     u16 seqNumber,
                     u32 timestamp,
                     u8 padding = 0,
                     u16 extId = 0,
                     u8 *extData = NULL,
                     u16 extLength = 0
                    );

    /**
    * Method to include SDES information for a given RTP flow which is being sending.
    * This information will be carried by RTCP SDES packets.
    * @param srcId: SSRC identifier of RTP flow.
    * @param ident: SDES element identifier.
    * @param info: SDES information.
    * @param prefix: If SDES identifier is PRIV, the SDES is made up by a prefix and the information. This parameter is the prefix.
    */
    void setSDESInfo (u32 srcId, int ident, const char *info, const char *prefix = NULL);

    /**
    * Method to show source description information which is carried by RTCP SDES packet.
    * An application can use this method to know the source description information received for a given RTP flow.
    * @param ssrc: received RTP flow identifier.
    * @returns SDES information represented by a string of charactes, which can be played on the standard output.
    */
    const char *showSDESInfo (u32 ssrc);

    /**
    * Method to show the set of SSRC identifiers of the entities which have sent RTCP RR packets to a given sender.
    * @param ssrc: RTP Sender identifier.
    * @returns Set of SSRC identifiers represented by a string of characters, which can be played on the standard output.
    */
    const char *showReceivers (u32 ssrc);

    /**
    * Method to show the cumulative number of packets of a determined
    * RTP flow that a given receiver hasn't received and it considers
    * lost packets.
    * @param ssrc: RTP Sender identifier.
    * @param receiver: SSRC identifier of the entity which have sent
    * RTCP RR packet with the lost packets information.
    * @returns Total number of lost packets represented by a string of
    * characters, which can be played on the standard output.
    */
    const char *getPacketsLost (u32 ssrc, u32 receiver);
    const char *getPacketsLost (u32 ssrc);

    /**
    * Method to show the % of packets of a determined RTP flow that a given receiver hasn't received and it considers
    * lost packets.
    * @param ssrc: RTP Sender identifier.
    * @param receiver: SSRC identifier of the entity which have sent RTCP RR packet with the % of lost packets information.
    * @returns % of lost packets represented by a string of charactes, which can be played on the standard output.
    */
    const char *getFractionLost (u32 ssrc, u32 receiver);

    /**
    * Method to show the round-trip time of the packets of a determined
    * RTP flow, calculated by a given receiver.
    * @param ssrc: RTP Sender identifier.
    * @param receiver: SSRC identifier of the entity which have sent
    * RTCP RR packet with the round-trip time information.
    * @returns Round-trip time represented by a string of charactes,
    * which can be played on the standard output.
    */
    const char *getRoundTripTime (u32 ssrc, u32 receiver);

    /**
    * Method to show the interarrival jitter of the packets of a determined
    * RTP flow, calculated by a given receiver.
    * @param ssrc: RTP Sender identifier.
    * @param receiver: SSRC identifier of the entity which have sent
    * RTCP RR packet with the interarrival jitter information.
    * @returns Interarrival jitter represented by a string of charactes,
    * which can be played on the standard output.
    */
    const char *getJitter (u32 ssrc, u32 receiver);
    const char *getJitter (u32 ssrc);

    /**
    * Method to begin computing RTCP statistics.
    * @param sched: scheduler in which task will be inserted.
    * @param sessionBandwidth: total bandwidth of the session.
    * @param rtcpFraction: % of total bandwidth reserved to RTCP.
    */
    void computeStatistics (double sessionBandwidth, double rtcpFraction);

    /**
    * Method to set the maximum packet size.
    * @param max: maximum packet size.
    */
    void setMaxPacketSize (int max);

    /**
    * Method to make the session receive a RTP packet.
    * @param packet: received RTP packet.
    */
    void receivePacket (RTPPacket_t *packet);

    /**
    * Method to make the session receive a RTCP report.
    * @param report: received RTCP report.
    */
    void receiveReport (RTCPReport_t *report);

    /**
    * Method to make the session receive a RTCP SDES packet.
    * @param packet: received RTCP SDES packet.
    */
    void receiveSDES (RTCPSDES_t *sdes);

    /**
    * Method to make the session receive a RTCP BYE packet.
    * @param packet: received RTCP BYE packet.
    */
    void receiveBYE (RTCPBYE_t *bye);

    /**
    * Method to make the session check if any flow has left sending packets for 30 minutes.
    * @param timeOutList: list of sources' SSRC identifiers wich has left sending.
    */
    void timeOut (ql_t<u32> *timeOutList);

protected:
    dgramSocket_t *rtpSocket;
    dgramSocket_t *rtcpSocket;

private:

    sched_t *theScheduler;

    RTPReceiver_t *rtpReceiver;
    RTCPReceiver_t *rtcpReceiver;
    RTPFlowsReceived_t *flowsReceived;
    dictionary_t<u32, RTPSender_t *> *senderDictionary;
    RTPStatisticsManager_t *statisticsManager;
    RTPTimeOutChecker_t *timeOutChecker;

    bool deliverAny;

    double timestampUnit;
    int maxPacketSize;

    char usernumber;

    u8 *rtpBuffer;

    int remainder;

    void initialize (const char *rtpHost,
                     const char *rtpPort,
                     const char *rtcpHost,
                     const char *rtcpPort
                    );

    void createCNAME (char *CNAMEBuffer);
    void getLoginName (char *buf, int buflength);

    virtual const char *className(void) const { return "RTPSession_t"; }
};
typedef smartReference_t<RTPSession_t> RTPSession_ref;


#endif

