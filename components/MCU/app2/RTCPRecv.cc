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
// $Id: RTCPReceiver.cc 6643 2005-05-11 15:30:59Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include "RTCPRecv.h"
#include "socketPool.h"
#include "sessionManager.h"
#include "returnCode.h"

class mcuRTCPReport_t
{
public:
    RTCPHeader_t *rtcpHeader;
    SSRCHeader_t *ssrcHeader;
    SenderInfo_t *senderInfo;
    ql_t<ReportBlock_t *> rrList;

    mcuRTCPReport_t(u8 *data);
    ~mcuRTCPReport_t(void);
};

mcuRTCPReport_t::mcuRTCPReport_t(u8 * data)
: rtcpHeader(NULL),
  ssrcHeader(NULL),
  senderInfo(NULL)
{
    int headerPos = 0;
    rtcpHeader = new RTCPHeader_t;

    memcpy(rtcpHeader,&data[headerPos],sizeof(RTCPHeader_t));

    rtcpHeader->length = ntohs(rtcpHeader->length);
    int SSRCPos   = headerPos + sizeof(RTCPHeader_t);
    ssrcHeader = NULL;

    int SRPos  = -1;
    int RRPos  = -1;
    ssrcHeader = NULL;
    senderInfo = NULL;

    // save info
    if (rtcpHeader->packettype != TYPE_RTCP_SR &&
        rtcpHeader->packettype != TYPE_RTCP_RR)
    {
        NOTIFY("mcuRTCPReport_t :: RTCP PT = %d (unknown PT)\n",
               rtcpHeader->packettype
              );
        return;
    }

    ssrcHeader = new SSRCHeader_t;
    memcpy(ssrcHeader, &data[SSRCPos], sizeof(SSRCHeader_t));
    ssrcHeader->ssrc = ntohl(ssrcHeader->ssrc);
    if (rtcpHeader->packettype == TYPE_RTCP_SR) // if sender report
    {
        SRPos     = SSRCPos + sizeof(SSRCHeader_t);
        RRPos     = SRPos + sizeof(SenderInfo_t);
        senderInfo = new SenderInfo_t;
        memcpy(senderInfo,&data[SRPos],sizeof(SenderInfo_t));
    }
    else
    {   // if receiver report can tell SDES
        RRPos = SSRCPos + sizeof(SSRCHeader_t);
    }

    u16 i = RRPos;
    for (u16 j = 0; j < rtcpHeader->blockcount; j++, i += sizeof(ReportBlock_t))
    {
        ReportBlock_t * reportBlock = new ReportBlock_t;
        memcpy(reportBlock,&data[i],sizeof(ReportBlock_t));
        NOTIFY("ReportBlock :: losses = %f\n",reportBlock->fractionlost);
        rrList.insert(reportBlock);
    }
}

mcuRTCPReport_t::~mcuRTCPReport_t(void)
{
    if (rtcpHeader)
    {
        delete rtcpHeader;
    }
    if (ssrcHeader)
    {
        delete ssrcHeader;
    }
    if (senderInfo)
    {
        delete senderInfo;
    }

    while (rrList.len())
    {
        delete rrList.head();
        rrList.behead();
    }
}

addrReport_t::addrReport_t(void)
{
}

addrReport_t::~addrReport_t(void)
{
}
 
RTCPRecv_t::RTCPRecv_t(u16 localPort)
{
    memset(addrReportArr, 0, sizeof(addrReport_t *) * MAX_USERS);

    port = localPort;
    socket = socketPool->get(localPort);

    add_IO(socket);

    APP->insertTask(this);
}

RTCPRecv_t::~RTCPRecv_t(void)
{
    del_IO(socket);
    APP->removeTask(this);
}

// RTCPPacket work
// we only need losses (for now)
void
RTCPRecv_t::IOReady(io_ref &io)
{
    int n = socket->read(data, MAX_PKT_LEN); // only 1 IO

    if (n < 0)
    {
        NOTIFY("portListener_t (%d) :: socket->read() returns %d\n",
               socket->localPort,
               n
              );
        return;
    }

    sockaddr_storage *IP = (sockaddr_storage *)&socket->temporaryClientAddr->saddr.ssaddr;
    
    // get IP position in hash table
    u16 pos = getPosition(*IP);
    
    if ( ! addrReportArr[pos])
    {
        return;
    }
    
    // check if RTCP pkt comes from the correct IP
    // save info
    mcuRTCPReport_t report(data);

    for (ql_t<ReportBlock_t*>::iterator_t i = report.rrList.begin();
         i != report.rrList.end();
         i++
        )
    {
        ReportBlock_t *rb= static_cast<ReportBlock_t*>(i);
        addrReportArr[pos]->rrList.insert(*rb);
    }
}

double 
RTCPRecv_t::getLosses(int type, sockaddr_storage const &IP)
{
    // get reportBlockList
    double losses = 0.0;
    u16 pos = getPosition(IP);
    u16 j = 0;
    for (ql_t<ReportBlock_t>::iterator_t i= addrReportArr[pos]->rrList.begin();
         i != addrReportArr[pos]->rrList.end();
         i++
        )
    {
        if (getFlowByPT(addrReportArr[pos]->PT) == type ||
            type == UNKNOWN_FLOW
           )
        {
            ReportBlock_t rb= static_cast<ReportBlock_t>(i);

            losses += rb.fractionlost;
            j++;
        }
        if ( ! j)
        {
            j = 1;
        }
    }

    // returns average
    return losses / j;
}

u16
RTCPRecv_t::getPort(void)
{
    return port;
}

u16
RTCPRecv_t::getPosition(sockaddr_storage const& IP)
{
    // hash table
    u16 position = getIndex(IP);

    // check duplicates
    while (addrReportArr[position])
    {
        if (addrEquals(IP,addrReportArr[position]->addr))
        {
            break;
        }
        position = (position + 1) % MAX_USERS;
    }
    
    return position;
}

// ADD/DELETE RTCP receiver
HRESULT 
RTCPRecv_t::addReceiver(sockaddr_storage const IP,u8 PT)
{
    // get position
    u16 pos = getPosition(IP);
    if ( ! addrReportArr[pos])
    {
        addrReportArr[pos] = new addrReport_t;
        addrReportArr[pos]->addr = IP;
        addrReportArr[pos]->PT = PT;
    }
    return S_OK;
}

HRESULT 
RTCPRecv_t::deleteReceiver(sockaddr_storage const& IP)
{
    u16 pos = getPosition(IP);
    if (addrReportArr[pos])
    {
        delete addrReportArr[pos];
        addrReportArr[pos] = NULL;
        for (u16 k = 0; k < MAX_USERS; k++)
        {
            int pos1 = (pos + k) % MAX_USERS;
            int pos2 = (pos1 + 1) % MAX_USERS;
            if (addrReportArr[pos2])
            {
                if (getPosition(addrReportArr[pos2]->addr) == pos1)
                {
                    addrReportArr[pos1] = addrReportArr[pos2];
                    addrReportArr[pos2] = NULL;
                }
            }
            else
            {
                break;
            }
        }
    }
    return S_OK;
}



RTCPDemux_t::RTCPDemux_t(void)
{
}

RTCPDemux_t::~RTCPDemux_t(void)
{
    while (RTCPRecvArray.size())
    {
        RTCPRecv_t *rr= RTCPRecvArray.elementAt(0);
        APP->removeTask(rr);
        RTCPRecvArray.remove(0);
    }
}

// ADD/DELETE RTCP receiver
HRESULT 
RTCPDemux_t::addReceiver(sockaddr_storage const IP,
                         u8 PT,
                         u16 localRTCPport
                        )
{
    RTCPRecv_t *RTCPRecv = NULL;

    for (int i= 0; i < RTCPRecvArray.size(); i++)
    {
        RTCPRecv = RTCPRecvArray.elementAt(i);

        if (RTCPRecv->getPort() == localRTCPport)
        {
            break;    
        }
        else
        {
            RTCPRecv = NULL;
        }
    }

    if ( ! RTCPRecv)
    {
        // create RTCPreceiver at localport
        RTCPRecv = new RTCPRecv_t(localRTCPport);
        RTCPRecvArray.add(RTCPRecv);
    }

    return RTCPRecv->addReceiver(IP, PT);
}

HRESULT 
RTCPDemux_t::deleteReceiver(sockaddr_storage const& IP, u16 localRTCPport)
{
    if (localRTCPport)
    {
        for (int i= 0; i < RTCPRecvArray.size(); i++)
        {
            RTCPRecv_t *RTCPRecv = RTCPRecvArray.elementAt(i);
            if (RTCPRecv->getPort() == localRTCPport)
            {
                RTCPRecv->deleteReceiver(IP);
                APP->removeTask(RTCPRecv);
                RTCPRecvArray.remove(i);
                i--;
                break;
            }
        }
    }

    return S_OK;
}

// get Report (fraction lost field)
// type can be:
//      -- AUDIO_LOSSES
//      -- VIDEO_LOSSES
//      -- TOTAL_LOSSES
double 
RTCPDemux_t::getLosses(int type,sockaddr_storage const& IP)
{
    double losses = 0.0;

    for (int i= 0; i < RTCPRecvArray.size(); i++)
    {
        if ((losses = RTCPRecvArray.elementAt(i)->getLosses(type, IP)) >= 0.0)
        {
            return losses;
        }
    }

    return 0.0;
}

