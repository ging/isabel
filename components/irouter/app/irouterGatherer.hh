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
// $Id: irouterGatherer.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __irouter_gatherer_hh__
#define __irouter_gatherer_hh__

#include <string>

using namespace std;

#include <icf2/icfTime.hh>

#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <icf2/dictionary.hh>
#include <icf2/sockIO.hh>
#include <icf2/icfMessages.hh>

#include "flow.hh"
#include "irouterStats.hh"

#define SYNC_FREQUENCY 10
#define STATISTICS_PERIOD_SENT 1000000

class sourceInfo_t
{
private:
    flowId_e  flow;
    string    addr;
    u32       ssrc;

public:
    sourceInfo_t(void)
    : flow((flowId_e)0), addr(), ssrc(0)
    {}

    sourceInfo_t(string &naddr, flowId_e nflow, u32 nssrc)
    : flow(nflow), addr(naddr), ssrc(nssrc)
    {}

    sourceInfo_t(const sourceInfo_t &other)
    {
        flow= other.flow;
        addr= other.addr;
        ssrc= other.ssrc;
    }

    int operator==(const sourceInfo_t &other) const
    {
        return (flow == other.flow) &&
               (addr.compare(other.addr) == 0 ) &&
               (ssrc == other.ssrc);
    }

    flowId_e getFlow(void) const { return flow; }
    string   getAddr(void) const { return addr; }
    u32      getSSRC(void) const { return ssrc; }
};

template <class V> class statsByFlowIdAndAddrDict_t
: public dictionary_t<sourceInfo_t, V>
{
};


class irouterStatsGatherer_t: public simpleTask_t
{
private:
    bool isAccountingFlag;
    bool localStats;
    bool globalStats;
    int  count;

    statsByFlowIdAndAddrDict_t <irouterRecvStats_t*> statsByFlowIdAndSource;
    statsByFlowIdAndAddrDict_t <irouterSendStats_t*> statsByFlowIdAndDestination;

    io_ref statsSocket;

    void createPkt(string &addr,
                   flowId_e flow,
                   u32 ssrc,
                   statsType_e t,
                   stats_t *iStats
                  );

    void sendSentStats(void);
    void sendRecvStats(void);

    void writePkt(string &addr,
                  flowId_e flow,
                  u32 ssrc,
                  statsType_e t,
                  stats_t *iStats
                 );

protected:
    virtual void heartBeat(void);

public:
    irouterStatsGatherer_t(const char *statsPort);

    virtual ~irouterStatsGatherer_t(void);

    void doStats(bool doIt);
    bool isAccounting(void) { return isAccountingFlag; }

    void getStats(int &totalPkts, int &lostPkts, int &recoveredPkts);

    void accountSentData(string &addrStr,
                         flowId_e flow,
                         u32 ssrc,
                         u16 size
                        );

    void accountRecvData(string &addrStr,
                         flowId_e flow,
                         u32 ssrc,
                         u32 timestamp,
                         u32 seqnum,
                         u16 size,
                         u32 time
                        );

    void accountSentParity(string &addrStr,
                           flowId_e flow,
                           u32 ssrc,
                           u16 size
                          );

    void accountRecvParity(string &addrStr,
                           flowId_e flow,
                           u32 ssrc,
                           u16 size
                          );

    void accountRecpData(string &addrStr,
                         flowId_e flow,
                         u32 ssrc,
                         u32 timestamp,
                         u32 seqnum,
                         u16 size
                        );

    void getLinksStats(char *retVal);

    virtual const char *className(void) const
    {
        return "irouterStatsGatherer_t";
    }

    friend class smartReference_t<irouterStatsGatherer_t>;
};

typedef smartReference_t<irouterStatsGatherer_t> irouterStatsGatherer_ref;

extern irouterStatsGatherer_ref theStatsGatherer;

#endif

