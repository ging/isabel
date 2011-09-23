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
// $Id: mixer.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_MIXER_H_
#define _MCU_MIXER_H_

// icf2
#include <icf2/sched.hh>

// audio codecs structures and payload types
#include <aCodecs/codec.h>
#include <aCodecs/codecs.h>

// sound utils
#include "soundUtils.h"
#include "soundCodecs.h"
#include "vuMeter.h"

#include "flowProcessor.h"

#define MAX_SOURCES 256
#define SAMPLE_SIZE 7680
// 44100 -- 7680 is divisible by 960,480,240...
//         length of most RTP payloads
#define DELAY 100 // 100 millis delay

class mixProcessor_t: public processor_t
{
private:

    // every processor_t must save usefull
    // info to check if the processor is valid
    // to be used by other inFlows
    int PT;
    int  nSources;
    short *samplePtr;
    short sample[44100]; // 1 second CD quality buffer (16 bits)

    timeval time; // to send every x msegs
    // each inFlow has a position in the sample (max 256 flows)
    int pos[MAX_SOURCES];

    silenceDetector_t silenceDetector;

    // RTP info
    u16 SQ;
    u32 TS;
    u32 SSRC;
    bool M; // M=1 if silence detected

    codecPool_t codecPool;

    virtual void heartBeat(void);

public:

    mixProcessor_t(int);
    virtual ~mixProcessor_t(void);

    int getPT(void);
    HRESULT setPT(int PT, int sessionID);

    HRESULT setSSRC(u32 SSRC);

    u32 getSSRC(void);

    HRESULT reset(void);

    bool isValid(flow_t);

    virtual HRESULT deliver(short *data,
                            u16 len,
                            flow_t inFlow,
                            timeval actualTime,
                            bool reset
                           );

    bool usesTarget(int ID, ql_t<u16> portList);
};

#define CODEC_LEN 128

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
                            int PT=0,
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

    HRESULT setPT(u8 newPT,int ID, int sessionID, u8 oldPT);

    HRESULT setSilenceLevel(int SilenceLevel);

    HRESULT setSSRC(u32 newSSRC, int ID, ql_t<u16> portList);
};

#endif

