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
// $Id: switch.cc 7138 2005-07-15 17:02:44Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include "sessionManager.h"
#include "switch.h"
#include "returnCode.h"

switchProcessor_t::switchProcessor_t(u8 PT,u32 SSRC)
{
    this->PT = PT;
    this->SSRC = SSRC;
}

switchProcessor_t::~switchProcessor_t(void)
{
}

u8
switchProcessor_t::getPT(void)
{
    return PT;
}

HRESULT
switchProcessor_t::deliver(RTPPacket_t *pkt)
{
    if (SSRC)
    {
        pkt->setSSRC(SSRC);
    }
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
switcher_t::getValidProcessor(flow_t inFlow, u32 SSRC)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            switchProcessor_t *switchProcessor =
                getValidProcessor(inFlow, outFlowMatrix[i], SSRC);
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
                              vector_t<outFlow_t *> *outFlowArray, u32 SSRC
                             )
{
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        switchProcessor_t *switchProcessor =
            static_cast<switchProcessor_t *>(outFlowArray->elementAt(i)->processor);

        if (inFlow.ID == outFlowArray->elementAt(i)->ID &&
            inFlow.PT == switchProcessor->getPT()
           )
        {
            if (!SSRC || switchProcessor->getSSRC() == SSRC) // Does this fix the bad SSRC problem?
                return switchProcessor;
        }
    }
    return NULL;
}

HRESULT
switcher_t::setFlow(flow_t inFlow,
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

    // find correct processor at whole array
    switchProcessor_t *switchProcessor1 = getValidProcessor(inFlow, SSRC);

    // find correct processor at my list
    switchProcessor_t *switchProcessor2 =
        getValidProcessor(inFlow, outFlowArray, SSRC);

    if ( ! switchProcessor1) // create processor and insert in list
    {
        switchProcessor1 = new switchProcessor_t(inFlow.PT,SSRC);
        switchProcessor1->addRef();
        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = inFlow.ID;
        outFlow->processor = switchProcessor1;
        outFlowArray->add(outFlow);
    }
    else
    {
        if ( ! switchProcessor2) //only insert processor in list
        {
            switchProcessor1->addRef();
            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = inFlow.ID;
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

    if (switchProcessor->deleteTarget(target)==0)
    {
        if (switchProcessor->decRef()==0)
        {
            deleteTarget(target); // this deletes processor
        }
    }
    return S_OK;
}

