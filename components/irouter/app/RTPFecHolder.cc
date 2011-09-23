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
// $Id: RTPFecHolder.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/icfTime.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>
#include <rtp/RTPFec.hh>

#include "random32.hh"
#include "RTPFecHolder.hh"
#include "irouterGatherer.hh"

#define RTP_SQ_MOD (1<<16)

RTPFecHolder_t::RTPFecHolder_t(u8 nfecPT)
: __sendChannelsDict(64),
  __recvChannelsDict(64),
  fecPT(nfecPT),
  k(0)
{
    NOTIFY("RTP Fec Holder created with PT=%d\n", fecPT);
}

RTPFecHolder_t::~RTPFecHolder_t(void)
{
    ql_t<RTPFecSender_t*> *listSenders= __sendChannelsDict.getValues();
    while (listSenders->len() > 0)
    {
        RTPFecSender_t *RTPFecSender= listSenders->head();
        listSenders->behead();
        delete RTPFecSender;
    }
    delete listSenders;

    ql_t<RTPFecReceiver_t*> *listReceivers= __recvChannelsDict.getValues();
    while (listReceivers->len() > 0)
    {
        RTPFecReceiver_t *RTPFecReceiver= listReceivers->head();
        listReceivers->behead();
        delete RTPFecReceiver;
    }
    delete listReceivers;
}

RTPPacket_t *
RTPFecHolder_t::sendPacket(RTPPacket_t *pkt)
{
    // si k = 0, no hago FEC
    if ( ! k)
    {
        return NULL;
    }

    RTPFecSender_t *RTPFecSender = __sendChannelsDict.lookUp(pkt->getSSRC());

    // if RTPFec doesn't exists
    if (RTPFecSender == NULL)
    {
        RTPFecSender = new RTPFecSender_t(fecPT);

        NOTIFY("RTPFecHolder_t::sendPacket: "
               "new RTPFec sender created for %d, with PT=%d\n",
               pkt->getSSRC(),
               fecPT
              );
        RTPFecSender->setK(k);

        __sendChannelsDict.insert(pkt->getSSRC(), RTPFecSender);
    }

    return RTPFecSender->sendPacket(pkt);
}

RTPPacket_t *
RTPFecHolder_t::recvPacket(RTPPacket_t *pkt)
{
    RTPFecReceiver_t *RTPFecReceiver= __recvChannelsDict.lookUp(pkt->getSSRC());

    if (pkt->getPayloadType() == fecPT && RTPFecReceiver == NULL)
    {
        // si es un paquete de proteccion creo el canal RTPFec

        RTPFecReceiver_t *RTPFecReceiver = new RTPFecReceiver_t(fecPT);

        NOTIFY("RTPFecHolder_t::New RTPFec receiver created for %d\n",
               pkt->getSSRC()
              );

        __recvChannelsDict.insert(pkt->getSSRC(), RTPFecReceiver);
    }

    if (RTPFecReceiver != NULL)
    {
        return RTPFecReceiver->recvPacket(pkt);
    }

    return NULL;
}

void
RTPFecHolder_t::setFecParams(const char *fecType, int new_n, int new_k)
{
    // to disable RTPFEC use fecType = None
    if ( ! strcmp(fecType, "None"))
    {
        k= 0;
        NOTIFY("RTPFecHolder_t::setFecPT: FEC Disabled for PT=%d\n", fecPT);
        return;
    }

    // k must be [0,24]
    if (new_k < 0)
    {
        return;
    }
    else
    {
        if (new_k > 24)
        {
            new_k= 24;
        }

        NOTIFY("RTPFecHolder_t::setFecPT: k=%d (MAX 24) FEC PT=%d\n", new_k, fecPT);

        // refresh k
        this->k= new_k;

        // refresh every RTPFec channel (only senders)

        ql_t<u32> *list=__sendChannelsDict.getKeys();
        ql_t<u32>::iterator_t i;

        for (i = list->begin(); i != list->end(); i++)
        {
            RTPFecSender_t *RTPFecSender =
                __sendChannelsDict.lookUp(static_cast<u32>(i));

            RTPFecSender->setK(new_k);
            // actually we only support parity, so we only
            // set K, and not N, for future upgrades this
            // method shold be changed --> setParams(N,K)
        }
        delete list;
    }
}

void
RTPFecHolder_t::delRTPFec(u32 ssrc)
{
    __sendChannelsDict.remove(ssrc);
    NOTIFY("RTPFecHolder_t::RTPFec: FEC removed for ssrc=%d\n", ssrc);
}


void
RTPFecHolder_t::getFecParams(int &n, int &k)
{
    k= this->k;
    n= k == 0 ? 0 : k+1; // only even parity implemented
}


