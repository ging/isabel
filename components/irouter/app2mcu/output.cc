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
// $Id: output.cc 12068 2007-11-02 12:27:54Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2007. Agora Systems S.A.
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
tgtSock_t::tgtSock_t(int _id, inetAddr_t *a)
: dgramSocket_t(*a), id(_id)
{
  sentBytes= recvBytes= 0;
  pType= noFlowId;
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
        inetAddr_t *locAddr= new inetAddr_t(lAddr,
                                            portStr,
                                            serviceAddr_t::DGRAM
                                           );

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
                   int pt,
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
    pType= pt;

    sp= _sp;
    tgtSock= sp->getSock(NULL, port);

    char portStr[16];
    memset(portStr, 0, 16);
    sprintf(portStr, "%d", port);
    tgtAddr= new inetAddr_t(tgt, portStr, serviceAddr_t::DGRAM);

    debugMsg(dbg_App_Paranoic,
             "target_t",
             "OJO!! Almacenando addr=[%s] =%s:%d", tgt,
             tgtAddr->getStrValue(),
             tgtAddr->getPort()
            );

    if (tgtAddr->mcastFlag && !SSMFlag)
    {
        assert(mcastTTL && "Error:: mcastTTL==0!!!");
        tgtSock->joinGroup(*tgtAddr, mcastTTL);
    }

    tgtSock->pType= pType;

    //check if it's a valid flow and
    //if set, protect it.
    RTPFecHolder= new RTPFecHolder_t((flowId_e)pType);
    if (pType == videoId || pType == audioId || pType == shDispId)
    {
        if (k > 0)
        {
            protectFlow("Parity",n,k);
        }
    }

    debugMsg(dbg_App_Verbose,
             "target_t",
             "id=%i pt=%p tgt=%s port=%d mcast=%d handle=%d",
             id,
             pt,
             tgt,
             port,
             tgtAddr->mcastFlag,
             tgtSock->sysHandle()
            );
    //
    // this is done, targetMgr_t is supposed to handle
    // demultiplexing
    //
}

int
target_t::getPort(void)
{
    return tgtAddr->getPort();

}

void
target_t::setPort(int port)
{
    inetAddr_t * oldAddr = tgtAddr;
    char portStr[20];
    sprintf(portStr,"%d",port);
    tgtAddr= new inetAddr_t(oldAddr->getStrValue(),
                            portStr,
                            serviceAddr_t::DGRAM
                           );

    delete oldAddr;
}

int
target_t::joinSSM(char * source)
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
target_t::leaveSSM(char * source)
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
target_t::videoTranscode(u32 SSRC, u32 BW, u8 Codec)
{
    videoTranscodeInfo_t videoTranscodeInfo(SSRC,BW,Codec);
    // comprobamos si estamos registrados a algun
    // transcodificador con mismo SSRC y si es el correcto
    ql_t<videoTranscodeInfo_t> * auxInfo = transcoderDict.getKeys();
    for (ql_t<videoTranscodeInfo_t>::iterator_t j = auxInfo->begin() ;
         j!= auxInfo->end();
         j = j++
        )
    {
        videoTranscodeInfo_t videoInfo = static_cast<videoTranscodeInfo_t>(j);
        if (videoInfo != videoTranscodeInfo &&
            videoInfo.SSRC == videoTranscodeInfo.SSRC
           )
        {
            //si tenemos un transcodificador para el mismo SSRC hay que quitarlo
            //de nuestro diccionario
            videoTranscoder_t * videoTranscoder =
                transcoderDict.lookUp(videoInfo);

            videoTranscoder->unRegisterTgt(this);
            transcoderDict.remove(videoInfo);

        }
        else if (videoInfo == videoTranscodeInfo)
        {
            //si encontramos un transcodificador bueno
            //en nuestro diccionario nos registramos
            videoTranscoder_t * videoTranscoder =
                transcoderDict.lookUp(videoInfo);

            videoTranscoder->registerTgt(this);
            delete auxInfo;
            return;
        }
    }
    delete auxInfo;

    // si llegamos aqui es porque no estamos registrados en un
    // transcodificador valido
    // buscamos uno y si no existe lo creamos
    videoTranscoder_t * videoTranscoder = NULL;
    ql_t<videoTranscodeInfo_t> * videoTranscodeInfoList =
        videoTranscoderList->getKeys();

    if (videoTranscodeInfoList->len() > 0)
    {
        for (videoTranscodeInfo_t videoInfo = videoTranscodeInfoList->head();
             videoTranscodeInfoList->len()>0;
             videoTranscodeInfoList->behead()
            )
        {
            if (videoInfo.SSRC == videoTranscodeInfo.SSRC &&
                videoInfo.BW   == videoTranscodeInfo.BW   &&
                videoInfo.Codec== videoTranscodeInfo.Codec
               )
            {
                // hemos encontrado un transcodificador valido
                videoTranscoder =
                    videoTranscoderList->lookUp(videoTranscodeInfo);

                break;
            }
        }
    }

    delete videoTranscodeInfoList;

    // si no existe el transcodificador adecuado lo creamos
    if (!videoTranscoder)
    {
       videoTranscoder = new videoTranscoder_t(videoTranscodeInfo);
    }

    videoTranscoder->registerTgt(this);
    transcoderDict.insert(videoTranscodeInfo,videoTranscoder);
}

// aqui comprobamos si existe un transcodificador valido
// si no lo creamos y nos registramos.

videoTranscoder_t *
target_t::checkVideoTranscoder(RTPPacket_t * rtp)
{
    // recorremos la lista de transcodificadores que tenemos en el dict
    // y vemos si hay que transcodificar el flujo

    u8 Codec = rtp->getPayloadtype();

    if (Codec != __MPEG4 && Codec != __MPEG1 && Codec != __H263)
    {
        return NULL;
    }

    ql_t<videoTranscodeInfo_t> * auxInfo = transcoderDict.getKeys();
    u32 SSRC = rtp->getSSRC();
    videoTranscoder_t * videoTranscoder = NULL;

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

target_t::~target_t(void)
{
    debugMsg(dbg_App_Verbose, "~target_t", "Destroyed");

    // lotsa zings to do
    link->delTarget(tgtId);
    RTPFecHolder=NULL;
}


void
target_t::deliver(sharedPkt_t * pkt)
{
    debugMsg(dbg_App_Verbose,
             "deliver",
             "putting bytes in the network "
             "tgtAddr=%x tgtAddr->getStrValue()=%s\n",
             tgtAddr,
             tgtAddr->getStrValue()
            );

    // mandamos el paquete

    RTPPacket_t * rtp =
        new RTPPacket_t(pkt->sharedBuf->data, pkt->sharedBuf->len);

    // comprobamos si tenemos el transcodificador montado
    // para el paquete rtp con el SSRC específico
    videoTranscoder_t * videoTranscoder = checkVideoTranscoder(rtp);

    if (videoTranscoder)
    {
         RTPPacket_t * videoPacket = videoTranscoder->processData(rtp,this);
         // videoPacket existira si el transcodificador
         // esta funcionando (working = true), en caso contrario
         // devuelve NULL y trabajamos de manera normal
         if (videoTranscoder->isWorking())
         {
             delete rtp;
             if (videoPacket)
             {
                 // solo cambiamos el contenido del
                 // paquete rtp si se transcodifica
                 rtp = videoPacket;
             }
             else
             {
                 return;
             }
         }
    }

    RTPPacket_t * fecPkt = NULL;

    tgtSock->writeTo(*tgtAddr, rtp->getPacket(), rtp->getTotalLength());
#ifdef IROUTER_SAVE_MEASURES
    measures->SavePkt("OUT",
                      rtp->getSSRC(),
                      rtp->getPayloadtype(),
                      rtp->getTotalLength(),
                      rtp->getTimestamp(),
                      rtp->getSequenceNumber()
                     );
#endif

    //vemos si es necesario mandar paquete FEC
    if (rtp->getPayloadtype() != ALIVE_PT)
    {
        fecPkt =
            RTPFecHolder->sendPacket(rtp,
                                     new icfString_t(tgtAddr->getStrValue())
                                    );
    }
    else
    {
        delete rtp;
    }

    if (fecPkt)
    {
        tgtSock->writeTo(*tgtAddr,
                         fecPkt->getPacket(),
                         fecPkt->getTotalLength()
                        );
#ifdef IROUTER_SAVE_MEASURES
        measures->SavePkt("OUT",
                          fecPkt->getSSRC(),
                          fecPkt->getPayloadtype(),
                          fecPkt->getTotalLength(),
                          fecPkt->getTimestamp(),
                          fecPkt->getSequenceNumber()
                         );
#endif

        delete fecPkt;
   }
}

void
target_t::protectFlow(char *fecType, int n, int k)
{
    RTPFecHolder->setFecPT(fecType, n, k);
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
             "got packet from network sock->pType =%d handle=%d",
             sock->pType,
             sock->sysHandle()
            );

    sharedPkt_t * pkt= new sharedPkt_t;

    if ((sock->pType==audioId)&&(myApp->audioMixer.isValid()))
    {
        struct timeval time;
        gettimeofday(&time, NULL);
        pkt->time.tv_sec  = time.tv_sec;
        pkt->time.tv_usec = time.tv_usec;
    }

    pkt->sharedBuf->len=
        sock->recvFrom(addr, pkt->sharedBuf->data, MAX_PKT_SIZE);

    RTPHeader * header = (RTPHeader *)pkt->sharedBuf->data;

    if (header->payloadtype == ALIVE_PT)
    {
        delete pkt;
        return;
    }
    if (pkt->sharedBuf->len<=0)
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

#ifdef IROUTER_SAVE_MEASURES
    measures->SavePkt("IN",
                      ntohl(header->ssrc),
                      header->payloadtype,
                      pkt->sharedBuf->len,
                      ntohl(header->timestamp),
                      ntohs(header->seqnum)
                     );
#endif

    //
    // mirar direccion y demultiplexar
    //

    debugMsg(//(debugMask_e)0,
             dbg_App_Verbose,
             "IOReady",
             "pkt from addr=%s:%d, type=%d",
             addr.getStrValue(),
             addr.getPort(),
             pkt->type
            );


    link_t *link=myApp->linkBinder->lookUp(addr.getStrValue());
    pkt->type= static_cast<flowId_e>(sock->pType);

    if (myApp->irouterParam->getSSM())
    {
        // test if link is in the SSM source group
        charList_t sources;
        sources=myApp->irouterParam->getSources();

        for (unsigned i = 0; i < sources.len(); i++)
        {
             char * source = sources.nth(i);
             if (strstr(addr.getStrValue(),source))
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
    RTPPacket_t * rtp =
        new RTPPacket_t(pkt->sharedBuf->data,pkt->sharedBuf->len);

    debugMsg(dbg_App_Verbose,
             "IOReady",
             "output RTP packet:: SSRC=%d, Timestamp=%u, SQ=%d\n",
             rtp->getSSRC(),
             rtp->getTimestamp(),
             rtp->getSequenceNumber()
            );

    rtp->setTV(&pkt->time);


    RTPPacket_t * fecPkt= NULL;
    bool fec= false;

    target_t * target = NULL;

    if (link != defaultMulticastLink)
    {
        target= link->getTarget(pkt->type, addr.getStrValue());
        if (rtp->getPayloadtype() == fecById->lookUp(target->pType))
        {
            fec = true;
        }

        // en el caso de haber recuperado a el paquete y llegue luego
        u16 lastRcpSQ;
        if (target->RTPFecHolder->getLastSQ(rtp->getSSRC(),lastRcpSQ))
        {
            if (lastRcpSQ == rtp->getSequenceNumber())
            {
                delete rtp;
                delete pkt;
                return;
            }
        }

        fecPkt= target->RTPFecHolder->recvPacket(rtp, new icfString_t(addr.getStrValue()), fec);

    }
    else
    {

        target=link->getMcastTarget(pkt->type);
        if (rtp->getPayloadtype()==fecById->lookUp(target->pType))
        {
            fec = true;
        }

        // en el caso de haber recuperado a el paquete y llegue luego
        u16 lastRcpSQ;
        if (target->RTPFecHolder->getLastSQ(rtp->getSSRC(),lastRcpSQ))
        {
            if (lastRcpSQ == rtp->getSequenceNumber())
            {
                delete rtp;
                delete pkt;
                return;
            }
        }

        fecPkt= target->RTPFecHolder->recvPacket(rtp, new icfString_t(target->tgtAddr->getStrValue()), fec);

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

    // if fecPkt we have recovered a pkt
    if (fecPkt)
    {
        sharedPkt_t * shfecPkt = new sharedPkt_t;

        memcpy(shfecPkt->sharedBuf->data,
               fecPkt->getPacket(),
               fecPkt->getTotalLength()
              );

        shfecPkt->sharedBuf->len= fecPkt->getTotalLength();
        shfecPkt->type= pkt->type;
        shfecPkt->pktInLink= pkt->pktInLink;

        myApp->centralSwitch.networkData(shfecPkt);

        delete shfecPkt;
        delete fecPkt;
    }

    // if fec = false the pkt received is not a fec pkt

    if ( ! fec)
    {
        if ((pkt->type == audioId) && (myApp->audioMixer.isValid()))
        {
            myApp->centralSwitch.audioToMix(pkt);
        }
        else
        {
            myApp->centralSwitch.networkData(pkt);
        }
    }

    delete pkt;
}

target_t *
targetMgr_t::newTarget(link_t *l,
                       int pt,
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
                                pt,
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

