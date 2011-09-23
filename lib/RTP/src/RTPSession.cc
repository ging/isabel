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
// $Id: RTPSession.cc 20767 2010-07-07 09:12:06Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifdef __BUILD_FOR_LINUX
#include <sys/types.h>
#include <pwd.h>
#endif

#include <icf2/general.h>

#include <rtp/RTPSession.hh>
#include <rtp/RTPReceiver.hh>
#include <rtp/RTCPReceiver.hh>
#include <rtp/RTPSourcesReceived.hh>

#ifdef WIN32
char *
getmyip(char * dest)
{
    char hname[512];
    memset(hname, 0, 256);

    if (gethostname(hname, 256)==-1)
    {
        printf("Gethostname returns -1 !\n");
        return NULL;
    }

    struct hostent *h=gethostbyname(hname);
    if (!h)
    {
        printf("Gethostbyname returns no hostent struct!\n");
        return NULL;
    }

    char *addr = h->h_addr_list[0];
    if (!addr)
    {
        printf("Gethotbyname returns no addr struct!\n");
        return NULL;
    }

    inet_ntop(AF_INET,(unsigned char *)addr,dest,100);
    return dest;
}
#endif


RTPSession_t::RTPSession_t (sched_t    *sched,
                            const char *rtpHost,
                            const char *rtpPort,
                            const char *rtcpHost,
                            const char *rtcpPort,
                            double      tsUnit
                           )
: theScheduler(sched),
  rtpBuffer(NULL)
{
    timestampUnit = tsUnit;
    maxPacketSize = RTP_MAX_PACKET_SIZE;
    usernumber = 'a';
#ifdef DYNAMIC
    deliverAny = true;
#else
    deliverAny = false;
#endif
    initialize (rtpHost, rtpPort, rtcpHost, rtcpPort);
}

RTPSession_t::~RTPSession_t (void)
{
    debugMsg(dbg_App_Paranoic, "~RTPSession_t", "destructor invoked");

    ql_t<u32> *senderList = senderDictionary -> getKeys ();
    for (ql_t<u32>::iterator_t i = senderList -> begin();
         i != senderList -> end();
         i++
        )
    {
        u32 ssrc = static_cast<u32> (i);
        deleteSender (ssrc, "RTPSession Destructor invoked");
    }

    delete senderList;
    delete senderDictionary;
    delete flowsReceived;

    if (rtpBuffer)
    {
        free (rtpBuffer);
    }
}

void
RTPSession_t::initialize (const char *rtpHost,
                          const char *rtpPort,
                          const char *rtcpHost,
                          const char *rtcpPort
                         )
{
    inetAddr_t rtpAddress(rtpHost, rtpPort, serviceAddr_t::DGRAM);
    rtpSocket = new dgramSocket_t(rtpAddress);
    rtpSocket->setSockMaxBuf();

    inetAddr_t rtcpAddress(rtcpHost, rtcpPort, serviceAddr_t::DGRAM);
    rtcpSocket = new dgramSocket_t(rtcpAddress);
    rtcpSocket->setSockMaxBuf();

    rtpReceiver = new RTPReceiver_t (rtpSocket, this);
    (*theScheduler)<< rtpReceiver;

    rtcpReceiver = new RTCPReceiver_t (rtcpSocket, this);
    (*theScheduler)<< rtcpReceiver;

    flowsReceived = new RTPFlowsReceived_t;
    senderDictionary = new dictionary_t<u32, RTPSender_t*>;
    statisticsManager = NULL;

    /*statisticsManager =
        new RTPStatisticsManager_t (rtcpSocket,
                                    flowsReceived,
                                    senderDictionary,
                                    bw,
                                    fragment,
                                    maxPacketSize
                                   );
    (*theScheduler)<< statisticsManager;*/

    timeOutChecker = new RTPTimeOutChecker_t (this, flowsReceived);
    (*theScheduler)<< timeOutChecker;

    rtpBuffer = (u8 *)malloc (RTP_BUFFER_SIZE);
    if ( ! rtpBuffer)
    {
        NOTIFY ("RTPSession_t::initialize: No enough memory for rtpBuffer\n");
        abort();
    }
}

void
RTPSession_t::deliverAnyPacket(bool deliver)
{
    debugMsg(dbg_App_Normal,
             "deliverAnyPacket",
             "deliverAny set to %d",
             deliver
            );
    deliverAny = deliver;
}

void
RTPSession_t::killSockets (void)
{
    debugMsg(dbg_App_Normal,
             "killSockets",
             "Eliminating tasks listening in sockets"
            );

    (*theScheduler)-timeOutChecker;
    (*theScheduler)-rtpReceiver;
    (*theScheduler)-rtcpReceiver;
}

void
RTPSession_t::assignSender (u32 ssrc)
{
    RTPSender_t *s = senderDictionary -> lookUp (ssrc);
    if (s != NULL)
    {
        debugMsg(dbg_App_Normal,
                 "assignSender",
                 "Sender(%d) already created",
                 ssrc
                );
        return;
    }

    char *CNAMEBuffer = (char *)malloc (256);
    createCNAME (CNAMEBuffer);
    RTPSender_t *sender = new RTPSender_t (ssrc,
                                           CNAMEBuffer,
                                           timestampUnit,
                                           SENDER,
                                           rtpSocket
                                          );

    senderDictionary -> insert (ssrc, sender);

    if (statisticsManager)
    {
        statisticsManager -> rtcpInterval();
    }
}

void
RTPSession_t::assignMixer (u32 ssrc)
{
    RTPSender_t *s = senderDictionary -> lookUp (ssrc);
    if (s != NULL)
    {
        debugMsg(dbg_App_Normal,
                 "assignMixer",
                 "Sender(%d) already created",
                 ssrc
                );
        return;
    }

    RTPSender_t *sender = new RTPSender_t (ssrc,
                                           NULL,
                                           timestampUnit,
                                           MIXER,
                                           rtpSocket
                                          );
    senderDictionary -> insert (ssrc, sender);

    if (statisticsManager)
        statisticsManager -> rtcpInterval();

}

void
RTPSession_t::addCsrc (u32 ssrc, u32 csrc)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL)
    {
        debugMsg(dbg_App_Normal, "addCsrc", "Sender(%d) not created", ssrc);
        return;
    }

    Sendertype type = sender -> getType();
    if (type != MIXER)
    {
        debugMsg(dbg_App_Normal, "addCsrc", "Sender(%d) is not a mixer", ssrc);
        return;
    }

    sender -> setCSRC (csrc);
}

void
RTPSession_t::assignTranslator (u32 ssrc)
{
    RTPSender_t *s = senderDictionary -> lookUp (ssrc);
    if (s != NULL)
    {
        debugMsg(dbg_App_Normal,
                 "assignTranslator",
                 "Sender(%d) already created",
                 ssrc
                );
        return;
    }
    RTPSender_t *sender = new RTPSender_t (ssrc,
                                           NULL,
                                           timestampUnit,
                                           TRANSLATOR,
                                           rtpSocket
                                          );

    senderDictionary -> insert (ssrc, sender);

    if (statisticsManager)
        statisticsManager -> rtcpInterval();
}

void
RTPSession_t::deleteSender (u32 ssrc, const char *reason)
{
    RTPSender_t *s = senderDictionary -> lookUp (ssrc);
    if (s == NULL)
    {
        debugMsg(dbg_App_Normal, "deleteSender", "Sender(%d) not created", ssrc);
        return;
    }

    Sendertype type = s -> getType();
    switch (type)
    {
        case SENDER:
            {
                ql_t<u32> *list = new ql_t<u32>;
                list -> insert (ssrc);
                if (statisticsManager)
                    statisticsManager -> sendRtcpPackets (s, false, true, list, reason, strlen(reason));
                delete list;
                break;
            }
        case MIXER:
            {
                debugMsg(dbg_App_Normal, "deleteSender", "Mixer RTCP");
                ql_t<u32> *list = s -> getCSRCList ();
                if (statisticsManager)
                    statisticsManager -> sendMixerRtcpPackets (s, false, true, list, reason, strlen(reason));
                break;
            }
        case TRANSLATOR:
            {
                debugMsg(dbg_App_Normal, "deleteSender", "Translator RTCP");
                ql_t<u32> *list = new ql_t<u32>;
                list -> insert (ssrc);
                if (statisticsManager)
                    statisticsManager -> sendTranslatorRtcpPackets (s, false, true, list, reason, strlen(reason));
                delete list;
                break;
            }
    }

    delete s;
    senderDictionary -> remove (ssrc);

    if (statisticsManager)
        statisticsManager -> rtcpInterval();

}

void
RTPSession_t::newFlow (u32 ssrc)
{
    debugMsg(dbg_App_Normal, "newFlow", "Creating a new flow SSRC: %u", ssrc);
    flowsReceived -> createFlow (ssrc, timestampUnit);

    if (statisticsManager)
        statisticsManager -> rtcpInterval();
}

void
RTPSession_t::deleteFlow (u32 ssrc)
{
    debugMsg(dbg_App_Normal, "deleteFlow", "Deleting flow SSRC: %u", ssrc);
    flowsReceived -> processBYEInfo (ssrc);

    if (statisticsManager)
        statisticsManager -> rtcpInterval();
}

bindId_t
RTPSession_t::rtpBind (u32 ssrc, const char *host, const char *port)
{
    RTPSender_t *sender= senderDictionary->lookUp(ssrc);
    if (sender == NULL)
    {
        NOTIFY("No sender [%d] for binding\n", ssrc);
        return -1;
    }

    return sender->bind(host, port);
}

bool
RTPSession_t::rtpUnbind (u32 ssrc, bindId_t bId)
{
    RTPSender_t *sender= senderDictionary->lookUp(ssrc);
    if (sender == NULL)
    {
        NOTIFY("No sender [%d] for unbinding\n", ssrc);
        return false;
    }

    return sender->unbind(bId);
}

const char *
RTPSession_t::rtpShowBindings (u32 ssrc)
{
    RTPSender_t *sender= senderDictionary->lookUp(ssrc);
    if (sender == NULL)
    {
        NOTIFY("No sender [%d] for showing bindings\n", ssrc);
        return NULL;
    }

    return sender->showBindings();
}

bindId_t
RTPSession_t::rtcpBind (char *host, char *port)
{
    if (statisticsManager)
    {
        return statisticsManager -> rtcpBind (host, port);
    }
    else
    {
        return -1;
    }
}

bool
RTPSession_t::rtcpUnbind (bindId_t bId)
{
    if (statisticsManager)
    {
        return statisticsManager -> rtcpUnbind (bId);
    }
    else
    {
        return false;
    }
}

const char *
RTPSession_t::rtcpShowBindings (void)
{
    if (statisticsManager)
    {
        return statisticsManager -> rtcpShowBindings ();
    }
    else
    {
        return NULL;
    }
}

int
RTPSession_t::getMaxPacketSize (u32 ssrc, u16 extLength)
{
    RTPSender_t *sender = senderDictionary->lookUp (ssrc);
    if (sender == NULL)
    {
        return -1;
    }

    ql_t<u32> *csrcList = sender->getCSRCList();
    if (csrcList == NULL)
    {
        if (extLength == 0)
            return maxPacketSize-sizeof(RTPHeader_t) ;
        else
            return maxPacketSize-(sizeof(RTPHeader_t) +4+extLength);
    }
    else
    {
        if (extLength == 0)
            return maxPacketSize-(sizeof(RTPHeader_t)+sizeof(u32)*csrcList->len());
        else
            return maxPacketSize-(sizeof(RTPHeader_t)+(sizeof(u32)*csrcList->len())+4+extLength);
    }
}

double
RTPSession_t::sendData (u32 ssrc,
                        u8 *data,
                        int length,
                        u8 pt,
                        bool marker,
                        u16 seqNumber,
                        u32 timestamp,
                        u8 padding,
                        u16 extId,
                        u8 *extData,
                        u16 extLength)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL) {
        debugMsg(dbg_App_Normal, "sendData", "Sender(%d) not created", ssrc);
        return -1 ;
    }

    sender -> updateParameters (length, seqNumber, timestamp);
    RTPHeader_t *rtpHeader = (RTPHeader_t *)rtpBuffer;
    rtpHeader -> version = RTP_VERSION;
    rtpHeader -> padding = padding;

    if (extData == NULL) {
        rtpHeader -> extension = 0;
    } else {
        rtpHeader -> extension = 1;
    }

    rtpHeader -> marker = marker;
    rtpHeader -> payloadtype = pt;
    rtpHeader -> seqnum = htons (seqNumber);
    rtpHeader -> timestamp = htonl (timestamp);
    rtpHeader -> ssrc = htonl (sender -> getSSRC());
    int rtpHeaderLength;
    Sendertype type = sender -> getType();

    if (type != MIXER) {
        rtpHeader -> cc = 0;
        rtpHeaderLength = sizeof(RTPHeader_t);
    } else {
        debugMsg(dbg_App_Verbose, "sendData", "Sending data mixed");
        u32 *aux = (u32 *) (rtpBuffer + sizeof (RTPHeader_t));
        ql_t<u32> *csrcList = sender -> getCSRCList ();
        rtpHeader -> cc = csrcList -> len();
        for (ql_t<u32>::iterator_t i = csrcList -> begin();
             i != csrcList -> end();
             i++)
        {
            u32 csrc = static_cast<u32> (i);
            *aux = htonl (csrc);
            aux++;
            debugMsg(dbg_App_Verbose, "sendData", "Mixed source: %u", csrc);
        }
        rtpHeaderLength = sizeof(RTPHeader_t) + ((rtpHeader->cc) * sizeof(u32));
    }

    if (rtpHeader -> extension)
    {
        ExtensionHeader_t *extHeader = (ExtensionHeader_t*)(rtpBuffer + rtpHeaderLength);
        extHeader -> id = htons (extId);
        extHeader -> length = htons (extLength);
        rtpHeaderLength += sizeof (ExtensionHeader_t);
        u8 *aux = (u8*)(rtpBuffer + rtpHeaderLength);
        memcpy (aux, extData, extLength);
        rtpHeaderLength += extLength;
    }

    memcpy (rtpBuffer + rtpHeaderLength, data, length);
    sender -> setSentData (true);

    const vector_t<bindDescriptor_t*> *dest = sender->getBindings();
    for (int i= 0; i < dest->size(); i++)
    {
        bindDescriptor_t *bd = dest->elementAt(i);

        rtpSocket->writeTo((*(bd->getInetAddr())),
                           rtpBuffer,
                           rtpHeaderLength+length
                          );
     }

     return 0;
}

void
RTPSession_t::setSDESInfo (u32 srcId, int ident, const char *info, const char *prefix)
{
    debugMsg(dbg_App_Normal,
             "setSDESInfo",
              "Setting SDES info for SSRC %d",
              srcId
            );
    RTPSender_t *sender = senderDictionary -> lookUp (srcId);
    if (sender == NULL)
    {
        debugMsg(dbg_App_Normal, "setSDESInfo", "Sender not created", srcId);
        return;
    }

    if (ident != TYPE_SDES_PRIV)
    {
        sender->setSDESInfo (info, ident);
    }
    else
    {
        sender->setSDESInfo (prefix, info, ident);
    }

}

const char *
RTPSession_t::showSDESInfo (u32 ssrc)
{
    return flowsReceived -> getSDESInfo (ssrc);
}

const char *
RTPSession_t::showReceivers (u32 ssrc)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL) {
        return NULL;
    }

    return sender -> showReceivers();
}

const char *
RTPSession_t::getPacketsLost (u32 ssrc, u32 receiver)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL) {
        return NULL;
    }

    return sender -> getPacketsLost(receiver);
}

const char *
RTPSession_t::getPacketsLost (u32 ssrc)
{
    return flowsReceived -> getPacketsLost(ssrc);
}

const char *
RTPSession_t::getFractionLost (u32 ssrc, u32 receiver)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL) {
        return NULL;
    }

    return sender -> getFractionLost(receiver);
}

const char *
RTPSession_t::getRoundTripTime (u32 ssrc, u32 receiver)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL) {
        return NULL;
    }

    return sender -> getRoundTripTime (receiver);
}

const char *
RTPSession_t::getJitter (u32 ssrc, u32 receiver)
{
    RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
    if (sender == NULL) {
        return NULL;
    }

    return sender -> getJitter (receiver);
}

const char *
RTPSession_t::getJitter (u32 ssrc)
{
    return flowsReceived -> getJitter (ssrc);
}

void
RTPSession_t::computeStatistics (double sessionBandwidth, double rtcpFraction)
{
    if (statisticsManager)
    {
        statisticsManager -> setBandwidth (sessionBandwidth);
        statisticsManager -> setRtcpFragment (rtcpFraction);
        statisticsManager -> rtcpInterval();
        return;
    }

    statisticsManager = new RTPStatisticsManager_t (rtcpSocket,
                                                    flowsReceived,
                                                    senderDictionary,
                                                    sessionBandwidth,
                                                    rtcpFraction,
                                                    maxPacketSize
                                                   );

    (*theScheduler)<< statisticsManager;
    statisticsManager -> rtcpInterval();
}

void
RTPSession_t::setMaxPacketSize (int max)
{
    maxPacketSize = max;
    if (statisticsManager)
    {
        statisticsManager -> setMaxPacketSize (maxPacketSize);
        statisticsManager -> rtcpInterval();
    }
}

void
RTPSession_t::receivePacket (RTPPacket_t* packet)
{
    bool status = flowsReceived->processRTPPacket (packet, timestampUnit);

    if (status || deliverAny )
    {
        debugMsg(dbg_Lib_Verbose,
                 "receivePacket",
                 "Passing packet from SSRC %u to application layer",
                 packet->getSSRC()
                );

        receivedData (packet);
    }
}

void
RTPSession_t::receiveReport (RTCPReport_t *report)
{
    debugMsg(dbg_App_Verbose, "receiveReport", "Receiving RTCPReport_t");
    if (report -> senderInfo != NULL)
    {
        flowsReceived->processSRInfo (report->ssrcHeader->ssrc,
                                      report -> senderInfo,
                                      timestampUnit
                                     );
    }

    ql_t<ReportBlock_t *>::iterator_t i;

    for (i= report -> rrList -> begin();
         i != report -> rrList -> end();
         i++
        )
    {
        ReportBlock_t *aux = static_cast<ReportBlock_t *> (i);
        u32 ssrc = aux -> ssrc;
        RTPSender_t *sender = senderDictionary -> lookUp (ssrc);
        if (sender != NULL)
        {
                 bool newRR = sender -> processReportBlock (report -> ssrcHeader -> ssrc,aux);
                 if (newRR)
                 {
                     debugMsg(dbg_App_Verbose,
                              "receiveReport",
                              "New receiver sending Receiver Reports"
                             );
                 }
        }
    }

    delete report -> rtcpHeader;
    delete report -> ssrcHeader;
    delete report -> senderInfo;

    for (i = report -> rrList -> begin();
         i != report -> rrList -> end();
         i++
        )
    {
        ReportBlock_t *aux = static_cast<ReportBlock_t *> (i);
        delete aux;
    }

    delete report -> rrList;
}

void
RTPSession_t::receiveSDES (RTCPSDES_t *sdes)
{
    debugMsg(dbg_App_Verbose, "receiveSDES", "Reciving RTCPSDES_t");
    flowsReceived->processSDESInfo (sdes->ssrcHeader->ssrc, sdes->sdesList);

    delete sdes -> ssrcHeader;

    for (ql_t<SDESItem_t *>::iterator_t i = sdes -> sdesList -> begin();
         i != sdes -> sdesList -> end();
         i++
        )
    {
        SDESItem_t *aux = static_cast<SDESItem_t*> (i);
        free (aux);
    }

    delete sdes -> sdesList;
}

void
RTPSession_t::receiveBYE (RTCPBYE_t *bye)
{
    debugMsg(dbg_App_Verbose, "receiveBYE", "receiving RTCPBYE_t");

    for (ql_t<u32>::iterator_t i = bye -> csrcList -> begin();
         i != bye -> csrcList -> end();
         i++)
    {
        u32 src = static_cast<u32> (i);
        ql_t<u32> *senderList = senderDictionary -> getKeys();
        for (ql_t<u32>::iterator_t j = senderList -> begin();
             j != senderList -> end();
             j++)
        {
            u32 senderSSRC = static_cast<u32> (j);
            RTPSender_t *sender = senderDictionary -> lookUp (senderSSRC);
            Sendertype type = sender -> getType();
            switch (type)
            {
                case TRANSLATOR:
                    {
                        if (senderSSRC == src)
                        {
                            ql_t<u32> *list = new ql_t<u32>;
                            list -> insert (src);
                            if (statisticsManager)
                                statisticsManager->
                                    sendTranslatorRtcpPackets(sender,
                                                              false,
                                                              true,
                                                              list,
                                                              bye -> byeReason,
                                                              strlen(bye -> byeReason)
                                                             );
                            delete list;
                        }
                        break;
                    }
                case MIXER:
                    {
                        ql_t<u32> *cList = sender -> getCSRCList();

                        for (ql_t<u32>::iterator_t i = cList -> begin();
                             i != cList -> end();
                             i++)
                        {
                            u32 aux = static_cast<u32> (i);
                            if (aux == src)
                            {
                                ql_t<u32> *list = new ql_t<u32>;
                                list -> insert (aux);
                                if (statisticsManager)
                                    statisticsManager->
                                        sendMixerRtcpPackets (sender,
                                                              false,
                                                              true,
                                                              list,
                                                              bye -> byeReason,
                                                              strlen(bye -> byeReason)
                                                             );

                                delete list;
                            }
                        }
                        break;
                    }
                case SENDER:
                    {
                        break;
                    }
            }
         }

         delete senderList;
         eliminateReceiver (src, bye -> byeReason);
    }
}

void
RTPSession_t::timeOut (ql_t<u32> *timeOutList)
{
    debugMsg(dbg_App_Verbose, "timeOut", "timeOut");

    /*char reason [256] = "Timeout";
    for (ql_t<u32>::iterator_t i = maux -> timeOutList -> begin();
         i != maux -> timeOutList -> end();
         i++)
    {
        u32 aux = static_cast<u32> (i);
        eliminateReceiver (aux, reason);
    }*/

    delete timeOutList;
}


void
RTPSession_t::createCNAME (char *CNAMEBuffer)
{
    char bufhost[256];
    char bufnumber[256];
    char bufname[256];
    int status;

    status = gethostname (bufhost, 256);
    if (status != 0)
        strcpy (bufhost, "unknown-host");

    bufhost[255] = 0;
    sprintf (bufnumber, "%c@", usernumber);

    getLoginName (bufname, 256);
    strcpy (CNAMEBuffer, bufname);
    strcat (CNAMEBuffer, bufnumber);
    strcat (CNAMEBuffer, bufhost);
    usernumber += 1;
}

void
RTPSession_t::getLoginName(char *buf, int buflength)
{
#if defined(__BUILD_FOR_LINUX)

    uid_t uid;
    struct passwd *pwdent;
    uid = geteuid();
    pwdent = getpwuid (uid);
    if (pwdent == NULL)
    {
        sprintf(buf, "uid_%d", uid);
        return;
    }

    strncpy (buf, pwdent -> pw_name, buflength - 1);
    buf [buflength - 1] = 0;

#elif defined(WIN32)

    char hostname[100];
    char dest[100];
    memset(hostname,0,100);
    memset(dest,0,100);
    gethostname(hostname,100);
    getmyip(dest);
    strcat(hostname,"@");
    strcat(hostname,dest);
    strcpy(buf,hostname);

#else
#error "Please, provide implementation for your O.S."
#endif
}


