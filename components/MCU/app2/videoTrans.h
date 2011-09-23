/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: videoTrans.h 6888 2005-06-20 09:36:13Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_VIDEO_TRANS_H_
#define _MCU_VIDEO_TRANS_H_

#include <map>

#include <rtp/fragmenter.hh>
#include <rtp/defragmenter.hh>

#include <vUtils/rtp2image.h>

#include "flowProcessor.h"
#include "returnCode.h"

class videoTransProcessor_t: public processor_t
{
private:

    // RTP info
    u16 SQ;
    u32 TS;

    // vCodecs Coder
    vCoder_t *Coder;
    vCoderArgs_t params;

    // frame fragmenter
    fragmenter_t fragmenter;

    videoInfo_t videoInfo; // static data
    videoInfo_t tmpInfo;   // dynamic data

    // to check FR
    timeval time1,time2;

public:

    videoTransProcessor_t(videoInfo_t info);

    virtual ~videoTransProcessor_t(void);

    virtual HRESULT deliver(u8 *data,
                            unsigned int len,
                            videoInfo_t info
                           );
    bool isValid(videoInfo_t videoInfo);
};

class videoTrans_t: public flowProcessor_t
{
private:
    // video Decoder
    u8 lastPT;

    std::map<u32, rtp2image_t *> vDecoderArr;

    // private methods
    videoTransProcessor_t *getValidProcessor(vector_t<outFlow_t *> *outFlowArray,
                                             videoInfo_t info
                                            );
public:

    videoTrans_t(void);

    virtual ~videoTrans_t(void);

    // to define inFlow-outFlow relationShip
   virtual HRESULT setFlow(flow_t inFlow,
                           target_t *target,
                           u8 PT=0,
                           u32 BW=0,
                           u16 width=0,
                           u16 height=0,
                           u32 FR = 0,
                           u8  Q = 0,
                           u32 SSRC = 0,
                           gridMode_e gridMode= GRID_AUTO
                          );

    virtual HRESULT unsetFlow(flow_t inFlow, target_t *target);

    virtual HRESULT deliver(RTPPacket_t *pkt, flow_t inFlow);
};

#endif

