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
// $Id: statGatherer.cc 7584 2005-11-03 10:20:05Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>

#ifdef __BUILD_FOR_LINUX
#include <sys/types.h> //to include inet_ntop()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#endif

#include <rtp/RTPDefinitions.hh>

#include "statGatherer.h"
#include "sessionManager.h"
#include "returnCode.h"

RXData_t::RXData_t(void)
{
    dataLengthRX           = 0;
    duplicatedPkts         = 0;
    lastSequenceNo         = 0;
    lostPkts               = 0;
    pktNo                  = 0;
    receivedPkts           = 0;
    recoveredPkts          = 0;
    recoveredRX            = 0;
    totalLengthRX          = 0;
    unorderedPkts          = 0;
    SNcounter              = 0;
    lostPktsInInterval     = 0;
    receivedPktsInInterval = 0;
    restartInterval        = false;
    stats.BWData           = 0;
    stats.BWFEC            = 0;
    stats.BWTotal          = 0;
    stats.jitter           = 0;
    stats.fractionLost     = 0;

    memset(&IPAddr,0,sizeof(sockaddr_storage));
    orgSSRC = 0;
}

RXData_t::~RXData_t(void)
{
}

IPPayloads_t::IPPayloads_t(void)
{
    memset(PayloadList, 0x00, sizeof(RXData_t*) * PAYLOAD_NUMBER);
}

IPPayloads_t::~IPPayloads_t(void)
{
}

IPList_t::IPList_t(void)
{
    memset(IPList, 0x00, sizeof(IPPayloads_t*) * MAX_USERS_PER_SESSION);
}

IPList_t::~IPList_t(void)
{
}

rtcpSender_t::rtcpSender_t(inetAddr_t destAddr,
                           u16        localRTCPport,
                           u32        period,
                           RXData_t  *stats,
                           u16        ID_session
                          )
{
    socket = socketPool->get(localRTCPport);;
    data = stats;
    addr = new inetAddr_t(destAddr);
    session = ID_session;
    this->set_period(period);

    // new seed
    srand((unsigned)::time(NULL));

    do
    {
#if defined(__BUILD_FOR_LINUX)
        SSRC = random();
#elif defined(WIN32)
        SSRC = rand();
#else
#error "Please, implement random funtion for your OS"
#endif
    }
    while (data->orgSSRC == SSRC);
}

rtcpSender_t::~rtcpSender_t(void)
{
    socketPool->release(socket);
}

void
rtcpSender_t::heartBeat(void)
{
    //
    // Compute the interarrival jitter: 32 bits

    // int transit = arrival - r->ts;
    // int d = transit - s->transit;
    // s->transit = transit;
    // if (d < 0) d = -d;
    // s->jitter += (1./16.) * ((double)d - s->jitter);
    // rr->jitter = (u_int32) s->jitter;
    //
    // s->jitter += d - ((s->jitter + 8) >> 4);
    // rr->jitter = s->jitter >> 4;
    //

    // Calculate initial period to send packets
    u32 avgRTCPlength = 74;
    gettimeofday(&(data->final), NULL);
    u32 interval = ((data->final.tv_sec*1000000 + data->final.tv_usec)-(data->initial.tv_sec*1000000 + data->initial.tv_usec));

    if ( ! interval)
    {
        return;
    }

    data->stats.BWData  = (data->stats.BWData + (data->dataLengthRX*1000000)/(interval))/2;
    data->stats.BWTotal = (data->stats.BWTotal + (data->totalLengthRX*1000000)/(interval))/2;
    data->stats.BWFEC = (data->stats.BWFEC + (data->recoveredRX*1000000)/(interval))/2;
    data->stats.jitter = 0;

    double bitFreq = ((double)data->stats.BWData/1000000)*0.05/(double)avgRTCPlength;

    if (data->lostPktsInInterval)
    {
        data->stats.fractionLost = (u8) (0xff * data->lostPktsInInterval/(data->lostPktsInInterval + data->receivedPktsInInterval));
    }
    else
    {
        data->stats.fractionLost = 0;
    }

//    NOTIFY("statGatherer_t :: Packet Sent");
    data->restartInterval        = true;
    data->dataLengthRX           = 0;
    data->totalLengthRX          = 0;
    data->lostPktsInInterval     = 0;
    data->receivedPktsInInterval = 0;
    data->recoveredRX            = 0;

    if (bitFreq <= 1e-8)
    {
        return;
    }

    u32 newPeriod = (u32)(1.0/bitFreq);
    if (newPeriod < 10000.0)
    {
        newPeriod = 10000;
    }
    if (newPeriod > 10000000)
    {
        newPeriod = 10000000;
    }

//    NOTIFY("heartbeat ::        \n"
//           " - data->dataLengthRX = %d bytes\n"
//           " - BW = %d bytes/sec\n"
//           " - bitFreq = %f     \n"
//           " - interval = %d    \n"
//           " - newPeriod = %d usec \n"
//           ,data->dataLengthRX
//           ,data->stats.BWData
//           ,bitFreq
//           ,interval
//           ,newPeriod
//          );

    buildRtcpPkt();
    set_period(newPeriod);
}


HRESULT
rtcpSender_t::buildRtcpPkt (void)
{
    // Fixed length for this purpose, as there is always
    // only one receiver report

    RTCPReport report;

    u16 length = 7;

    report.header.blockcount     = 1;
    report.header.padding        = 0;
    report.header.version        = 2;
    report.header.packettype     = TYPE_RTCP_RR;
    report.header.length         = htons(length);
    report.ssrcHeader.ssrc       = htonl(SSRC);
    report.report.ssrc           = htonl(data->orgSSRC);
    report.report.fractionlost   = data->stats.fractionLost;
    report.report.packetslost[0] = (u8) (data->lostPkts >> 16);
    report.report.packetslost[1] = (u8) (data->lostPkts >> 8);;
    report.report.packetslost[2] = (u8) data->lostPkts;
    report.report.exthsnr        = htonl(data->SNcounter << 16 | data->lastSequenceNo);
    report.report.jitter         = 0;
    report.report.lsr            = 0;
    report.report.dlsr           = 0;

    HRESULT result = flush(report);

    return result;
}


HRESULT
rtcpSender_t::flush(RTCPReport const& pkt)
{
    int n = socket->writeTo(*addr, &pkt, 32); // 32? QUITAR LITERAL

    if (n < 0)
    {
        NOTIFY("rtcpSender_t::socket write returns %d\n",n);
        return E_ERROR;
    }

    return S_OK;
}


statGatherer_t::statGatherer_t(void)
{
}


statGatherer_t::~statGatherer_t(void)
{
    // Aqui se destruyen todos los objetos de todas las sesiones
    // en sessionlist
    // Tb se deben borrar todos los sender que esten activos (FALTA)

    std::map<u16, IPList_t*>::iterator iter;

    for (iter= SessionList.begin(); iter != SessionList.end(); iter++)
    {
        u16 sessionID= iter->first;
        IPList_t *SessionI = iter->second;

        // Deletes every flow inside a Session
        for (u16 j= 0; j < MAX_USERS_PER_SESSION; j++)
        {
            if (SessionI->IPList[j])
            {
                for (u8 k= 0; k <= PAYLOAD_NUMBER; k++)
                {
                    if (SessionI->IPList[j]->PayloadList[k])
                    {
                        RXData_t *tmp;
                        tmp = SessionI->IPList[j]->PayloadList[k];
                        delete tmp;
                    }
                }

                delete SessionI->IPList[j];
            }
        }

        delete SessionI;

        SessionList.erase(sessionID);
    }
}


HRESULT
statGatherer_t::createSession(u16 ID_session)
{
    // create a new session if not exist yet

    std::map<u16, IPList_t*>::iterator iter;

    iter= SessionList.find(ID_session);

    if (iter == SessionList.end()) // not found, create it
    {
        IPList_t *list = new IPList_t();
        SessionList[ID_session]= list;

        return S_OK;
    }
    else
    {
        return E_SESSION_EXISTS;
    }

    return E_ERROR;
}


HRESULT
statGatherer_t::removeSession(u16 ID_session)
{
    std::map<u16,IPList_t*>::iterator iter;

    iter= SessionList.find(ID_session);

    if ( iter == SessionList.end()) // not found
    {
        return E_SESSION_NOT_EXISTS;
    }

    IPList_t *sessionToRemove = iter->second;

    for (u16 j= 0; j < MAX_USERS_PER_SESSION; j++)
    {
        if (sessionToRemove->IPList[j])
        {
            for (u8 k= 0; k < PAYLOAD_NUMBER; k++)
            {
                if (sessionToRemove->IPList[j]->PayloadList[k])
                {
                    RXData_t *tmp;
                    tmp = sessionToRemove->IPList[j]->PayloadList[k];
                    delete tmp;
                }
            }

            delete sessionToRemove->IPList[j];
        }
    }

    SessionList.erase(ID_session);
    delete sessionToRemove;

    for (int i = 0; i < rtcpClientsArray.size(); i++)
    {
        if (rtcpClientsArray.elementAt(i)->session == ID_session)
        {
            APP->removeTask(rtcpClientsArray.elementAt(i));
            rtcpClientsArray.remove(i);
            i--;
        }
    }

    return S_OK;
}


HRESULT
statGatherer_t::bindRtcp(u16 ID_session,
                         sockaddr_storage const IPdest,
                         u16 remoteRTCPport,
                         u16 localRTCPport
                        )
{
    // create sender
    char portStr[6];
    char IP[128];

    sockaddr_in  *IP4 = NULL;
    sockaddr_in6 *IP6 = NULL;

    switch (IPdest.ss_family)
    {
    case AF_INET:
        IP4 = (sockaddr_in *)(&IPdest);
        inet_ntop(IP4->sin_family,(u8 *)&(IP4->sin_addr),IP,128);
        break;
    case AF_INET6:
        IP6 = (sockaddr_in6 *)(&IPdest);
        inet_ntop(IP6->sin6_family,(u8 *)&(IP6->sin6_addr),IP,128);
        break;
    default:
        NOTIFY("statGatherer_t::bindRtcp::Unknown family [%d]\n",IPdest.ss_family);
        abort();
    }

    sprintf(portStr,"%d",remoteRTCPport);
    inetAddr_t inetAddr(IP,portStr,SOCK_DGRAM);

    // Find client data
    for (u8 PT = 0; PT < 128; PT++)
    {
        u16 index = getPosition(IPdest, ID_session,PT);
        IPList_t *IPList = SessionList[ID_session];
        IPPayloads_t *IPPayloads = IPList->IPList[index];
        if ( ! IPPayloads)
        {
            continue;
        }

        RXData_t *data = IPPayloads->PayloadList[PT];

        if ( ! data)
        {
            continue;
        }

        bool already = false;
        for (int i = 0; i < rtcpClientsArray.size(); i++)
        {
            if (rtcpClientsArray.elementAt(i)->data == data)
            {
                already = true;
                break;
            }
        }

        if (already)
        {
            continue;
        }

        // Calculate initial period to send packets
        u32 avgRTCPlength = 32;
        gettimeofday(&(data->final), NULL);
        u32 interval = ((data->final.tv_sec*1000000 + data->final.tv_usec)-(data->initial.tv_sec*1000000 + data->initial.tv_usec));
        double bitFreq = ((double)data->dataLengthRX/(double)interval)*0.05/(double)avgRTCPlength;

        // Create Sender Object with original period
        rtcpSender_t *sender = new rtcpSender_t(inetAddr,
                                                localRTCPport,
                                                (u32)(1.0/bitFreq),
                                                data,
                                                ID_session
                                               );
        APP->insertTask(sender);

        // Add Reference to active rtcp receivers
        rtcpClientsArray.add(sender);
    }
    return S_OK;
}

HRESULT
statGatherer_t::setRXData(RTPPacket_t *pkt,
                          sockaddr_storage const IPorg,
                          u16 ID_session
                         )
{
    // We get the index from the IP address
    // in order to access the hash table
    u8 payloadType = pkt->getPayloadType();
    u16 index = getPosition(IPorg, ID_session, payloadType);

    // If this client didn't exist before, it's created
    IPList_t *session = SessionList[ID_session];

    if ( ! session->IPList[index])
    {
        session->IPList[index] = new IPPayloads_t();
    }

    RXData_t *data;

    // If data flow for this PT didn't exist before,
    // it's created for this client
    data= session->IPList[index]->PayloadList[payloadType];
    if ( ! data )
    {
        data = new RXData_t();

        memcpy(&(data->IPAddr), &IPorg, sizeof(sockaddr_storage));
        session->IPList[index]->PayloadList[payloadType] = data;

        // Fill first pkt
        int dataLength = pkt->getDataLength();
        int totalLength = pkt->getTotalLength();
        u16 sequenceNumber = pkt->getSequenceNumber();

        data->dataLengthRX += dataLength;
        data->totalLengthRX += totalLength;
        gettimeofday(&(data->initial), NULL);
        data->receivedPkts++;
        data->receivedPktsInInterval++;
        data->lastSequenceNo = sequenceNumber;
        data->orgSSRC = pkt->getSSRC();

        return S_OK;
    }

    // Update statistics data with current packet
    int dataLength = pkt->getDataLength();
    int totalLength = pkt->getTotalLength();
    u16 sequenceNumber = pkt->getSequenceNumber();

    data->dataLengthRX += dataLength;
    data->totalLengthRX += totalLength;

    if (data->restartInterval)
    {
        data->restartInterval=false;
        gettimeofday(&(data->initial), NULL);
    }

    // Normal mode
    if (sequenceNumber > data->lastSequenceNo)
    {
        u16 rest = sequenceNumber - data->lastSequenceNo;

        // Packets with expected Sequence number
        if (rest == 1)
        {
            data->receivedPkts++;
            data->receivedPktsInInterval++;
            data->lastSequenceNo = sequenceNumber;
        }

        // Lost packets
        else if (rest > 1 && rest <= (MAX_SESSIONS/2))
        {
            data->lostPktsInInterval += sequenceNumber - data->lastSequenceNo + 1;
            data->lostPkts += data->lostPktsInInterval;
            data->lastSequenceNo = sequenceNumber;
        }

        // Not in order packets once sequence number is reseted to 0
        else if (rest > (MAX_SESSIONS/2))
        {
            data->unorderedPkts++;
            data->lostPkts--;
            data->lostPktsInInterval--;
        }
    }

    // Duplicated Sequence Number
    else if (sequenceNumber == data->lastSequenceNo)
    {
        data->duplicatedPkts++;
    }

    // If sequence number is reseted, then this condition applies
    else if (sequenceNumber < data->lastSequenceNo)
    {
        u16 rest = data->lastSequenceNo - sequenceNumber;

        if (rest > (MAX_SESSIONS/2))
        {
            // It may be correct (if lastSequence is 0xffff and
            // the pkt secuence number is 0)
            if (sequenceNumber == (data->lastSequenceNo++) % MAX_SESSIONS)
            {
                data->receivedPkts++;
                data->receivedPktsInInterval++;
                data->lastSequenceNo = sequenceNumber;
                data->SNcounter++;
            }

            // Lost Packets
            else
            {
                data->lostPktsInInterval += (MAX_SESSIONS/2) - data->lastSequenceNo + sequenceNumber - 1;
                data->lostPkts += data->lostPktsInInterval;
                data->lastSequenceNo = sequenceNumber;
                data->SNcounter++;
            }
        }
        else if (rest < (MAX_SESSIONS/2))
        {
            data->unorderedPkts ++;
            data->lostPkts--;
            data->lostPktsInInterval--;
        }
    }

    return S_OK;
}


HRESULT
statGatherer_t::setRCVData(RTPPacket_t *pkt,
                           sockaddr_storage const IPorg,
                           u16 ID_session
                          )
{
    u8 payloadType = pkt->getPayloadType();
    u16 index = getPosition(IPorg, ID_session, payloadType);

    if ( ! index)
    {
        return E_ERROR;
    }
    else
    {
        RXData_t *data = SessionList[ID_session]->IPList[index]->PayloadList[payloadType];

        data->lostPkts--;
        data->recoveredPkts++;
        data->recoveredRX += pkt->getDataLength();
    }

    return S_OK;
}


u16
statGatherer_t::getPosition(sockaddr_storage const & IP,
                            u16 ID_session,
                            u8 PT
                           )
{
    // hash table
    u16 position = getIndex(IP);

    IPList_t *session = SessionList[ID_session];

    while (session->IPList[position])
    {
        // Temporal pointers
        RXData_t *tmpStats = session->IPList[position]->PayloadList[PT];
        if (tmpStats)
        {
            if (addrEquals(tmpStats->IPAddr, IP))
            {
                break;
            }
        }
        else
        {
            break;
        }
        // else we go to the next position in buffer
        position = (position + 1) % MAX_USERS_PER_SESSION;
    }

    // PT slot is empty and ready for use
    return position;
}

