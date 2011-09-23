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
// $Id: flowProcessor.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "flowProcessor.h"
#include "sessionManager.h"
#include "returnCode.h"

processor_t::processor_t(void)
{
    referenceCount = 0;
    set_period(0);
    APP->insertTask(this);
}

processor_t::~processor_t(void)
{
}

HRESULT
processor_t::addTarget(target_t *target)
{
    // check if target already exists
    for (int i = 0; i < targetArray.size(); i++)
    {
        target_t *auxTarget = targetArray.elementAt(i);
        if (target == auxTarget)
        {
            return S_OK;
        }
    }

    targetArray.add(target);
    return S_OK;
}

HRESULT
processor_t::deleteTarget(target_t *target)
{
    for (u16 i = 0; i < targetArray.size(); i++)
    {
        target_t *auxTarget = targetArray.elementAt(i);
        if (target->getID() == auxTarget->getID())
        {
            targetArray.remove(i);
            return S_OK;
        }
    }
    return E_ERROR;
}

u16
processor_t::addRef(void)
{
    referenceCount++;
    return referenceCount;
}

u16
processor_t::decRef(void)
{
    if (referenceCount!=0)
    {
        referenceCount--;
    }
    return referenceCount;
}

HRESULT
processor_t::deliver(RTPPacket_t *pkt)
{
    return E_NOT_IMPLEMENTED;
}

outFlow_t::outFlow_t(flow_t inFlow)
{
    this->inFlow = inFlow;
    ptr = NULL;
    processor = NULL;
    ID = 0;
}

outFlow_t::~outFlow_t(void)
{
    if (processor)
    {
        APP->removeTask(processor);
    }
    if (ptr)
    {
    //    delete ptr;
    }
}

flowProcessor_t::flowProcessor_t(flowProcessor_e type)
{
    flowProcessorType = type;
    memset(outFlowMatrix, 0, sizeof(vector_t<outFlow_t*>*)*MAX_FLOW_LEN);
}

flowProcessor_t::~flowProcessor_t(void)
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

vector_t<outFlow_t *> *
flowProcessor_t::getProcessorArray(flow_t& inFlow)
{
    // u16 position = getIndex(inFlow.IP);

    if ( ! outFlowMatrix[inFlow.ID])
    {
        outFlowMatrix[inFlow.ID] = new vector_t<outFlow_t *>;
    }

    return outFlowMatrix[inFlow.ID];
}

flowProcessor_e
flowProcessor_t::getFlowProcessorType(void)
{
    return flowProcessorType;
}

HRESULT
flowProcessor_t::deleteProcessor(processor_t *processor, flow_t inFlow)
{
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    for (int i = 0; i < outFlowArray->size(); i++)
    {
        if (processor == outFlowArray->elementAt(i)->processor)
        {
            outFlowArray->remove(i);
            if (outFlowArray->size() == 0)
            {
                delete outFlowArray;
                outFlowMatrix[inFlow.ID] = NULL;
                break;
            }
        }
    }

    APP->removeTask(processor);

    return S_OK;
}

HRESULT
flowProcessor_t::deleteTarget(target_t *target)
{
test:
    u16 i = 0;
    for (i= 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            for (u16 j = 0; j < outFlowMatrix[i]->size(); j++)
            {
               if (outFlowMatrix[i]->elementAt(j)->processor->deleteTarget(target)==S_OK)
               {
                    if (outFlowMatrix[i]->elementAt(j)->processor->decRef()==0)
                    {
                        // remove processor
                        APP->removeTask(outFlowMatrix[i]->elementAt(j)->processor);

                        for (long k = 0; k < MAX_FLOW_LEN; k++)
                        {
                            // remove processor references
                            // in flow array

                            if (outFlowMatrix[k] &&
                                outFlowMatrix[k] != outFlowMatrix[i])
                            {
                                for (u16 n = 0; n < outFlowMatrix[k]->size(); n++)
                                {
                                    if (outFlowMatrix[i]->elementAt(j)->processor ==
                                        outFlowMatrix[k]->elementAt(n)->processor
                                        )
                                    {
                                        outFlowMatrix[k]->remove(n);
                                        if (outFlowMatrix[k]->size() == 0)
                                        {
                                            delete outFlowMatrix[k];
                                            outFlowMatrix[k] = NULL;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        outFlowMatrix[i]->remove(j);
                        if (outFlowMatrix[i]->size() == 0)
                        {
                            delete outFlowMatrix[i];
                            outFlowMatrix[i] = NULL;
                            goto test;
                        }
                    }
                }
            }
        }
    }
    return S_OK;
}

