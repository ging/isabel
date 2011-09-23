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
// $Id: grid2.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <rtp/RTPUtils.hh>

#include "grid2.h"
#include "sessionManager.h"
#include "returnCode.h"

#include <vCodecs/vUtils.h>

grid2_t::grid2_t(void)
: flowProcessor_t(FLOW_GRID2)
{
    for (int i= 0; i < MAX_VIDEO_SOURCES; i++)
    {
        vDecoderArr[i]= NULL;
    }

    gridProcessor = NULL;
}

grid2_t::~grid2_t(void)
{
    // delete processor
    if (gridProcessor)
    {
        delete gridProcessor;
    }

    // destroy decoderPacks
    for (u16 i = 0; i < MAX_VIDEO_SOURCES; i++)
    {
        if (vDecoderArr[i] != NULL)
        {
            delete vDecoderArr[i];
            vDecoderArr[i]= NULL;
        }
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
    // Find DecoderPack struct
    //--------------------------------------------------
    bool found = false;
    int i= 0;
    int gap = -1;

    for ( ; i < MAX_VIDEO_SOURCES; i++)
    {
        if (vDecoderArr[i] == NULL)
        {
            if (gap == -1) // remember first hole
            {
                gap= i;
            }
            continue;
        }
        if (vDecoderArr[i]->SSRC == pkt->getSSRC())
        {
            found = true;
            break;
        }
    }

    if ( ! found)
    {
        if (gap == -1)
        {
            NOTIFY("grid2_t::deliver: too many decoders, bailing out\n");
            exit(1);
        }

        i = gap;

        // init decoderPack
        vDecoderArr[i]= new vDecoderPack_t(pkt->getSSRC());
    }

    //--------------------------------------------------
    // Defragment frame
    //--------------------------------------------------
    VCimage_t *image = vDecoderArr[i]->process(pkt);

    if (image)
    {
        //--------------------------------------------------
        // Decode video
        //--------------------------------------------------

        // Check if decoder has changed
        vDecoderArgs_t params;
        if ( ! vDecoderArr[i]->checkPT(pkt->getPayloadType()))
        {
            NOTIFY("grid2_t::deliver: can't get decoder PT=%d\n",
                   vDecoderArr[i]->PT
                  );
            delete[] image->buffer;
            delete image;
            return E_VIDEO_DECODE;
        }

        vGetParams(vDecoderArr[i]->Decoder, &params);
        if (image->h &&
            image->w &&
            params.height != image->h &&
            params.width  != image->w
           )
        {
            params.height = image->h;
            params.width  = image->w;
        }

        if (params.height && ! image->h) image->h = params.height;
        if (params.width  && ! image->w) image->w = params.width;

        if (image->h * image->w)
        {
            unsigned int imageSize = image->h * image->w * 3;
            u8 *buffer = new u8[imageSize];
            imageSize = vDecode(vDecoderArr[i]->Decoder,
                                image->buffer,
                                image->numBytes,
                                buffer,
                                imageSize
                               );

            if (imageSize <= 0 ||
                imageSize > image->h * image->w * IMAGE_FACTOR
               )
            {
                NOTIFY("grid2_t::deliver: vDecoder returns %d\n", imageSize);
                delete[] buffer;
                delete[] image->buffer;
                delete image;
                return E_VIDEO_DECODE;
            }

            videoInfo_t info;
            info.Height = image->h;
            info.Width  = image->w;
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
                gridProcessor->deliver(buffer, imageSize, info, inFlow.ID);
            }
            delete[] buffer;
        }
        delete[] image->buffer;
        delete image;
    }

    return S_OK;
}

// to define inFlow-outFlow relationShip
HRESULT
grid2_t::setFlow(flow_t inFlow,
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
        outFlow->ID = target->getID();
        outFlow->processor = gridProcessor;
        outFlowArray->add(outFlow);
    }
    else
    {
        // find correct processor at my list
        gridProcessor_t *gridProcessor2 =
            getValidProcessor(outFlow, outFlowArray);

        if ( ! gridProcessor2)
        {
            // only insert processor in list

            gridProcessor->addRef();
            gridProcessor->addFlowID(inFlow.ID);

            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = target->getID();
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
    outFlow.ID = target->getID();
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

