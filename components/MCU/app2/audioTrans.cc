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
// $Id: audioTrans.cc 7506 2005-10-20 13:29:33Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include "audioTrans.h"
#include "soundCodecs.h"
#include "sessionManager.h"

audioTransProcessor_t::audioTransProcessor_t(u8 PT, u32 SSRC)
{
    this->PT = PT;
    memset(sample,0xff,SAMPLE_SIZE*sizeof(short));
    samplePtr = &sample[0];
    time.tv_sec = 0;
    time.tv_usec = 0;
    pos = 0;

    // new seed
    srand((unsigned)::time(NULL));

    this->SQ   = RAND();
    this->TS   = RAND();
    this->SSRC = RAND();
    if (SSRC)
    {
        this->SSRC = SSRC;
    }
    M  = 0;
}

audioTransProcessor_t::~audioTransProcessor_t(void)
{
}

bool
audioTransProcessor_t::usesTarget(sockaddr_storage const &IP, int PT)
{
    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *target = targetArray.elementAt(i);
        if (addrEquals(target->getIP(), IP) &&
            (target->getPT() == PT || PT == -1)
           )
        {
            return true;
        }
    }
    return false;
}

u8
audioTransProcessor_t::getPT(void)
{
    return PT;
}

HRESULT
audioTransProcessor_t::setPT(u8 PT)
{
    // check if PT is an audio PT
    if (PT < 128)
    {
        if (codecPool.getCoder(PT, this->SSRC))
        {
            this->PT = PT;
            return S_OK;
        }
    }
    return E_ERROR;
}

HRESULT
audioTransProcessor_t::deliver(short *data,
                               u16 len,
                               flow_t inFlow,
                               timeval actualTime,
                               bool reset
                              )
{
    u16 rate = getRateByPT(PT);

    //-------------------------------------------
    //if mark --> reset position
    //-------------------------------------------
    if (reset || pos == -1)
    {
        pos = (samplePtr - &sample[0]) + DELAY*rate*2/1000;
    }

    //-------------------------------------------
    // audioTrans sample
    //-------------------------------------------
    for (int i = 0;i<len;i++)
    {
        u16 flowPos = (pos + i)%SAMPLE_SIZE;
        sample[flowPos] = data[i];
    }

    pos = (pos + len)%SAMPLE_SIZE;

    //check time
    if (!time.tv_sec && !time.tv_usec)
    {
        NOTIFY("audioTrans_t :: buffering (100 ms)...\n");
        time.tv_sec  = actualTime.tv_sec;
        time.tv_usec = actualTime.tv_usec;
    }
    int millis = 0;

    if (get_period()==0)
    {
        millis = (actualTime.tv_sec - time.tv_sec)*1000 + (actualTime.tv_usec - time.tv_usec)/1000;
    }

    if(millis > DELAY) //activate sending
    {
        NOTIFY("audioTransProcessor_t :: end buffering period\n");
        u16 msPerPacket = getMSPerPacketByPT(PT);
        this->set_period(msPerPacket*1000);
    }
    return S_OK;
}

void
audioTransProcessor_t::heartBeat(void)
{
    u16 rate = getRateByPT(PT);
    u16 msPerPacket = getMSPerPacketByPT(PT);
    u16 samples = msPerPacket*rate/1000;

    //refresh TS
    TS  += msPerPacket*8;

    //-------------------------------------------
    //check data
    //-------------------------------------------
    bool isEmpty = true;
    for (u16 j = 0; j<samples; j++)
    {
        if (samplePtr[j] != -1)
        {
            isEmpty = false;
            break;
        }
    }
    if (isEmpty)
    {
        reset();
        return;
    }

    //-------------------------------------------
    // codec sample
    //-------------------------------------------

    aCoder_t *coder = codecPool.getCoder(PT, this->SSRC);

    if (coder == NULL)
    {
        NOTIFY("audioTransProcessor_t:: NO CODER AVAILABLE!!\n");
        return;
    }

    u8 *data2 = new u8[samples*2 + sizeof(RTPHeader_t)];
    memset(data2, 0x00, samples*2 + sizeof(RTPHeader_t));

    int n= aEncode(coder,
                   (u8 *)samplePtr,
                   samples,
                   data2 + sizeof(RTPHeader_t)
                  );

    if (n <= 0)
    {
        NOTIFY("audioTransProcessor_t:: "
               "Encoding returns %d bytes encoded of %d samples\n",
               n,
               samples
              );
        delete[] data2;
        return;
    }

    //-------------------------------------------
    // create RTPPacket
    //-------------------------------------------
    RTPPacket_t *pkt    = new RTPPacket_t(data2,n + sizeof(RTPHeader_t));
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
    //reset sample
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
    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *tgt= targetArray.elementAt(i);
        tgt->deliver(pkt);
    }
    delete pkt; // delete pkt and data2
}

HRESULT
audioTransProcessor_t::reset(void)
{
    this->set_period(0);
    time.tv_sec  = 0;
    time.tv_usec = 0;
    M=1;

    pos = 0;

    memset(sample, 0xff, SAMPLE_SIZE * sizeof(short));

    samplePtr = &sample[0];

    return S_OK;
}

audioTrans_t::audioTrans_t(void)
: flowProcessor_t(FLOW_AUDIO_TRANS)
{
    SilenceLevel = -100; // dB
}


audioTrans_t::~audioTrans_t(void)
{
   for (u16 i = 0; i < MAX_FLOW_LEN; i++)
   {
        if (outFlowMatrix[i])
        {
            while(outFlowMatrix[i]->size())
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
audioTrans_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
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

    u8 data[SAMPLE_SIZE]; // decoded data
    memset(data, 0xff, SAMPLE_SIZE);

    int n= aDecode(decoder, pkt->getData(), pkt->getDataLength(), data);

    if (n <= 0)
    {
        NOTIFY("audioTrans_t::deliver: "
               "can't decode [PT = %d, bytes RX = %d, bytes decoded = %d]\n",
               pkt->getPayloadType(),
               pkt->getDataLength(),
               n
              );
        return E_AUDIO_DECODE;
    }

    //--------------------------------------------------
    // Check silence detection
    //--------------------------------------------------
    if (vuMeter.checkSilence(data,n,SilenceLevel))
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
        outFlow_t *outFlow= outFlowArray->elementAt(i);
        audioTransProcessor_t *audioTransProcessor =
            static_cast<audioTransProcessor_t*>(outFlow->processor);

        // check if resample is needed
        if (getRateByPT(audioTransProcessor->getPT()) != getRateByPT(pkt->getPayloadType()))
        {
            int m;
            u8 data2[SAMPLE_SIZE];
            memset(data2,0xff,SAMPLE_SIZE);
            m = resample(data,
                         n,
                         getRateByPT(pkt->getPayloadType()),
                         data2,
                         SAMPLE_SIZE,
                         getRateByPT(audioTransProcessor->getPT())
                        );

            audioTransProcessor->deliver((short *)data2,
                                         m/2,
                                         inFlow,
                                         actualTime,
                                         pkt->getMark()
                                        );
        }
        else
        {
            audioTransProcessor->deliver((short *)data,
                                         n/2,
                                         inFlow,
                                         actualTime,
                                         pkt->getMark()
                                        );
        }
    }

    return S_OK;
}


audioTransProcessor_t *
audioTrans_t::getValidProcessor(vector_t<outFlow_t *> *outFlowArray, u8 PT)
{
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        outFlow_t *outFlow= outFlowArray->elementAt(i);
        audioTransProcessor_t *audioTransProcessor =
            static_cast<audioTransProcessor_t *>(outFlow->processor);

        if (audioTransProcessor->getPT() == PT)
        {
            return audioTransProcessor;
        }
    }
    return NULL;
}

HRESULT
audioTrans_t::setFlow(flow_t inFlow,
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
    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    audioTransProcessor_t *audioTransProcessor =
        getValidProcessor(outFlowArray, PT);

    if ( ! audioTransProcessor) // create processor and insert in list
    {
        audioTransProcessor = new audioTransProcessor_t(PT,SSRC);
        audioTransProcessor->addRef();
        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = inFlow.ID;
        outFlow->processor = audioTransProcessor;
        outFlowArray->add(outFlow);
    }
    audioTransProcessor->addTarget(target);

    return S_OK;
}

HRESULT
audioTrans_t::unsetFlow(flow_t inFlow, target_t *target)
{
    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    for (int i = 0; i < outFlowArray->size(); i++)
    {
        outFlow_t *outFlow= outFlowArray->elementAt(i);
        processor_t *processor = outFlow->processor;
        if (processor->deleteTarget(target) == S_OK)
        {
            if (processor->decRef()==0)
            {
                deleteTarget(target); // this deletes processor
                outFlowArray = getProcessorArray(inFlow); // refresh outFlowArray
                if ( ! outFlowArray->size())
                    break;
                i--;
            }
        }
    }
    return S_OK;
}

// This method changes outGoing PT that targets to
// IP with oldPT and sets it with newPT
//
HRESULT
audioTrans_t::setPT(u8 newPT, sockaddr_storage const &IP, u8 oldPT)
{
    // look for any transcoder with target = IP
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            vector_t<outFlow_t *> *outFlowArray = outFlowMatrix[i];
            for (int j = 0; j < outFlowArray->size(); j++)
            {
                // check target list for IP
                outFlow_t *outFlow = outFlowArray->elementAt(j);
                audioTransProcessor_t *audioTransProcessor =
                    static_cast<audioTransProcessor_t*>(outFlow->processor);
                if (audioTransProcessor->usesTarget(IP, oldPT))
                {
                    audioTransProcessor->setPT(newPT);
                    audioTransProcessor->reset();
                }
            }
        }
    }

    return S_OK;
}

HRESULT
audioTrans_t::setSilenceLevel(int SL)
{
    if (SL < 0)
    {
        SilenceLevel = SL;
        return S_OK;
    }
    return E_SILENCE_LEVEL;
}

