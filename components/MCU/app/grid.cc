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
// $Id: grid.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPUtils.hh>

#include "grid.h"
#include "sessionManager.h"
#include "returnCode.h"

// grid composers
#include "gridComposer/gridAuto.h"
#include "gridComposer/gridBorder1.h"
#include "gridComposer/gridBorderTR.h"
#include "gridComposer/gridPip.h"

// masks
#include "gridComposer/circleMask.h"

#include <vCodecs/vUtils.h>

gridProcessor_t::gridProcessor_t(videoInfo_t info)
{
    time1.tv_sec = time2.tv_sec = 0;
    time1.tv_usec = time2.tv_usec = 0;

    videoInfo = info;
    Coder = NULL;

    // reset window positions
    memset(windowPos, 0, MAX_VIDEO_SOURCES * sizeof(window_t));

    // new seed
    srand((unsigned)::time(NULL));
#if defined(WIN32)
    if ( ! videoInfo.SSRC)
    {
        videoInfo.SSRC = rand();
    }
    SQ   = rand() ;
    TS   = rand() ;
#elif defined(__BUILD_FOR_LINUX)
    if ( ! videoInfo.SSRC)
    {
        videoInfo.SSRC = random();
    }
    SQ   = random();
    TS   = random();
#else
#error "Please, initilized gridProcessor_t for your OS"
#endif

    double period = 1.0 / info.FR * 1000000.0;
    set_period((int)period);

    image.numBytes = info.Height * info.Width * IMAGE_FACTOR;
    image.buffer = new u8[image.numBytes];
    image.h = videoInfo.Height;
    image.w = videoInfo.Width;

    mask = NULL;

    unsigned imageMaskSize;
    unsigned maskSize;

    switch (info.gridMode)
    {
    case GRID_AUTO:
        gridComposer= new gridAuto_t(info.Width, info.Height);
        break;
    case GRID_BORDER:
        gridComposer= new gridBorder1_t(info.Width, info.Height);
        break;
    case GRID_BORDER_TR:
        gridComposer= new gridBorderTR_t(info.Width, info.Height);
        break;
    case GRID_PIP:
        gridComposer= new gridPip_t(GRID_PIP,info.Width, info.Height);

        maskSize = info.Width * info.Height * IMAGE_FACTOR;
        mask = new u8[imageMaskSize];
        memset(mask, 1, imageMaskSize);

        break;
    case GRID_CIRCLE:
        imageMaskSize = circle_mask.h * circle_mask.w * IMAGE_FACTOR;
        maskSize = info.Width * info.Height * IMAGE_FACTOR;
        mask = new u8[maskSize];
        vSetMask(mask,
                 maskSize,
                 circle_mask.mask,
                 circle_mask.w,
                 circle_mask.h,
                 info.Width,
                 info.Height,
                 1,
                 VIDEO_FORMAT
                );

        gridComposer= new gridPip_t(GRID_CIRCLE,info.Width, info.Height);

        break;
    default:
        NOTIFY("gridProcessor_t::gridProcessor_t: "
               "unknown grid mode\n!. Aborting...\n"
              );
        abort();
    }

    // create codec
    params.bitRate   = info.BW;
    params.frameRate = info.FR;
    params.height    = info.Height;
    params.width     = info.Width;
    params.quality   = info.Q;
    params.format    = VIDEO_FORMAT;

    Coder = vGetCoderByFmt(getFmtByPT(info.PT), &params);
    if ( ! Coder)
    {
        NOTIFY("gridProcessor_t::gridProcessor_t: can't create codec PT=%d\n",
               info.PT
              );
        abort();
    }
}


gridProcessor_t::~gridProcessor_t(void)
{
    if (mask)
    {
        delete[] mask;
    }
}

bool
gridProcessor_t::isValid(flow_t inFlow)
{
    if (inFlow.PT == videoInfo.PT)
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

HRESULT
gridProcessor_t::addFlowID(int flowID)
{
    resetI420PImage();
    gridComposer->addFlowID(flowID);
    if (videoInfo.gridMode == GRID_PIP)
    {
        window_t window;
        HRESULT res= gridComposer->getWindow(flowID, window);
        if ( FAILED (res) )
        {
            NOTIFY("gridProcessor_t::addFlow: "
                   "could not find geometry for flowID=%d\n",
                   flowID
                  );
        }

        vSetMaskRect(mask,
                     window.w,
                     window.h,
                     window.x,
                     window.y,
                     videoInfo.Width,
                     videoInfo.Height,
                     gridComposer->getMask(flowID) == MASK_NORMAL ? 1 : 0,
                     VIDEO_FORMAT
                    );
    }
    return S_OK;
}

HRESULT gridProcessor_t::delFlowID(int flowID)
{
    resetI420PImage();
    gridComposer->delFlowID(flowID);
    memset(&windowPos[flowID], 0, sizeof(window_t));

    if (videoInfo.gridMode == GRID_PIP)
    {
        window_t window;
        HRESULT res= gridComposer->getWindow(flowID, window);
        if ( FAILED (res) )
        {
            NOTIFY("gridProcessor_t::delFlow: "
                   "could not find geometry for flowID=%d\n",
                   flowID
                  );
        }

        vSetMaskRect(mask,
                     window.w,
                     window.h,
                     window.x,
                     window.y,
                     videoInfo.Width,
                     videoInfo.Height,
                     gridComposer->getMask(flowID) == MASK_NORMAL ? 0 : 1,
                     VIDEO_FORMAT
                    );
    }
    return S_OK;
}

#define DELTA_GRID 5

HRESULT
gridProcessor_t::deliver(unsigned char *data,  // yuv420p image data
                         unsigned int   len,   // image len
                         videoInfo_t    info,  // image info
                         int            flowID // flow Identifier
                        )
{
    //-------------------------------------------
    // In this method, we receive a yuv image
    // and must mix it in grid image
    //-------------------------------------------
    if (videoInfo.gridMode == GRID_AUTO)
    {
        window_t window;
        gridComposer->getWindow(flowID, window);
#if 0

        if ( ! windowPos[flowID].w)
        {
            windowPos[flowID].w = 1;
            windowPos[flowID].h = 1;
            windowPos[flowID].x = window.x;
            windowPos[flowID].y = window.y;
        }
        else
        {
            bool changed = false;
            window_t previous = windowPos[flowID];

            if (windowPos[flowID].w < window.w)
            {
                windowPos[flowID].w+=DELTA_GRID;
            }
            else if (windowPos[flowID].w > window.w + DELTA_GRID)
            {
                windowPos[flowID].w-=DELTA_GRID;
                changed = true;
            }
            else if (windowPos[flowID].w != window.w)
            {
                changed = true;
                windowPos[flowID].w = window.w;
            }

            if (windowPos[flowID].h < window.h)
            {
                windowPos[flowID].h+=DELTA_GRID;
            }
            else if (windowPos[flowID].h > window.h + DELTA_GRID)
            {
                windowPos[flowID].h-=DELTA_GRID;
                changed = true;
            }
            else if (windowPos[flowID].h != window.h)
            {
                changed = true;
                windowPos[flowID].h = window.h;
            }

            if (windowPos[flowID].x < window.x)
            {
                windowPos[flowID].x+=DELTA_GRID;
                changed = true;
            }
            else if (windowPos[flowID].x > window.x + DELTA_GRID)
            {
                windowPos[flowID].x-=DELTA_GRID;
                changed = true;
            }
            else if (windowPos[flowID].x != window.x)
            {
                changed = true;
                windowPos[flowID].x = window.x;
            }

            if (windowPos[flowID].y < window.y)
            {
                windowPos[flowID].y+=DELTA_GRID;
                changed = true;
            }
            else if (windowPos[flowID].y > window.y + DELTA_GRID)
            {
                windowPos[flowID].y-=DELTA_GRID;
                changed = true;
            }
            else if (windowPos[flowID].y != window.y)
            {
                changed = true;
                windowPos[flowID].y = window.y;
            }

            if (changed)
            {
                resetI420PImage(previous);
            }
        }
#else
        windowPos[flowID] = window;
#endif
    }
    else
    {
        if (windowPos[flowID].w*windowPos[flowID].h == 0)
        {
            window_t window;
            gridComposer->getWindow(flowID, window);
            windowPos[flowID] = window;
        }
    }

    vPutImage(data,
              len,
              image.buffer,
              image.numBytes,
              info.Width,
              info.Height,
              windowPos[flowID].w,
              windowPos[flowID].h,
              windowPos[flowID].x,
              windowPos[flowID].y,
              image.w,
              image.h,
              VIDEO_FORMAT,
              mask,
              gridComposer->getMask(flowID)==MASK_INVERTED?true:false
             );

    return S_OK;
}

void
gridProcessor_t::heartBeat(void)
{
    //-------------------------------------------
    // In this method, we code the image,
    // fragment it and send by RTP/UDP protocol
    //-------------------------------------------

    // refresh timeStamp
    gettimeofday(&time2, NULL);
    if ( ! time1.tv_sec)
    {
        time1 = time2;
    }
    double delta = (time2.tv_sec + time2.tv_usec/1000000) -
                   (time1.tv_sec + time1.tv_sec /1000000);

    if (delta < 10) delta = (1.0/videoInfo.FR)/VIDEO_RTP_TSU;

    TS += (u32)delta;

    int outBuffLen = image.numBytes;
    u8 *outBuff = new u8[outBuffLen];

    outBuffLen = vEncode(Coder,image.buffer,image.numBytes,outBuff,outBuffLen);

    if (outBuffLen <= 0)
    {
        NOTIFY("gridProcessor_t::heartBeat: vEncoded[%d] returns %d\n",
               videoInfo.PT,
               outBuffLen
              );
        return;
    }

    u8 linePT= fragmenter.setFrame(outBuff,
                                   outBuffLen,
                                   Coder->format,
                                   image.w,
                                   image.h
                                  );
    assert (linePT == videoInfo.PT);

    int n = 0;
    int size = MTU_SIZE;
    u8 fragment[MTU_SIZE];

    u8 RTPData[MTU_SIZE+sizeof(RTPHeader_t)];

    while((n = fragmenter.getFragment(fragment,size)) >= 0)
    {
        //-------------------------------------------
        // create RTPPacket
        //-------------------------------------------
        memcpy(RTPData+sizeof(RTPHeader_t),fragment,size);
        RTPHeader_t * header= (RTPHeader_t *)RTPData;
        header->version     = RTP_VERSION;
        header->extension   = 0;
        header->marker      = n?0:1;
        header->padding     = 0;
        header->cc          = 0;
        header->seqnum      = htons(SQ++);
        header->timestamp   = htonl(TS);
        header->ssrc        = htonl(videoInfo.SSRC);
        header->payloadtype = videoInfo.PT;
        RTPPacket_t *pkt    = new RTPPacket_t(RTPData,size+sizeof(RTPHeader_t));

        //-------------------------------------------
        // Send sample
        //-------------------------------------------
        for (u16 j = 0 ; j < targetArray.size(); j++)
        {
            targetArray.elementAt(j)->deliver(pkt);
        }

        delete pkt;
    }
    delete[] outBuff;
}

void
gridProcessor_t::resetI420PImage(window_t window)
{
    int size= image.w * image.h;
    double luminance = 100;
    double decrement = luminance/image.h*2;

    if (window.w + window.x > image.w)
        window.w = image.w - window.x;
    if (window.h + window.y > image.h)
        window.h = image.h - window.y;

    for (unsigned int i = 0; i < image.h-1; i++)
    {
        if (i<window.h+window.y && i>=window.y)
        {
            memset(image.buffer+i*image.w+window.x,abs((int)luminance), window.w);
            memset(image.buffer+size+i*image.w/4+window.x/2, 150, window.w/2);
            memset(image.buffer+size*5/4+i*image.w/4+window.x/2, 128, window.w/2);
        }
        luminance -= decrement;
    }
}

void
gridProcessor_t::resetI420PImage(void)
{
    int size= image.w * image.h;
    double luminance = 100;
    double decrement = luminance/image.h*2;

    for (unsigned int i = 0; i < image.h; i++)
    {
        memset(image.buffer+i*image.w,abs((int)luminance), image.w);
        luminance -= decrement;
    }
    memset(image.buffer + size,     150, size/4);
    memset(image.buffer + 5*size/4, 128, size/4);
}

grid_t::grid_t(void)
: flowProcessor_t(FLOW_GRID)
{
    for (int i= 0; i < MAX_VIDEO_SOURCES; i++)
    {
        vDecoderArr[i]= NULL;
    }

}

grid_t::~grid_t(void)
{
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
grid_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
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
            NOTIFY("grid_t::deliver: can't get decoder PT=%d\n",
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
                NOTIFY("grid_t::deliver: vDecoder returns %d\n", imageSize);
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
grid_t::setFlow(flow_t inFlow,
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

    // find correct processor at hole array
    gridProcessor_t *gridProcessor1 = getValidProcessor(outFlow);

    if ( ! gridProcessor1)
    {
        // create processor and insert in list

        videoInfo_t info;
        info.BW = BW;
        info.FR = FR;
        info.SSRC = SSRC;
        info.videoMode = VIDEO_GRID_MODE;
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

        gridProcessor1 = new gridProcessor_t(info);
        gridProcessor1->addRef();
        gridProcessor1->addFlowID(inFlow.ID);

        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = target->getID();
        outFlow->processor = gridProcessor1;
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

            gridProcessor1->addRef();
            gridProcessor1->addFlowID(inFlow.ID);

            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = target->getID();
            outFlow->processor = gridProcessor1;
            outFlowArray->add(outFlow);
        }
    }

    gridProcessor1->addTarget(target);

    return S_OK;
}

HRESULT
grid_t::unsetFlow(flow_t inFlow, target_t *target)
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
grid_t::getValidProcessor(flow_t outFlow, vector_t<outFlow_t *> *outFlowArray)
{
    for (u16 i = 0; i < outFlowArray->size(); i++)
    {
        gridProcessor_t *gridProcessor =
            static_cast<gridProcessor_t *>(outFlowArray->elementAt(i)->processor);

        if (gridProcessor->isValid(outFlow))
        {
            return gridProcessor;
        }
    }
    return NULL;
}

gridProcessor_t*
grid_t::getValidProcessor(flow_t outFlow)
{
    for (u16 i = 0; i < MAX_FLOW_LEN; i++)
    {
        if (outFlowMatrix[i])
        {
            gridProcessor_t *gridProcessor =
                getValidProcessor(outFlow, outFlowMatrix[i]);
            if (gridProcessor)
            {
                return gridProcessor;
            }
        }
    }
    return NULL;
}

