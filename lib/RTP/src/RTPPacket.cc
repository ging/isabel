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
// $Id: RTPPacket.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <icf2/general.h>

#if defined(__BUILD_FOR_LINUX)
#include <arpa/inet.h>
#elif defined(WIN32)
// winsock2 includes ntohs, ntohl, etc. included in icf2/general.h
#else
#error "Please, include or implement ntohs, ntohl for your O.S."
#endif

#include <icf2/notify.hh>

#include <rtp/RTPPacket.hh>

RTPPacket_t::RTPPacket_t (u8 *pkt, int length)
{
    packet = new u8[length];
    assert(packet != NULL && "RTPPacket_t(pkt,length) run out of memory");

    memcpy(packet, pkt, length);
    rtpHeader = (RTPHeader_t*)packet;
    rtpFixedHeaderLength = sizeof(RTPHeader_t)+(sizeof(u32)*rtpHeader->cc);
    rtpHeaderLength = rtpFixedHeaderLength;
    if (rtpHeader->extension)
    {
        ExtensionHeader_t *extHeader =
            (ExtensionHeader_t*)(packet+rtpFixedHeaderLength);

        extensionId = ntohs (extHeader -> id);
        extLength = sizeof(ExtensionHeader_t)+ntohs(extHeader -> length);
        extensionData = packet+rtpHeaderLength+sizeof(ExtensionHeader_t);
        rtpHeaderLength += extLength;
    }
    else
    {
        extensionData = NULL;
        extLength     = 0;
    }
    totalLength = length;
    dataLength  = length-rtpHeaderLength;
    data = packet+rtpHeaderLength;
}

RTPPacket_t::RTPPacket_t (const RTPPacket_t &other)
{
    packet = new u8[other.totalLength];
    assert(packet != NULL && "RTPPacket_t(other) run out of memory");

    memcpy(packet, other.packet, other.totalLength);
    rtpHeader = (RTPHeader_t*)packet;
    rtpFixedHeaderLength = sizeof(RTPHeader_t)+(sizeof(u32)*rtpHeader->cc);
    rtpHeaderLength = rtpFixedHeaderLength;
    if (rtpHeader->extension)
    {
        ExtensionHeader_t *extHeader =
            (ExtensionHeader_t*)(packet+rtpFixedHeaderLength);

        extensionId = ntohs (extHeader -> id);
        extLength = sizeof(ExtensionHeader_t)+ntohs(extHeader -> length);
        extensionData = packet+rtpHeaderLength+sizeof(ExtensionHeader_t);
        rtpHeaderLength += extLength;
    }
    else
    {
        extensionData = NULL;
        extLength     = 0;
    }
    totalLength = other.totalLength;
    dataLength  = other.dataLength;
    data = packet+rtpHeaderLength;
}

RTPPacket_t::~RTPPacket_t (void)
{
    delete[] packet;
}

u8*
RTPPacket_t::getPacket(void)
{
    return packet;
}

u8*
RTPPacket_t::getData (void)
{
    return data;
}

int
RTPPacket_t::getDataLength (void)
{
    return dataLength;
}

int
RTPPacket_t::getTotalLength (void)
{
    return totalLength;
}

u32
RTPPacket_t::getSSRC (void)
{
    return ntohl(rtpHeader->ssrc);
}

u16
RTPPacket_t::getSequenceNumber (void)
{
    return ntohs(rtpHeader->seqnum);
}

u8
RTPPacket_t::getPayloadType (void)
{
    return rtpHeader->payloadtype;
}

ql_t<u32> *
RTPPacket_t::getCSRCList (void)
{
    if (rtpHeader -> cc != 0)
    {
        u32 *aux2 = (u32 *)(packet+sizeof(RTPHeader_t));
        for (unsigned char i = 0; i < rtpHeader->cc; i++)
        {
            u32 csrc = ntohl (*aux2);
            list.insert (csrc);
            aux2++;
        }
    }
    return &list;
}

bool
RTPPacket_t::getMark (void)
{
    return rtpHeader -> marker;
}

u32
RTPPacket_t::getTimestamp(void)
{
    return ntohl(rtpHeader -> timestamp);
}

u8
RTPPacket_t::getCC (void)
{
    return rtpHeader -> cc;
}

bool
RTPPacket_t::getPadding (void)
{
    return rtpHeader -> padding;
}

bool
RTPPacket_t::getExt (void)
{
    return rtpHeader -> extension;
}

u16
RTPPacket_t::getTotalExtensionLength (void)
{
    return extLength;
}

u16
RTPPacket_t::getExtensionLength (void)
{
    return (extensionData != NULL) ? extLength-sizeof(ExtensionHeader_t) : 0;
}

u8 *
RTPPacket_t::getExtensionData (void)
{
    return extensionData;
}

u16
RTPPacket_t::getExtensionId (void)
{
    return extensionId;
}

void
RTPPacket_t::setDataLength(u16 len)
{
    totalLength = len + rtpHeaderLength;
    dataLength  = len;
}

void
RTPPacket_t::setSSRC (u32 SSRC)
{
    rtpHeader->ssrc = htonl(SSRC);
}

