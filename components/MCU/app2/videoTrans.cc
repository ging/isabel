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
// $Id: videoTrans.cc 9149 2006-10-11 11:25:48Z fec $
//
/////////////////////////////////////////////////////////////////////////

#include <vCodecs/vUtils.h>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include "sessionManager.h"
#include "videoTrans.h"

videoTransProcessor_t::videoTransProcessor_t(videoInfo_t info)
{
    videoInfo = info;
    tmpInfo = info;
    Coder = NULL;

    // new seed
    srand((unsigned)::time(NULL));
#if defined(WIN32)
    SQ   = rand() ;
    TS   = rand() ;
#elif defined(__BUILD_FOR_LINUX)
    SQ   = random();
    TS   = random();
#else
#error "Please, initilized videoTransProcessor_t for your OS"
#endif

    gettimeofday(&time1, NULL);
    time2 = time1;
}

videoTransProcessor_t::~videoTransProcessor_t(void)
{
    if (Coder)
    {
        vDeleteCoder(Coder);
    }
}

bool
videoTransProcessor_t::isValid(videoInfo_t info)
{
    if (videoInfo.BW == info.BW &&
        videoInfo.Q  == info.Q  &&
        videoInfo.FR == info.FR &&
        videoInfo.SSRC == info.SSRC &&
        videoInfo.Width == info.Width &&
        videoInfo.Height == info.Height
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}

HRESULT
videoTransProcessor_t::deliver(u8 *data, unsigned int len, videoInfo_t info)
{
    if (videoInfo.FR > 0)
    {
        gettimeofday(&time2, NULL);
        int diff = (time2.tv_sec*1000000 + time2.tv_usec)
                   - (time1.tv_sec*1000000 + time1.tv_usec);
        int frame_interval = 1000000/videoInfo.FR; // frame interval in usec;
        if (diff < frame_interval)
        {
            return S_OK;
        }
        if (diff > 100000)
        {
            time1= time2; // if time too big, resync times
        }
        else
        {
            time1.tv_sec  =  time1.tv_sec  + (time1.tv_usec + frame_interval)/1000000;
            time1.tv_usec = (time1.tv_usec + frame_interval)%1000000;
        }
    }

    //-------------------------------------------
    // check if output resolution is static and if
    // re-scaling is needed
    //-------------------------------------------

    u8 *imageToEnc = data;
    unsigned int imageToEncLen = len;

    if (  videoInfo.Width     &&
          videoInfo.Height    &&
         (videoInfo.Width != info.Width || videoInfo.Height != videoInfo.Height)
       )
    {
        imageToEncLen = vFrameSize(videoInfo.Width, videoInfo.Height, I420P_FORMAT);

        imageToEnc = new u8[imageToEncLen];
        int ret = vRescale (data,
                            len,
                            imageToEnc,
                            imageToEncLen,
                            info.Width,
                            info.Height,
                            videoInfo.Width,
                            videoInfo.Height,
                            I420P_FORMAT
                           );
        if (ret <= 0)
        {
            NOTIFY("videoTransProcessor_t::deliver: "
                   "error rescaling [%d]\n",
                   ret
                  );
            delete[] imageToEnc;
            return E_VIDEO_SCALE;
        }
    }

    // if params are not static and
    // are defined in "info" then take them
    if (videoInfo.PT     != -1) info.PT = videoInfo.PT;
    if (videoInfo.BW     !=  0) info.BW = videoInfo.BW;
    if (videoInfo.FR     !=  0) info.FR = videoInfo.FR;
    if (videoInfo.Height !=  0) info.Height = videoInfo.Height;
    if (videoInfo.Width  !=  0) info.Width = videoInfo.Width;
    if (videoInfo.SSRC   !=  0) info.SSRC = videoInfo.SSRC;

    //-----------------------
    // tmpInfo = info;
    //-----------------------
    if (info.PT     != -1) tmpInfo.PT = info.PT;
    if (info.BW     !=  0) tmpInfo.BW = info.BW;
    if (info.FR     !=  0) tmpInfo.FR = info.FR;
    if (info.Height !=  0) tmpInfo.Height = info.Height;
    if (info.Width  !=  0) tmpInfo.Width = info.Width;
    if (info.SSRC   !=  0) tmpInfo.SSRC = info.SSRC;

    vCoderArgs_t params;
    if (Coder)
    {
        //---------------------------------
        // check if actualParams are valid
        //---------------------------------
        vGetParams(Coder, &params);

        if (params.bitRate != tmpInfo.BW   ||
            params.frameRate != tmpInfo.FR ||
            params.height != tmpInfo.Height||
            params.width != tmpInfo.Width  ||
            params.quality != tmpInfo.Q    ||
            getPTByFmt(Coder->format) != tmpInfo.PT
           )
        {
            vDeleteCoder(Coder);
            Coder = NULL;
        }
    }
    if ( ! Coder)
    {
        // create new codec
        params.bitRate   = tmpInfo.BW;
        params.frameRate = tmpInfo.FR;
        params.height    = tmpInfo.Height;
        params.width     = tmpInfo.Width;
        params.quality   = tmpInfo.Q;
        params.format    = I420P_FORMAT;

        Coder = vGetCoderByFmt(getFmtByPT(tmpInfo.PT), &params);
        if ( ! Coder)
        {
            NOTIFY("videoTrans_t::deliver: "
                   "can't create codec PT=%d\n",
                   tmpInfo.PT
                  );
            return E_VIDEO_CODE;
        }
    }
    vGetParams(Coder, &params);

    int outBuffLen = tmpInfo.Height*tmpInfo.Width*3;
    u8 *outBuff = new u8[outBuffLen];

    outBuffLen = vEncode(Coder,imageToEnc,imageToEncLen,outBuff,outBuffLen);

    if (outBuffLen <= 0) // encoded failed
    {
        NOTIFY("videoTransProcessor_t::deliver: vEncoded[%d] returns %d\n",
               tmpInfo.PT,
               outBuffLen
              );
        return E_VIDEO_CODE;
    }

    u8 linePT= fragmenter.setFrame(outBuff,
                                   outBuffLen,
                                   Coder->format,
                                   tmpInfo.Width,
                                   tmpInfo.Height
                                  );

    int n = 0;
    int size = MTU_SIZE;
    u8 fragment[MTU_SIZE];
    u8 RTPData[MTU_SIZE + sizeof(RTPHeader_t)];

    while ((n = fragmenter.getFragment(fragment, size)) >= 0)
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
        header->ssrc        = htonl(tmpInfo.SSRC);
        header->payloadtype = linePT;

        RTPPacket_t *pkt= new RTPPacket_t(RTPData, size+sizeof(RTPHeader_t));

        //-------------------------------------------
        // Send sample
        //-------------------------------------------
        for (int j = 0; j < targetArray.size(); j++)
        {
            targetArray.elementAt(j)->deliver(pkt);
        }

        delete pkt;
    }

    double delta = (1.0/tmpInfo.FR)/(VIDEO_RTP_TSU);
    TS += (u32)delta;

    if (imageToEnc != data) // if we've resized the image
    {
        delete[] imageToEnc;
    }

    delete[] outBuff;

    return S_OK;
}


videoTrans_t::videoTrans_t(void)
: flowProcessor_t(FLOW_VIDEO_TRANS)
{
    lastPT = 0;
}


videoTrans_t::~videoTrans_t(void)
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
videoTrans_t::deliver(RTPPacket_t *pkt, flow_t inFlow)
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
    for (int i = 0; i < outFlowArray->size(); i++)
    {
        videoTransProcessor_t *videoTransProcessor =
            static_cast<videoTransProcessor_t *>(outFlowArray->elementAt(i)->processor);
        // send
        videoTransProcessor->deliver(buffer, newImageSize, info);
    }

    delete []buffer;
    delete newImage;

    return S_OK;
}

videoTransProcessor_t *
videoTrans_t::getValidProcessor(vector_t<outFlow_t *> *outFlowArray,
                                videoInfo_t info
                               )
{
    for (u16 i = 0; i < outFlowArray->size(); i++)
    {
        videoTransProcessor_t *videoTransProcessor =
            static_cast<videoTransProcessor_t *>(outFlowArray->elementAt(i)->processor);

        if (videoTransProcessor->isValid(info))
        {
            return videoTransProcessor;
        }
    }
    return NULL;
}

HRESULT
videoTrans_t::setFlow(flow_t inFlow,
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
    videoInfo_t info;
    info.BW = BW;
    info.FR = FR;
    info.SSRC = SSRC;
    info.videoMode = VIDEO_TRANS_MODE;
    info.Width = width;
    info.Height = height;
    info.PT = PT;
    //----------------------------
    // This is a temporal PATCH!!
    // every single SetFlow call must
    // be changed (u8 PT --> int PT param.)
    //----------------------------
    if (info.PT >= 255)
        info.PT = -1;
    info.Q = Q;
    if (BW)
    {
        info.rateMode = CBR;
    }
    else
    {
        info.rateMode = VBR;
    }

    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    // find correct processor at my list
    videoTransProcessor_t *videoTransProcessor =
        getValidProcessor(outFlowArray, info);

    if ( ! videoTransProcessor)
    {
        // create processor and insert in list

        videoTransProcessor = new videoTransProcessor_t(info);
        videoTransProcessor->addRef();
        outFlow_t *outFlow = new outFlow_t(inFlow);
        outFlow->ID = inFlow.ID;
        outFlow->processor = videoTransProcessor;
        outFlowArray->add(outFlow);
    }

    videoTransProcessor->addTarget(target);

    return S_OK;
}

HRESULT
videoTrans_t::unsetFlow(flow_t inFlow, target_t *target)
{
    // get inFlow position
    vector_t<outFlow_t *> *outFlowArray = getProcessorArray(inFlow);

    for (u16 i = 0 ; i < outFlowArray->size(); i++)
    {
        processor_t *processor = outFlowArray->elementAt(i)->processor;
        if (processor->deleteTarget(target) == S_OK)
        {
            if (processor->decRef() == 0)
            {
                deleteTarget(target); // this deletes processor
                outFlowArray = getProcessorArray(inFlow); // refresh outFlowList

                if ( ! outFlowArray->size()) break;
                i--;
            }
        }
    }

    return S_OK;
}

