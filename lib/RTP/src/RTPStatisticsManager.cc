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
// $Id: RTPStatisticsManager.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#include <rtp/RTPStatisticsManager.hh>
#include <rtp/RTPSourcesReceived.hh>

RTPStatisticsManager_t::RTPStatisticsManager_t (dgramSocket_t *rtcp,
                                                RTPFlowsReceived_t *rtpFlows,
                                                dictionary_t<u32, RTPSender_t*> *senderDict,
                                                double bw,
                                                double fragment,
                                                int max
                                                )
                                                :simpleTask_t(0)
{
    debugMsg(dbg_App_Verbose,
             "RTPStatisticsManager_t",
             "Creating RTPStatisticsManager_t"
            );

    rtcpSocket = rtcp;
    rtcpBuffer = (u8 *)malloc (RTP_BUFFER_SIZE);

    if ( ! rtcpBuffer)
    {
        NOTIFY("RTPSession_t:: No enough memory for rtcpBuffer, bailing out\n");
        abort();
    }

    rtcpHolder = new bindingHolder_t;
    RTPRandom_t random;
    provisionalSSRC = (u32)random.random32();

    debugMsg(dbg_App_Normal,
             "RTPStatisticsManager_t",
             "Creating SSRC for RTCP RR packets: %u",
             provisionalSSRC
            );

    flowsReceived = rtpFlows;
    senderDictionary = senderDict;
    bandwidth = bw;
    rtcpFragment = fragment;
    maxPacketSize = max;
    compoundPacketSize = 0;
    rtcpCount = RTCP_SDES_INTERVAL;
    averageRtcpSize = 128; //20 IP + 8 UDP + 52 SR + 48 SDES CNAME
    period = 0;
}


RTPStatisticsManager_t::~RTPStatisticsManager_t (void)
{
    debugMsg(dbg_App_Verbose,
             "~RTPStatisticsManager_t",
             "Deleting RTPStatisticsManager_t"
            );

    delete rtcpHolder;
}


bindId_t
RTPStatisticsManager_t::rtcpBind (char *host, char *port)
{
    return rtcpHolder -> bind (host, port);
}

bool
RTPStatisticsManager_t::rtcpUnbind (bindId_t bId)
{
    return rtcpHolder -> unbind (bId);
}

char *
RTPStatisticsManager_t::rtcpShowBindings (void)
{
    return rtcpHolder -> showBindings ();
}


void
RTPStatisticsManager_t::rtcpInterval (void)
{
    double delay = 0;
    //double rtcpMinTime = RTCP_MIN_TIME;
    int numberOfSenders = 0;
    int numberOfParticipants = 0;
    int n;

    ql_t<u32> *senderList = senderDictionary -> getKeys();
    dictionary_t<u32, RTPSource_t *> *flowDictionary =
        flowsReceived -> getFlowDictionary ();

    ql_t<u32> *flowList = flowDictionary -> getKeys();

    if (senderList -> len() != 0)
    {
        numberOfParticipants = flowList -> len() + senderList -> len();

    }
    else if (flowList -> len() != 0)
    {
        numberOfParticipants = flowList -> len() + 1;
    }

    bool sentData = false;

    for (ql_t<u32>::iterator_t i = senderList -> begin();
         i != senderList -> end();
         i++)
         {
             u32 src = static_cast<u32>(i);
             RTPSender_t *sender = senderDictionary -> lookUp (src);
             if (sender -> getSentData())
             {
                 sentData = true;
             }
             sender -> setSentData(false);
             ql_t<u32> *rrList = sender -> getRRList();

             for (ql_t<u32>::iterator_t j = rrList -> begin();
                  j != rrList -> end();
                  j++)
                  {
                      u32 rrSrc = static_cast<u32>(j);
                      bool found = false;

                      for (ql_t<u32>::iterator_t k = flowList -> begin();
                           k != flowList -> end();
                           k++)
                           {
                               u32 sourceSSRC = static_cast<u32>(k);
                               if (sourceSSRC == rrSrc)
                                   found = true;
                           }
                           if (!found)
                               numberOfParticipants++;
                  }
             //delete rrList;
         }

    numberOfSenders = flowList -> len() + senderList -> len();
    n = numberOfParticipants;
    if (n == 0)
    {
        period = 0;
        this -> set_period (0);
        return;
    }

    if ((numberOfSenders > 0) &&
        (numberOfSenders < numberOfParticipants * RTCP_SENDER_BW_FRACTION))
    {
        if (sentData)
        {
            rtcpFragment *= RTCP_SENDER_BW_FRACTION;
            n = numberOfSenders;

        }else{

            rtcpFragment *= (1 - RTCP_SENDER_BW_FRACTION);
            n -= numberOfSenders;
        }
    }

    averageRtcpSize += (int)((compoundPacketSize - averageRtcpSize) * RTCP_SIZE_GAIN);
    rtcpBandwidth = bandwidth * rtcpFragment;
    delay = averageRtcpSize * n / rtcpBandwidth;
    delay *= 1000000;

    if (delay < RTCP_MIN_TIME)
    {
        delay = RTCP_MIN_TIME;
    }

    unsigned long seed = time(NULL);
    seed -= clock();
    seed += (unsigned int)this;
    srand(seed);
    int random = rand ();
    double offset = ( (double)random )/( (double)RAND_MAX + 1.0) + 0.5;
    offset *= 1000000;
    delay += offset;

    u32 intDelay = (u32)delay;
    if (intDelay != period)
    {
        this -> set_period (intDelay);
        period = intDelay;
    }

    delete senderList;
    delete flowList;

    debugMsg(dbg_App_Normal,
             "rtcpInterval",
             "Interval for RTCP packets: %d"
             "Bandwidth for RTCP packets: %.3f"
             "Number of participants: %d\n",
             period,
             rtcpBandwidth,
             n
            );
}


void
RTPStatisticsManager_t::heartBeat (void)
{
    bool rrs = true;
    ql_t<u32> *senderList = senderDictionary -> getKeys();
    if (senderList -> len() != 0)
    {
        for (ql_t<u32>::iterator_t i = senderList -> begin();
             i != senderList -> end();
             i++)
        {
            u32 ssrc = static_cast<u32>(i);
            RTPSender_t *s = senderDictionary -> lookUp (ssrc);
            Sendertype t = s -> getType ();
            switch (t)
            {
            case SENDER:
                sendRtcpPackets (s, rrs, false);
                break;

            case MIXER:
                debugMsg(dbg_App_Normal, "heartBeat", "Mixer RTCP");
                sendMixerRtcpPackets (s, rrs, false);
                break;

            case TRANSLATOR:
                debugMsg(dbg_App_Normal, "heartBeat", "Translator RTCP");
                sendTranslatorRtcpPackets (s, rrs, false);
                break;
            }

            rrs = false;
        }
    }
    else
    {
        sendRtcpPackets (NULL, rrs, false);
    }

    rtcpCount--;
    if (rtcpCount < 0)
    {
        rtcpCount = RTCP_SDES_INTERVAL;
    }

    rtcpInterval ();
    compoundPacketSize = 0;

    delete senderList;
}

int
RTPStatisticsManager_t::sendRtcpPackets (RTPSender_t *sender,
                                         bool rrs,
                                         bool bye,
                                         ql_t<u32> *byeList,
                                         const char *byeReason,
                                         u8 byeLength
                                        )
{
    int status;
    status = sendReports (sender, rrs);
    if ( ! status)
        return -1;

    sendSDES (sender, bye);
    if (bye)
        sendBye (byeList, byeReason, byeLength);

    return 0;
}

int
RTPStatisticsManager_t::sendMixerRtcpPackets (RTPSender_t *sender,
                                              bool rrs,
                                              bool bye,
                                              ql_t<u32> *byeList,
                                              const char *byeReason,
                                              u8 byeLength
                                             )
{
    int status;
    status = sendReports (sender, rrs);
    if ( ! status)
        return -1;

    sendMixerOrTranslatorSDES (sender -> getCSRCList(), bye);

    if (bye)
        sendBye (byeList, byeReason, byeLength);

    return 0;
}

int
RTPStatisticsManager_t::sendTranslatorRtcpPackets (RTPSender_t *sender,
                                                   bool rrs,
                                                   bool bye,
                                                   ql_t<u32> *byeList,
                                                   const char *byeReason,
                                                   u8 byeLength
                                                  )
{
    int status;
    status = sendReports (sender, rrs);
    if ( ! status)
        return -1;

    ql_t<u32> *flowList = new ql_t<u32>;
    flowList -> insert (sender -> getSSRC());
    sendMixerOrTranslatorSDES (flowList, bye);
    delete flowList;

    if (bye)
        sendBye (byeList, byeReason, byeLength);

    return 0;
}

int
RTPStatisticsManager_t::sendReports (RTPSender_t *sender, bool rrs)
{
    u8 *block;
    RTCPHeader_t *rtcpHeaderReports;
    dictionary_t<u32, RTPSource_t *> *flowDictionary;
    int blockLength = 0;
    int RRsInPacket = 0;
    int packetLength = 0;

    bufferOffset = 0;
    rtcpHeaderReports = (RTCPHeader_t *)rtcpBuffer;

    if ( (sender != NULL) && (sender -> getSentData()) )
    {
        block = getSRBlock (maxPacketSize, sender, blockLength);
        if (block == NULL)
        {
            fprintf (stderr, "No enough packet size for Sender Report\n");
            return 0;
        }
        memcpy ( (rtcpBuffer + bufferOffset), block, blockLength);
        delete block;
        block = NULL;
        bufferOffset += blockLength;
        packetLength += blockLength;
        debugMsg(dbg_App_Normal, "sendReports", "SR length: %d", blockLength);
        debugMsg(dbg_App_Verbose, "sendReports", "Offset after SR: %d", bufferOffset);
    }

    flowDictionary = flowsReceived -> getFlowDictionary ();
    ql_t<u32> *flowList = flowDictionary -> getKeys();
    if ( (rrs == true) && (flowList -> len() > 0) )
    {
        for (ql_t<u32>::iterator_t i = flowList -> begin ();
             i != flowList -> end ();
             i++
            )
        {
            u32 id = static_cast<u32> (i);
            RTPSource_t * flowReceived = flowDictionary -> lookUp (id);
            if (flowReceived -> getPacketReceived ())
            {
                u32 src;

                src = (sender == NULL) ? provisionalSSRC : sender -> getSSRC();

                block = getRRBlock (maxPacketSize - bufferOffset,
                                    flowReceived,
                                    blockLength,
                                    src);

                if (block == NULL)
                {
                    rtcpHeaderReports->blockcount = RRsInPacket;
                    rtcpHeaderReports->length = htons(packetLength / 4 - 1);
                    compoundPacketSize += packetLength;
                    sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
                    bufferOffset = 0;
                    RRsInPacket = 0;
                    packetLength = 0;
                }
                else
                {
                    memcpy ( (rtcpBuffer + bufferOffset), block, blockLength);
                    delete block;
                    block = NULL;
                    bufferOffset += blockLength;
                    packetLength += blockLength;
                    RRsInPacket++;
                    if (RRsInPacket == 31)
                    {
                        rtcpHeaderReports->blockcount = RRsInPacket;
                        rtcpHeaderReports->length = htons(packetLength / 4 - 1);
                        compoundPacketSize += packetLength;
                        sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
                        bufferOffset = 0;
                        RRsInPacket = 0;
                        packetLength = 0;
                    }
                }
            }
        }
    }

    delete flowList;

    if (bufferOffset != 0)
    {
        rtcpHeaderReports -> blockcount = RRsInPacket;
        rtcpHeaderReports -> length = htons(packetLength / 4 - 1);
        compoundPacketSize += packetLength;
    }

    return 1;
}


void
RTPStatisticsManager_t::sendSDES (RTPSender_t *sender, bool bye)
{
    u8 *block;
    int blockLength = 0;
    bool newPacket;
    RTCPHeader_t *rtcpHeaderSDES;

    if (bufferOffset == 0)
    {
        newPacket = true;
        rtcpHeaderSDES = (RTCPHeader_t *)(rtcpBuffer + sizeof(RTCPHeader_t));
    }
    else
    {
        newPacket = false;
        rtcpHeaderSDES = (RTCPHeader_t *)(rtcpBuffer + bufferOffset);
    }

    int SDESPacketLength = 0;
    bool SDESInPacket = false;
    int numberOfSDESItems = 1;
    if (sender != NULL)
    {
        while (numberOfSDESItems <= (int)(TYPE_SDES - 1))
        {
            if ( (numberOfSDESItems == TYPE_SDES_CNAME ) || (rtcpCount == 0) )
            {
                const char *item = sender -> getSDESInfo (numberOfSDESItems);
                if (item != NULL)
                {
                    block = getSDESBlock (maxPacketSize - bufferOffset,
                                          item,
                                          numberOfSDESItems,
                                          blockLength,
                                          newPacket,
                                          SDESInPacket,
                                          false,
                                          sender -> getSSRC()
                                         );

                    if (block == NULL)
                    {
                        if (SDESPacketLength > 0)
                        {
                            rtcpHeaderSDES -> blockcount = 1;
                            rtcpBuffer [bufferOffset++] = 0;
                            SDESPacketLength++;

                            if ( (bufferOffset % sizeof (u32) ) != 0)
                            {
                                while ((bufferOffset % sizeof (u32)) != 0)
                                {
                                    rtcpBuffer [bufferOffset++] = 0;
                                    SDESPacketLength++;
                                }
                            }
                            rtcpHeaderSDES -> length = htons ((SDESPacketLength / 4) - 1);
                        }
                        compoundPacketSize += SDESPacketLength;
                        sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
                        newPacket = true;
                        //es posible que falte SDESInPacket = false;
                        rtcpHeaderSDES = (RTCPHeader_t *)(rtcpBuffer + sizeof (RTCPHeader_t));
                        bufferOffset = 0;
                        SDESPacketLength = 0;
                    }
                    else
                    {
                        memcpy ( (rtcpBuffer + bufferOffset), block, blockLength);
                        delete block;
                        block = NULL;

                        if (bufferOffset == 0)
                            SDESPacketLength += (blockLength - sizeof (RTCPHeader_t));
                        else
                            SDESPacketLength += blockLength;

                        bufferOffset += blockLength;
                        numberOfSDESItems++;
                        SDESInPacket = true;
                    }
                }
                else
                {
                    numberOfSDESItems++;
                }
            }
            else
            {
                numberOfSDESItems++; //es posible que break;
            }
        }

        if (bufferOffset != 0)
        {
            if (SDESPacketLength > 0)
            {
                rtcpHeaderSDES -> blockcount = 1;
                rtcpBuffer [bufferOffset++] = 0;
                SDESPacketLength++;
                if ( (bufferOffset % sizeof (u32)) != 0)
                {
                    while ((bufferOffset % sizeof (u32)) != 0)
                    {
                        rtcpBuffer [bufferOffset++] = 0;
                        SDESPacketLength++;
                    }
                }

                rtcpHeaderSDES -> length = htons ((SDESPacketLength / 4) - 1);
            }
            compoundPacketSize += SDESPacketLength;
        }
    }

    if ( ( ! bye) && (bufferOffset != 0) )
    {
        sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
    }
}


void
RTPStatisticsManager_t::sendMixerOrTranslatorSDES (ql_t<u32> *list, bool bye)
{
    u8 *block;
    int blockLength = 0;
    bool newPacket;
    RTCPHeader_t *rtcpHeaderSDES;

    if (bufferOffset == 0)
    {
        newPacket = true;
        rtcpHeaderSDES = (RTCPHeader_t *)(rtcpBuffer + sizeof(RTCPHeader_t));
    }
    else
    {
        newPacket = false;
        rtcpHeaderSDES = (RTCPHeader_t *)(rtcpBuffer + bufferOffset);
    }

    int SDESPacketLength = 0;
    bool SDESInPacket = false;
    int sourceCount = 0;
    int numberOfSDESItems = 1;
    dictionary_t<u32, RTPSource_t*> *flowDictionary =
        flowsReceived -> getFlowDictionary();

    for (ql_t<u32>::iterator_t i = list -> begin();
         i!= list -> end();
         i++
        )
    {
        u32 csrc = static_cast<u32> (i);
        RTPSource_t *source = flowDictionary -> lookUp (csrc);
        bool sourceProcessed = false;
        while (numberOfSDESItems <= (int) (TYPE_SDES - 1))
        {
            if ( (numberOfSDESItems == 1 ) || (rtcpCount == 0) )
            {
                const char *item = source -> getSDESItem (csrc, (u8)numberOfSDESItems);
                if (item != NULL)
                {
                    block = getSDESBlock (maxPacketSize - bufferOffset,
                                          item,
                                          numberOfSDESItems,
                                          blockLength,
                                          newPacket,
                                          SDESInPacket,
                                          !sourceProcessed,
                                          csrc
                                         );

                    if (block == NULL)
                    {
                        if (SDESPacketLength > 0)
                        {
                            rtcpHeaderSDES -> blockcount = sourceCount;
                            rtcpBuffer [bufferOffset++] = 0;
                            SDESPacketLength++;

                            if ( (bufferOffset % sizeof (u32)) != 0)
                            {
                                while ((bufferOffset % sizeof (u32)) != 0)
                                {
                                    rtcpBuffer [bufferOffset++] = 0;
                                    SDESPacketLength++;
                                }
                            }
                            rtcpHeaderSDES -> length = htons ((SDESPacketLength / 4) - 1);
                        }
                        compoundPacketSize += SDESPacketLength;
                        sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
                        newPacket = true;
                        rtcpHeaderSDES = (RTCPHeader_t *)(rtcpBuffer + sizeof (RTCPHeader_t));
                        bufferOffset = 0;
                        SDESPacketLength = 0;
                        sourceProcessed = false;
                        sourceCount = 0;
                    }
                    else
                    {
                        memcpy ( (rtcpBuffer + bufferOffset), block, blockLength);
                        delete block;
                        block = NULL;
                        if (bufferOffset == 0)
                            SDESPacketLength += ( blockLength - sizeof (RTCPHeader_t) );
                        else
                            SDESPacketLength += blockLength;
                        bufferOffset += blockLength;
                        numberOfSDESItems++;
                        SDESInPacket = true;
                        if (!sourceProcessed)
                            sourceCount++;
                        sourceProcessed = true;
                    }
                }else
                    numberOfSDESItems++;
            }else
                numberOfSDESItems++;
        }

        if (sourceProcessed)
        {
            numberOfSDESItems = 1;
            rtcpBuffer [bufferOffset++] = 0;
            SDESPacketLength++;
        }
    }

    if (SDESPacketLength > 0)
    {
        rtcpHeaderSDES -> blockcount = sourceCount;
        if ( (bufferOffset % sizeof (u32)) != 0)
        {
            while ((bufferOffset % sizeof (u32)) != 0)
            {
                rtcpBuffer [bufferOffset++] = 0;
                SDESPacketLength++;
            }
        }
        rtcpHeaderSDES -> length = htons ((SDESPacketLength / 4) - 1);
    }

    if ( (! bye) && (bufferOffset != 0) )
    {
        sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
    }
}


void
RTPStatisticsManager_t::sendBye (ql_t<u32> *byeList,
                                 const char *byeReason,
                                 u8 byeLength
                                )
{
    u8 *block;
    int blockLength;
    RTCPHeader_t *rtcpHeaderBYE;
    int BYELength = 0;
    bool inserted = false;
    rtcpHeaderBYE = (RTCPHeader_t *)(rtcpBuffer + bufferOffset);

    if (byeList -> len() != 0)
    {
        while ( ! inserted)
        {
            block = getBYEBlock (maxPacketSize - bufferOffset,
                                 byeList,
                                 blockLength,
                                 byeLength,
                                 byeReason
                                );

            if (block == NULL)
            {
                sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
                bufferOffset = 0;
                rtcpHeaderBYE = (RTCPHeader_t *)(rtcpBuffer + sizeof(RTCPHeader_t));
            }
            else
            {
                inserted = true;
                memcpy ( (rtcpBuffer + bufferOffset), block, blockLength);
                delete block;
                block = NULL;
                if (bufferOffset == 0)
                    BYELength += ( blockLength - sizeof (RTCPHeader_t) );
                else
                    BYELength += blockLength;
                bufferOffset += blockLength;
                rtcpHeaderBYE -> blockcount = byeList -> len();
                rtcpBuffer [bufferOffset++] = 0;
                BYELength++;

                if ( (bufferOffset % sizeof (unsigned long)) != 0)
                {
                    while ((bufferOffset % sizeof (unsigned long)) != 0)
                    {
                        rtcpBuffer [bufferOffset++] = 0;
                        BYELength++;
                    }
                }
                compoundPacketSize += BYELength;
            }
        }
        rtcpHeaderBYE -> length = htons ((BYELength / 4) - 1);
    }
    sendRtcpCompoundPacket (rtcpBuffer, bufferOffset);
}



u8*
RTPStatisticsManager_t::getSRBlock (int len, RTPSender_t *sender, int &blockLen)
{
    RTCPHeader_t *rtcpHeader;
    SSRCHeader_t *ssrcHeader;
    SenderInfo_t *senderInfo;
    struct timeval curtv;
    struct timeval initialtv = sender -> initialTime;
    blockLen =   sizeof(RTCPHeader_t)
               + sizeof(SSRCHeader_t)
               + sizeof(SenderInfo_t);

    if (len < blockLen)
    {
        blockLen = 0;
        return NULL;
    }

    u8 *block = new u8[blockLen];
    rtcpHeader = (RTCPHeader_t *)block;
    rtcpHeader -> version = RTP_VERSION;
    rtcpHeader -> padding = 0;
    rtcpHeader -> packettype = TYPE_RTCP_SR;
    ssrcHeader = (SSRCHeader_t *)(block + sizeof(RTCPHeader_t));
    ssrcHeader -> ssrc = htonl (sender -> ssrc);
    senderInfo = (SenderInfo_t *)(block + sizeof(RTCPHeader_t) + sizeof(SSRCHeader_t));
    gettimeofday (&curtv, NULL);
    senderInfo -> NTPmsw = htonl (curtv.tv_sec + RTP_NTP_OFFSET);
    double x = (double) curtv.tv_usec;
    x /= 1000000.0;
    x *= (65536.0*65536.0);
    u32 ntplsw = (u32)x;
    senderInfo -> NTPlsw = htonl (ntplsw);
    senderInfo -> rtptimestamp =
        getRtpTimestamp (&curtv,
                         &initialtv,
                         sender -> timestampOffset,
                         sender -> timestampUnit
                        );

    senderInfo -> senderoctetcount = htonl (sender -> octetCount);
    senderInfo -> senderpacketcount = htonl (sender -> packetCount);

    debugMsg(dbg_App_Verbose,
             "getSRBlock",
             "Packet type: %d",
             rtcpHeader -> packettype
            );

    debugMsg(dbg_App_Verbose,
             "getSRBlock",
             "SSRC: %u",
             ssrcHeader -> ssrc
            );

    debugMsg(dbg_App_Verbose,
             "getSRBlock",
             "NTP msw: %u, lsw: %u, RTP timestamp: %u,"
             "Packet count: %u, Octet count: %u\n",
             senderInfo -> NTPmsw,
             senderInfo -> NTPlsw,
             senderInfo -> rtptimestamp,
             senderInfo -> senderpacketcount,
             senderInfo -> senderoctetcount
            );

    return block;
}


u8 *
RTPStatisticsManager_t::getRRBlock (int len,
                                    RTPSource_t *source,
                                    int &blockLen,
                                    u32 ssrc
                                   )
{
    RTCPHeader_t *rtcpHeader;
    SSRCHeader_t *ssrcHeader;
    ReportBlock_t *reportBlock;
    RTPSourceStatistics_t *stats;
    u32 extendedsnr, expected, lost, expectedInterval,
        lostInterval, receivedInterval;
    double fraction;
    struct timeval localtv;
    long diffsec, diffusec;
    double diff;
    unsigned long lsr;

    blockLen=
        (len == maxPacketSize)
        ? sizeof(RTCPHeader_t) + sizeof(SSRCHeader_t) + sizeof(ReportBlock_t)
        : sizeof (ReportBlock_t);

    if (len < blockLen)
    {
        blockLen = 0;
        return NULL;
    }

    u8 *block = new u8[blockLen];

    if (len == maxPacketSize)
    {
        rtcpHeader = (RTCPHeader_t *)block;
        rtcpHeader -> version = RTP_VERSION;
        rtcpHeader -> padding = 0;
        rtcpHeader -> packettype = TYPE_RTCP_RR;
        ssrcHeader = (SSRCHeader_t *)(block + sizeof(RTCPHeader_t));
        ssrcHeader -> ssrc = htonl (ssrc);
    }

    reportBlock=
        (len == maxPacketSize)
        ? (ReportBlock_t*)(block + sizeof(RTCPHeader_t) + sizeof(SSRCHeader_t))
        : (ReportBlock_t *)block;

    reportBlock -> ssrc = htonl (source -> getSSRC());
    stats = source -> statistics;
    extendedsnr = stats -> cycles + stats -> maxSeq;
    reportBlock -> exthsnr = htonl (extendedsnr);
    expected = extendedsnr - stats -> baseSeq;
    lost = expected - stats -> packetsReceived;
    reportBlock -> packetslost[2] = lost&255;
    reportBlock -> packetslost[1] = (lost>>8)&255;
    reportBlock -> packetslost[0] = (lost>>16)&255;

    expectedInterval = expected - stats -> expectedPrior;
    stats -> expectedPrior = expected;
    receivedInterval = stats -> packetsReceived - stats -> receivedPrior;
    stats -> receivedPrior = stats -> packetsReceived;
    lostInterval = expectedInterval - receivedInterval;

    if (expectedInterval == 0 || lostInterval <= 0)
        fraction = 0;
    else
        fraction = (lostInterval << 8) / expectedInterval;

    reportBlock -> fractionlost = (u8)fraction;
    reportBlock -> jitter = htonl (stats -> jitter);

    if (stats -> srreceived)
    {
        // dlsr
        gettimeofday(&localtv,NULL);
        diffsec = localtv.tv_sec - stats -> srtime.tv_sec;
        diffusec = localtv.tv_usec - stats -> srtime.tv_usec;
        while (diffusec < 0)
        {
            diffsec--;
            diffusec += 1000000;
        }

        diff = (double)diffsec+(((double)diffusec)/1000000.0);
        diff *= 65536;
        reportBlock -> dlsr = htonl ((u32)diff);
        // lsr
        lsr = ((stats -> senderInfo -> NTPmsw&65535)<<16)|
              ((stats -> senderInfo -> NTPlsw>>16)&65535);

        reportBlock -> lsr = htonl (lsr);
    }
    else
    {
        reportBlock -> dlsr = 0;
        reportBlock -> lsr = 0;
    }

    return block;
}


u8 *
RTPStatisticsManager_t::getSDESBlock (int len,
                                      const char *item,
                                      int itemIdentifier,
                                      int &blockLen,
                                      bool newPacket,
                                      bool SDESInPacket,
                                      bool needSSRC,
                                      u32 ssrc
                                     )
{
    RTCPHeader_t *rtcpHeader;
    SSRCHeader_t *ssrcHeader;
    u8 *sdesItem;
    int itemLength = strlen (item);
    int lenaux = (2 * sizeof (u8)) + itemLength;

    if (len == maxPacketSize)
    {
        blockLen = (2 * sizeof (RTCPHeader_t)) + sizeof (SSRCHeader_t) + lenaux;
    }
    else
    {
        if (newPacket)
        {
            blockLen= needSSRC ? sizeof(SSRCHeader_t) + lenaux : lenaux;
        }
        else
        {
            if (SDESInPacket)
            {
                blockLen= needSSRC ? sizeof(SSRCHeader_t) + lenaux : lenaux;
            }
            else
            {
                blockLen = sizeof(RTCPHeader_t) + sizeof(SSRCHeader_t) + lenaux;
            }
        }
    }

    if (len < blockLen)
    {
        blockLen = 0;
        return NULL;
    }

    u8 *block = new u8[blockLen];

    if (len == maxPacketSize)
    {
        rtcpHeader = (RTCPHeader_t *)block;
        rtcpHeader -> version = RTP_VERSION;
        rtcpHeader -> padding = 0;
        rtcpHeader -> blockcount = 0;
        rtcpHeader -> packettype = TYPE_RTCP_RR;
        rtcpHeader -> length = 0;
        rtcpHeader = (RTCPHeader_t *)(block + sizeof(RTCPHeader_t));
        rtcpHeader -> version = RTP_VERSION;
        rtcpHeader -> padding = 0;
        rtcpHeader -> packettype = TYPE_RTCP_SDES;

        ssrcHeader = (SSRCHeader_t *)(block + 2 * sizeof (RTCPHeader_t));
        ssrcHeader -> ssrc = htonl (ssrc);

        sdesItem = (block + 2 * sizeof(RTCPHeader_t) + sizeof(SSRCHeader_t));
        *sdesItem = itemIdentifier;
        sdesItem++;
        *sdesItem = itemLength;
        sdesItem++;

        memcpy (sdesItem, item, itemLength);
        debugMsg(dbg_App_Verbose,
                 "getSDESBlock",
                 "RTCP_RR,RTCP_SDES,SSRC,SDESItem"
                );
    }
    else
    {
        if (newPacket)
        {
            if (needSSRC)
            {
                ssrcHeader = (SSRCHeader_t *)block;
                ssrcHeader -> ssrc = htonl (ssrc);
                sdesItem = block + sizeof(SSRCHeader_t);
                *sdesItem = itemIdentifier;
                sdesItem++;
                *sdesItem = itemLength;
                sdesItem++;
                memcpy (sdesItem, item, itemLength);
                debugMsg(dbg_App_Verbose, "getSDESBlock", "SSRC, SDESItem");
            }
            else
            {
                sdesItem = block;
                *sdesItem = itemIdentifier;
                sdesItem++;
                *sdesItem = itemLength;
                sdesItem++;
                memcpy (sdesItem, item, itemLength);
                debugMsg(dbg_App_Verbose, "getSDESBlock", "SDESItem");
            }
        }
        else
        {
            if (SDESInPacket)
            {
                if (needSSRC)
                {
                    ssrcHeader = (SSRCHeader_t *)block;
                    ssrcHeader -> ssrc = htonl (ssrc);
                    sdesItem = block + sizeof(SSRCHeader_t);
                    *sdesItem = itemIdentifier;
                    sdesItem++;
                    *sdesItem = itemLength;
                    sdesItem++;
                    memcpy (sdesItem, item, itemLength);
                    debugMsg(dbg_App_Verbose, "getSDESBlock", "SSRC, SDESItem");
                }
                else
                {
                    sdesItem = block;
                    *sdesItem = itemIdentifier;
                    sdesItem++;
                    *sdesItem = itemLength;
                    sdesItem++;
                    memcpy (sdesItem, item, itemLength);
                    debugMsg(dbg_App_Verbose, "getSDESBlock", "SDESItem");
                }
            }
            else
            {
                rtcpHeader = (RTCPHeader_t *)block;
                rtcpHeader -> version = RTP_VERSION;
                rtcpHeader -> padding = 0;
                rtcpHeader -> packettype = TYPE_RTCP_SDES;
                ssrcHeader = (SSRCHeader_t *)(block +  sizeof (RTCPHeader_t));
                ssrcHeader -> ssrc = htonl (ssrc);
                sdesItem= (block + sizeof(RTCPHeader_t) + sizeof(SSRCHeader_t));
                *sdesItem = itemIdentifier;
                sdesItem++;
                *sdesItem = itemLength;
                sdesItem++;
                memcpy (sdesItem, item, itemLength);
                debugMsg(dbg_App_Verbose,
                         "getSDESBlock",
                         "RTCP_SDES, SSRC, SDESItem"
                        );
            }
        }
    }

    return block;
}


u8 *
RTPStatisticsManager_t::getBYEBlock (int len,
                                     ql_t<u32> *ssrcList,
                                     int &blockLen,
                                     u8 byeLength,
                                     const char *byeReason
                                    )
{
    RTCPHeader_t *rtcpHeader;
    u8 *aux;
    //char *aux2;
    int n = ssrcList -> len();
    int lenaux = sizeof (unsigned char) + byeLength;

    blockLen= 
        (len == maxPacketSize)
            ? (2 * sizeof(RTCPHeader_t)) + n *sizeof(SSRCHeader_t) + lenaux
            : sizeof (RTCPHeader_t) + n * sizeof (SSRCHeader_t) + lenaux;

    if (len < blockLen)
    {
        blockLen = 0;
        return NULL;
    }

    u8 *block = new u8[blockLen];

    if (len == maxPacketSize)
    {
        rtcpHeader = (RTCPHeader_t *)block;
        rtcpHeader -> version = RTP_VERSION;
        rtcpHeader -> padding = 0;
        rtcpHeader -> blockcount = 0;
        rtcpHeader -> packettype = TYPE_RTCP_RR;
        rtcpHeader -> length = htonl ((sizeof(u32)/4) - 1);
        rtcpHeader = (RTCPHeader_t *)(block + sizeof (RTCPHeader_t));
        rtcpHeader -> version = RTP_VERSION;
        rtcpHeader -> padding = 0;
        rtcpHeader -> packettype = TYPE_RTCP_BYE;

        u32 *auxlong = (u32 *)(block + 2 * sizeof (RTCPHeader_t));

        for (ql_t<u32>::iterator_t i = ssrcList -> begin();
             i != ssrcList -> end();
             i++)
             {
                 u32 ssrc = static_cast<u32> (i);
                 *auxlong = htonl (ssrc);
                 auxlong++;
             }

        aux = block + 2 * sizeof(RTCPHeader_t) + n * sizeof(SSRCHeader_t);
        *aux = byeLength; aux++;
        memcpy (aux, byeReason, byeLength);
        debugMsg(dbg_App_Verbose, "getBYEBlock", "RTCP_BYE, SSRC, BYE Reason");
    }
    else
    {
        rtcpHeader = (RTCPHeader_t*)block;
        rtcpHeader -> version = RTP_VERSION;
        rtcpHeader -> padding = 0;
        rtcpHeader -> packettype = TYPE_RTCP_BYE;
        u32 *auxlong = (u32 *)(block + sizeof (RTCPHeader_t));

        for (ql_t<u32>::iterator_t i = ssrcList -> begin();
             i != ssrcList -> end();
             i++
            )
        {
            u32 ssrc = static_cast<u32> (i);
            *auxlong = htonl (ssrc);
            auxlong++;
        }

        aux = block + sizeof(RTCPHeader_t) + n * sizeof(SSRCHeader_t);
        *aux = byeLength;
        aux++;
        memcpy (aux, byeReason, byeLength);
        debugMsg(dbg_App_Verbose, "getBYEBlock", "RTCP_BYE, SSRC, BYE Reason");
    }

    return block;
}


u32
RTPStatisticsManager_t::getRtpTimestamp (struct timeval *curtv,
                                         struct timeval *initialtv,
                                         u32 timestampOffset,
                                         double tsUnit
                                        )
{
    long diffsec, diffusec;
    double x, diff;

    diffsec = curtv -> tv_sec - initialtv -> tv_sec;
    diffusec = curtv -> tv_usec - initialtv -> tv_usec;

    while (diffusec < 0)
    {
        diffsec--;
        diffusec += 1000000;
    }

    diff = (double)diffsec + (((double)diffusec)/1000000.0);
    x = diff/tsUnit;
    debugMsg(dbg_App_Verbose,
             "getRtpTimestamp",
             "difference in timestamp units: %u",
             (u32)x
            );

    return htonl ( (u32)x + timestampOffset );

}

void
RTPStatisticsManager_t::sendRtcpCompoundPacket (u8 *buffer, int length)
{
    const vector_t<bindDescriptor_t*> *dest = rtcpHolder->getBindings();

    for (int i= 0; i < dest->size(); i++)
    {
        bindDescriptor_t *bd = dest->elementAt(i);

        rtcpSocket->writeTo (*(bd->getInetAddr()), buffer, length);
    }
}

