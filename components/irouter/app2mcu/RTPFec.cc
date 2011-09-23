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
// $Id: RTPFec.cc 6204 2005-02-11 14:44:40Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/general.h>

#include "random32.hh"
#include "RTPFec.hh"
#include "irouterGatherer.hh"

#ifdef __BUILD_FOR_LINUX
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include <icf2/icfTime.hh>
#endif

#define RTP_SQ_MOD (1<<16)

//RTP Buffer Defaults
#define DEFAULT_FRAME_SIZE 200
#define MAX_SEQ_NUM 2^16
#define MAX_SEQ_DIFF 0x0fff

RTPContainer_t::RTPContainer_t(void)
{
    nData = 0;
    for (int j=0; j<=MAX_BUFF;j++)
    {
        frame[j] = NULL; 
    }
}

RTPContainer_t::~RTPContainer_t(void)
{
    free();
}

void
RTPContainer_t::free(void)
{
    for (int j=0; j<=MAX_BUFF;j++)
    {
        if (frame[j]!=NULL)
        {
            delete frame[j];
            frame[j] = NULL; 
        }
    }
    nData = 0;
}
    
void
RTPContainer_t::free(int SQ)
{
    for (int j=0; j<MAX_BUFF;j++)
    {
        if (frame[j]!=NULL)
        {
	    if (frame[j]->getSequenceNumber()==SQ)
	    {
                delete frame[j];
                frame[j] = NULL; 
	    }
        }
    }
    nData = 0;
}

bool 
RTPContainer_t::insert(RTPPacket_t * packet)
{
    int diff = 0;
    
    if (frame[0]!= NULL)
        diff = packet->getSequenceNumber()-frame[0]->getSequenceNumber();
	    
    // sequence cicle test
    if (abs(diff) > MAX_SEQ_DIFF)
    {
        if (diff>0)
            diff-=MAX_SEQ_NUM;
        else
            diff+=MAX_SEQ_NUM;
    }

    // duplication test
    for (int j = 0; j<MAX_BUFF;j++)
    { 
        if (frame[j]!=NULL)
        {
            if (frame[j]->getSequenceNumber() == packet->getSequenceNumber())
            {
                return true;
            }
        }
    }
	
    //if we receive a delayed packet
	
    if (diff < 0)
    {
        for (int j=MAX_BUFF+diff-1; j>=0; j--)
        {
            frame[j-diff] = frame[j];
            frame[j] = NULL;
        }
        frame[0] = packet;
        return true;
    }
    
    //if we receive a big diff
    //reset buffer
    if (diff >= MAX_BUFF)
    {
//      NOTIFY("Big jump\n");
        free();
        frame[0] = packet;
        return false;
    }

    //Save the frame
    frame[diff] = packet;
    return true;
}

RTPPacket_t *
RTPContainer_t::getData(void)
{
    RTPPacket_t * pkt = frame[nData];
    nData++;
    if (nData>MAX_BUFF)
    {
        free();
    }
    return pkt;
}
	
int 
RTPContainer_t::checkLost(ql_t<int> SQList,int &SQ)
{
    int nLost = 0;
    for (int j = 0;j<(int)SQList.len();j++)
    {
        int Seq = SQList.nth(j);
        bool found = false;
        for (int i = 0; i < MAX_BUFF; i++)
        {
            if (frame[i])  
            {
                if (frame[i]->getSequenceNumber() == Seq)
                {
                    found = true;
                    break;
                }
            }
        }
        if (!found)
        {
            nLost++;
            SQ = Seq;
        }
       
        if (nLost == 2)
            return -1; //can't recover
    }
    
    if (nLost)
        return nLost; //can recover
    else
        return 0; //no losses
}

unsigned long
RTPContainer_t::getTimestamp(void)
{
    for (int pos = 0;pos<MAX_BUFF;pos++)
    {
       if (frame[pos])
       {
           return frame[pos]->getTimestamp();
       }
    }
    return 0;
}

RTPFec_t::RTPFec_t(flowId_e flowId,u32 ssrc, u8 pt,int k)
{
    __ssrc=ssrc;
    __flowId = flowId;
    __seqNumber=(u16)random32();
    __lastRcpSQ=0;
    __pt=pt;
    __matrix=NULL; //matrix;
    __n=k+1;
    __k=k;
    __lastTS=0;
    __lastPT=0;
    __working = true;
   
    // get correct timeStamp Unit
    switch(flowId)
    {
        case videoId:
        {
            __timestampUnit = VIDEO_RTP_TSU;
            break;
        }
        case audioId:
        {
            __timestampUnit = AUDIO_RTP_TSU;
            break;
        }
        case shDispId:
        {
            __timestampUnit = SHDISP_RTP_TSU;
            break;
        }
        case pointerId:
        {
            __timestampUnit = POINTER_RTP_TSU;
            break;
        }
        default:
        {
            __timestampUnit = DEFAULT_RTP_TSU;
            break;
        }
    }

    resetRX();
    resetTX();
    NOTIFY("RTPFec_t::RTPFec_t: Creating RTPFec_t: "
           "SSRC=%d, n=%d, k=%d, PT=%d\n",
           __ssrc, __n, __k, __pt);
}

RTPFec_t::~RTPFec_t(void)
{
    resetRX();
    resetTX();
    if(__matrix) {
        free(__matrix);
    }
}

void
RTPFec_t::resetTX(void)
{
    while (__packetsList.len())
    {
        RTPPacket_t * rtp = __packetsList.head();
        delete rtp;
        __packetsList.behead();
    }
}

void 
RTPFec_t::resetRX(void)
{
    //NOTIFY("Clean RX buffer\n");
    __buffer.free();
}

RTPPacket_t *
RTPFec_t::fecPktToSend()
{
    if (!__k)
    {
        NOTIFY("RTPFec sender with k=0!\n");
        return NULL;
    }

    //Lista de los numeros de secuencia que metemos
    ql_t<u16> SQList;
   
    //mascara donde indicamos los SN que forman el FEC
    u16 SNBase =__packetsList.head()->getSequenceNumber();;
    u32 mask=0;
    
    RTPPacket_t * fecPkt=NULL;
    
    //los paquetes que protejo me lo indica el SQ 
    //determinaci� del paquete m� largo
    int maxLength=((__packetsList.head()->getCC()*sizeof(u32))+
                      __packetsList.head()->getTotalExtensionLength()+
                      __packetsList.head()->getDataLength());
    int  i=0;
    for (i=0; i<(int)__packetsList.len(); i++)
    {
        RTPPacket_t * pkt=__packetsList.nth(i);
        int diffSQ = pkt->getSequenceNumber()-SNBase;
 
        //buscamos numero de secuencia maximo dentro
        //del margen que nos permite la __k
        if (diffSQ<24 &&
            diffSQ>=0  &&
            (int)SQList.len()<__k)
        {
                mask = (mask|(0x0001<<diffSQ));
                SQList.insert(pkt->getSequenceNumber());
                u16 plength=((pkt->getCC()*sizeof(u32))+
                    pkt->getTotalExtensionLength()+
                    pkt->getDataLength());
                if (plength>maxLength)
                    maxLength=plength;
        }
    }
    
    /*Formaci� de un paquete RTP/FEC: RFC 2733*/

    u8 *buffer= new u8[12+12+maxLength];
    u8 *protection = new u8[maxLength+1];          
    protectionBlock_t protectionBlock;
    memset(&protectionBlock, 0, sizeof(protectionBlock));
    memset(protection,0,maxLength);
        
    //creaci� de bloques de proteccion
    for(i=0;i<(int)__packetsList.len(); i++)
    {
        
        RTPPacket_t * pkt=__packetsList.nth(i);
        
        bool found = false;
        for (int nn = 0;nn<(int)SQList.len();nn++)
        {
            if (SQList.nth(nn) == pkt->getSequenceNumber())
            found = true;
        }

        if (!found)
            continue;

        protectionBlock.version=0;
        protectionBlock.padding^=pkt->getPadding();
        protectionBlock.extension^=pkt->getExt();
        protectionBlock.cc^=pkt->getCC();
        protectionBlock.marker^=pkt->getMark();
        protectionBlock.payloadtype^=pkt->getPayloadtype();
        protectionBlock.timestamp^=htonl(pkt->getTimestamp());
        u16 lengthSum=((pkt->getCC()*sizeof(u32))+
                        pkt->getTotalExtensionLength()+
                        pkt->getDataLength());
        
        protectionBlock.lengthsSum^=htons(lengthSum);
               
        u8 * data = pkt->getData();
        int length = 0;
        while(length<pkt->getDataLength())
        {
            protection[length]^=data[length];
            length++;
        }

    }
     
    //Ahora eliminamos los paquetes que no 
    //volveremos a usar
    for(i =0;(unsigned)i<__packetsList.len(); i++)
    {
        RTPPacket_t * pkt=__packetsList.nth(i);
        if (pkt)
        {
            for(int j=0;j<(int)SQList.len();j++)
            {
                u16 SQ = SQList.nth(j);
                if (SQ==pkt->getSequenceNumber() ||
                    pkt->getSequenceNumber()<SNBase)
                {
                    __packetsList.behead(i);
                    delete pkt;
                    i--;
                    break;
                }
            }
        }
    }  

    //creaci� del paquete RTP: cabecera RTP del paquete
    RTPHeader *rtpHeader     =(RTPHeader*)buffer;
    rtpHeader->version       = RTP_VERSION;
    rtpHeader->padding       = protectionBlock.padding;
    rtpHeader->extension     = protectionBlock.extension;
    rtpHeader->cc            = protectionBlock.cc;
    rtpHeader->marker        = protectionBlock.marker;
    rtpHeader->payloadtype   = __pt;
    rtpHeader->seqnum        = htons(__seqNumber++);
    struct timeval __currentTime;
    gettimeofday(&__currentTime, NULL);
    u32 timestamp = (__currentTime.tv_sec*1000000) + __currentTime.tv_usec;
    rtpHeader->timestamp     = htonl((u32)((((double)timestamp)/(1000000.0))/(__timestampUnit)));
    rtpHeader->ssrc          = htonl(__ssrc);

    //creaci� del paquete RTP: cabecera FEC
    RTPFecHeader_t *rtpFecHeader=(RTPFecHeader_t*)(buffer+12);
    rtpFecHeader->SNBase     = htons(SNBase);
    rtpFecHeader->lengthRecv = protectionBlock.lengthsSum;
    rtpFecHeader->secondWord = 0; //Extension
    rtpFecHeader->secondWord = protectionBlock.payloadtype; //PT
    rtpFecHeader->secondWord = (rtpFecHeader->secondWord<<24); //Move 24 bits
    rtpFecHeader->secondWord = (rtpFecHeader->secondWord|mask);//Insert mask    
    rtpFecHeader->secondWord = htonl(rtpFecHeader->secondWord);    
    rtpFecHeader->TSRecovery = protectionBlock.timestamp;

    //creaci� del paquete RTP: datos del paquete
    memcpy(buffer+12+12, protection, maxLength);
    fecPkt=new RTPPacket_t(buffer, 12+12+maxLength);
    delete[] protection;
    delete[] buffer;
//  NOTIFY("Mando FEC SQ = %d\n",fecPkt->getSequenceNumber());
    return fecPkt; 
}

RTPPacket_t * 
RTPFec_t::fecPktToRecv(RTPPacket_t * pkt)
{
   
    RTPFecHeader_t *rtpFecHeader=(RTPFecHeader_t*)(pkt->getPacket()+12);
    u16 SNBase=ntohs(rtpFecHeader->SNBase);
    u32 mask = ntohl(rtpFecHeader->secondWord);
    u16 len = 0;
    mask = ((mask<<8)>>8);
    
    ql_t<int> SQList;

    //los nºs de seq pueden ir
    //desde SNBase a SNBase + 24    
    for(int i=0;i<24;i++)
    {
        bool bitMask = (mask)&(0x0001<<i);
        if (bitMask)
        {
            SQList.insert(SNBase + i);
            len++;
        }
    }

    /* NOTIFY("Busco Base = %d,FEC SQ = %d, len = %d\n",SNBase,pkt->getSequenceNumber(),len);*/
    //comprobamos si nos salimos
    if(len<=0 || len>24)
    {
        NOTIFY("RTPFec_t :: protection SQ = %d, first protected packet SQ = %d\n",
               pkt->getSequenceNumber(),
               SNBase);
        //Ahora eliminamos los paquetes 
        //que ya no vamos a necesitar    
        for(int k = 0;k<(int)SQList.len();k++)
        {
            int SQ = SQList.nth(k);
            __buffer.free(SQ);
        }   
        return NULL;
    }    

    int lostSQ = 0;

    //si se pierden mas de dos paquetes, 
    //nada que hacer
    int ret=0;
    if ((ret=__buffer.checkLost(SQList,lostSQ))<=0)
    {
        if (ret == -1)
            NOTIFY("RTPFec :: >1 pkts lost , can't recover!\n");
        //Ahora eliminamos los paquetes 
        //que ya no vamos a necesitar    
        for(int k = 0;k<(int)SQList.len();k++)
        {
            int SQ = SQList.nth(k);
            __buffer.free(SQ);
        }
        return NULL;
    }

    //eliminamos de la lista el SQ perdido
    for(int nn=0; nn<(int)SQList.len(); nn++)
    {
        if (SQList.nth(nn)==lostSQ)
        {
            SQList.behead(nn);
        }
    }

    //si hemos perdido un paquete
   
    NOTIFY("Pkt Lost detected, ssrc = %d, k=%d\n",
            __ssrc,len);
    
    int maxLength=pkt->getTotalLength()-12;
    u8 * buffer = (u8*)malloc(maxLength+12);
    u8 * protection;
    protection = buffer + sizeof(RTPHeader);
    protectionBlock_t protectionBlock;
    memset(buffer, 0, maxLength);
   //Creaci� del bloque de protecci� del fec
    protectionBlock.version=0;
    protectionBlock.padding=pkt->getPadding();
    protectionBlock.extension=pkt->getExt();
    protectionBlock.cc=pkt->getCC();
    protectionBlock.marker=pkt->getMark();
    u32 secondWord=ntohl(rtpFecHeader->secondWord);
    protectionBlock.payloadtype=(secondWord>>24);
    protectionBlock.timestamp=rtpFecHeader->TSRecovery;
    protectionBlock.lengthsSum=htons(rtpFecHeader->lengthRecv);
    
    //copiamos la parte de datos
    u8 * data = pkt->getData();
    memcpy(protection,data+12,maxLength);
   
    int n = 0; 
    //Creaci� de bloques de protecci� del resto de paquetes
    for (RTPPacket_t * auxpkt = __buffer.getData();
         (int)SQList.len()!=n; 
         auxpkt = __buffer.getData())
             {
                 if(!auxpkt) continue; //llegamos al perdido
                 bool found = false;
          
                 for (int nn = 0;nn<(int)SQList.len();nn++)
                 {
                     if (SQList.nth(nn) == auxpkt->getSequenceNumber())
                     {  
                         found = true;
                         n++; 
                     }
                 }

                 if (!found)
                     continue;

                 protectionBlock.version^=0;
                 protectionBlock.padding^=auxpkt->getPadding();
                 protectionBlock.extension^=auxpkt->getExt();
                 protectionBlock.cc^=auxpkt->getCC();
                 protectionBlock.marker^=auxpkt->getMark();
                 protectionBlock.payloadtype^=auxpkt->getPayloadtype();
                 protectionBlock.timestamp^=htonl(auxpkt->getTimestamp());
                 
                 u16 lengthSum=((auxpkt->getCC()*sizeof(u32))+
                         auxpkt->getTotalExtensionLength()+
                         auxpkt->getDataLength());
          
                 protectionBlock.lengthsSum^=lengthSum;
                 
                 data = auxpkt->getData();
                 for(int length = 0;length<auxpkt->getDataLength();length++)
                     protection[length]^=data[length];
               
             }

    //creaci� del paquete RTP: cabecera RTP del paquete
        
    RTPHeader *rtpHeader=(RTPHeader*)buffer;
    rtpHeader->version       = RTP_VERSION;
    rtpHeader->padding       = protectionBlock.padding;
    rtpHeader->extension     = protectionBlock.extension;
    rtpHeader->cc            = protectionBlock.cc;
    rtpHeader->marker        = protectionBlock.marker;
    rtpHeader->payloadtype   = protectionBlock.payloadtype;
    rtpHeader->seqnum        = htons(lostSQ);
    rtpHeader->timestamp     = protectionBlock.timestamp;
    rtpHeader->ssrc          = htonl(__ssrc);
        
    if (!rtpHeader->timestamp)
             rtpHeader->timestamp = ntohl(__buffer.getTimestamp());
	     
    u16 bufferLen = protectionBlock.lengthsSum + sizeof(RTPHeader);
    
    if (bufferLen<=sizeof(RTPHeader) || bufferLen>(maxLength+12))
        bufferLen = maxLength;

    RTPPacket_t * newPkt= new RTPPacket_t(buffer,bufferLen);
    free(buffer);

    //Ahora eliminamos los paquetes 
    //que ya no vamos a necesitar    
    for(int k = 0;k<(int)SQList.len();k++)
    {
        int SQ = SQList.nth(k);
        __buffer.free(SQ);
    }
    
    __lastRcpSQ = lostSQ;

    return newPkt;
}


RTPPacket_t *
RTPFec_t::sendPacket(RTPPacket_t * pkt, icfString_ref addr)
{

    if (theStatsGatherer->isAccounting())
    {
   
        theStatsGatherer->accountSentData(addr,
                                          __flowId,
                                          __ssrc,
                                          pkt->getTotalLength()
                                          );
    }
   
    RTPPacket_t * fecPkt=NULL;
    if (__k==0)
    {
        delete pkt;
        return NULL;
    }
    /*NOTIFY("Inserto TX SQ = %d, TS = %d , M = %d\n",
                pkt->getSequenceNumber(),
                pkt->getTimestamp(),
                pkt->getMark());*/

    __packetsList.insert(pkt);
    //si hay metidos k paquetes 
    //envio uno de paridad
    if(__packetsList.len()>=(unsigned)__k)
    {
        fecPkt=fecPktToSend();
     
        if (!fecPkt)
            return NULL;
        
        if (theStatsGatherer->isAccounting())
        {
            theStatsGatherer->accountSentParity(addr,
                                                __flowId,
                                                __ssrc,
                                                fecPkt->getTotalLength()
                                               );
        }
        
    }
    return fecPkt;
}

void 
RTPFec_t::setK(u8 k)
{
    if (k<24)
    {
        NOTIFY("RTPFec_t ssrc = %d :: k = %d -> k = %d\n",__ssrc,__k,k);
        __k = k;
    }
}
    

RTPPacket_t * 
RTPFec_t::recvPacket(RTPPacket_t * pkt, icfString_ref addr)
{
    RTPPacket_t * fecPkt=NULL;
    if(!__working)
    {
        if(pkt->getPayloadtype()==__pt)
        {
            NOTIFY("RTPFec :: active RX, SSRC = %d\n",__ssrc);
            __working = true;
        }
        delete pkt;
        return NULL;
    }

    if(pkt->getPayloadtype()!=__pt)
    {
        //a la cola
        __lastPT = pkt->getSequenceNumber();
       /*NOTIFY("Inserto RX SSRC = %d, SQ = %d, TS = %d , M = %d\n",
                pkt->getSSRC(),
                pkt->getSequenceNumber(),
                pkt->getTimestamp(),
                pkt->getMark());*/
        if (!__buffer.insert(pkt))
        {
            //deactivate RX
            NOTIFY("RTPFec :: disabled RX, SSRC = %d\n",__ssrc);
            resetRX();
            __working = false;
            return NULL;
        }
        return NULL;
        
    }else{
    
        __lastTS=pkt->getTimestamp();
        
        //procesamos el paquete
        fecPkt=fecPktToRecv(pkt);
        
        //guardamos ultimo pkt recuperado 
        //por si luego llega atrasado tirarlo
        if(fecPkt)
        {
            __lastRcpSQ=fecPkt->getSequenceNumber();
        }
        if (fecPkt)
        {
            NOTIFY("PKT RECOVERED!!\n"
                   "----------------------\n"
                   "SQ      = %d\n"
                   "TS      = %d\n"
                   "SSRC    = %d\n"
                   "DataLen = %d\n"
                   "M       = %d\n",
                   fecPkt->getSequenceNumber(),
                   fecPkt->getTimestamp(),
                   fecPkt->getSSRC(),
                   fecPkt->getDataLength(),
                   fecPkt->getMark());
        }
        
        if (theStatsGatherer->isAccounting())
        {
           
            //si es paquete de FEC
            theStatsGatherer->accountRecvParity(addr,
                                                __flowId,
                                                __ssrc,
                                                pkt->getTotalLength()
                                               );
        
            if(fecPkt)
            {
                //Recovered data
                theStatsGatherer->accountRecpData(addr,
                                                  __flowId,
                                                  __ssrc,
                                                  fecPkt->getTimestamp(),
                                                  fecPkt->getSequenceNumber(),
                                                  fecPkt->getTotalLength()
                                                  );

            }
        }
        delete pkt;
        
    }
    
    return fecPkt;
}

//
// RTP FEC HOLDER
//

RTPFecHolder_t::RTPFecHolder_t(flowId_e flowId)
: __flowId(flowId),
  __sendChannelsDict(64),
  __recvChannelsDict(64)
{
    __fecPT = (fecById->lookUp(flowId));
    NOTIFY("RTP Fec Holder created PT = %d\n", __fecPT);
    __k=0;
}

RTPFecHolder_t::~RTPFecHolder_t(void)
{
    ql_t<u32> *list=__sendChannelsDict.getKeys();
    ql_t<u32>::iterator_t i;
    for (i = list->begin(); i!=list->end(); i++){
        __sendChannelsDict.lookUp(static_cast<u32>(i))=NULL;
    }
    delete list;
    list=__recvChannelsDict.getKeys();
    for (i=list->begin(); i!=list->end(); i++){
        __recvChannelsDict.lookUp(static_cast<u32>(i))=NULL;
    }
    delete list;
}

u8  
RTPFecHolder_t::getPT(void)
{
    return __fecPT;
}

RTPPacket_t *
RTPFecHolder_t::sendPacket(RTPPacket_t * pkt, icfString_ref addr)
{
    //cuento el paquete normal
    if (theStatsGatherer->isAccounting())
    {
        theStatsGatherer->accountSentData(addr,
                                          __flowId,
                                          pkt->getSSRC(),
                                          pkt->getTotalLength());
    }
  
    //si k = 0, no hago FEC
    if (!__k)
    {
        delete pkt;
        return NULL;
    }
    
    RTPFec_ref RTPFec = __sendChannelsDict.lookUp(pkt->getSSRC());
    
    //if RTPFec doesn't exists
    if (!RTPFec.isValid())
    {
        RTPFec = newRTPFec(pkt->getSSRC(),__k);
    }
    return RTPFec->sendPacket(pkt, addr);
}

RTPPacket_t *
RTPFecHolder_t::recvPacket(RTPPacket_t * pkt, icfString_ref addr, bool &fec)
{
    //si es paquete normal cuenta en las estadisticas
    if (pkt->getPayloadtype()!=__fecPT)
    {
        if (theStatsGatherer->isAccounting())
        {
            struct timeval *time=pkt->getTV();
            u32 tvusec = time->tv_sec * 1000000 + time->tv_usec;
            theStatsGatherer->accountRecvData(addr,
                                          __flowId,
                                          pkt->getSSRC(),
                                          pkt->getTimestamp(),
                                          pkt->getSequenceNumber(),
                                          pkt->getTotalLength(),
                                          tvusec);
        }
    }
    
    //si es un paquete de proteccion
    //creo el canal RTPFec
    RTPFec_ref RTPFec = __recvChannelsDict.lookUp(pkt->getSSRC());
    if (!RTPFec.isValid() && pkt->getPayloadtype() == __fecPT)
    {
            RTPFec = newRTPFec(pkt->getSSRC());
          
    }else{
        if (RTPFec.isValid())
            return RTPFec->recvPacket(pkt, addr);
    }
    delete pkt;
    return NULL;
}

void
RTPFecHolder_t::setFecPT(char *fecType, int n, int k)
{
    
    //to disable RTPFEC use fecType = None
    if(!strcmp(fecType, "None"))
    {
        __k=0;
        NOTIFY("RTPFecHolder_t::setFecPT: FEC Disabled\n", k);
        return;
    }

    //k must be [0,24]
    if(k<0)
    {
        return;
    }else{

        if(k>24)
        {
            NOTIFY("RTPFecHolder_t::setFecPT: k=%d>24"
                   " -> NO Dynamic PT available for" 
                   "protecting flow\n", k);
            return;
        }
  
        //refresh __k
        __k=k;
        //refresh every RTPFec channel (only senders)
        NOTIFY("RTPFecHolder_t::setFecPT: k=%d\n",k);
        ql_t<u32> *list=__sendChannelsDict.getKeys();
        ql_t<u32>::iterator_t i;
        for (i = list->begin(); i!=list->end(); i++)
        {
            RTPFec_t * RTPFec = __sendChannelsDict.lookUp(static_cast<u32>(i));
            RTPFec->setK(__k); 
            //actually we only support parity, so we only
            //set K, and not N, for future upgrades this
            //method shold be changed --> setParams(N,K)
        }
        delete list;
 
    }
}

RTPFec_ref
RTPFecHolder_t::newRTPFec(u32 ssrc, u8 k)
{
    RTPFec_ref RTPFec = new RTPFec_t(__flowId,ssrc,__fecPT,k);
    NOTIFY("RTPFecHolder_t::New RTPFec created ssrc=%d,PT=%d,k=%d\n",
           ssrc,__fecPT,k);
    if (!k)
    {
        __recvChannelsDict.insert(ssrc, RTPFec);
        NOTIFY("RTPFec inserted in receivers channels\n");
    }else{
        __sendChannelsDict.insert(ssrc, RTPFec);
        NOTIFY("RTPFec inserted in senders channels\n");
    }
    return RTPFec;
}

void
RTPFecHolder_t::delRTPFec(u32 ssrc)
{
    __sendChannelsDict.remove(ssrc);
    NOTIFY("RTPFecHolder_t::RTPFec doing fec removed ssrc=%d,PT=%d\n",ssrc,__fecPT);
    return;
}

bool
RTPFecHolder_t::getLastSQ(u32 ssrc,u16& SQ)
{
    RTPFec_ref RTPFec = __recvChannelsDict.lookUp(ssrc);
    if (RTPFec.isValid())
    {
	 SQ = RTPFec->getLastSQ();
         return true;
    }
    return false;
} 
