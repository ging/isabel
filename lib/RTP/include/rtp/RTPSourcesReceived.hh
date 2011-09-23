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
// $Id: RTPSourcesReceived.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_sources_received_hh__
#define __rtp_sources_received_hh__

#include <icf2/item.hh>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>

#include <rtp/RTPSession.hh>
#include <rtp/RTPSource.hh>
#include <rtp/RTPPacket.hh>
#include <rtp/RTPStructs.hh>

class RTPSession_t;

/**
* This class represents the set of RTP flows which are being received.
*/
class  RTPFlowsReceived_t: public item_t
{
public:

    /**
    * RTPFlowsReceived_t constructor.
    */
    RTPFlowsReceived_t (void);

    /**
    * RTPFlowsReceived_t destructor.
    */
    ~RTPFlowsReceived_t (void);

    /**
    * This method is used to create an RTPSource_t object which
    * represents a new RTP flow which is going to be received.
    * @param id: new RTP flow's SSRC identifier.
    * @param tsUnit: used to calculate the interarrival jitter.
    */
    void createFlow (u32 id, double tsUnit);

    /**
    * This method is used to eliminate a given RTP flow which is being received.
    * @param id: RTP flow's SSRC identifer.
    */
    void deleteFlow (u32 id);

    /**
    * This method is used to process a RTP packet from a given RTP flow.
    * @param packet: RTP packet received.
    * @param tsUnit: used to calculate the interarrival jitter.
    */
    bool processRTPPacket (RTPPacket_t *packet, double tsUnit);

    /**
    * This method is used to process a Sender Info block
    * corresponding to a given RTP flow.
    * @param ssrc: SSRC identifier.
    * @param senderInfo: sender info block carried into a RTCP SR packet.
    * @param tsUnit: used to calculate the interarrival jitter.
    */
    void processSRInfo (u32 ssrc, SenderInfo_t *si, double tsUnit);

    /**
    * This method is used to process a set of SDES items for a given RTP flow.
    * @param ssrc: SSRC identifier.
    * @param list: set of SDES items to store.
    */
    void processSDESInfo (u32 ssrc, ql_t<SDESItem_t *> *list);

    /**
    * This method is used to process a RTCP BYE packet.
    * This method will be used when the RTP flow is mixed by a given mixer.
    * @param ssrc: RTP flow's identifier.
    */
    void processBYEInfo (u32 ssrc);

    /**
    * This method permits to get the set of RTP flows received
    * by means of a dictionary.
    */
    dictionary_t<u32, RTPSource_t *> *getFlowDictionary (void)
    {
        return flowDictionary;
    }

    const char *getJitter (u32 ssrc);
    const char *getPacketsLost (u32 ssrc);

    /**
    * This method is used to get the SDES items received for a a given RTP flow.
    * @param ssrc: SSRC identifier.
    */
    const char *getSDESInfo (u32 ssrc);

    virtual const char *className(void) const { return "RTPFlowsReceived_t"; }

private:
    dictionary_t<u32, RTPSource_t *> *flowDictionary;

};


/**
* This class is a task which is used to check if some RTP flow have not received a packet during a period of time superior than 30 minutes.
*/
class  RTPTimeOutChecker_t: public simpleTask_t
{
public:
    /**
    * RTPTimeOutChecker_t constructor.
    * @param session: When a RTP flow exceeds the timeout,
    * this fact is indicated to the session by means of a message.
    * @param flowsReceived: set of RTP flows which are being received.
    */
    RTPTimeOutChecker_t (RTPSession_t *session, RTPFlowsReceived_t *flows);

    /*
    * RTPTimeOutChecker_t destructor.
    */
    ~RTPTimeOutChecker_t (void);

    void heartBeat (void);

    virtual const char *className(void) const { return "RTPTimeOutChecker_t"; }

    friend class smartReference_t<RTPTimeOutChecker_t>;

private:
    RTPSession_t *rtpSession;
    RTPFlowsReceived_t *flowsReceived;
};
typedef smartReference_t<RTPTimeOutChecker_t> RTPTimeOutChecker_ref;


#endif

