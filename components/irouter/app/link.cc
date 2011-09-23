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
// $Id: link.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/dictionary.hh>

#include "output.hh"
#include "transApp.hh"
#include "link.hh"

#ifdef __BUILD_FOR_LINUX
#include <netdb.h>
#include <arpa/inet.h>
#endif

// link_t -- a network link as seen by end user

link_t::link_t(transApp_t *app,
               linkClass_e lc,
               bool        echoBool,
               int         bandwidth,
               bool        typeFlag
              )
: myApp(app),
  lclass(lc),
  type(typeFlag),
  shouldEcho(echoBool),
  byTargetId(256),
  byPayloadType(256)
{
}


link_t::~link_t(void)
{
    debugMsg(dbg_App_Paranoic, "~link", "No hay link!\n");
    ql_t<int> *toDelete= byTargetId.getKeys();

    while (toDelete->len())
    {
        delTarget(toDelete->head());
        toDelete->behead();
    }

    delete toDelete;
}


void
link_t::enqueue(sharedPkt_t * &pkt)
{
    debugMsg(dbg_App_Verbose,
             "enqueue",
             "sending packet to scheduler flow=%d",
             pkt->flowId
            );

#if 0
    if (pkt->type == audioId)
    {
        if (myApp->audioMixer.isValid())
        {
            RTPPacket_ref rtp =
                new RTPPacket_t(pkt->sharedBuf->data, pkt->sharedBuf->len);

            myApp->depacketizer->pkt(new pktMsg_t(rtp, pkt->pktInLink));

            return;
        }
    }
#endif

    copyPktToTargets(pkt);
}


void
link_t::copyPktToTargets(sharedPkt_t * &pkt)
{
    if (lclass == inputOnly || lclass == noWay)
    {
        debugMsg(dbg_App_Normal,
                 "copyPktToTargets",
                 "discarding pkts for inputOnly||noway link\n"
                );
        return;
    }

    targetList_t *tl= byPayloadType.lookUp(pkt->flowId);

    if ( ! tl)
    {
        NOTIFY("copyPktToTargets: Target list not found\n");
        return;
    }

    if (tl->len() == 0)
    {
        debugMsg(dbg_App_Normal, "copyPktToTargets", "Empty targetList\n");
        NOTIFY("copyPktToTargets: Empty targetList\n");
    }
    else
    {
        for (targetList_t::iterator_t i = tl->begin(); i != tl->end(); i++)
        {
            target_t *target = static_cast<target_t *>(i);

            target->deliver(pkt);

            debugMsg(dbg_App_Verbose,
                     "copyPktToTargets",
                     "enqueueing packet toAddr =%s flowId=%d",
                     target->tgtAddr->getStrValue(),
                     pkt->flowId
                    );
        }
    }
}


int
link_t::addTarget(flowId_e flowId,
                  const char *addr,
                  int port,
                  int n,
                  int k,
                  i8 mcastTTL
                 )
{
    targetList_t *tl= byPayloadType.lookUp(flowId);

    if ( ! tl)
    {
        tl= new targetList_t;
        byPayloadType.insert(flowId, tl);
    }

    inetAddr_t inetAddr(addr, "666", SOCK_DGRAM);
    if (type == NORMAL)
    {
        target_t *tgt=
            myApp->targetMgr->newTarget(this, flowId, addr, port, n, k, mcastTTL);

        byTargetId.insert(tgt->tgtId, tgt);
        *tl<< tgt;
        return tgt->tgtId;
    }

    return 0;
}


void
link_t::delTarget(int id)
{
    debugMsg(dbg_App_Paranoic, "delTarget", "DelTarget from Link id=%d\n", id);

    target_t *target= byTargetId.lookUp(id);

    if ( ! target) return;

    targetList_t aux;
    targetList_t *tl= byPayloadType.lookUp(target->tgtSock->flowId);

    while (tl->len())
    {
        if (tl->head()->tgtId != id)
        {
            aux.insert(tl->head());
        }
        tl->behead();
    }
    tl->insert(aux);

    byTargetId.remove(id);

    myApp->targetMgr->delTarget(target);
}


target_t *
link_t::getTarget(flowId_e flowId, const char *addr)
{
    targetList_t *tl= byPayloadType.lookUp(flowId);

    if ( ! tl)
    {
        debugMsg(dbg_App_Normal, "getTarget", "Target list not found\n");
        return NULL;
    }

    if (tl->len() == 0)
    {
        debugMsg(dbg_App_Normal, "getTarget", "Empty targetList\n");
        return NULL;
    }
    else
    {
        for (targetList_t::iterator_t i = tl->begin(); i!= tl->end(); i++)
        {
            target_t *target = static_cast<target_t *>(i);
            if ( ! strcmp(addr, target->tgtAddr->getStrValue()))
            {
                return target;
            }
        }
    }

    return NULL;
}


target_t *
link_t::getMcastTarget(flowId_e flowId)
{
    targetList_t *tl= byPayloadType.lookUp(flowId);

    if ( ! tl)
    {
        debugMsg(dbg_App_Normal, "getTarget", "Target list not found\n");
        return NULL;
    }

    if (tl->len() == 0)
    {
        debugMsg(dbg_App_Normal, "getTarget", "Empty targetList\n");
        return NULL;
    }
    else
    {
        for (targetList_t::iterator_t i = tl->begin(); i!= tl->end(); i++)
        {
            target_t *target = static_cast<target_t *>(i);
            if (flowId == target->tgtSock->flowId)
            {
                return target;
            }
        }
    }

    return NULL;
}


targetList_t*
link_t::getTargets(void)
{
    return byTargetId.getValues();
}


void
link_t::protectFlow(const char *addr,
                    flowId_e flowId,
                    const char *fecType,
                    int n,
                    int k
                   )
{
    targetList_t *tl= byPayloadType.lookUp(flowId);

    if ( ! tl)
    {
        debugMsg(dbg_App_Normal, "protectFlow", "Target list not found\n");
        return;
    }

    if (tl->len() == 0)
    {
        debugMsg(dbg_App_Normal, "protectFlow", "Empty targetList\n");
    }
    else
    {
        for (targetList_t::iterator_t i = tl->begin(); i != tl->end(); i++)
        {
            target_t *target = static_cast<target_t *>(i);

            if ( ! strcmp(addr, target->tgtAddr->getStrValue()))
            {
                target->protectFlow(fecType, n, k);
            }
        }
    }
}


bool
link_t::videoTranscode(u32 SSRC, char *dest, u32 BW, u32 lineFmt)
{
    // buscamos los targets de video
    targetList_t *tl= byPayloadType.lookUp(videoId);
    inetAddr_t destin((const char*)dest,NULL,SOCK_DGRAM);

    if ( ! tl)
    {

        return false;
    }



    if (tl->len() > 0)
    {
        for (targetList_t::iterator_t i = tl->begin(); i != tl->end(); i++)
        {
            target_t *target = static_cast<target_t *>(i);
	    
            if ( ! strcmp(destin.getStrValue(), target->tgtAddr->getStrValue()))
            {
	        NOTIFY("link::videoTranscode\n");
                target->videoTranscode(SSRC, BW, lineFmt);
            }
        }
        return true;
    }
    return false;
}


int
link_t::joinSSM(char *source)
{
    ql_t<targetList_t *> *list = byPayloadType.getValues();

    for (ql_t<targetList_t*>::iterator_t j = list->begin();
         j != list->end();
         j++
        )
    {
        targetList_t *tl = static_cast<targetList_t*>(j);
        for (ql_t<target_t*>::iterator_t i = tl->begin(); i != tl->end(); i++)
        {
            target_t *target = static_cast<target_t*>(i);
            target->joinSSM(source);
        }
    }

    delete list;

    return 0;
}


int
link_t::leaveSSM(char *source)
{
    ql_t<targetList_t *> *list = byPayloadType.getValues();

    for (ql_t<targetList_t*>::iterator_t j = list->begin();
         j != list->end();
         j++
        )
    {
        targetList_t *tl = static_cast<targetList_t*>(j);
        for (ql_t<target_t*>::iterator_t i = tl->begin(); i != tl->end(); i++)
        {
            target_t *target = static_cast<target_t*>(i);
            target->leaveSSM(source);
        }
    }

    delete list;

    return 0;
}


inetAddr_t *
link_t::getAddr(int targetId)
{
    target_t *tl= byTargetId.lookUp(targetId);

    if (tl)
    {
       return tl->tgtAddr;
    }

    return NULL;
}


//
// linkBinder_t stuff
//
dictionary_t<string, link_t *>  linkBinder_t::linkDict(MAX_FLOW);

linkBinder_t::linkBinder_t(transApp_t *app)
{
    myApp= app;
}


linkBinder_t::~linkBinder_t(void)
{
    debugMsg(dbg_App_Paranoic, "~linkBinder_t", "Destroying linkBinder_t\n");
}


link_t *
linkBinder_t::newLink(const char *nam,
                      link_t::linkClass_e c,
                      bool echoMode,
                      int bw,
                      bool type
                     )
{
    link_t *theLink= linkDict.lookUp(nam);

    if (theLink)
    {  // exists!!
        return theLink;
    }

    NOTIFY("linkBinder_t::newLink: LinkName=[%s]\n", nam);

    theLink= new link_t(myApp, c, echoMode, bw, type);
    myApp->audioMixer2->newLink(theLink);
    linkDict.insert(nam, theLink);

    return theLink;
}


bool
linkBinder_t::deleteLink(const char *name)
{
    link_t *theLink= linkDict.lookUp(name);    
    
    if ( ! theLink)
    {
        return false;
    }

    myApp->audioMixer2->delLink(theLink);
    linkDict.remove(name);
    delete theLink;

    return true;
}


link_t *
linkBinder_t::lookUp(const char *name)
{
    return linkDict.lookUp(name);
}


