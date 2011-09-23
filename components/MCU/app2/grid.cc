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
// $Id: grid.cc 8518 2006-05-18 09:42:07Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <vCodecs/vUtils.h>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include <vUtils/rtp2image.h>

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
    if ( ! videoInfo.SSRC)
    {
        videoInfo.SSRC = RAND() ;
    }
    SQ   = RAND() ;
    TS   = RAND() ;

    double period = 1.0 / info.FR * 1000000.0;
    set_period((int)period);

    int numBytes= vFrameSize(info.Width, info.Height, I420P_FORMAT);
    u8 *data= new u8[numBytes];

    if (data == NULL)
    {
        NOTIFY("gridProcessor_t:: run out of memory, bailing out\n");
        abort();
    }

    image= new image_t(data,
                       numBytes,
                       I420P_FORMAT,
                       videoInfo.Width,
                       videoInfo.Height,
                       0
                      );

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

        maskSize = vFrameSize(info.Width, info.Height, I420P_FORMAT);
        mask = new u8[imageMaskSize];
        memset(mask, 1, imageMaskSize);

        break;
    case GRID_CIRCLE:
        imageMaskSize = vFrameSize(circle_mask.w, circle_mask.h, I420P_FORMAT);
        maskSize = vFrameSize(info.Width, info.Height, I420P_FORMAT);
        mask = new u8[maskSize];
        vSetMask(mask,
                 maskSize,
                 circle_mask.mask,
                 circle_mask.w,
                 circle_mask.h,
                 info.Width,
                 info.Height,
                 1,
                 I420P_FORMAT
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
    params.format    = I420P_FORMAT;

    u32 lineFmt= getFmtByPT(info.PT);

    Coder = vGetCoderByFmt(lineFmt, &params);
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
    if (image)
    {
        delete image;
    }
    if (mask)
    {
        delete []mask;
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
            if (addrEquals(aux->getIP(), inFlow.IP))
            {
                return true;
            }
        }
    }
    return false;
}

HRESULT gridProcessor_t::addFlowID(int flowID) {
    resetI420PImage();
    gridComposer->addFlowID(flowID);
    if (videoInfo.gridMode == GRID_PIP)
    {
        window_t window;
        HRESULT res= gridComposer->getWindow(flowID, window);
        if ( FAILED (res) ) {
            NOTIFY("gridProcessor_t::addFlow: "
                    "could not find geometry for flowID=%d\n",
                    flowID);
        }

        vSetMaskRect(mask,
                     window.w,
                     window.h,
                     window.x,
                     window.y,
                     videoInfo.Width,
                     videoInfo.Height,
                     gridComposer->getMask(flowID)==MASK_NORMAL?1:0,
                     I420P_FORMAT
                     );
    }
    return S_OK;
}

HRESULT gridProcessor_t::delFlowID(int flowID) {
    resetI420PImage();
    gridComposer->delFlowID(flowID);
    memset(&windowPos[flowID],0,sizeof(window_t));

    if (videoInfo.gridMode == GRID_PIP)
    {
        window_t window;
        HRESULT res= gridComposer->getWindow(flowID, window);
        if ( FAILED (res) ) {
            NOTIFY("gridProcessor_t::delFlow: "
                "could not find geometry for flowID=%d\n",
                flowID);
        }

        vSetMaskRect(mask,
                     window.w,
                     window.h,
                     window.x,
                     window.y,
                     videoInfo.Width,
                     videoInfo.Height,
                     gridComposer->getMask(flowID)==MASK_NORMAL?0:1,
                     I420P_FORMAT
                     );
    }
    return S_OK;
}

#define DELTA_GRID 5

HRESULT
gridProcessor_t::deliver(unsigned char *data,  // yuv420p image data
                         unsigned int len,     // image len
                         videoInfo_t info,     // image info
                         int         flowID    // flow Identifier
                        )
{
    //-------------------------------------------
    //In this method, we receive a yuv image
    //and must mix it in grid image
    //-------------------------------------------
    if (videoInfo.gridMode == GRID_AUTO)
    {
        window_t window;
        gridComposer->getWindow(flowID, window);
#if 0

        if(!windowPos[flowID].w)
        {
            windowPos[flowID].w = 1;
            windowPos[flowID].h = 1;
            windowPos[flowID].x = window.x;
            windowPos[flowID].y = window.y;
        }else{
            bool changed = false;
            window_t previous = windowPos[flowID];

            if (windowPos[flowID].w < window.w){
                windowPos[flowID].w+=DELTA_GRID;
            }else if (windowPos[flowID].w > window.w + DELTA_GRID){
                windowPos[flowID].w-=DELTA_GRID;
                changed = true;
            }else if (windowPos[flowID].w != window.w){
                    changed = true;
                    windowPos[flowID].w = window.w;
            }

            if (windowPos[flowID].h < window.h){
                windowPos[flowID].h+=DELTA_GRID;
            }else if (windowPos[flowID].h > window.h + DELTA_GRID){
                windowPos[flowID].h-=DELTA_GRID;
                changed = true;
            }else if (windowPos[flowID].h != window.h){
                    changed = true;
                    windowPos[flowID].h = window.h;
            }

            if (windowPos[flowID].x < window.x){
                windowPos[flowID].x+=DELTA_GRID;
                changed = true;
            }else if (windowPos[flowID].x > window.x + DELTA_GRID){
                windowPos[flowID].x-=DELTA_GRID;
                changed = true;
            }else if (windowPos[flowID].x != window.x){
                    changed = true;
                    windowPos[flowID].x = window.x;
            }

            if (windowPos[flowID].y < window.y){
                windowPos[flowID].y+=DELTA_GRID;
                changed = true;
            }else if (windowPos[flowID].y > window.y + DELTA_GRID){
                windowPos[flowID].y-=DELTA_GRID;
                changed = true;
            }else if (windowPos[flowID].y != window.y){
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
              image->getBuff(),
              image->getNumBytes(),
              info.Width,
              info.Height,
              windowPos[flowID].w,
              windowPos[flowID].h,
              windowPos[flowID].x,
              windowPos[flowID].y,
              image->getWidth(),
              image->getHeight(),
              I420P_FORMAT,
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

    int outBuffLen = image->getNumBytes();
    u8 *outBuff = new u8[outBuffLen];

    outBuffLen = vEncode(Coder,
                         image->getBuff(),
                         image->getNumBytes(),
                         outBuff,
                         outBuffLen
                        );

    if (outBuffLen <= 0)
    {
        NOTIFY("gridProcessor_t::heartBeat: vEncoded[%d] returns %d\n",
               videoInfo.PT,
               outBuffLen
              );
        delete[] outBuff;
        return;
    }

    u8 linePT= fragmenter.setFrame(outBuff,
                                   outBuffLen,
                                   Coder->format,
                                   image->getWidth(),
                                   image->getHeight()
                                  );
    assert (linePT == videoInfo.PT);

    int n = 0;
    int size = MTU_SIZE;
    u8 fragment[MTU_SIZE];

    u8 RTPData[MTU_SIZE+sizeof(RTPHeader_t)];

    while ((n = fragmenter.getFragment(fragment,size)) >= 0)
    {
        //-------------------------------------------
        // create RTPPacket
        //-------------------------------------------
        memcpy(RTPData+sizeof(RTPHeader_t), fragment, size);
        RTPHeader_t *header = (RTPHeader_t *)RTPData;
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
    int size= image->getWidth() * image->getHeight();
    double luminance = 100;
    double decrement = luminance / image->getHeight() * 2;

    if (window.w + window.x > image->getWidth())
        window.w = image->getWidth() - window.x;
    if (window.h + window.y > image->getHeight())
        window.h = image->getHeight() - window.y;

    for (unsigned int i = 0; i < image->getHeight()-1; i++)
    {
        if (i<window.h+window.y && i>=window.y)
        {
            memset(image->getBuff()+i*image->getWidth()+window.x,(int)(abs(luminance)), window.w);
            memset(image->getBuff()+size+i*image->getWidth()/4+window.x/2,(int)150, window.w/2);
            memset(image->getBuff()+size*5/4+i*image->getWidth()/4+window.x/2,(int)128, window.w/2);
        }
        luminance -= decrement;
    }
}

void
gridProcessor_t::resetI420PImage(void)
{
    int size= image->getWidth() * image->getHeight();
    double luminance = 100;
    double decrement = luminance / image->getHeight() * 2;

    for (unsigned int i = 0; i < image->getHeight(); i++)
    {
        memset(image->getBuff()+i*image->getWidth(),(int)(abs(luminance)), image->getWidth());
        luminance -= decrement;
    }
    memset(image->getBuff() + size,     150, size/4);
    memset(image->getBuff() + 5*size/4, 128, size/4);
}

grid_t::grid_t(void)
: flowProcessor_t(FLOW_GRID)
{
}

grid_t::~grid_t(void)
{
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
grid_t::setFlow(flow_t inFlow,
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

    // find correct processor at hole array
    gridProcessor_t *gridProcessor1 = getValidProcessor(outFlow);

    // find correct processor at my list
    gridProcessor_t *gridProcessor2 = getValidProcessor(outFlow, outFlowArray);

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
        outFlow->ID = inFlow.ID;
        outFlow->processor = gridProcessor1;
        outFlowArray->add(outFlow);
    }
    else
    {
        if ( ! gridProcessor2)
        {
            // only insert processor in list

            gridProcessor1->addRef();
            gridProcessor1->addFlowID(inFlow.ID);

            outFlow_t *outFlow = new outFlow_t(inFlow);
            outFlow->ID = inFlow.ID;
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

