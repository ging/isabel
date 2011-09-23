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
// $Id: mixer.h 7591 2005-11-03 13:58:44Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_MIXER_H_
#define _MCU_MIXER_H_

// icf2
#include <icf2/sched.hh>

// audio codecs structures and payload types
#include <aCodecs/codecs.h>

// sound utils
#include "soundUtils.h"
#include "soundCodecs.h"
#include "vuMeter.h"

#include "flowProcessor.h"

#include <map>

const int MAX_SOURCES= 256;
const int SAMPLE_SIZE= 7680*4;
// 44100 -- 7680 is divisible by 960,480,240...
//         length of most RTP payloads
const int DELAY= 100; // 100 millis delay

class mixProcessor_t: public processor_t
{
private:

    // every processor_t must save usefull
    // info to check if the processor is valid
    // to be used by other inFlows
    u8 PT;
    int  nSources;
    short *samplePtr;
    short sample[44100]; // 1 second CD quality buffer (16 bits)

    timeval time; // to send every x msegs
    // each inFlow has a position in the sample (max 256 flows)
    std::map<u32,int> pos;
    //we have a <ID,SSRC> output available if any input is deleted
    //we can change the SSRC output to another.
    std::map<u32,u32> SSRCmap;

    silenceDetector_t silenceDetector;

    // RTP info
    u16 SQ;
    u32 TS;
    u32 SSRC;
    bool M; // M=1 if silence detected
    bool resetMode;
           
    codecPool_t codecPool;

    virtual void heartBeat(void);

public:

    mixProcessor_t(u8 PT, u32 SSRC, target_t * target);
    virtual ~mixProcessor_t(void);

    void setSSRCfromID(u32 SSRC, u32 id);
    void unsetSSRCfromID(u32 id);

    u8 getPT(void);
    HRESULT setPT(u8 PT);

    HRESULT reset(void);

    bool isValid(flow_t);

    virtual HRESULT deliver(short *data, 
                            u16 len,
                            u32 SSRC,
                            timeval actualTime,
                            bool reset
                           );
};

class mixer_t: public flowProcessor_t
{
private:
    
    // to create new processors
    mixProcessor_t *getValidProcessor(flow_t inFlow,
                                      vector_t<outFlow_t *> *outFlowArray
                                     );
    mixProcessor_t *getValidProcessor(flow_t inFlow);

    codecPool_t codecPool;

    int SilenceLevel;

    vuMeter_t vuMeter;

public:

    mixer_t(void);
    virtual ~mixer_t(void);

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

    HRESULT setPT(u8 newPT, sockaddr_storage const &IP, u8 oldPT);

    HRESULT setSilenceLevel(int SilenceLevel);
};

#endif

