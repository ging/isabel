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
// $Id: mixer.cc 8518 2006-05-18 09:42:07Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <limits.h>

#include <icf2/notify.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include "mixer.h"
#include "soundCodecs.h"
#include "sessionManager.h"
#include "returnCode.h"

#ifdef __BUILD_FOR_LINUX
#include <sys/types.h> //to include inet_ntop()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#endif

mixProcessor_t::mixProcessor_t(u8 PT, u32 SSRC, target_t *target)
{
    this->PT = PT;
    memset(sample, 0xff, SAMPLE_SIZE*sizeof(short));
    samplePtr = &sample[0];
    time.tv_sec = 0;
    time.tv_usec = 0;

    // new seed
    srand((unsigned)::time(NULL));

    this->SQ   = RAND();

    // El TS lo inicio con la hora del sistema
    timeval actualTime;
    gettimeofday(&actualTime, NULL);
    int rate = getRateByPT(PT);
    long tsinit = actualTime.tv_sec*rate + actualTime.tv_usec*rate/1000000;
    NOTIFY("Initial TS = %d\n", tsinit);
    this->TS   = tsinit;

    this->SSRC = RAND();
    if (SSRC)
    {
        this->SSRC = SSRC;
    }
    M  = 0;
    resetMode = 0;

    if ( APP->noSL )
    {
        NOTIFY("mixProcessor:: started in noSL mode\n");
        u16 msPerPacket = getMSPerPacketByPT(PT);
        this->set_period(msPerPacket*1000);
        resetMode = 1;
    }
}

mixProcessor_t::~mixProcessor_t(void)
{
}

void
mixProcessor_t::setSSRCfromID(u32 SSRC, u32 id)
{
    NOTIFY("setSSRCfromID : in <%d,%d>\n",id,SSRC);
    SSRCmap[id] = SSRC;
}

void
mixProcessor_t::unsetSSRCfromID(u32 id)
{
    NOTIFY("unsetSSRCfromID : out <%d,%d>\n",id,SSRCmap[id]);
    if (SSRCmap[id] == SSRC)
    {
        NOTIFY("Need to change output SSRC!\n");
        SSRCmap[id] = 0;
        for (std::map<u32,u32>::iterator iter = SSRCmap.begin();
        iter != SSRCmap.end();++iter)
        {
             if (iter->second > 0)
             {
                 NOTIFY("New SSRC = %d\n",iter->second);
                 SSRC = iter->second;
                 break;
             }
         }
    }
}
u8
mixProcessor_t::getPT(void)
{
    return PT;
}

HRESULT
mixProcessor_t::setPT(u8 PT)
{
    //-------------------------------------------
    // check if PT is an audio PT
    //-------------------------------------------

    if (PT < 128)
    {
        if (codecPool.getCoder(PT,this->SSRC))
        {
            this->PT = PT;
        }
        else
        {
            NOTIFY("mixProcessor_t::setPT: "
                   "PT %d belongs to unknown codec\n",
                   PT
                  );
            return E_ERROR;
        }
    }

    //-------------------------------------------
    // Now we load a valid target for the PT with
    // the same IP destination, if not found,
    // it's a problem of the MCU user.
    // So , notification is made and MCU continues
    // with the same target (could abort execution!)
    //-------------------------------------------

    for (u16 k = 0; k < targetArray.size(); k++)
    {
        bool tgtFound = false;
        // for every processor target
        target_t *tgt1 = targetArray.elementAt(k);

        // find the correct one at hole list
        for (int i = 0; i < APP->targetArray.size(); i++)
        {
            target_t *tgt2 = APP->targetArray.elementAt(i);
            if (addrEquals(tgt1->getIP(),tgt2->getIP())) // same IP dest
            {
                if (tgt2->getPT() == PT) // if correct PT
                {
                    deleteTarget(tgt1);
                    addTarget(tgt2);
                    tgtFound = true;
                    break;
                }
            }
        }
        if ( ! tgtFound)
        {
            char IP[128];
            switch (tgt1->getIP().ss_family)
            {
            case AF_INET:
                {
                    sockaddr_storage tgt1IP= tgt1->getIP();
                    sockaddr_in *IP4 = (sockaddr_in *)(&tgt1IP);
                    inet_ntop(IP4->sin_family,(u8 *)&IP4->sin_addr.s_addr,IP,128);
                    break;
                }
            case AF_INET6:
                {
                    sockaddr_storage tgt1IP= tgt1->getIP();
                    sockaddr_in6 *IP6 = (sockaddr_in6 *)(&tgt1IP);
                    inet_ntop(IP6->sin6_family,(u8 *)&(IP6->sin6_addr),IP,128);
                    break;
                }
            default:
                NOTIFY("target_t::target_t::Unknown family [%d]\n",tgt1->getIP().ss_family);
                abort();
            }
            NOTIFY("WARNING ::mixProcessor_t::setPT::\n"
                   "Can't found a valid target for PT= %d\n"
                   "with destination IP= %s\n"
                   "MCU will use same target with PT= %d\n"
                   "Please, bind all PTs before use!\n",
                   PT,
                   IP,
                   tgt1->getPT()
                  );
        }
    }
    return E_ERROR;
}

HRESULT
mixProcessor_t::deliver(short *data,
                        u16 len,
                        u32 SSRC,
                        timeval actualTime,
                        bool reset
                       )
{
    u16 rate = getRateByPT(PT);

    //-------------------------------------------
    // if mark --> reset position
    //-------------------------------------------
    if (reset || pos[SSRC]==-1)
    {
        pos[SSRC] = (samplePtr - &sample[0]) + DELAY*rate*2/1000;
    }

    //-------------------------------------------
    // mix sample
    //-------------------------------------------
    u16 position = pos[SSRC];
    for (int i = 0; i < len; i++)
    {
        u16 flowPos = (position + i)%SAMPLE_SIZE;
        int data1 = (int)(sample[flowPos]);
        int data2 = (int)(data[i]);
        int tmpData = data1 + data2;
        if (tmpData > SHRT_MAX) tmpData= SHRT_MAX;
        if (tmpData < SHRT_MIN) tmpData= SHRT_MIN;
        short realData = (short)tmpData;
        sample[flowPos] = realData;
    }

    // refresh position to inFlow
    pos[SSRC] = (position + len)%SAMPLE_SIZE;

    // check time
    if (!time.tv_sec && !time.tv_usec)
    {
        //NOTIFY("mixer_t :: buffering (100 ms)...\n");
        time.tv_sec  = actualTime.tv_sec;
        time.tv_usec = actualTime.tv_usec;
    }
    int millis = 0;

    if (get_period() == 0 || resetMode == 0)
    {
        millis = (actualTime.tv_sec - time.tv_sec)*1000 +
                 (actualTime.tv_usec - time.tv_usec)/1000;
    }
    if (millis > DELAY) // activate sending
    {
        //NOTIFY("mixer_t :: end buffering period\n");
        u16 msPerPacket = getMSPerPacketByPT(PT);
        this->set_period(msPerPacket*1000);
        resetMode = 1;
    }
    return S_OK;
}

void
mixProcessor_t::heartBeat(void)
{
    u16 rate = getRateByPT(PT);
    u16 msPerPacket = getMSPerPacketByPT(PT);
    u16 samples = msPerPacket*rate/1000;

    // refresh TS
    TS += samples;
    // Allows the timestamp to grow even when not sending audio
    if (resetMode == 0)
    {
        return;
    }
    //-------------------------------------------
    // check mixed data
    //-------------------------------------------
    bool isEmpty = true;

    for (u16 j = 0; j < samples; j++)
    {
        int idx = (samplePtr - sample + j)%SAMPLE_SIZE;
        if (sample[idx] != -1)
        {
            isEmpty = false;
            break;
        }
    }
    if (isEmpty && ! APP->noSL)
    {
        reset();
        return;
    }

    aCoder_t *coder= codecPool.getCoder(PT, this->SSRC);
    if (coder == NULL)
    {
        NOTIFY("mixProcessor_t::heartBeat: Codec %d not found\n", PT);
        return;
    }

    //-------------------------------------------
    // codec sample
    //-------------------------------------------
    unsigned char *data2 = new unsigned char[samples*2 + sizeof(RTPHeader_t)];
    memset(data2, 0x00, samples*2 + sizeof(RTPHeader_t));

    int n= aEncode(coder,
                   (u8*)samplePtr,
                   samples,
                   data2 + sizeof(RTPHeader_t)
                  );

    if (n <= 0)
    {
        NOTIFY("mixer_t :: Encoding returns %d bytes encoded of %d\n",
                n,
                samples);
        delete[] data2;
        return;
    }

    //-------------------------------------------
    // create RTPPacket
    //-------------------------------------------
    RTPPacket_t *pkt = new RTPPacket_t(data2, n + sizeof(RTPHeader_t));
    RTPHeader_t *header = (RTPHeader_t*)pkt->getPacket();
    header->version     = RTP_VERSION;
    header->extension   = 0;
    header->marker      = M;
    header->padding     = 0;
    header->cc          = 0;
    header->seqnum      = htons(SQ++);
    header->timestamp   = htonl(TS);
    header->ssrc        = htonl(SSRC);
    header->payloadtype = PT;
    M = 0;

    //-------------------------------------------
    // reset sample
    //-------------------------------------------

    u16 newPos = (u16)((samplePtr - &sample[0] + samples)%SAMPLE_SIZE);

    if (&sample[newPos] < samplePtr)
    {
        u16 data1Len = (SAMPLE_SIZE - (samplePtr - &sample[0]));
        u16 data2Len = samples - data1Len;
        memset(samplePtr,0xff,data1Len*2);
        memset(&sample[0],0xff,data2Len*2);
    }
    else
    {
        memset(samplePtr,0xff,samples*2);
    }
    samplePtr = &sample[newPos];

    //-------------------------------------------
    // Send sample
    //-------------------------------------------
    for (int j = 0 ; j < targetArray.size(); j++)
    {
        targetArray.elementAt(j)->deliver(pkt);
    }
    // delete pkt and data2
    delete pkt;
    delete []data2;

    //-------------------------------------------
    // check if has passed any writer
    //-------------------------------------------
    for (std::map<u32,int>::iterator iter = pos.begin();
        iter != pos.end();++iter)
    {
        if (iter->second)
        {
            double distance1 = (samplePtr - &sample[0]) + 30.0*rate*2.0/1000.0;
            double distance2 = (samplePtr - &sample[0])/2;
            if (iter->second < distance1 && iter->second > distance2)
            {
                iter->second = -1;
                //NOTIFY("mixer_t :: reseting ID = %d\n",k);
            }
        }
    }
}

HRESULT
mixProcessor_t::reset(void)
{
     NOTIFY("mixProcessor_t::reset\n");
    //this->set_period(0);
    resetMode = 0;
    time.tv_sec  = 0;
    time.tv_usec = 0;
    M=1;

    pos.clear();
//  memset(sample,0xff,SAMPLE_SIZE*sizeof(short));

/*
    // Create confortably noise
    short * ptr = (short *)sample;
    unsigned int NOISE_AMP = 0xff;
    for (int i = 0;i<SAMPLE_SIZE;i++)
    {
         double random = rand()/(double)RAND_MAX;
         short noise = (short)(abs((int)(random*NOISE_AMP)));
         *ptr = (short)(0xff - noise);
         ptr++;
    }
*/
    samplePtr = &sample[0];
    return S_OK;
}

bool
mixProcessor_t::isValid(flow_t inFlow)
{
    if (inFlow.PT == PT)
    {
        for (int i = 0; i < targetArray.size(); i++)
        {
            target_t *aux = targetArray.elementAt(i);
            if (addrEquals(aux->getIP(), inFlow.IP))
            {
                return true;
            }
        }
    }

    return false;
}

mixer_t::mixer_t(void)
: flowProcessor_t(FLOW_MIX)
{
     SilenceLevel = -100; // dBs
}


mixer_t::~mixer_t(void)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            while (outFlowMatrix[i]->size())
            {
                outFlow_t *outFlow= outFlowMatrix[i]->elementAt(0);
                outFlowMatrix[i]->remove(0);
                delete outFlow;
            }
            delete outFlowMatrix[i];
            outFlowMatrix[i] = NULL;
        }
    }
}


HRESULT
mixer_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
{
    if (getFlowByPT(pkt->getPayloadType()) != AUDIO_FLOW)
    {
        return S_OK;
    }

    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);
    if (outFlowArray->size() == 0)
    {
        return E_PROC_NOT_EXISTS;
    }

    //--------------------------------------------------
    // Decode audio
    //--------------------------------------------------

    aDecoder_t *decoder =
        codecPool.getDecoder(pkt->getPayloadType(), pkt->getSSRC());

    if (decoder == NULL)
    {
        NOTIFY("mixer_t :: Codec %d not found for decoding\n",
               pkt->getPayloadType()
              );
        return E_AUDIO_DECODE;
    }

    u8 data[SAMPLE_SIZE]; // decoded data
    memset(data, 0xff, SAMPLE_SIZE);

    int n= aDecode(decoder, pkt->getData(), pkt->getDataLength(), data);

    if (n <= 0)
    {
        /*
        NOTIFY("mixer_t:: can't decode "
               "[PT = %d, bytes RX = %d, bytes decoded = %d]\n",
               pkt->getPayloadType(),
               pkt->getDataLength(),
               n
              );
        */
        return E_AUDIO_DECODE;
    }

    //--------------------------------------------------
    // Check silence detection
    //--------------------------------------------------
    if (vuMeter.checkSilence(data, n, SilenceLevel))
    {
        return S_OK;
    }

    //--------------------------------------------------
    // Get localTime and send it with the pkt
    //--------------------------------------------------
    timeval actualTime;
    gettimeofday(&actualTime, NULL);

    // send sample to every valid processor
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        mixProcessor_t *mixProcessor =
            static_cast<mixProcessor_t *>(outFlowArray->elementAt(i)->processor);

        // check if resample is needed
        if (getRateByPT(mixProcessor->getPT()) != getRateByPT(pkt->getPayloadType()))
        {
            int m;
            u8 data2[SAMPLE_SIZE];
            memset(data2,0xff,SAMPLE_SIZE);
            m = resample(data,
                         n,
                         getRateByPT(pkt->getPayloadType()),
                         data2,
                         SAMPLE_SIZE,
                         getRateByPT(mixProcessor->getPT())
                        );

            mixProcessor->deliver((short *)data2,m/2,pkt->getSSRC(),actualTime,pkt->getMark());
        }
        else
        {
            mixProcessor->deliver((short *)data,n/2,pkt->getSSRC(),actualTime,pkt->getMark());
        }
    }

    return S_OK;
}


mixProcessor_t *
mixer_t::getValidProcessor(flow_t inFlow)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            mixProcessor_t *mixProcessor =
                getValidProcessor(inFlow, outFlowMatrix[i]);
            if (mixProcessor)
            {
                return mixProcessor;
            }
        }
    }
    return NULL;
}

mixProcessor_t *
mixer_t::getValidProcessor(flow_t inFlow,
                           vector_t<outFlow_t *> *outFlowArray
                          )
{
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        mixProcessor_t *mixProcessor =
            static_cast<mixProcessor_t*>(outFlowArray->elementAt(i)->processor);

        if (mixProcessor->isValid(inFlow))
        {
            return mixProcessor;
        }
    }
    return NULL;
}

HRESULT
mixer_t::setFlow(flow_t inFlow,
                 target_t *target,
                 u8 PT,
                 u32 BW,
                 u16 width,
                 u16 height,
                 u32 FR,
                 u8  Q,
                 u32 SSRC,
                 gridMode_e gridMode
                )
{
    flow_t outFlow;
    outFlow.IP = target->getIP();
    outFlow.PT = target->getPT();

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at global array
    mixProcessor_t *mixProcessor1 = getValidProcessor(outFlow);

    if ( ! mixProcessor1)
    {
        // create processor and insert in list

        mixProcessor1 = new mixProcessor_t(PT,SSRC,target);
        mixProcessor1->addRef();
        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = inFlow.ID;
        outFlow->processor = mixProcessor1;
        outFlowArray->add(outFlow);
    }
    else
    {
        // find correct processor at my list
        mixProcessor_t *mixProcessor2 =
            getValidProcessor(outFlow, outFlowArray);

        if ( ! mixProcessor2)
        {
            // only insert processor in list

            mixProcessor1->addRef();
            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = inFlow.ID;
            outFlow->processor = mixProcessor1;
            outFlowArray->add(outFlow);
        }
    }
    mixProcessor1->setSSRCfromID(SSRC,inFlow.ID);
    mixProcessor1->addTarget(target);
    return S_OK;
}

HRESULT
mixer_t::unsetFlow(flow_t inFlow, target_t *target)
{
    flow_t outFlow;
    outFlow.IP = target->getIP();
    outFlow.PT = target->getPT();

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    mixProcessor_t *mixProcessor = getValidProcessor(outFlow, outFlowArray);

    if ( ! mixProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }
    mixProcessor->unsetSSRCfromID(inFlow.ID);
    // now we must delete mixProcessor from position
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        if (mixProcessor == outFlowArray->elementAt(i)->processor)
        {
            outFlowArray->remove(i);
            if (mixProcessor->decRef() == 0)
            {
                APP->removeTask(mixProcessor); // this deletes processor
            }
            break;
        }
    }
    return S_OK;
}

HRESULT
mixer_t::setPT(u8 newPT, sockaddr_storage const &IP, u8 oldPT)
{
    flow_t outFlow;
    outFlow.IP = IP;
    outFlow.PT = oldPT;

    // find correct processor at hole array
    mixProcessor_t * mixProcessor = getValidProcessor(outFlow);

    if (!mixProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

    mixProcessor->setPT(newPT);
    mixProcessor->reset();

    return S_OK;
}

HRESULT
mixer_t::setSilenceLevel(int SL)
{
    if (SL < 0)
    {
        SilenceLevel = SL;
        return S_OK;
    }
    return E_SILENCE_LEVEL;
}

