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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/general.h>
#include <icf2/icfTime.hh>
#include <icf2/notify.hh>

#ifdef __BUILD_FOR_LINUX
#include <arpa/inet.h>
#endif

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>
#include <rtp/RTPFec.hh>

// RTP Buffer Defaults
#define DEFAULT_FRAME_SIZE 200

const int MAX_PKT_LEN= 4096;

RTPContainer_t::RTPContainer_t(void)
{
    nData = 0;
    for (int j= 0; j < MAX_BUFF; j++)
    {
        frame[j] = NULL;
    }
}

RTPContainer_t::~RTPContainer_t(void)
{
    free();
}

void
RTPContainer_t::free(void)
{
    for (int j= 0; j < MAX_BUFF; j++)
    {
        if (frame[j] != NULL)
        {
            delete frame[j];
            frame[j] = NULL;
        }
    }
    nData = 0;
}

void
RTPContainer_t::free(int SQ)
{
    for (int j= 0; j < MAX_BUFF; j++)
    {
        if (frame[j] != NULL)
        {
            if (frame[j]->getSequenceNumber() == SQ)
            {
                delete frame[j];
                frame[j] = NULL;
            }
        }
    }
    nData = 0;
}

bool
RTPContainer_t::insert(RTPPacket_t *pkt)
{
    int diff = 0;

    if (frame[0] != NULL)
    {
        diff = pkt->getSequenceNumber()-frame[0]->getSequenceNumber();
    }

    // sequence cicle test
    if (abs(diff) > RTP_MAX_SEQ_DIFF)
    {
        if (diff > 0)
            diff-= RTP_MAX_SEQ_NUM;
        else
            diff+= RTP_MAX_SEQ_NUM;
    }

    // duplication test
    for (int j = 0; j < MAX_BUFF; j++)
    {
        if (frame[j] != NULL)
        {
            if (frame[j]->getSequenceNumber() == pkt->getSequenceNumber())
            {
                return true;
            }
        }
    }

    // if we receive a delayed packet

    if (diff < 0)
    {
        for (int j= MAX_BUFF+diff-1; j >= 0; j--)
        {
            frame[j-diff] = frame[j];
            frame[j] = NULL;
        }
        frame[0] = new RTPPacket_t(*pkt);
        return true;
    }

    // if we receive a big diff
    // reset buffer
    if (diff >= MAX_BUFF)
    {
        free();
        diff = 0;
        NOTIFY("RTPFecReceiver_t (%d):: active = false\n", pkt->getPayloadType());
        return false;
    }

    // Save the frame
    frame[diff] = new RTPPacket_t(*pkt);

    return true;
}

RTPPacket_t*
RTPContainer_t::getData(void)
{
    RTPPacket_t *pkt = frame[nData];
    nData++;
    if (nData > MAX_BUFF)
    {
        free();
    }
    return pkt;
}

int
RTPContainer_t::checkLost(vector_t<int> *SQArray, int &SQ)
{
    int nLost = 0;
    for (int j = 0; j < SQArray->size(); j++)
    {
        int Seq = SQArray->elementAt(j);

        bool found = false;
        for (int i = 0; i < MAX_BUFF; i++)
        {
            if (frame[i])
            {
                if (frame[i]->getSequenceNumber() == Seq)
                {
                    found = true;
                    break;
                }
            }
        }
        if ( ! found)
        {
            nLost++;
            SQ = Seq;
        }

        if (nLost == 2)
            return -1; // cannot recover
    }

    if (nLost)
        return nLost; // can recover
    else
        return 0; // no losses
}

u32
RTPContainer_t::getTimestamp(void)
{
    for (int pos = 0; pos < MAX_BUFF; pos++)
    {
       if (frame[pos])
       {
           return frame[pos]->getTimestamp();
       }
    }
    return 0;
}

//
// FEC receiver
//
RTPFecReceiver_t::RTPFecReceiver_t(u8 nfecPT)
: fecPT(nfecPT),
  active(false)
{
}

RTPFecReceiver_t::~RTPFecReceiver_t(void)
{
    resetRX();
}

void
RTPFecReceiver_t::resetRX(void)
{
    __buffer.free();
}

RTPPacket_t*
RTPFecReceiver_t::fecPktToRecv(RTPPacket_t *pkt)
{
    RTPFecHeader_t *rtpFecHeader= (RTPFecHeader_t*)(pkt->getPacket()+12);
    u16 SNBase= ntohs(rtpFecHeader->SNBase);
    u32 mask = ntohl(rtpFecHeader->secondWord);
    u16 len = 0;
    mask = ((mask<<8)>>8);

    vector_t<int> SQArray;

    // SQ is between [SNBase..SNBase + 24]
    for (int i= 0; i < 24; i++)
    {
        u32 bitMask = mask & (0x0001<<i);
        if (bitMask != 0)
        {
            SQArray.add(SNBase + i);
            len++;
        }
    }

    //NOTIFY("Busco Base = %d,FEC SQ = %d, len = %d\n",SNBase,pkt->getSequenceNumber(),len);

    // check if we are out of range
    if (len <= 0 || len > 24)
    {
        NOTIFY("RTPFecReceiver_t::fecPktToRecv: "
               "protection SQ = %d, first protected packet SQ = %d\n",
               pkt->getSequenceNumber(),
               SNBase
              );

        // delete pkts not used any more
        for (int k = 0; k < SQArray.size(); k++)
        {
            int SQ = SQArray.elementAt(k);
            __buffer.free(SQ);
        }
        return NULL;
    }

    int lostSQ = 0;

    // if more than one pkt lost, there is nothing to do
    int ret= 0;
    if ((ret = __buffer.checkLost(&SQArray, lostSQ)) <= 0)
    {
        if (ret == -1)
        {
            NOTIFY("RTPFec :: >1 pkts lost , can't recover!\n");
        }

        // delete pkts not used any more
        for (int k = 0; k < SQArray.size(); k++)
        {
            int SQ = SQArray.elementAt(k);
            __buffer.free(SQ);
        }
        return NULL;
    }

    // delete from list lost SQ
    for (int nn= 0; nn < SQArray.size(); nn++)
    {
        if (SQArray.elementAt(nn) == lostSQ)
        {
            SQArray.remove(nn);
            break;
        }
    }

    // if a packet was lost
    NOTIFY("Pkt Lost detected, ssrc = %d, k=%d\n", pkt->getSSRC(), len);

    int maxLength= pkt->getTotalLength()-12;
    u8 *buffer = new u8[maxLength+12];
    u8 *protection;
    protection = buffer + sizeof(RTPHeader_t);
    protectionBlock_t protectionBlock;
    memset(buffer, 0, maxLength);

    // Block protection creation
    protectionBlock.version     = 0;
    protectionBlock.padding     = pkt->getPadding();
    protectionBlock.extension   = pkt->getExt();
    protectionBlock.cc          = pkt->getCC();
    protectionBlock.marker      = pkt->getMark();
    u32 secondWord              = ntohl(rtpFecHeader->secondWord);
    protectionBlock.payloadtype = secondWord >> 24;
    protectionBlock.timestamp   = rtpFecHeader->TSRecovery;
    protectionBlock.lengthsSum  = htons(rtpFecHeader->lengthRecv);

    // copy data
    u8 *data = pkt->getData();
    memcpy(protection, data+12, maxLength);

    // Block protection for the rest of packets
    int n = 0;
    for (RTPPacket_t *auxpkt = __buffer.getData();
         SQArray.size() != n;
         auxpkt = __buffer.getData()
        )
    {
        if ( ! auxpkt)
            continue; // this is the one lost

        bool found = false;

        for (int nn = 0; nn < SQArray.size(); nn++)
        {
            if (SQArray.elementAt(nn) == auxpkt->getSequenceNumber())
            {
                found = true;
                n++;
            }
        }

        if ( ! found)
            continue;

        protectionBlock.version     ^= 0;
        protectionBlock.padding     ^= auxpkt->getPadding();
        protectionBlock.extension   ^= auxpkt->getExt();
        protectionBlock.cc          ^= auxpkt->getCC();
        protectionBlock.marker      ^= auxpkt->getMark();
        protectionBlock.payloadtype ^= auxpkt->getPayloadType();
        protectionBlock.timestamp   ^= htonl(auxpkt->getTimestamp());

        u16 lengthSum= (auxpkt->getCC() * sizeof(u32) +
                        auxpkt->getTotalExtensionLength() +
                        auxpkt->getDataLength()
                       );

        protectionBlock.lengthsSum ^= lengthSum;

        data = auxpkt->getData();
        for (int length = 0; length < auxpkt->getDataLength(); length++)
        {
            protection[length] ^= data[length];
        }
    }

    // Recovered PKT creation

    RTPHeader_t *rtpHeader = (RTPHeader_t*)buffer;
    rtpHeader->version     = RTP_VERSION;
    rtpHeader->padding     = protectionBlock.padding;
    rtpHeader->extension   = protectionBlock.extension;
    rtpHeader->cc          = protectionBlock.cc;
    rtpHeader->marker      = protectionBlock.marker;
    rtpHeader->payloadtype = protectionBlock.payloadtype;
    rtpHeader->seqnum      = htons(lostSQ);
    rtpHeader->timestamp   = protectionBlock.timestamp;
    rtpHeader->ssrc        = htonl(pkt->getSSRC());

    if ( ! rtpHeader->timestamp)
    {
        rtpHeader->timestamp = ntohl(__buffer.getTimestamp());
    }

    u16 bufferLen = protectionBlock.lengthsSum + sizeof(RTPHeader_t);

    if (bufferLen<=sizeof(RTPHeader_t) || bufferLen>(maxLength+12))
    {
        bufferLen = maxLength;
    }

    RTPPacket_t *newPkt= new RTPPacket_t(buffer, bufferLen);

    delete []buffer;

    // delete pkts not used any more
    for (int k = 0; k < SQArray.size(); k++)
    {
        int SQ = SQArray.elementAt(k);
        __buffer.free(SQ);
    }

    return newPkt;
}


RTPPacket_t*
RTPFecReceiver_t::recvPacket(RTPPacket_t *pkt)
{
    RTPPacket_t *fecPkt= NULL;

    if (pkt->getPayloadType() != fecPT)
    {
        //NOTIFY("Insert RX SSRC = %d, SQ = %d, TS = %d , M = %d\n",
        //       pkt->getSSRC(),
        //       pkt->getSequenceNumber(),
        //       pkt->getTimestamp(),
        //       pkt->getMark()
        //      );

        if (active)
        {
            if ( ! __buffer.insert(pkt))
            {
                active = false;
            }
        }
        return NULL;
    }
    else
    {
          if ( ! active)
          {
              active = true;
              return NULL;
          }

          // procesamos el paquete
          fecPkt= fecPktToRecv(pkt);

          if (fecPkt)
          {
              NOTIFY("PKT RECOVERED!!\n"
                     "----------------------\n"
                     "SQ      = %d\n"
                     "TS      = %d\n"
                     "SSRC    = %d\n"
                     "DataLen = %d\n"
                     "M       = %d\n",
                     fecPkt->getSequenceNumber(),
                     fecPkt->getTimestamp(),
                     fecPkt->getSSRC(),
                     fecPkt->getDataLength(),
                     fecPkt->getMark()
                    );
          }
    }
    return fecPkt;
}

//
// FEC Sender
//
RTPFecSender_t::RTPFecSender_t(u8 nfecPT)
: fecPT(nfecPT)
{
    u8 pkt[MAX_PKT_LEN];    
    k = 0;
    fecPkt = new RTPPacket_t(pkt, MAX_PKT_LEN);
    resetTX();
}

RTPFecSender_t::~RTPFecSender_t(void)
{
    delete fecPkt;
}

void
RTPFecSender_t::resetTX(void)
{
    maxLength = 0;
    mask= 0;
    memset(fecPkt->getPacket(), 0, MAX_PKT_LEN);

    while (SQArray.size())
    {
        SQArray.remove(0);
    }
}

RTPPacket_t *
RTPFecSender_t::fecPktToSend(RTPPacket_t *pkt)
{
    if ( ! k )
    {
        NOTIFY("RTPFecSender_t::fecPktToSend: sender with k = 0!\n");
        return NULL;
    }

    if (SQArray.size() >= k)
    {
        // if k packets were processed, a FEC pkt was sent,
        // so I need to be reseted to start a new data processing

        resetTX();
    }

    if (pkt->getTotalLength() > maxLength)
    {
        maxLength = pkt->getTotalLength();
    }

    // mask where tells SQ protected by FEC
    int diffSQ = 0;
    if (SQArray.size())
    {
        u16 SNBase = SQArray.elementAt(0);
        diffSQ = pkt->getSequenceNumber()-SNBase;
    }

    if (diffSQ < 24 && diffSQ >= 0)
    {
        mask = (mask|(0x0001<<diffSQ));
    }
    else
    {
        return NULL;
    }

    // valid pkt, work with it
    SQArray.add(pkt->getSequenceNumber());

    // Create FEC pkt (RFC 2733)
    u8 *protection = (u8*)(fecPkt->getPacket()+12+12);

    protectionBlock_t *protectionBlock =
        (protectionBlock_t *)(fecPkt->getPacket()+12);

    protectionBlock->version= 0;
    protectionBlock->padding^= pkt->getPadding();
    protectionBlock->extension^= pkt->getExt();
    protectionBlock->cc^= pkt->getCC();
    protectionBlock->marker^= pkt->getMark();
    protectionBlock->payloadtype^= pkt->getPayloadType();
    protectionBlock->timestamp^= htonl(pkt->getTimestamp());

    u16 lengthSum= (pkt->getCC() * sizeof(u32) +
                    pkt->getTotalExtensionLength() +
                    pkt->getDataLength()
                   );

    protectionBlock->lengthsSum^= htons(lengthSum);

    u8 *data = pkt->getData();
    int length = 0;
    while (length < pkt->getDataLength())
    {
        protection[length]^= data[length];
        length++;
    }

    if (SQArray.size() < k)
    {
        return NULL;
    }

    // Create FEC PKT
    u16 SQ = 0;
    for (u16 i = 0; i < SQArray.size(); i++)
    {
        u16 auxSQ = SQArray.elementAt(i);
        if (SQ < auxSQ)
        {
            SQ = auxSQ;
        }
    }

    double __timestampUnit = getTimestampUnitByPT(pkt->getPayloadType());

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    double currtime = currentTime.tv_sec*1000000 + currentTime.tv_usec;
    u32 timestamp= (u32)(((currtime)/1000000.0)/__timestampUnit);

    RTPHeader_t *rtpHeader   = (RTPHeader_t*)fecPkt->getPacket();
    rtpHeader->version       = RTP_VERSION;
    rtpHeader->padding       = protectionBlock->padding;
    rtpHeader->extension     = protectionBlock->extension;
    rtpHeader->cc            = protectionBlock->cc;
    rtpHeader->marker        = protectionBlock->marker;
    rtpHeader->payloadtype   = fecPT;
    rtpHeader->seqnum        = htons(SQ++);
    rtpHeader->timestamp     = htonl(timestamp);
    rtpHeader->ssrc          = htonl(pkt->getSSRC());

    // RTP FEC creation: FEC Header
    RTPFecHeader_t *rtpFecHeader=(RTPFecHeader_t*)(fecPkt->getPacket()+12);
    rtpFecHeader->SNBase     = htons(SQArray.elementAt(0));
    rtpFecHeader->lengthRecv = protectionBlock->lengthsSum;
    rtpFecHeader->secondWord = 0; //Extension
    rtpFecHeader->secondWord = pkt->getPayloadType(); //PT
    rtpFecHeader->secondWord = (rtpFecHeader->secondWord<<24); //Move 24 bits
    rtpFecHeader->secondWord = (rtpFecHeader->secondWord|mask);//Insert mask
    rtpFecHeader->secondWord = htonl(rtpFecHeader->secondWord);
    rtpFecHeader->TSRecovery = protectionBlock->timestamp;

    //NOTIFY("Send FEC SQ = %d\n", fecPkt->getSequenceNumber());

    fecPkt->setDataLength(maxLength + 12); //dataLen + ProtectionBlockLen

    return fecPkt;
}

void
RTPFecSender_t::setK(u8 k)
{
    if (k > 24)
    {
        k= 24;
    }
    if (k < this->k)
    {
        resetTX();
    }
    this->k = k;
}

RTPPacket_t *
RTPFecSender_t::sendPacket(RTPPacket_t *pkt)
{
    //NOTIFY("Inserto TX SQ = %d, TS = %d , M = %d\n",
    //       pkt->getSequenceNumber(),
    //       pkt->getTimestamp(),
    //       pkt->getMark()
    //      );

    if (k > 0)
    {
        return fecPktToSend(pkt);
    }

    return NULL;
}


