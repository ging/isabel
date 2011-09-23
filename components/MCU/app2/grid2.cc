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
// $Id: grid2.cc 8518 2006-05-18 09:42:07Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <rtp/RTPPayloads.hh>

#include "grid2.h"
#include "sessionManager.h"
#include "returnCode.h"

grid2_t::grid2_t(void)
: flowProcessor_t(FLOW_GRID2)
{
    gridProcessor = NULL;
}

grid2_t::~grid2_t(void)
{
    // delete processor
    if (gridProcessor)
    {
        delete gridProcessor;
    }

    std::map<u32, rtp2image_t*>::iterator iter;

    // destroy decoderPacks
    for (iter= vDecoderArr.begin(); iter != vDecoderArr.end(); iter++)
    {
        rtp2image_t *tmp= iter->second;

        vDecoderArr.erase(iter->first);

        delete tmp;
    }
}


HRESULT
grid2_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
{
    if (getFlowByPT(pkt->getPayloadType()) != VIDEO_FLOW)
    {
        return S_OK;
    }

    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);
    if (outFlowArray->size() == 0)
    {
        return E_PROC_NOT_EXISTS;
    }

    //--------------------------------------------------
    // Find / Build DecoderPack struct
    //--------------------------------------------------
    u32 SSRC= pkt->getSSRC();

    rtp2image_t *rtp2image= vDecoderArr[SSRC];

    if (rtp2image == NULL)
    {
        vDecoderArr[SSRC]= new rtp2image_t(2); // 2 frames max stored
    }

    //--------------------------------------------------
    // Defragment frame
    //--------------------------------------------------
    image_t *newImage = rtp2image->addPkt(pkt);

    if ( ! newImage)
    {
        return E_VIDEO_DECODE;
    }

    // newImage->translate(YUV_420P);

    unsigned int newImageSize = newImage->getNumBytes();
    u8 *buffer = new u8[newImageSize];

    memcpy(buffer, newImage->getBuff(), newImageSize);
 
    videoInfo_t info;
    info.Width  = newImage->getWidth();
    info.Height = newImage->getHeight();
    info.PT     = pkt->getPayloadType();
    info.SSRC   = pkt->getSSRC();
    info.BW     = 0;
    info.FR     = 25;

    // send sample to every valid processor
    for (u16 i = 0; i < outFlowArray->size(); i++)
    {
        gridProcessor_t *gridProcessor =
            static_cast<gridProcessor_t*>(outFlowArray->elementAt(i)->processor);
        // send
        gridProcessor->deliver(buffer, newImageSize, info, inFlow.ID);
    }

    delete []buffer;
    delete newImage;

    return S_OK;
}

// to define inFlow-outFlow relationShip
HRESULT
grid2_t::setFlow(flow_t inFlow,
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

    // find correct processor at my list
    gridProcessor_t *gridProcessor2 = getValidProcessor(outFlow, outFlowArray);

    if ( ! gridProcessor)
    {
        // create processor (unique) and insert in list

        videoInfo_t info;
        info.BW = BW;
        info.FR = FR;
        info.SSRC = SSRC;
        info.videoMode = VIDEO_GRID2_MODE;
        info.Width = width;
        info.Height = height;
        info.PT = PT;
        info.Q = Q;
        info.gridMode = gridMode;
        if (BW)
        {
            info.rateMode = CBR;
        }
        else
        {
            info.rateMode = VBR;
        }

        gridProcessor = new gridProcessor_t(info);
        gridProcessor->addRef();
        gridProcessor->addFlowID(inFlow.ID);

        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = inFlow.ID;
        outFlow->processor = gridProcessor;
        outFlowArray->add(outFlow);
    }
    else
    {
        if ( ! gridProcessor2)
        {
            // only insert processor in list

            gridProcessor->addRef();
            gridProcessor->addFlowID(inFlow.ID);

            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = inFlow.ID;
            outFlow->processor = gridProcessor;
            outFlowArray->add(outFlow);
        }
    }
    gridProcessor->addTarget(target);
    return S_OK;
}

HRESULT
grid2_t::unsetFlow(flow_t inFlow, target_t *target)
{
    flow_t outFlow;
    outFlow.IP = target->getIP();
    outFlow.PT = target->getPT();

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    gridProcessor_t *gridProcessor = getValidProcessor(outFlow, outFlowArray);

    if ( ! gridProcessor)
    {
        return E_PROC_NOT_EXISTS;
    }

    // remove from the GRID
    gridProcessor->delFlowID(inFlow.ID);

    // now we must delete mixProcessor from position
    for (u16 i = 0; i < outFlowArray->size(); i++)
    {
        if (gridProcessor == outFlowArray->elementAt(i)->processor)
        {
            outFlowArray->remove(i);
            if (gridProcessor->decRef() == 0)
            {
                APP->removeTask(gridProcessor); // this deletes processor
            }
            break;
        }
    }
    return S_OK;
 }

gridProcessor_t*
grid2_t::getValidProcessor(flow_t outFlow, vector_t<outFlow_t *> *outFlowArray)
{
    for (u16 i = 0; i < outFlowArray->size(); i++)
    {
        gridProcessor_t *gridProcessor =
            static_cast<gridProcessor_t *>(outFlowArray->elementAt(i)->processor);

        return gridProcessor;
    }
    return NULL;
}

