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
// $Id: grid.h 8518 2006-05-18 09:42:07Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_GRID_H_
#define _MCU_GRID_H_

#include <map>

#include <rtp/fragmenter.hh>
#include <rtp/defragmenter.hh>

#include <vUtils/rtp2image.h>

#include "flowProcessor.h"
#include "gridComposer/gridComposer.h"

class gridProcessor_t: public processor_t
{
private:
    timeval time1,time2;

    // vector where window
    // positions are saved
    window_t windowPos[MAX_VIDEO_SOURCES];

    // grid image
    image_t *image;
    u8 *mask;

    // grid composer
    gridComposer_t *gridComposer;

    // RTP info
    u16 SQ;
    u32 TS;

    // vCodecs Coder
    vCoder_t *Coder;
    vCoderArgs_t params;

    // static params
    videoInfo_t videoInfo;

    // frame fragmenter
    fragmenter_t fragmenter;

    virtual void heartBeat(void);

    void resetI420PImage();
    void resetI420PImage(window_t window);

public:

    gridProcessor_t(videoInfo_t info);

    virtual ~gridProcessor_t(void);

    bool isValid(flow_t outFlow);

    HRESULT addFlowID(int flowID);
    HRESULT delFlowID(int flowID);

    virtual HRESULT deliver(u8 *data,
                            unsigned int len,
                            videoInfo_t info,
                            int flowID
                           );
};

class grid_t: public flowProcessor_t
{
private:
    std::map<u32, rtp2image_t *> vDecoderArr;

    // to create new processors
    gridProcessor_t *getValidProcessor(flow_t outFlow,
                                       vector_t<outFlow_t *> *outFlowArray
                                      );

    gridProcessor_t *getValidProcessor(flow_t outFlow);

public:
    grid_t(void);
    virtual ~grid_t(void);

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

