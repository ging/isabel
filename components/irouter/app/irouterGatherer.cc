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
// $Id: irouterGatherer.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "transApp.hh"
#include "irouterGatherer.hh"

irouterStatsGatherer_ref theStatsGatherer= NULL;


irouterStatsGatherer_t::irouterStatsGatherer_t(const char *statsPort)
: isAccountingFlag(false)
{
    char name[256];
    memset(name, 0, 256);
    gethostname(name, 256);

    localStats= false;
    globalStats= false;
    count = SYNC_FREQUENCY;

    inetAddr_t gathererAddr1("localhost", NULL, serviceAddr_t::DGRAM);
    inetAddr_t gathererAddr2("localhost", statsPort, serviceAddr_t::DGRAM);

    statsSocket = new dgramSocket_t(gathererAddr1, gathererAddr2);

    NOTIFY("irouterStatsGatherer_t::irouterStatsGatherer_t: "
           " Sending statistics to statsPort=%s\n",
           statsPort
          );
}

irouterStatsGatherer_t:: ~irouterStatsGatherer_t(void)
{
    debugMsg(dbg_App_Paranoic,
             "irouterStatsGatherer_t",
             "Destroying irouterStatsGatherer_t\n"
            );
}

void
irouterStatsGatherer_t::doStats(bool doIt)
{
    isAccountingFlag= doIt;

    set_period(doIt ? STATISTICS_PERIOD_SENT : 0);
}

void
irouterStatsGatherer_t::createPkt(string &addr,
                                  flowId_e flow,
                                  u32 ssrc,
                                  statsType_e t,
                                  stats_t *iStats
                                 )
{
    assert(iStats != NULL && "El diccionario no funciona!!\n");

    iStats->computeStats();

    if (iStats->getElapsed() > 1800000)
    {
        // stats resetting every 30 minutes
        NOTIFY("irouterStatsGatherer_t::createPkt: resetting stats\n");
        iStats->reset();
    }

    writePkt(addr, flow, ssrc, t, iStats);
}

void
irouterStatsGatherer_t::writePkt(string &addr,
                                 flowId_e flow,
                                 u32 ssrc,
                                 statsType_e t,
                                 stats_t *iStats
                                )
{
    u8 *buffer;
    u8 *addrBuffer;

    switch (t)
    {
    case SENT:
        {
        stats_t *stats = &(*iStats);
        irouterSendStats_t *iSendStats= static_cast<irouterSendStats_t*>(stats);
        const char *dest = addr.c_str();
        int addrLength = strlen(dest);
        addrBuffer=(u8*)malloc(addrLength+1);
        addrBuffer[0]=(u8)addrLength;
        memcpy(addrBuffer+1, dest, addrLength);

        int bufLen = 8 + 14 + 20 + addrLength + 1;
        buffer = (u8*)malloc(bufLen);

        u8 *aux = buffer;
        memset(aux, 0, bufLen); // cleaning

        u8 pktType = 1;
        ssrc = htonl(ssrc);
        memcpy(aux, &pktType, 1);   aux += 1;
        memcpy(aux, "IRT",    3);   aux += 3;
        memcpy(aux, &ssrc,    4);   aux += 4;

        u32 type = htonl(4); //irouter table index = "SEND"
        const char *flowStr= getFlowStr(flow);
        memcpy(aux, &type,    4);   aux += 4;

        int copyLen= strlen(flowStr);
        copyLen= copyLen > 6 ? 6 : copyLen; // 6 as much
        memcpy(aux, flowStr, copyLen); aux += 6;

        u32 dataPktN = htonl((u32)(iSendStats->getDataPktN()));
        u32 dataBw   = htonl((u32)(100*iSendStats->getDataBandwidth()));
        u32 fecPktN  = htonl((u32)(iSendStats->getFecPktN()));
        u32 fecBw    = htonl((u32)(100*iSendStats->getFecBandwidth()));
        u32 enqu     = htonl((u32)iSendStats->getEnqueued());
        u32 disc     = htonl((u32)iSendStats->getDiscarded());
        memcpy(aux, &dataPktN, 4);   aux += 4;
        memcpy(aux, &dataBw,   4);   aux += 4;
        memcpy(aux, &fecPktN,  4);   aux += 4;
        memcpy(aux, &fecBw,    4);   aux += 4;
        memcpy(aux, &enqu,     4);   aux += 4;
        memcpy(aux, &disc,     4);   aux += 4;

        memcpy(aux, addrBuffer, addrLength+1);

        statsSocket->write(buffer, bufLen);
        free(buffer);
        free(addrBuffer);

        break;
        }
    case RECEIVED:
        {
        stats_t *stats = &(*iStats);
        irouterRecvStats_t *iRecvStats= static_cast<irouterRecvStats_t*>(stats);
        const char *source = addr.c_str();
        int addrLength = strlen(source);
        addrBuffer=(u8*)malloc(addrLength+1);
        addrBuffer[0]=(u8)addrLength;
        memcpy(addrBuffer+1, source, addrLength);

        int bufLen= 8 + 14 + 32 + addrLength + 1;
        buffer = (u8*)malloc(bufLen);

        u8 *aux = buffer;
        memset(aux, 0, bufLen);

        u8 pktType = 1;
        ssrc = htonl(ssrc);
        memcpy(aux, &pktType, 1);   aux += 1;
        memcpy(aux, "IRT",    3);   aux += 3;
        memcpy(aux, &ssrc,    4);   aux += 4;

        u32 type = htonl(2); //irouter table index = "RECV"
        const char *flowStr= getFlowStr(flow);
        memcpy(aux, &type,       4);   aux += 4;

        int copyLen= strlen(flowStr);
        copyLen= copyLen > 6 ? 6 : copyLen; // 6 as much
        memcpy(aux, flowStr, copyLen);   aux += 6;

        u32 dataPktN = htonl((u32)(iRecvStats->getDataPktN()));
        u32 dataBw   = htonl((u32)(100*iRecvStats->getDataBandwidth()));
        u32 fecPktN  = htonl((u32)(iRecvStats->getFecPktN()));
        u32 fecBw    = htonl((u32)(100*iRecvStats->getFecBandwidth()));
        u32 recovered= htonl((u32)(iRecvStats->getRecovered()));
        u32 lost     = htonl((u32)(iRecvStats->getLost()));
        u32 dis      = htonl((u32)(iRecvStats->getDisorder()));
        u32 dup      = htonl((u32)(iRecvStats->getDuplicated()));

        memcpy(aux, &dataPktN,   4);   aux += 4;
        memcpy(aux, &dataBw,     4);   aux += 4;
        memcpy(aux, &fecPktN,    4);   aux += 4;
        memcpy(aux, &fecBw,      4);   aux += 4;
        memcpy(aux, &recovered,  4);   aux += 4;
        memcpy(aux, &lost,       4);   aux += 4;
        memcpy(aux, &dis,        4);   aux += 4;
        memcpy(aux, &dup,        4);   aux += 4;

        memcpy(aux, addrBuffer, addrLength+1);

        statsSocket->write(buffer, bufLen);
        free(buffer);
        free(addrBuffer);

        break;
        }
    default:
        NOTIFY("irouterStatsGatherer_t::writePkt: Unknown statistic type!\n");
    }
}

void
irouterStatsGatherer_t::accountSentData(string &addrStr,
                                        flowId_e flow,
                                        u32 ssrc,
                                        u16 size
                                       )
{
    debugMsg(dbg_App_Paranoic, "accountSentPkt", "Pkt enviado ");

    sourceInfo_t info(addrStr, flow, ssrc);
    irouterSendStats_t *iStats = statsByFlowIdAndDestination.lookUp(info);
    if (iStats == NULL)
    {
        iStats = new irouterSendStats_t;
        statsByFlowIdAndDestination.insert(info, iStats);
    }
    iStats->addSentPkt(size, 0);
    debugMsg(dbg_App_Paranoic, "accountSentPkt", "Pkt enviado fin account");
}

void
irouterStatsGatherer_t::accountRecvData(string &addrStr,
                                        flowId_e flow,
                                        u32 ssrc,
                                        u32 timestamp,
                                        u32 seqnum,
                                        u16 size,
                                        u32 time
                                       )
{
    debugMsg(dbg_App_Paranoic, "accountRecvData", "Pkt recibido ");

    sourceInfo_t dest(addrStr, flow, ssrc);
    irouterRecvStats_t *iStats = statsByFlowIdAndSource.lookUp(dest);
    if (iStats == NULL)
    {
        iStats = new irouterRecvStats_t;
        statsByFlowIdAndSource.insert(dest, iStats);
    }
    iStats->addRecvPkt(timestamp, seqnum, size, time);
}

void
irouterStatsGatherer_t::accountSentParity(string &addrStr,
                                          flowId_e flow,
                                          u32 ssrc,
                                          u16 size
                                         )
{
    sourceInfo_t info(addrStr, flow, ssrc);
    irouterSendStats_t *iStats = statsByFlowIdAndDestination.lookUp(info);
    if (iStats == NULL)
    {
        iStats = new irouterSendStats_t;
        statsByFlowIdAndDestination.insert(info, iStats);
    }
    iStats->addSentParity(size);
}

void
irouterStatsGatherer_t::accountRecvParity(string &addrStr,
                                          flowId_e flow,
                                          u32 ssrc,
                                          u16 size
                                         )
{
    sourceInfo_t info(addrStr, flow, ssrc);
    irouterRecvStats_t *iStats = statsByFlowIdAndSource.lookUp(info);
    if (iStats == NULL)
    {
        iStats = new irouterRecvStats_t;
        statsByFlowIdAndSource.insert(info, iStats);
    }
    iStats->addRecvParity(size);
}

void
irouterStatsGatherer_t::accountRecpData(string &addrStr,
                                        flowId_e flow,
                                        u32 ssrc,
                                        u32 timestamp,
                                        u32 seqnum,
                                        u16 size
                                       )
{
    debugMsg(dbg_App_Paranoic, "accountRecpPkt", "Pkt recovered ");

    sourceInfo_t dest(addrStr, flow, ssrc);
    irouterRecvStats_t *iStats = statsByFlowIdAndSource.lookUp(dest);
    if (iStats == NULL)
    {
        iStats = new irouterRecvStats_t;
        statsByFlowIdAndSource.insert(dest, iStats);
    }
    iStats->addRecoveredPkt();
    //iStats->addRecvPkt(timestamp, seqnum, size, 0);
}

void
irouterStatsGatherer_t::sendRecvStats(void)
{
    ql_t <sourceInfo_t> *keys= statsByFlowIdAndSource.getKeys();
    for (ql_t<sourceInfo_t>::iterator_t i = keys->begin();
         i != keys->end();
         i++
        )
    {
        sourceInfo_t info = static_cast<sourceInfo_t>(i);
        flowId_e flow = info.getFlow();
        string addrStr = info.getAddr();
        u32 ssrc = info.getSSRC();

        stats_t *iStats = statsByFlowIdAndSource.lookUp(info);

        createPkt(addrStr, flow, ssrc, RECEIVED, iStats);

        if (iStats->isDead())
        {
            statsByFlowIdAndSource.remove(info);
        }
    }
    delete keys;
}

void
irouterStatsGatherer_t::sendSentStats(void)
{
    ql_t <sourceInfo_t> *keys= statsByFlowIdAndDestination.getKeys();

    for (ql_t<sourceInfo_t>::iterator_t i = keys->begin();
         i != keys->end();
         i++
        )
    {
        sourceInfo_t info = static_cast<sourceInfo_t>(i);
        flowId_e flow = info.getFlow();
        u32 ssrc = info.getSSRC();
        string addrStr = info.getAddr();
        stats_t *iStats = statsByFlowIdAndDestination.lookUp(info);

        createPkt(addrStr, flow, ssrc, SENT, iStats);

        if (iStats->isDead())
        {
            statsByFlowIdAndDestination.remove(info);
        }
    }

    delete keys;
}


void
irouterStatsGatherer_t::heartBeat(void)
{
    debugMsg(dbg_App_Paranoic, "heartBeat", "Enviando estadisticas");

    transApp_t *app = static_cast<transApp_t *>(get_owner());
    globalStats = app->getGlobalStats();
    localStats  = app->getLocalStats();

    sendRecvStats();
    sendSentStats();
}

void
irouterStatsGatherer_t::getLinksStats(char *retVal)
{
    char buf[4096];

    ql_t <sourceInfo_t> *keys= statsByFlowIdAndSource.getKeys();
    ql_t <irouterRecvStats_t*> *values= statsByFlowIdAndSource.getValues();

    retVal[0]= '\0';
    strcat(retVal,"{");

    ql_t<sourceInfo_t>::iterator_t i = keys->begin();
    ql_t<irouterRecvStats_t*>::iterator_t j = values->begin();
    for (  ;
         i != keys->end() && j != values->end();
         i++, j++
        )
    {
        sourceInfo_t info = i;
        irouterRecvStats_t* stats = j;

        string addrStr = info.getAddr();
        u32 ssrc = info.getSSRC();
        flowId_e flow = info.getFlow();
        int totalPkts= stats->getDataPktN();
        int lostPkts= stats->getLost();
        int recoveredPkts= stats->getRecovered();
        long elapsed= stats->getElapsed();

        sprintf(buf,
                "{%s,%u,%s,%d,%d,%d,%ld},",
                addrStr.c_str(),
                ssrc,
                getFlowStr(flow),
                totalPkts,
                lostPkts,
                recoveredPkts,
                elapsed
               );
        strcat(retVal, buf);
    }
    delete keys;
    delete values;

    int len= strlen(retVal);
    if (len > 1)
        retVal[len-1]= '\0'; // removes last ','

    strcat(retVal,"}\n");
}

