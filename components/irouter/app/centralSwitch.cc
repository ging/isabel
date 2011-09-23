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
// $Id: centralSwitch.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/item.hh>
#include <icf2/notify.hh>

#include "centralSwitch.hh"
#include "webirouterConfig.hh"
#include "pkt.hh"
#include "link.hh"
#include "transApp.hh"

#include <assert.h>

centralSwitch_t::centralSwitch_t(transApp_t *app)
: downstreamRoutingDict(256),
  upstreamRoutingDict(256)
{
    myApp= app;
}


centralSwitch_t::~centralSwitch_t(void)
{
    debugMsg(dbg_App_Paranoic, "~centralSwitch", "Destroying centralSwitch\n");
}

void
centralSwitch_t::clientData(sharedPkt_t * &pkt)
{
    debugMsg(dbg_App_Verbose,
             "clientData",
             "got packet from client module type= %d",
             pkt->flowId
            );

    downstreamRoutingInfo_t *dri= downstreamRoutingDict.lookUp(pkt->flowId);

    assert(dri);

    if (dri->links.len() == 0)
    {
        debugMsg(dbg_App_Verbose, "clientData", "Empty link list in DRI\n");
    }
    else
    {
        debugMsg(dbg_App_Verbose,
                 "clientData",
                 "link list length=%d\n",
                 dri->links.len()
                );

        for (linkList_t::iterator_t i = dri->links.begin();
             i != dri->links.end();
             i++
            )
        {
            link_t *destLink= static_cast<link_t *>(i);
            debugMsg(dbg_App_Verbose,
                     "clientData",
                     "Considering link %x",
                     destLink
                    );

            if ((destLink != pkt->pktInLink) || destLink->getShouldEcho())
            {
                debugMsg(dbg_App_Verbose,
                         "clientData",
                         "enqueueing packet type=%d",
                         pkt->flowId
                        );

                // Esta copia no es necesaria, se hace despues
                //sharedPkt_ref newP = new sharedPkt_t(*pkt);
                destLink->enqueue(pkt);
            }
            else
            {
                debugMsg(dbg_App_Verbose,
                         "clientData",
                         "skipping visited link"
                        );
            }
        }
    }
}


void
centralSwitch_t::pktToClient(sharedPkt_t * &pkt)
{
    debugMsg(dbg_App_Verbose, "pktToClient", "got packet for local client");

    upstreamRoutingInfo_t *uri= upstreamRoutingDict.lookUp(pkt->flowId);

    if ( ! uri)
    {
        debugMsg(dbg_App_Verbose, "pktToClient", "NO URI\n");
    }
    else if (uri->clients.len() == 0)
    {
        debugMsg(dbg_App_Verbose, "pktToClient", "Empty client list in URI\n");
    }
    else
    {
        for (clientModuleList_t::iterator_t i = uri->clients.begin();
             i != uri->clients.end();
             i++
            )
        {
            clientModule_t *clientModule = static_cast<clientModule_t *>(i);
            debugMsg(dbg_App_Verbose,
                     "pktToClient",
                     "upDelivering packet"
                    );

            clientModule->deliver(pkt->sharedBuf->data,
                                  pkt->sharedBuf->clientLen()
                                 );
        }
    }
}


void
centralSwitch_t::audioToMix(sharedPkt_t * &pkt)
{
    RTPPacket_t *rtp
        = new RTPPacket_t(pkt->sharedBuf->data, pkt->sharedBuf->len);

    //Audio para mezclar
    myApp->audioMixer2->deliverPkt(rtp, pkt->pktInLink);

    delete rtp;
}


void
centralSwitch_t::networkData(sharedPkt_t * &pkt)
{
    debugMsg(dbg_App_Verbose, "networkData", "got packet from targetMgr");
    ///
    /// patch info into packet
    ///
    flow_t *pktFlow= myApp->flowBinder.lookUp(pkt->flowId);

    if ( ! pktFlow)
    {
        NOTIFY("centralSwitch_t::networkData:: "
               "Unknown flow type (%d) in network packet\n",
               pkt->flowId
              );
        return;
    }

    clientData(pkt);
}

bool
centralSwitch_t::insertDri(int flowId, downstreamRoutingInfo_t *dri)
{
    if (downstreamRoutingDict.lookUp(flowId))
    {
        NOTIFY("centralSwitch::insertDri(%d):: already inserted\n", flowId);

        return true;
    }

    return downstreamRoutingDict.insert(flowId, dri);
}

bool
centralSwitch_t::removeDri(int flowId)
{
    downstreamRoutingInfo_t *dri= downstreamRoutingDict.lookUp(flowId);

    if ( ! dri)
    {
        return false;
    }

    downstreamRoutingDict.remove(flowId);

    delete dri;

    return true;
}

bool
centralSwitch_t::addLinkToDri(int flowId, link_t *l)
{
    downstreamRoutingInfo_t *dri= downstreamRoutingDict.lookUp(flowId);

    if ( ! dri)
    {
        return false;
    }

    bool found=false;
    for (ql_t<link_t *>::iterator_t i = dri->links.begin();
         i != dri->links.end();
         i++
        )
    {
        link_t *link = static_cast<link_t*>(i);
        if (link == l)
        {
            found=true;
            break;
        }
    }

    if ( ! found)
    {
        dri->links.insert(l);
    }
    else
    {
        NOTIFY("centralSwitch_t::addLinkToDri:: "
               "link was already in downstream(flowId=%d)\n",
               flowId
              );
    }

    return true;
}

bool
centralSwitch_t::removeLinkFromDri(int flowId, link_t *l)
{
    downstreamRoutingInfo_t *dri= downstreamRoutingDict.lookUp(flowId);

    if ( ! dri)
    {
        return false;
    }

    linkList_t aux;

    while(dri->links.len())
    {
        if (dri->links.head() != l)
        {
            aux.insert(dri->links.head());
        }
        dri->links.behead();
    }
    dri->links.insert(aux);

    return true;
}

bool
centralSwitch_t::insertUri(int flowId, upstreamRoutingInfo_t *uri)
{
    return upstreamRoutingDict.insert(flowId, uri);
}

bool
centralSwitch_t::removeUri(int flowId)
{
    upstreamRoutingInfo_t *uri= upstreamRoutingDict.lookUp(flowId);

    if ( ! uri)
    {
        return false;
    }

    upstreamRoutingDict.remove(flowId);
    delete uri;

    return true;
}

