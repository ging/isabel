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
// $Id: RTPPacketProcessor.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <rtp/RTPPacketProcessor.hh> 

RTPPacketProcessor_t::RTPPacketProcessor_t (void)
{
    debugMsg(dbg_App_Verbose,
             "RTPPacketProcessor_t",
             "Creating RTPPacketProcessor_t"
            );
}

RTPPacketProcessor_t::~RTPPacketProcessor_t (void)
{
    debugMsg(dbg_App_Verbose,
             "~RTPPacketProcessor_t",
             "Deleting RTPPacketProcessor_t"
            );
}

bool 
RTPPacketProcessor_t::checkRTP (u8 *packet, int length, int &realLength)
{
    RTPHeader_t *rtpHeader;
    if ( length < sizeof (RTPHeader_t) )
    {
        debugMsg(dbg_App_Verbose,
                 "checkRTP",
                 "Packet's length < RTP Header's length"
                );
        return false;
    }
    rtpHeader = (RTPHeader_t *)packet;
    if ( (rtpHeader -> version) != RTP_VERSION )
    {
        debugMsg(dbg_App_Verbose, "checkRTP", "RTP Version != %d", RTP_VERSION); 
        return false;
    }
    int rtpHeaderLength;
    int rtpFixedHeaderLength = sizeof(RTPHeader_t) + ( (rtpHeader -> cc) * sizeof(u32) );
    if ( rtpHeader -> extension )
    {
        ExtensionHeader_t *extHeader = (ExtensionHeader_t *)(packet + rtpFixedHeaderLength);
        int extLength = 4 + ntohs (extHeader -> length);
        rtpHeaderLength = rtpFixedHeaderLength + extLength; 
    }else{
        rtpHeaderLength = rtpFixedHeaderLength;
    }
    realLength = length;
    if (rtpHeader -> padding)
    {
        u16 *aux = (u16*)(packet + length - 2);
        realLength = (length - ((u16)(*aux)));
        if ( realLength > length )
        {
            debugMsg(dbg_App_Verbose, "checkRTP", "RTP Padding incorrect"); 
            return false;
        }
        if ( realLength < 0 )
        {
            debugMsg(dbg_App_Verbose, "checkRTP", "RTP Padding incorrect");
            return false;
        }
    }
    debugMsg(dbg_App_Verbose, "checkRTP", "Packet checked correctly"); 
    return true;
}

bool 
RTPPacketProcessor_t::checkRTCP (u8 *packet, int length)
{
    RTCPHeader_t *rtcpHeader = (RTCPHeader_t *)packet;
    if ( rtcpHeader -> version != RTP_VERSION )
    {
        debugMsg(dbg_App_Normal, "checkRTCP", "RTP Version != %d", RTP_VERSION); 
        return false;
    }
    if  (( rtcpHeader -> packettype != TYPE_RTCP_SR ) && 
         ( rtcpHeader -> packettype != TYPE_RTCP_RR ))
    {
        debugMsg(dbg_App_Normal, "checkRTCP", "Packet doesn't start with SR or RR");
        return false;
    }
    if ( rtcpHeader -> padding != 0 )
    {
        debugMsg(dbg_App_Normal, "checkRTCP", "Padding shouldn't exist");
        return false;
    }
    int length2 = 0;
    RTCPHeader_t *rtcpHeader2 = NULL;
    u8 *aux = packet; 
    while (length2 < length)
    {
        rtcpHeader2 = (RTCPHeader_t *)aux;
        length2 +=  4 * ( ntohs(rtcpHeader2 -> length) + 1); 
        aux +=  4 * ( ntohs(rtcpHeader2 -> length) + 1 );
    }
    if (length2 != length )
    {
        debugMsg(dbg_App_Normal, "checkRTCP", "Packet length is not correct");
        return false;
    }
    return true;
}

RTCPReport_t *
RTPPacketProcessor_t::getReport (u8 *packet, int &length)
{
    u8 *aux = packet;
    RTCPHeader_t *headeraux = (RTCPHeader_t *)aux;
    if (headeraux -> length == 0)
    {
        length = 4 * (headeraux -> length + 1);
        return NULL;
    }
    RTCPHeader_t *rtcpHeader = new RTCPHeader_t; 
    rtcpHeader -> version = headeraux -> version;
    rtcpHeader -> padding = headeraux -> padding;
    rtcpHeader -> blockcount = headeraux -> blockcount;
    rtcpHeader -> packettype = headeraux -> packettype;
    rtcpHeader -> length = ntohs (headeraux -> length);
    aux += sizeof (RTCPHeader_t);
    SSRCHeader_t *ssrcaux = (SSRCHeader_t *)aux;
    SSRCHeader_t *ssrcHeader = new SSRCHeader_t;
    ssrcHeader -> ssrc = ntohl (ssrcaux -> ssrc);
    aux += sizeof (SSRCHeader_t);
    SenderInfo_t *senderInfo = NULL;
    if (rtcpHeader -> packettype == TYPE_RTCP_SR)
    {
        SenderInfo_t *senderaux = (SenderInfo_t *)aux;
        senderInfo = new SenderInfo_t;
        senderInfo -> NTPmsw = ntohl (senderaux -> NTPmsw);
        senderInfo -> NTPlsw = ntohl (senderaux -> NTPlsw);
        senderInfo -> rtptimestamp = ntohl (senderaux -> rtptimestamp);
        senderInfo -> senderpacketcount = ntohl (senderaux -> senderpacketcount);
        senderInfo -> senderoctetcount = ntohl (senderaux -> senderoctetcount); 
        aux += sizeof (SenderInfo_t);
    }
    ql_t<ReportBlock_t *> *rrList = new ql_t<ReportBlock_t *>;
    if (rtcpHeader -> blockcount > 0)
    {
        int blockcount = 0;
        while (blockcount < rtcpHeader -> blockcount)
        {
            ReportBlock_t *rraux = (ReportBlock_t *)aux;
            ReportBlock_t *rr = new ReportBlock_t;
            rr -> ssrc = ntohl (rraux -> ssrc);
            rr -> fractionlost = rraux -> fractionlost; 
            rr -> packetslost[2] = rraux -> packetslost[2];
            rr -> packetslost[1] = rraux -> packetslost[1];
            rr -> packetslost[0] = rraux -> packetslost[0];
            rr -> exthsnr = ntohl (rraux -> exthsnr); 
            rr -> jitter = ntohl (rraux -> jitter);
            rr -> lsr = ntohl (rraux -> lsr);
            rr -> dlsr = ntohl (rraux -> dlsr);
            rrList -> insert (rr);
            blockcount++;
            aux += sizeof (ReportBlock_t);
        }
    }
    length += 4 * (rtcpHeader -> length + 1);
    RTCPReport_t *message = new RTCPReport_t;  
    message->rtcpHeader = rtcpHeader;
    message->ssrcHeader = ssrcHeader;
    message->senderInfo = senderInfo;
    message->rrList     = rrList;
    return message;
}


ql_t<RTCPSDES_t *> *
RTPPacketProcessor_t::getSDES (u8 *packet, int &length)
{
    u8 *aux = packet + length;
    RTCPHeader_t *headeraux = (RTCPHeader_t *)aux;
    if (headeraux -> packettype != TYPE_RTCP_SDES)
    {
        return NULL;
    }
    RTCPHeader_t *rtcpHeader = new RTCPHeader_t (*headeraux);
    aux += sizeof (RTCPHeader_t);
    ql_t<RTCPSDES_t *> *list = new ql_t<RTCPSDES_t *>;
    int lengthtotal = 4 * (ntohs (rtcpHeader -> length) + 1);
    int lengthaux = sizeof (RTCPHeader_t);
    int sourcecount = rtcpHeader -> blockcount;
    for (int i = 0; i < sourcecount; i++)
    {
        SSRCHeader_t *ssrcaux = (SSRCHeader_t *)aux;
        SSRCHeader_t *ssrcHeader = new SSRCHeader_t;
        ssrcHeader -> ssrc = ntohl (ssrcaux -> ssrc);
        aux += sizeof (SSRCHeader_t);
        ql_t<SDESItem_t *> *sdesList = new ql_t<SDESItem_t *>;
        lengthaux += sizeof (SSRCHeader_t);
        bool end = false;
        while (!end)
        {
            SDESItem_t *item = (SDESItem_t*)malloc (sizeof(SDESItem_t));
            item -> identifier = *aux; aux++; lengthaux++;
            item -> length = *aux; aux++; lengthaux++;
            item -> info = (char*)malloc (sizeof(char) * (item -> length + 1));
            char *aux2 = item -> info;
            for (int i = 0; i < item -> length; i++)
            {
                *aux2 = *aux;
                aux2++; aux++;
            }
            *aux2 = '\0';
            lengthaux += item -> length;
            sdesList -> insert (item);
            if ((*aux) == 0)
            {
                end = true;
                do{
                    aux++;
                    lengthaux++; 
                }while (((*aux) == 0) && (lengthaux < lengthtotal));
            }
        }
        RTCPSDES_t *message = new RTCPSDES_t;
        message->ssrcHeader = ssrcHeader;
        message->sdesList   = sdesList;
        list -> insert (message);
    } 
    length += lengthaux; 
    return list;
}


RTCPBYE_t *
RTPPacketProcessor_t::getBYE (u8 *packet, int &length)
{
    u8 *aux = packet + length;
    RTCPHeader_t *headeraux = (RTCPHeader_t *)aux;
    if (headeraux -> packettype != TYPE_RTCP_BYE)
    {
        return NULL;
    }
    int lengthaux = 0;
    RTCPHeader_t *rtcpHeader = new RTCPHeader_t (*headeraux);
    aux += sizeof (RTCPHeader_t);
    ql_t<u32> *list = new ql_t<u32>;
    int sourcecount = rtcpHeader -> blockcount;
    u32 ssrc;
    int i = 0;
    for (i = 0; i < sourcecount; i++)
    {
        SSRCHeader_t *ssrcaux = (SSRCHeader_t *)aux;
        ssrc = ntohl (ssrcaux -> ssrc);
        list -> insert (ssrc);
        aux += sizeof (SSRCHeader_t);
    }
    char *byeReason = new char [256];
    char *aux2 = byeReason;
    u8 lengthaux2 = *aux;
    aux++;
    lengthaux = sizeof(RTCPHeader_t) + sourcecount * sizeof(SSRCHeader_t) + sizeof(u8);
    for (i = 0; i < lengthaux2; i++)
    {
        *aux2 = *aux;
        aux2++; aux++;
    }
    *aux2 = '\0';
    lengthaux += (int)lengthaux2;
    int lengthtotal = 4 * (ntohs (rtcpHeader -> length) + 1);
    if ( (*aux) == 0 )
    {
        while (lengthaux < lengthtotal)
        {
            aux++;
            lengthaux++;
        }
    } 
    length += lengthaux;
    RTCPBYE_t *message = new RTCPBYE_t;
    message->rtcpHeader = rtcpHeader;
    message->csrcList   = list;
    message->byeReason  = byeReason;
    return message;
}
