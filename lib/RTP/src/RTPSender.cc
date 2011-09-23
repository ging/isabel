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
// $Id: RTPSender.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPSender.hh>

RRSender_t::RRSender_t (u32 src, ReportBlock_t *rb)
{
    ssrc = src;
    reportBlock = new ReportBlock_t;
    processReportBlock (rb);
}

RRSender_t::~RRSender_t (void)
{
    delete reportBlock;
    debugMsg(dbg_App_Verbose, "~RRSender_t", "Deleting RRSender_t");
}

void
RRSender_t::processReportBlock (ReportBlock_t *rb)
{
    gettimeofday(&rrtime, NULL);
    reportBlock -> ssrc = rb -> ssrc;
    reportBlock -> fractionlost = rb -> fractionlost;
    reportBlock -> packetslost[0] = rb -> packetslost[0];
    reportBlock -> packetslost[1] = rb -> packetslost[1];
    reportBlock -> packetslost[2] = rb -> packetslost[2];
    reportBlock -> exthsnr = rb -> exthsnr;
    reportBlock -> jitter = rb -> jitter;
    reportBlock -> lsr = rb -> lsr;
    reportBlock -> dlsr = rb -> dlsr;
    //computing round-trip time
    if ((reportBlock -> lsr != 0) && (reportBlock -> dlsr != 0))
    {
        unsigned long msw = rrtime.tv_sec + RTP_NTP_OFFSET;
        double x = (double) rrtime.tv_usec;
        x /= 1000000.0;
        x *= (65536.0*65536.0);
        unsigned long lsw = (unsigned long)x;
        unsigned long A = ((msw&65535)<<16)|((lsw>>16)&65535);
        unsigned long roundtrip = A - (reportBlock -> lsr) - (reportBlock -> dlsr);
        rttime.tv_sec = (int)(((double)roundtrip)/65536.0);
        rttime.tv_usec = (int)(((double)(roundtrip&0xFFFF))/(65536.0)*1000000.0);
    }
}

RTPSender_t::RTPSender_t (u32 src,
                          const char *CNAMEBuffer,
                          double tsUnit,
                          Sendertype type,
                          dgramSocket_t *rtpSocket
                         )
{
    debugMsg(dbg_App_Verbose, "RTPSender_t", "Creating RTPSender_t");
    random = new RTPRandom_t;
    ssrc = src;
    timestampUnit = tsUnit;
    octetCount = 0;
    packetCount = 0;
    sentData = false;
    timestamp = 0;
    timestampOffset = timestamp;
    first = true;
    sequenceNumber = 0;
    if (CNAMEBuffer != NULL)
    {
        SDESInfo [TYPE_SDES_CNAME] = strdup(CNAMEBuffer);

        for (int i = TYPE_SDES_NAME; i < TYPE_SDES; i++)
        {
            SDESInfo [i] = NULL;
        }
    }
    rtpHolder = new bindingHolder_t;
    rrSenderList = new ql_t<RRSender_t *>;
    sendertype = type;
    if (sendertype == MIXER)
    {
        csrcList = new ql_t<u32>;
    }
    else
    {
        csrcList = NULL;
    }
    debugMsg(dbg_App_Verbose, "RTPSender_t", "SSRC: %u", ssrc);
    debugMsg(dbg_App_Verbose, "RTPSender_t", "Timestamp: %u", timestamp);
    debugMsg(dbg_App_Verbose, "RTPSender_t", "Sequence Number: %u", sequenceNumber);
}

RTPSender_t::~RTPSender_t (void)
{
    delete random;
    if (sendertype == MIXER)
    {
        delete csrcList;
    }
    if (sendertype == SENDER)
    {
        for (int i = TYPE_SDES_CNAME; i < TYPE_SDES; i++)
        {
            free (SDESInfo[i]);
        }
    }
    if (rrSenderList -> len() > 0)
    {
        for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
             i != rrSenderList -> end();
             i++
            )
        {
            RRSender_t *rrSender = static_cast<RRSender_t *> (i);
            delete rrSender;
        }
    }
    delete rrSenderList;

    delete rtpHolder;

    debugMsg(dbg_App_Verbose, "~RTPSender_t", "Deleting RTPSender_t");
}


bindId_t
RTPSender_t::bind (const char *h, const char *p)
{
    return rtpHolder->bind (h, p);
}

bool
RTPSender_t::unbind (bindId_t bId)
{
    return rtpHolder->unbind (bId);
}

const char *
RTPSender_t::showBindings (void)
{
    return rtpHolder->showBindings();
}

const vector_t<bindDescriptor_t*>*
RTPSender_t::getBindings(void)
{
    return rtpHolder->getBindings();
}

void
RTPSender_t::updateParameters (int length, u16 seqNum, u32 ts)
{
    octetCount += length;
    packetCount++;
    sequenceNumber = seqNum;
    timestamp = ts;
    if (first)
    {
        timestampOffset = ts;
        gettimeofday(&initialTime, NULL);
        first = false;
    }
}


void
RTPSender_t::setCSRC (u32 csrc)
{
    bool found = false;

    for (ql_t<u32>::iterator_t i = csrcList->begin();
         i != csrcList->end();
         i++
        )
    {
        u32 src = static_cast<u32> (i);
        if (src == csrc)
            found = true;
    }

    if ( ! found)
    {
        csrcList->insert (csrc);
    }
}

ql_t<u32> *
RTPSender_t::getCSRCList (void)
{
    return csrcList;
}

void
RTPSender_t::setSDESInfo (const char *info, int pos)
{
    if (pos < TYPE_SDES_NAME || pos > TYPE_SDES_NOTE)
        return;

    size_t length = strlen (info);
    if (length > 256)
        length = 256;

    char *sdesInfo = (char*)malloc (length+1);
    memcpy (sdesInfo, info, length);
    sdesInfo[length] = '\0';

    SDESInfo[pos] = sdesInfo;

    debugMsg(dbg_App_Verbose, "setSDESInfo", "Info: %s", SDESInfo[pos]);
}

void
RTPSender_t::setSDESInfo (const char *prefix, const char *info, int pos)
{
    if (pos != TYPE_SDES_PRIV)
        return;

    char *sdesInfo = (char*)malloc (strlen(info) + 1);
    strcpy (sdesInfo, info);
    SDESInfo[pos] = sdesInfo;
    char *sdesPrefix = (char*)malloc (strlen(prefix) + 1);
    strcpy (sdesPrefix, prefix);

    SDESInfo[pos+1] = sdesPrefix;

    debugMsg(dbg_App_Verbose,
             "setSDESInfo",
             "Prefix: %s -- Info: %s", SDESInfo[pos+1],
             SDESInfo[pos]
            );
}

const char *
RTPSender_t::getSDESInfo (int pos)
{
    if ((pos >= TYPE_SDES_CNAME) && (pos < TYPE_SDES_PRIV))
    {
        return SDESInfo[pos];
    }
    if ((pos == TYPE_SDES_PRIV) && (SDESInfo[TYPE_SDES_PRIV] != NULL))
    {
        char *retVal= (char*)malloc(256);
        retVal[0] = '\0';
        size_t prefixLength = strlen (SDESInfo[pos+1]);
        assert (prefixLength <= 255);
        sprintf (retVal, "%c", prefixLength);
        strcat (retVal, SDESInfo[pos+1]);
        strcat (retVal, SDESInfo[pos]);
        return retVal;
    }
    return NULL;
}

bool
RTPSender_t::processReportBlock (u32 ssrc, ReportBlock_t *reportBlock)
{
    bool newRR = false;
    RRSender_t *rrSender = NULL;
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        rrSender = static_cast<RRSender_t *> (i);
        if (rrSender -> ssrc == ssrc)
            break;
        else
            rrSender = NULL;
    }

    if (rrSender == NULL)
    {
        rrSender = new RRSender_t (ssrc, reportBlock);
        rrSenderList -> insert (rrSender);
        newRR = true;
    }
    else
    {
        rrSender -> processReportBlock (reportBlock);
    }

    debugMsg(dbg_App_Verbose,
             "processReportBlock",
             "Receiver Report processed correctly"
            );

    return newRR;
}

ql_t<u32> *
RTPSender_t::getRRList(void)
{
    ql_t<u32> *rrList = new ql_t<u32>;
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        RRSender_t *rrSender = static_cast<RRSender_t*>(i);
        rrList -> insert (rrSender -> ssrc);
    }
    return rrList;
}

const char *
RTPSender_t::showReceivers (void)
{
    char aux[512];
    char *retVal= (char*)malloc(10*1024*sizeof(char));
    retVal[0]='\0';
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        RRSender_t *rrSender = static_cast<RRSender_t *> (i);
        sprintf(aux, "Id=%u(%u)", rrSender -> ssrc, ssrc);
        if (strlen(retVal) + strlen(aux) < 10*1024)
        {
            sprintf (retVal, "%s\n %s", retVal, aux);
        }
    }

    if (strlen(retVal) == 0)
    {
        sprintf(retVal, "No receivers\n");
    }
    else
    {
        sprintf(retVal, "%s\n", retVal);
    }
    return retVal;
}

const char *
RTPSender_t::getFractionLost (u32 receiver)
{
    u8 fractionLost = 0;
    char aux[256];
    char *retVal= (char*)malloc(1024*sizeof(char));
    retVal[0]= '\0';
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        RRSender_t *rrSender = static_cast<RRSender_t *> (i);
        if (rrSender -> ssrc == receiver)
        {
            fractionLost = rrSender -> reportBlock -> fractionlost;
            sprintf(aux, "Id=%u(%u) Fraction lost: [%u]", rrSender -> ssrc, ssrc, fractionLost);
            if (strlen(retVal) + strlen(aux) < 1024)
            {
                sprintf (retVal, "%s\n %s", retVal, aux);
            }
            break;
        }
    }

    if (strlen(retVal) == 0)
    {
        sprintf(retVal, "bad receiver\n");
    }
    else
    {
        sprintf(retVal, "%s\n", retVal);
    }
    return retVal;
}

const char *
RTPSender_t::getRoundTripTime (u32 receiver)
{
    struct timeval rttime;
    char aux[256];
    char *retVal= (char*)malloc(1024*sizeof(char));
    retVal[0]='\0';
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        RRSender_t *rrSender = static_cast<RRSender_t *> (i);
        if (rrSender -> ssrc == receiver)
        {
            rttime = rrSender -> rttime;
            sprintf(aux, "Id=%u(%u) Round-Trip Time: [%lu.%lu]", rrSender -> ssrc, ssrc, rttime.tv_sec, rttime.tv_usec);
            if (strlen(retVal) + strlen(aux) < 1024)
            {
                sprintf (retVal, "%s\n %s", retVal, aux);
            }
            break;
        }
    }

    if (strlen(retVal) == 0)
    {
        sprintf(retVal, "bad receiver\n");
    }
    else
    {
        sprintf(retVal, "%s\n", retVal);
    }
    return retVal;
}

const char *
RTPSender_t::getPacketsLost (u32 receiver)
{
    u32 lost;
    char aux[256];
    char *retVal= (char*)malloc(1024*sizeof(char));
    retVal[0]='\0';
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        RRSender_t *rrSender = static_cast<RRSender_t *> (i);
        if (rrSender -> ssrc == receiver)
        {
            lost = ((rrSender -> reportBlock -> packetslost[0]&255) << 16 ) |
                   ((rrSender -> reportBlock -> packetslost[1]&255) << 8 )  |
                   ((rrSender -> reportBlock -> packetslost[2]&255));
            sprintf(aux, "Id=%u(%u) Lost: [%u]", rrSender -> ssrc, ssrc, lost);
            if (strlen(retVal) + strlen(aux) < 1024)
            {
                sprintf (retVal, "%s\n %s", retVal, aux);
            }
            break;
        }
    }

    if (strlen(retVal) == 0)
    {
        sprintf(retVal, "bad receiver\n");
    }
    else
    {
        sprintf(retVal, "%s\n", retVal);
    }

    return retVal;
}

const char *
RTPSender_t::getJitter (u32 receiver)
{
    u32 jitter;
    char aux[256];
    char *retVal= (char*)malloc(1024*sizeof(char));
    retVal[0]='\0';
    for (ql_t<RRSender_t *>::iterator_t i = rrSenderList -> begin();
         i != rrSenderList -> end();
         i++
        )
    {
        RRSender_t *rrSender = static_cast<RRSender_t *> (i);
        if (rrSender -> ssrc == receiver)
        {
            jitter = (u32)rrSender -> reportBlock -> jitter;
            double timeJitter = (double)jitter * timestampUnit;
            sprintf(aux, "Id=%u(%u) Jitter: [%g]", rrSender -> ssrc, ssrc, timeJitter);
            if (strlen(retVal) + strlen(aux) < 1024)
            {
                sprintf (retVal, "%s\n %s", retVal, aux);
            }
            break;
        }
    }

    if (strlen(retVal) == 0)
    {
        sprintf(retVal, "bad receiver\n");
    }
    else
    {
        sprintf(retVal, "%s\n", retVal);
    }

    return retVal;
}

