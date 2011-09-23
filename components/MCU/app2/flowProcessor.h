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
// $Id: flowProcessor.h 6859 2005-06-14 09:57:20Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_FLOWPROCESSOR_H_
#define _MCU_FLOWPROCESSOR_H_

#include <icf2/vector.hh>

#include "output.h"

class processor_t: public simpleTask_t
{
protected:
    vector_t<target_t*> targetArray;
    u16 referenceCount;

public:
    processor_t(void);
    virtual ~processor_t(void);

    virtual HRESULT deliver(RTPPacket_t *pkt);

    // target works
    HRESULT addTarget(target_t *target);
    HRESULT deleteTarget(target_t *target);

    // work with reference counter
    u16 decRef(void);
    u16 addRef(void);
};

class outFlow_t
{
public:
    u16 ID;
    processor_t *processor;
    void *ptr;
    flow_t inFlow; // to save inFlow info

    outFlow_t(flow_t inFlow);
    ~outFlow_t(void);
};

#define MAX_FLOW_LEN 0xffff

class flowProcessor_t
{
protected:

    flowProcessor_e flowProcessorType;
    vector_t<outFlow_t *> *outFlowMatrix[MAX_FLOW_LEN] ;

    // to get output processor list by an input
    //TODO : Depender de ID
    vector_t<outFlow_t *> *getProcessorArray(flow_t&);

public:

    flowProcessor_t(flowProcessor_e type);
    virtual ~flowProcessor_t(void);

    // receive data
    virtual HRESULT deliver(RTPPacket_t *pkt, flow_t inFlow) = 0;

    flowProcessor_e getFlowProcessorType(void);

    // NOTE:When a target is deleted
    // all references to it must be
    // deleted too.
    HRESULT deleteTarget(target_t *target);

    // pure abtract flows methods:
    // must have every opt used in flowProcessors
    // only use the needed in each one
    virtual HRESULT setFlow(flow_t inFlow,     // used in video/audio/switch/
                            target_t *target,  // used in video/audio/switch/
                            u8 PT=0,           // used in video/audio/
                            u32 BW=0,          // used in video/
                            u16 width=0,       // used in video/
                            u16 height=0,      // used in video/
                            u32 FR = 0,        // used in video/
                            u8  Q = 0,         // used in video/
                            u32 SSRC = 0,      // used in video/
                            gridMode_e gridMode=GRID_AUTO
                           ) = 0;

    virtual HRESULT unsetFlow(flow_t inFlow, target_t *target) = 0;

};

#endif

