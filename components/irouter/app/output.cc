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
// $Id: output.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/ql.hh>
#include <icf2/sockIO.hh>
#include <icf2/stdTask.hh>
#include <icf2/sched.hh>

#include "link.hh"
#include "transApp.hh"
#include "output.hh"

u8 *sendMatrix = NULL;

//
// tgtSock_t -- a target socket
//

//
// Target sockets are managed by socketPool_t (see below)
// A tgtSock_t is just a dgramSocket_t with a reference
// count and identifier (to be used by socketPool_t)
//
tgtSock_t::tgtSock_t(int _id, inetAddr_t &a)
: dgramSocket_t(a), id(_id)
{
  sentBytes= recvBytes= 0;
  flowId= noFlowId;
}

tgtSock_t::~tgtSock_t(void)
{
}


//
// socketPool_t -- the actual packet sender
//

//
// The socketPool_t implements the handling of sockets.
// It allows the sharing of a given socket between different
// targets.
//
// A socket must be shared because the same local port
// can be used to connect to different remote targets.
// If this happens demultiplexing on receipt is required.
//
// The demux logic is as follows:
//
//     + if pkt.dstAddr == mcast lookup target by mcast addr
//     + else lookup target by pkt.srcAddr
//
//     + if no target found... discard
//     + else dispatch packet
//
// The demultiplexing logic is not handled here but at
// targetMgr_t (see below)
//
socketPool_t::socketPool_t(void)
: MAX_SOCKETS(128),
  byId(MAX_SOCKETS),
  byLocalPort(MAX_SOCKETS),
  sockCount(0)
{
}

socketPool_t::~socketPool_t(void)
{
    debugMsg(dbg_App_Paranoic, "~socketPool_t", "Destroying socketPool_t\n");
};


bool
socketPool_t::lookUpSock(const int port) const
{
    assert(port);

    tgtSock_ref s= byLocalPort.lookUp(port);
    if (s.isValid())
    {
        return true;
    }
    else
    {
        return false;
    }
}


tgtSock_ref
socketPool_t::getSock(const char *lAddr, const int port)
{
    assert(port);

    tgtSock_ref s= byLocalPort.lookUp(port);

    //
    // not already found
    //
    if ( ! s.isValid())
    {
        char portStr[16];
        memset(portStr, 0, 16);
        sprintf(portStr, "%d", port);
        inetAddr_t locAddr(lAddr, portStr, serviceAddr_t::DGRAM);

        sockCount++;

        s= new tgtSock_t(sockCount, locAddr);

        byId       .insert(s->id, s);
        byLocalPort.insert(port , s);
    }

    return s;
}


//
// target_t -- a given network target
//

//
// target_t objects are handled by the targetMgr_t (see below)
//
// A target is characterized by a:
//
//     + tgtSock
//     + tgtAddr
//
target_t::target_t(socketPool_t *_sp,
                   int id,
                   link_t *l,
                   flowId_e flowId,
                   const char *tgt,
                   const int port,
                   int n,
                   int k,
                   i8 mcastTTL,
                   bool SSMFlag
                  )
{
    tgtId= id;
    link = l;

    sp= _sp;
    tgtSock= sp->getSock(NULL, port);

    char portStr[16];
    memset(portStr, 0, 16);
    sprintf(portStr, "%d", port);
    tgtAddr= new inetAddr_t(tgt, portStr, serviceAddr_t::DGRAM);

    debugMsg(dbg_App_Paranoic,
             "target_t",
             "OJO!! Almacenando addr=[%s] =%s:%d",
             tgt,
             tgtAddr->getStrValue(),
             tgtAddr->getPort()
            );

    if (tgtAddr->mcastFlag && !SSMFlag)
    {
        assert(mcastTTL && "Error:: mcastTTL==0!!!");
        tgtSock->joinGroup(*tgtAddr, mcastTTL);
    }

    tgtSock->flowId= flowId;

    RTPFecHolder= new RTPFecHolder_t(fecById->lookUp(flowId));

    // check if it's a valid flow and if set, protect it.

    if (flowId == videoId || flowId == audioId || flowId == shDispId)
    {
        if (k > 0)
        {
            protectFlow("Parity",n,k);
        }
    }

    debugMsg(dbg_App_Verbose,
             "target_t",
             "id=%i flow=%d tgt=%s port=%d mcast=%d handle=%d",
             id,
             flowId,
             tgt,
             port,
             tgtAddr->mcastFlag,
             tgtSock->sysHandle()
            );
    //
    // this is done, targetMgr_t is supposed to handle demultiplexing
    //
}

target_t::~target_t(void)
{
    debugMsg(dbg_App_Verbose, "~target_t", "Destroyed");

    link->delTarget(tgtId);

    delete RTPFecHolder;
}


int
target_t::getPort(void)
{
    return tgtAddr->getPort();

}

void
target_t::setPort(int port)
{
    inetAddr_t *oldAddr = tgtAddr;
    char portStr[20];
    sprintf(portStr,"%d",port);
    tgtAddr= new inetAddr_t(oldAddr->getStrValue(),
                            portStr,
                            serviceAddr_t::DGRAM
                           );

    delete oldAddr;
}

int
target_t::joinSSM(char *source)
{
    if (!tgtAddr->mcastFlag)
    {
        NOTIFY("target_t::joinSSM: "
               "ERROR: trying to join non-mcast addr group %s\n",
               tgtAddr->getStrValue()
              );
        return -1;
    }

    inetAddr_t src = inetAddr_t(source, NULL,serviceAddr_t::DGRAM);
    
    return tgtSock->joinSSM(src, *tgtAddr);
}

int
target_t::leaveSSM(char *source)
{
    if (!tgtAddr->mcastFlag)
    {
        NOTIFY("target_t::leaveSSM: "
               "ERROR: trying to leave non-mcast addr group %s\n",
               tgtAddr->getStrValue()
              );
        return -1;
    }

    inetAddr_t src = inetAddr_t(source, NULL,serviceAddr_t::DGRAM);

    return tgtSock->leaveSSM(src, *tgtAddr);
}


void
target_t::videoTranscode(u32 SSRC, u32 BW, u32 lineFmt)
{
   videoTranscodeInfo_t videoTranscodeInfo(SSRC, BW, lineFmt);
   videoTranscoder_t *videoTranscoder = NULL;
   ql_t<videoTranscodeInfo_t> *videoTranscodeInfoList = transcoderDict.getKeys();

   if (videoTranscodeInfoList->len() > 0)
   {
      for(videoTranscodeInfo_t videoInfo = videoTranscodeInfoList->head();
         videoTranscodeInfoList->len()>0;
         videoTranscodeInfoList->behead())
        {
            if (videoInfo.SSRC == videoTranscodeInfo.SSRC) 
            {   
               //hemos encontrado un transcodificador para el mismo SSRC a este target
	       if (videoInfo.BW != videoTranscodeInfo.BW ||
                   videoInfo.lineFmt != videoTranscodeInfo.lineFmt
                  )
               {
	          //Si cambia el BW o el lineFmt quitamos ese videoTranscoder
                  transcoderDict.remove(videoInfo); 
	          break;
	       }
               // Si no ha cambiado el SSRC o el lineFmt devolvemos el que teniamos
               videoTranscoder = transcoderDict.lookUp(videoTranscodeInfo);
               break;
            }
        }
   }
   delete videoTranscodeInfoList;

   //si no existe el transcodificador adecuado lo creamos ... Si BW = 0 No hacemos Nada
   if (!videoTranscoder && videoTranscodeInfo.BW!=0)
   {
      videoTranscoder = new videoTranscoder_t(videoTranscodeInfo);
   }
      transcoderDict.insert(videoTranscodeInfo,videoTranscoder);
   return;
}

// aqui comprobamos si existe un transcodificador valido
// si no lo creamos y nos registramos.

videoTranscoder_t *
target_t::checkVideoTranscoder(RTPPacket_t *rtp)
{
    // recorremos la lista de transcodificadores que tenemos en el dict
    // y vemos si hay que transcodificar el flujo

    u8 Codec = rtp->getPayloadType();

    if (   Codec != MPEG4_PT
        && Codec != MPEG1_PT
        && Codec != MPEG2_PT
        && Codec != H263_PT
       )
    {
        return NULL;
    }

    ql_t<videoTranscodeInfo_t> *auxInfo = transcoderDict.getKeys();
    u32 SSRC = rtp->getSSRC();
    videoTranscoder_t *videoTranscoder = NULL;

    for (ql_t<videoTranscodeInfo_t>::iterator_t j = auxInfo->begin();
         j != auxInfo->end();
         j = j++
        )
    {
        videoTranscodeInfo_t videoInfo = static_cast<videoTranscodeInfo_t>(j);
        if (SSRC == videoInfo.SSRC)
        {
            // si tenemos un transcodificador para el mismo SSRC
            videoTranscoder = transcoderDict.lookUp(videoInfo);

            break;
        }
    }

    delete auxInfo;

    return videoTranscoder;
}


void
target_t::deliver(sharedPkt_t *pkt)
{
    debugMsg(dbg_App_Verbose,
             "deliver",
             "putting bytes in the network "
             "tgtAddr=%x tgtAddr->getStrValue()=%s\n",
             tgtAddr,
             tgtAddr->getStrValue()
            );

    // mandamos el paquete

    RTPPacket_t *rtp =
        new RTPPacket_t(pkt->sharedBuf->data, pkt->sharedBuf->len);

    // comprobamos si tenemos el transcodificador montado
    // para el paquete rtp con el SSRC específico
    videoTranscoder_t *videoTranscoder = NULL;
    videoTranscoder = checkVideoTranscoder(rtp);
    RTPPacketList_ref videoPacketList= new ql_t <RTPPacket_t *>;

    if (videoTranscoder)
    {       
       videoPacketList = videoTranscoder->processData(rtp, this);
       delete rtp;
    }
    else
    {
       videoPacketList->insert(rtp);
    }

    // packets are in the list
    while (videoPacketList->len() > 0)
    {
        RTPPacket_t *pkt1 = videoPacketList->head();
        videoPacketList->behead();

        RTPPacket_t *fecPkt = NULL;

        tgtSock->writeTo(*tgtAddr, pkt1->getPacket(), pkt1->getTotalLength());

        measures->savePkt("OUT",
                          pkt1->getSSRC(),
                          pkt1->getPayloadType(),
                          pkt1->getTotalLength(),
                          pkt1->getTimestamp(),
                          pkt1->getSequenceNumber()
                         );

        // vemos si es necesario mandar paquete FEC
        if (pkt1->getPayloadType() != ALIVE_PT)
        {
            fecPkt = RTPFecHolder->sendPacket(pkt1);
            if (theStatsGatherer->isAccounting())
            {
                string addrStr= string(tgtAddr->getStrValue());

                theStatsGatherer->accountSentData(addrStr,
                                                  tgtSock->flowId,
                                                  pkt1->getSSRC(),
                                                  pkt1->getTotalLength()
                                                 );
                if (fecPkt != NULL)
                {
                    theStatsGatherer->accountSentParity(addrStr,
                                                        tgtSock->flowId,
                                                        fecPkt->getSSRC(),
                                                        fecPkt->getTotalLength()
                                                       );
                }
            }
        }

       if (fecPkt)
       {
           tgtSock->writeTo(*tgtAddr,
                            fecPkt->getPacket(),
                            fecPkt->getTotalLength()
                           );

           measures->savePkt("OUT",
                            fecPkt->getSSRC(),
                            fecPkt->getPayloadType(),
                            fecPkt->getTotalLength(),
                            fecPkt->getTimestamp(),
                            fecPkt->getSequenceNumber()
                            );           
      }

      delete pkt1;
   }
}

void
target_t::protectFlow(const char *fecType, int n, int k)
{
    RTPFecHolder->setFecParams(fecType, n, k);
}

void
target_t::getFecParams(flowId_e &flowId, int &n, int &k)
{
    flowId= tgtSock->flowId;
    RTPFecHolder->getFecParams(n, k);
}

//
// targetMgr_t -- the target manager
//

//
// targetMgr_t handles a target_t list (creation/deletion of targets)
// and performs demultiplexing on packet receipt.
//
// The demux logic is as follows:
//
//     + if pkt.dstAddr == mcast lookup target by mcast addr
//     + else lookup target by pkt.srcAddr
//
//     + if no target found... discard
//     + else dispatch packet
//

targetMgr_t::targetMgr_t(transApp_t *app)
{
    myApp= app;
    sockPool= new socketPool_t;
    tgtCount= 0;
}

targetMgr_t::~targetMgr_t(void)
{
    debugMsg(dbg_App_Paranoic, "~targetMgr_t", "Destroying targetMgr_t\n");

    delete sockPool;

    debugMsg(dbg_App_Paranoic, "~targetMgr_t", "Destroyed targetMgr_t\n");
}

void
targetMgr_t::IOReady(io_ref &io)
{
    inetAddr_t addr;

    io_t *io_p = static_cast<io_t *>(io);
    tgtSock_t *sock= static_cast<tgtSock_t *>(io_p);

    debugMsg(dbg_App_Verbose,
             "IOReady",
             "got packet from network sock->flowId =%d handle=%d",
             sock->flowId,
             sock->sysHandle()
            );

    sharedPkt_t *pkt= new sharedPkt_t;

    gettimeofday(&pkt->time, NULL);

    pkt->sharedBuf->len=
        sock->recvFrom(addr, pkt->sharedBuf->data, MAX_PKT_SIZE);

    RTPHeader_t *header = (RTPHeader_t*)pkt->sharedBuf->data;

    if (header->payloadtype == ALIVE_PT)
    {
        delete pkt;
        return;
    }
    if (pkt->sharedBuf->len <= 0)
    {
        NOTIFY("targetMgr_t::IOReady::FD_ISSET but can't read\n");
        delete pkt;
        return;
    }

    debugMsg(dbg_App_Verbose,
             "IOReady",
             "Pkt len=%d MAX_PKT=%d",
             pkt->sharedBuf->len,
             MAX_PKT_SIZE
            );

    measures->savePkt("IN",
                      ntohl(header->ssrc),
                      header->payloadtype,
                      pkt->sharedBuf->len,
                      ntohl(header->timestamp),
                      ntohs(header->seqnum)
                     );

    //
    // mirar direccion y demultiplexar
    //

    debugMsg(dbg_App_Verbose,
             "IOReady",
             "pkt from addr=%s:%d, flow=%d",
             addr.getStrValue(),
             addr.getPort(),
             pkt->flowId
            );


    link_t *link= myApp->linkBinder->lookUp(addr.getStrValue());
    pkt->flowId= sock->flowId;

    if (myApp->irouterParam->getSSM())
    {
        // test if link is in the SSM source group
        stringArray_ref sources;
        sources= myApp->irouterParam->getSources();

        for (int i = 0; i < sources->size(); i++)
        {
             char *source = sources->elementAt(i);
             if (strstr(addr.getStrValue(), source))
             {
                 link = defaultMulticastLink;
                 break;
             }
        }
    }

    if (link)
    {
        debugMsg(dbg_App_Verbose, "IOReady", "pktInlink = link ");

        pkt->pktInLink= link;
    }
    else
    {
        debugMsg(dbg_App_Verbose,
                 "IOReady",
                 "pktInlink = defaultMulticastLink "
                );
        pkt->pktInLink= link= defaultMulticastLink;   // chapuza
    }

    if (link == NULL)
    {   // pa'quien es esto?
        NOTIFY("targetMgr_t::IOReady: I listen bells from %s!!\n",
               addr.getStrValue()
              );

        delete pkt;
        return;
    }

    // here we don`t make a copy of pkt mem data
    // data will be deleted at fec __buffer (resetRX())
    RTPPacket_t *rtp =
        new RTPPacket_t(pkt->sharedBuf->data, pkt->sharedBuf->len);

    debugMsg(dbg_App_Verbose,
             "IOReady",
             "received RTP packet:: SSRC=%d, SQ=%d, TS=%u, PT=%d, M=%d\n",
             rtp->getSSRC(),
             rtp->getSequenceNumber(),
             rtp->getTimestamp(),
             rtp->getPayloadType(),
             rtp->getMark()
            );

    RTPPacket_t *fecPkt= NULL;
    bool fec= false;

    target_t *target = NULL;

    if (link != defaultMulticastLink)
    {
        target= link->getTarget(pkt->flowId, addr.getStrValue());
    }
    else
    {
        target=link->getMcastTarget(pkt->flowId);
    }

    if (target->getPort() != addr.getPort())
    {
        NOTIFY("NAT DETECTED at %s::changing port from %d to %d\n",
               addr.getStrValue(),
               target->getPort(),
               addr.getPort()
              );
        target->setPort(addr.getPort());
    }

    if (rtp->getPayloadType() == fecById->lookUp(target->tgtSock->flowId))
    {
        fec = true;
    }

    fecPkt= target->RTPFecHolder->recvPacket(rtp);

    // if fecPkt we have recovered a pkt
    if (fecPkt)
    {
        sharedPkt_t *shfecPkt = new sharedPkt_t;

        memcpy(shfecPkt->sharedBuf->data,
               fecPkt->getPacket(),
               fecPkt->getTotalLength()
              );

        shfecPkt->sharedBuf->len= fecPkt->getTotalLength();
        shfecPkt->flowId= pkt->flowId;
        shfecPkt->pktInLink= pkt->pktInLink;
        myApp->centralSwitch.pktToClient(shfecPkt);
        if ((shfecPkt->flowId == audioId) && (myApp->audioMixer2->isActive()))
        {            
            myApp->centralSwitch.audioToMix(shfecPkt);
        }
        else
        {
            myApp->centralSwitch.networkData(shfecPkt);
        }
        delete shfecPkt;
    }

    // if fec = false the pkt received is not a fec pkt

    if (!fec)
    {
        myApp->centralSwitch.pktToClient(pkt);
        if ((pkt->flowId == audioId) && (myApp->audioMixer2->isActive()))
        {            
            myApp->centralSwitch.audioToMix(pkt);
        }
        else
        {
            myApp->centralSwitch.networkData(pkt);
        }
    }

    if (theStatsGatherer->isAccounting())
    {
        // si es paquete de FEC
        string addrStr= string(addr.getStrValue());

        if (fecPkt)     // Recovered data
        {
            theStatsGatherer->accountRecpData(addrStr,
                                              sock->flowId,
                                              rtp->getSSRC(),
                                              fecPkt->getTimestamp(),
                                              fecPkt->getSequenceNumber(),
                                              fecPkt->getTotalLength()
                                              );

        }

        if (fec)   // FEC RTP packet
        {
            theStatsGatherer->accountRecvParity(addrStr,
                                                sock->flowId,
                                                rtp->getSSRC(),
                                                rtp->getTotalLength()
                                               );
        }
        else       // common RTP packet
        {
            u32 tvusec = pkt->time.tv_sec * 1000000 + pkt->time.tv_usec;
            theStatsGatherer->accountRecvData(addrStr,
                                              sock->flowId,
                                              rtp->getSSRC(),
                                              rtp->getTimestamp(),
                                              rtp->getSequenceNumber(),
                                              rtp->getTotalLength(),
                                              tvusec
                                             );
        }
    }

    delete pkt;
    delete rtp;
}

target_t *
targetMgr_t::newTarget(link_t *l,
                       flowId_e flowId,
                       const char *addr,
                       const int port,
                       int n,
                       int k,
                       i8 mcastTTL
                      )
{
    bool addScheduler = sockPool->lookUpSock(port);

    target_t *tgt= new target_t(sockPool,
                                tgtCount,
                                l,
                                flowId,
                                addr,
                                port,
                                n,
                                k,
                                mcastTTL,
                                myApp->irouterParam->getSSM()
                               );

    if (addScheduler)
    {
        debugMsg(dbg_App_Paranoic,
                 "newTarget",
                 "Target tgtSock->sysHandle()=%d already in!! :)\n",
                 tgt->tgtSock->sysHandle()
                );
    }
    else
    {
        debugMsg(dbg_App_Paranoic,
                 "newTarget",
                 "Adding target tgtSock->sysHandle()=%d\n",
                 tgt->tgtSock->sysHandle()
                );
        io_ref io = static_cast<tgtSock_t *>(tgt->tgtSock);
        add_IO(io);
    }

    tgtCount++;

    return tgt;
}

void
targetMgr_t::delTarget(target_t *t)
{
    debugMsg(dbg_App_Paranoic, "delTarget", "Destroying target!!");
}

