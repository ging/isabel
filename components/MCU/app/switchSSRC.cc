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
// $Id: switchSSRC.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "sessionManager.h"
#include "switchSSRC.h"
#include "returnCode.h"

switchSSRCProcessor_t::switchSSRCProcessor_t(u32 SSRC, int PT)
{
    this->SSRC = SSRC;
    this->PT = PT;
}

switchSSRCProcessor_t::~switchSSRCProcessor_t(void)
{
}

u32
switchSSRCProcessor_t::getSSRC(void)
{
    return SSRC;
}

int
switchSSRCProcessor_t::getPT(void)
{
    return PT;
}

HRESULT
switchSSRCProcessor_t::deliver(RTPPacket_t *pkt)
{
    pkt->setSSRC(SSRC);

    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *target = targetArray.elementAt(i);

        int payT = pkt->getPayloadType();
        if (payT == target->getPT())
        {
            target->deliver(pkt);
        }
    }
    return S_OK;
}

bool
switchSSRCProcessor_t::operator==(switchSSRCProcessor_t *processor)
{
    return true;
}

bool
switchSSRCProcessor_t::operator==(switchSSRCProcessor_t processor)
{
    return true;
}

switcherSSRC_t::switcherSSRC_t(void)
: flowProcessor_t(FLOW_SWITCH_SSRC)
{
}

switcherSSRC_t::~switcherSSRC_t(void)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            while (outFlowMatrix[i]->size())
            {
                switchSSRCProcessor_t *switchSSRCProcessor =
                    static_cast<switchSSRCProcessor_t *>(outFlowMatrix[i]->elementAt(0)->processor);
                APP->removeTask(switchSSRCProcessor);
                outFlowMatrix[i]->remove(0);
            }
            delete outFlowMatrix[i];
            outFlowMatrix[i] = NULL;
        }
    }
}

HRESULT
switcherSSRC_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
{
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    for (int i = 0; i < outFlowArray->size(); i++)
    {
        outFlow_t *flow = outFlowArray->elementAt(i);
        switchSSRCProcessor_t *switchSSRCProcessor =
            static_cast<switchSSRCProcessor_t *>(flow->processor);

        if ( ! switchSSRCProcessor)
        {
            return E_PROC_NOT_EXISTS;
        }

        //if (switchSSRCProcessor->getPT() != inFlow.PT)
        //    continue;

        switchSSRCProcessor->deliver(pkt);
    }

    return S_OK;
}

switchSSRCProcessor_t *
switcherSSRC_t::getValidProcessor(flow_t outFlow,
                                  vector_t<outFlow_t *> *outFlowArray
                                 )
{
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        switchSSRCProcessor_t *switchSSRCProcessor =
            static_cast<switchSSRCProcessor_t *>(outFlowArray->elementAt(i)->processor);

        if (outFlow.ID == outFlowArray->elementAt(i)->ID &&
            outFlow.PT == switchSSRCProcessor->getPT()
           )
        {
            return switchSSRCProcessor;
        }
    }
    return NULL;
}

HRESULT
switcherSSRC_t::setFlow(flow_t inFlow,
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
    outFlow.ID = inFlow.PT;

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at hole array
    switchSSRCProcessor_t *switchSSRCProcessor =
        getValidProcessor(outFlow, outFlowArray);

    if ( ! switchSSRCProcessor) // create processor and insert in list
    {
        switchSSRCProcessor = new switchSSRCProcessor_t(SSRC, inFlow.PT);
        switchSSRCProcessor->addRef();
        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = target->getID();
        outFlow->processor = switchSSRCProcessor;
        outFlowArray->add(outFlow);
        switchSSRCProcessor->addTarget(target);
    }

    return S_OK;
}

HRESULT
switcherSSRC_t::unsetFlow(flow_t inFlow, target_t *target)
{
    flow_t outFlow;
    outFlow.ID = target->getID();
    outFlow.PT = inFlow.PT;

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    switchSSRCProcessor_t *switchSSRCProcessor =
        getValidProcessor(outFlow, outFlowArray);

    if ( ! switchSSRCProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

    if (switchSSRCProcessor->deleteTarget(target) == 0)
    {
        for (int i = 0; i < outFlowArray->size(); i++)
        {
            if (switchSSRCProcessor == outFlowArray->elementAt(i)->processor)
            {
                outFlowArray->remove(i);
                if (switchSSRCProcessor->decRef() == 0)
                {
                    // this deletes processor
                    APP->removeTask(switchSSRCProcessor);
                }
                break;
            }
        }
    }
    return S_OK;
}

