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
// $Id: RTPStatisticsManager.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_statistics_manager_hh__
#define __rtp_statistics_manager_hh__

#include <stdlib.h>
#include <string.h>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPSender.hh>
#include <rtp/RTPSource.hh>
#include <rtp/RTPStructs.hh>

#include <icf2/item.hh>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>
#include <icf2/task.hh>

class RTPFlowsReceived_t;


/**
* This class is used to manage the statistics for all RTP flows sent and received.
* Periodically, RTCP compound packets wiil be sent with the corresponding blocks (SR, RR, SDES, BYE...). When a RTCP compound packet
* is sent, the period is calculate again.
*/
class  RTPStatisticsManager_t: public simpleTask_t
{
public:

    /**
    * RTPStatisticsManager_t constructor
    * @param rtcp: socket by means of RTCP compound packets will be sent.
    * @param rtpFlows: set of RTP flows which are being received.
    * @param senderDict: set of RTP flows which are being sent.
    * @param bw: bandwidth destined for RTP session.
    * @param fragment: % of total bandwidth destined for RTCP.
    * @param max: maximum RTP/RTCP packet size.
    */
    RTPStatisticsManager_t (dgramSocket_t *rtcp,
                            RTPFlowsReceived_t *rtpFlows,
                            dictionary_t<u32, RTPSender_t*> *senderDict,
                            double bw,
                            double fragment,
                            int max
                           );

    /**
    * RTPStatisticsManager destructor.
    */
    ~RTPStatisticsManager_t (void);

    /**
    * Method to assign a binding which RTCP compound packets will be sent to.
    * @param host: IP address of the destiny.
    * @param port: UDP port of the destiny.
    * @returns Binding Identifier.
    */
    bindId_t rtcpBind (char *host, char *port);

    /**
    * Eliminates a binding which RTCP compound packets are being sent to.
    * @param bId: binding Identifier.
    * @returns The result of the elimination.
    */
    bool rtcpUnbind (bindId_t bId);

    /**
    * Shows the set of bindings wich RTCP componund packets are being sent to.
    * @returns The set of bindings represented by a string of characters,
    *          which can be played on the standard output.
    */
    char *rtcpShowBindings (void);

    /**
    * Calculates the period of time in which RTCP packets will be sent.
    */
    void rtcpInterval (void);
    void heartBeat (void);

    /**
    * This method is used to set the total bandwidth for the session.
    * @param bw: bandwidth.
    */
    inline void setBandwidth (double bw){ bandwidth = bw;}

    /**
    * This method is used to set the maximum RTP/RTCP packet size.
    * @param max: maximum packet size.
    */
    inline void setMaxPacketSize (int max){ maxPacketSize = max;}

    /**
    * This method is used to set the % of bandwidth destined for the session.
    * @param f: % of bandwidth destined to RTCP.
    */
    inline void setRtcpFragment (double f) { rtcpFragment = f; }
    inline double getRtcpFragment (void) { return rtcpFragment; }

    /**
    * Sends the RTCP compound packets corresponding to a given sender
    * which is not a MIXER or TRANSLATOR.
    * @param sender: RTP Sender. A RTCP SR for this sender will be included
    *                in RTCP compound packet.
    * @param rrs: If its value is true, Report blocks will be included
    *             in RTCP compound packets.
    * @param bye: If its value is true, a RTCP BYE packet will be included
    *             in RTCP compound packet for this sender.
    * @param byeList: list of SSRC identifiers to include in RTCP BYE packet,
    *                 if any. In this case the only SSRC identifier to include
    * is the sender's identifier.
    * @param byeReason: Reason to send a RTCP BYE packet, if any.
    * @param byeLenegth: Reason length, if any.
    */
    int sendRtcpPackets (RTPSender_t *sender,
                         bool rrs,
                         bool bye,
                         ql_t<u32> *byeList = NULL,
                         const char *byeReason = NULL,
                         u8 byeLength = 0
                        );

    /**
    * Sends the RTCP compound packets corresponding to a given sender
    * which is a MIXER.
    * @param sender: RTP Sender. A RTCP SR for this sender will be included
    *                in RTCP compound packet.
    * @param rrs: If its value is true, Report blocks will be included
    *             in RTCP compound packets.
    * @param bye: If its value is true, a RTCP BYE packet will be included
    *             in RTCP compound packet for this sender.
    * @param byeList: list of SSRC identifiers to include in RTCP BYE packet,
    *                 if any. In this case CSRC identifiers will be included.
    * @param byeReason: Reason to send a RTCP BYE packet, if any.
    * @param byeLenegth: Reason length, if any.
    */
    int sendMixerRtcpPackets (RTPSender_t *sender,
                              bool rrs,
                              bool bye,
                              ql_t<u32> *byeList = NULL,
                              const char *byeReason = NULL,
                              u8 byeLength = 0
                             );

    /**
    * Sends the RTCP compound packets corresponding to a given sender
    * which is a TRANSLATOR.
    * @param sender: RTP Sender. A RTCP SR for this sender will be included
    *                in RTCP compound packet.
    * @param rrs: If its value is true, Report blocks will be included
    *             in RTCP compound packets.
    * @param bye: If its value is true, a RTCP BYE packet will be included
    *             in RTCP compound packet for this sender.
    * @param byeList: list of SSRC identifiers to include in RTCP BYE packet,
    *                 if any. In this case translated RTP source's SSRC
    * identifier will be included.
    * @param byeReason: Reason to send a RTCP BYE packet, if any.
    * @param byeLenegth: Reason length, if any.
    */
    int sendTranslatorRtcpPackets (RTPSender_t *sender,
                                   bool rrs,
                                   bool bye,
                                   ql_t<u32> *byeList = NULL,
                                   const char *byeReason = NULL,
                                   u8 byeLength = 0
                                  );

    virtual const char *className(void) const
    {
        return "RTPStatisticsManager_t";
    }

    friend class smartReference_t<RTPStatisticsManager_t>;

private:
    dgramSocket_t *rtcpSocket;
    u8 *rtcpBuffer;
    bindingHolder_t *rtcpHolder;

    RTPFlowsReceived_t *flowsReceived;
    dictionary_t<u32, RTPSender_t*> *senderDictionary;

    u32 provisionalSSRC;

    double bandwidth;
    double rtcpFragment;
    int maxPacketSize;

    int averageRtcpSize;
    double rtcpBandwidth;

    int bufferOffset;
    int compoundPacketSize;
    int rtcpCount;

    u32 period;

    int sendReports (RTPSender_t *sender, bool rrs);
    void sendSDES (RTPSender_t *Sender, bool bye);
    void sendMixerOrTranslatorSDES (ql_t<u32> *flowList, bool bye);
    void sendBye (ql_t<u32> *byeList, const char *byeReason, u8 byeLength);

    u8 *getSRBlock (int len,
                    RTPSender_t *s,
                    int& blockLen
                   );
    u8 *getRRBlock (int len,
                    RTPSource_t *source,
                    int &blockLen,
                    u32 ssrc
                   );
    u8 *getSDESBlock (int len,
                      const char *item,
                      int itemIdentifier,
                      int &blockLen,
                      bool newPacket,
                      bool SDESInPacket,
                      bool needSSRC,
                      u32 ssrc
                     );
    u8 *getBYEBlock (int len,
                     ql_t<u32> *ssrcList,
                     int &blockLen,
                     u8 byeLength,
                     const char *byeReason
                    );
    u32 getRtpTimestamp (struct timeval *curtv,
                         struct timeval *initialtv,
                         u32 timestampOffset,
                         double tsUnit
                        );

    void sendRtcpCompoundPacket (u8 *buffer, int length);

};
typedef smartReference_t<RTPStatisticsManager_t> RTPStatisticsManager_ref;


#endif

