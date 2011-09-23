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
// $Id: switch.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "sessionManager.h"
#include "switch.h"
#include "returnCode.h"

switchProcessor_t::switchProcessor_t(int PT)
{
    this->PT = PT;
}

switchProcessor_t::~switchProcessor_t(void)
{
}

int
switchProcessor_t::getPT(void)
{
    return PT;
}

HRESULT
switchProcessor_t::deliver(RTPPacket_t *pkt)
{
    for (int i = 0 ; i < targetArray.size(); i++)
    {
        targetArray.elementAt(i)->deliver(pkt);
    }
    return S_OK;
}

bool
switchProcessor_t::operator==(switchProcessor_t *processor)
{
    return true;
}

bool
switchProcessor_t::operator==(switchProcessor_t processor)
{
    return true;
}

switcher_t::switcher_t(void)
: flowProcessor_t(FLOW_SWITCH)
{
}

switcher_t::~switcher_t(void)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            while (outFlowMatrix[i]->size())
            {
                switchProcessor_t *switchProcessor =
                    static_cast<switchProcessor_t *>(outFlowMatrix[i]->elementAt(0)->processor);
                APP->removeTask(switchProcessor);
                outFlowMatrix[i]->remove(0);
            }
            delete outFlowMatrix[i];
            outFlowMatrix[i] = NULL;
        }
    }
}

HRESULT
switcher_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
{
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);
    switchProcessor_t *switchProcessor =
        getValidProcessor(inFlow, outFlowArray);

    if ( ! switchProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

    return switchProcessor->deliver(pkt);
}

switchProcessor_t *
switcher_t::getValidProcessor(flow_t inFlow)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            switchProcessor_t *switchProcessor =
                getValidProcessor(inFlow, outFlowMatrix[i]);
            if (switchProcessor)
            {
                return switchProcessor;
            }
        }
    }
    return NULL;
}

switchProcessor_t *
switcher_t::getValidProcessor(flow_t inFlow,
                              vector_t<outFlow_t *> *outFlowArray
                             )
{
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        switchProcessor_t *switchProcessor =
            static_cast<switchProcessor_t *>(outFlowArray->elementAt(i)->processor);

        if (/*inFlow.ID == outFlowArray->elementAt(i)->ID &&*/
            inFlow.PT == switchProcessor->getPT())
        {
            return switchProcessor;
        }
    }
    return NULL;
}

HRESULT
switcher_t::setFlow(flow_t inFlow,
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
    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at hole array
    switchProcessor_t *switchProcessor1 = getValidProcessor(inFlow);

    if ( ! switchProcessor1) // create processor and insert in list
    {
        switchProcessor1 = new switchProcessor_t(inFlow.PT);
        switchProcessor1->addRef();
        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = target->getID();
        outFlow->processor = switchProcessor1;
        outFlowArray->add(outFlow);
    }
    else
    {
        // find correct processor at my list
        switchProcessor_t *switchProcessor2 =
            getValidProcessor(inFlow, outFlowArray);

        if ( ! switchProcessor2) // only insert processor in list
        {
            switchProcessor1->addRef();
            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = target->getID();
            outFlow->processor = switchProcessor1;
            outFlowArray->add(outFlow);
        }
    }

    switchProcessor1->addTarget(target);

    return S_OK;
}

HRESULT
switcher_t::unsetFlow(flow_t inFlow, target_t *target)
{
    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    switchProcessor_t *switchProcessor =
        getValidProcessor(inFlow, outFlowArray);

    if ( ! switchProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

    if (switchProcessor->deleteTarget(target) == 0)
    {
        for (int i = 0; i < outFlowArray->size(); i++)
        {
            if (switchProcessor == outFlowArray->elementAt(i)->processor)
            {
                outFlowArray->remove(i);
                if (switchProcessor->decRef() == 0)
                {
                    // this deletes processor
                    APP->removeTask(switchProcessor);
                }
                break;
            }
        }
    }
    return S_OK;
}

