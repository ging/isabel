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
// $Id: mixer.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <limits.h>

#include <icf2/notify.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPUtils.hh>

#include "mixer.h"
#include "soundCodecs.h"
#include "sessionManager.h"
#include "returnCode.h"

mixProcessor_t::mixProcessor_t(int PT)
{
    this->PT = PT;
    memset(pos, 0, MAX_SOURCES*sizeof(int));
    memset(sample, 0xff, SAMPLE_SIZE*sizeof(short));
    samplePtr = &sample[0];
    time.tv_sec = 0;
    time.tv_usec = 0;

    //new seed
    srand((unsigned)::time(NULL));

#if defined(WIN32)
    SQ   = rand();
    TS   = rand();
    SSRC = rand();
#elif defined(__BUILD_FOR_LINUX)
    SQ   = random();
    TS   = random();
    SSRC = random();
#else
#error "Please, initialize mixProcessor_t for your OS"
#endif

    M  = 0;
}

mixProcessor_t::~mixProcessor_t(void)
{
}

int
mixProcessor_t::getPT(void)
{
    return PT;
}

HRESULT
mixProcessor_t::setPT(int PT, int sessionID)
{
    //-------------------------------------------
    // check if PT is an audio PT
    //-------------------------------------------

    if (PT < 128)
    {
        if (codecPool[PT])
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

    sessionManager_t::sessionInfo_t *session = NULL;
    APP->getSession(sessionID, &session);

    for (u16 k = 0; k < targetArray.size(); k++)
    {
        bool tgtFound = false;
        // for every processor target
        target_t *tgt1 = targetArray.elementAt(k);

        // find the correct one at hole list
        for (int i = 0; i < session->targetArray.size(); i++)
        {
            target_t *tgt2 = session->targetArray.elementAt(i);
            if (tgt1->getID() == tgt2->getID()) //same dest
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
            NOTIFY("WARNING ::mixProcessor_t::setPT::\n"
                   "Can't found a valid target for PT= %d\n"
                   "for participant with ID= %s\n"
                   "Please, bind all PTs before use!\n",
                   PT,
                   tgt1->getID()
                  );
        }
    }
    return E_ERROR;
}

u32
mixProcessor_t::getSSRC(void)
{
    return SSRC;
}

HRESULT
mixProcessor_t::setSSRC(u32 numSSRC)
{
    SSRC = numSSRC;
    return S_OK;
}

HRESULT
mixProcessor_t::deliver(short * data,
                        u16 len,
                        flow_t inFlow,
                        timeval actualTime,
                        bool reset
                       )
{
    u16 rate = GetCodecRate(PT);

    //-------------------------------------------
    //if mark --> reset position
    //-------------------------------------------
    if (reset || pos[inFlow.ID]==-1)
    {
        pos[inFlow.ID] = (samplePtr - &sample[0]) + DELAY*rate*2/1000;
    }

    //-------------------------------------------
    // mix sample
    //-------------------------------------------
    u16 position = pos[inFlow.ID];
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

    //refresh position to inFlow
    pos[inFlow.ID] = (position + len)%SAMPLE_SIZE;

    //check time
    if (!time.tv_sec && !time.tv_usec)
    {
        //NOTIFY("mixer_t :: buffering (100 ms)...\n");
        time.tv_sec  = actualTime.tv_sec;
        time.tv_usec = actualTime.tv_usec;
    }
    int millis = 0;

    if (get_period()==0)
    {
        millis = (actualTime.tv_sec - time.tv_sec)*1000 +
                 (actualTime.tv_usec - time.tv_usec)/1000;
    }

    if (millis > DELAY) //activate sending
    {
        //NOTIFY("mixer_t :: end buffering period\n");
        u16 msPerPacket = GetCodecMSPerPacket(PT);
        this->set_period(msPerPacket*1000);
    }
    return S_OK;
}

void
mixProcessor_t::heartBeat(void)
{
    u16 rate = GetCodecRate(PT);
    u16 msPerPacket = GetCodecMSPerPacket(PT);
    u16 samples = msPerPacket*rate/1000;

    //refresh TS
    TS += msPerPacket*8;

    //-------------------------------------------
    // check mixed data
    //-------------------------------------------
    bool isEmpty = true;
    for (u16 j = 0; j < samples; j++)
    {
        if (samplePtr[j] != 0xffff &&
            samplePtr[j] != -1)
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

    Codec *codec = codecPool[PT];
    if (codec == NULL)
    {
        NOTIFY("mixProcessor_t::heartBeat: Codec %d not found\n", PT);
        return;
    }

    //-------------------------------------------
    // codec sample
    //-------------------------------------------
    unsigned char * data2 =
    new unsigned char[samples*2 + sizeof(RTPHeader_t)];
    memset(data2,0x00,samples*2 + sizeof(RTPHeader_t));

    int n = CodecEncode(codec,
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
    RTPPacket_t *pkt = new RTPPacket_t(data2,n + sizeof(RTPHeader_t));
    RTPHeader_t *header= (RTPHeader_t*)pkt->getPacket();
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
    for (u16 k = 0; k < MAX_SOURCES; k++)
    {
        if (pos[k])
        {
            double distance1 = (samplePtr - &sample[0]) + 30.0*rate*2.0/1000.0;
            double distance2 = (samplePtr - &sample[0])/2;
            if (pos[k] < distance1 && pos[k] > distance2)
            {
                pos[k] = -1;
                //NOTIFY("mixer_t :: reseting ID = %d\n",k);
            }
        }
    }
}

HRESULT
mixProcessor_t::reset(void)
{
    this->set_period(0);
    time.tv_sec  = 0;
    time.tv_usec = 0;
    M=1;

    memset(pos,0,MAX_SOURCES*sizeof(int));
//  memset(sample,0xff,SAMPLE_SIZE*sizeof(short));

    //Create confortably noise
    short * ptr = (short *)sample;
    unsigned int NOISE_AMP = 0xff;
    for (int i = 0;i<SAMPLE_SIZE;i++)
    {
         double random = rand()/(double)RAND_MAX;
         short noise = (short)(abs((int)(random*NOISE_AMP)));
         *ptr = (short)(0xff - noise);
         ptr++;
    }
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
            if (aux->getID() == inFlow.ID)
            {
                return true;
            }
        }
    }

    return false;
}

bool
mixProcessor_t::usesTarget(int ID, ql_t<u16> portList)
{
    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *target = targetArray.elementAt(i);
        if (target->getID() == ID)
        {
            for (ql_t<u16>::iterator_t j = portList.begin();
                 j != portList.end();
                 j++
                )
            {
                u16 port = j;
                if(target->getRTPport() == port)
                    return true;
            }
        }

    }
    return false;
}


mixer_t::mixer_t(void)
: flowProcessor_t(FLOW_MIX)
{
     SilenceLevel = -100; //dBs
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

    Codec *codec = codecPool[pkt->getPayloadType()];
    if (codec == NULL)
    {
        NOTIFY("mixer_t :: Codec %d not found for decoding\n",
               pkt->getPayloadType()
              );
        return E_AUDIO_DECODE;
    }

    u8 data[SAMPLE_SIZE]; //decoded data
    memset(data, 0xff, SAMPLE_SIZE);

    int n = CodecDecode(codec,pkt->getData(),pkt->getDataLength(),data);

    if (n <= 0)
    {
        NOTIFY("mixer_t:: can't decode "
               "[PT = %d, bytes RX = %d, bytes decoded = %d]\n",
               pkt->getPayloadType(),
               pkt->getDataLength(),
               n
              );
        return E_AUDIO_DECODE;
    }

    //--------------------------------------------------
    // Check silence detection
    //--------------------------------------------------
    if (vuMeter.checkSilence(data, n, SilenceLevel))
    {
        //return S_OK;
    }

    //--------------------------------------------------
    //Get localTime and send it with the pkt
    //--------------------------------------------------
    timeval actualTime;
    gettimeofday(&actualTime, NULL);

    // send sample to every valid processor
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        mixProcessor_t *mixProcessor =
            static_cast<mixProcessor_t *>(outFlowArray->elementAt(i)->processor);

        // check if resample is needed
        if (GetCodecRate(mixProcessor->getPT()) != GetCodecRate(pkt->getPayloadType()))
        {
            int m;
            u8 data2[SAMPLE_SIZE];
            memset(data2,0xff,SAMPLE_SIZE);
            m = resample(data,
                         n,
                         GetCodecRate(pkt->getPayloadType()),
                         data2,
                         SAMPLE_SIZE,
                         GetCodecRate(mixProcessor->getPT())
                        );

            mixProcessor->deliver((short *)data2,m/2,inFlow,actualTime,pkt->getMark());
        }
        else
        {
            mixProcessor->deliver((short *)data,n/2,inFlow,actualTime,pkt->getMark());
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
                static_cast<mixProcessor_t *>(outFlowArray->elementAt(i)->processor);

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
                 int PT,
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
    outFlow.ID = target->getID();
    outFlow.PT = target->getPT();

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at global array
    mixProcessor_t *mixProcessor1 = getValidProcessor(outFlow);

    if ( ! mixProcessor1) // create processor and insert in list
    {
        mixProcessor1 = new mixProcessor_t(PT);
        mixProcessor1->addRef();
        outFlow_t * outFlow = new outFlow_t(inFlow);
        outFlow->ID = target->getID();
        outFlow->processor = mixProcessor1;
        outFlowArray->add(outFlow);
    }
    else
    {
        // find correct processor at my list
        mixProcessor_t *mixProcessor2 =
            getValidProcessor(outFlow, outFlowArray);

        if ( ! mixProcessor2) // only insert processor in list
        {
            mixProcessor1->addRef();
            outFlow_t * outFlow = new outFlow_t(inFlow);
            outFlow->ID = target->getID();
            outFlow->processor = mixProcessor1;
            outFlowArray->add(outFlow);
        }
    }
    mixProcessor1->addTarget(target);
    if (SSRC != 0)
    {
        mixProcessor1->setSSRC(SSRC);
    }

    return S_OK;
}

HRESULT
mixer_t::unsetFlow(flow_t inFlow, target_t *target)
{
    flow_t outFlow;
    outFlow.ID = target->getID();
    outFlow.PT = target->getPT();

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    mixProcessor_t *mixProcessor = getValidProcessor(outFlow, outFlowArray);

    if (!mixProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

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
mixer_t::setPT(u8 newPT,int ID, int sessionID, u8 oldPT)
{
    flow_t outFlow;
    outFlow.ID = ID;
    outFlow.PT = oldPT;

    // find correct processor at hole array
    mixProcessor_t * mixProcessor = getValidProcessor(outFlow);

    if (!mixProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

    mixProcessor->setPT(newPT,sessionID);
    mixProcessor->reset();
    return S_OK;
}

HRESULT
mixer_t::setSilenceLevel(int SL)
{
    if (SL<0)
    {
        SilenceLevel = SL;
        return S_OK;
    }
    return E_SILENCE_LEVEL;
}

HRESULT
mixer_t::setSSRC(u32 newSSRC, int ID, ql_t<u16> portList)
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
                mixProcessor_t *mixProcessor =
                    static_cast<mixProcessor_t*>(outFlow->processor);

                if (mixProcessor->usesTarget(ID, portList))
                {
                    mixProcessor->setSSRC(newSSRC);
                    //audioTransProcessor->reset();
                }
            }
        }
    }
    return S_OK;
}

