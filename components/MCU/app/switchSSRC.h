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
// $Id: switchSSRC.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_SWITCH_SSRC_H_
#define _MCU_SWITCH_SSRC_H_

#include "flowProcessor.h"

class switchSSRCProcessor_t: public processor_t
{
private:
    u32 SSRC;
    int PT;
public:
    switchSSRCProcessor_t(u32 SSRC, int PT);
    virtual ~switchSSRCProcessor_t(void);

    u32 getSSRC(void);
    int getPT(void);

    virtual HRESULT deliver(RTPPacket_t *pkt);

    // to check if we've got a valid
    // processor type;
    virtual bool operator==(switchSSRCProcessor_t *);
    virtual bool operator==(switchSSRCProcessor_t);

};

class switcherSSRC_t: public flowProcessor_t
{
private:

     // to create new processors
     switchSSRCProcessor_t *getValidProcessor(flow_t inFlow,
                                              vector_t<outFlow_t *> *outFlowArray
                                             );

public:

    switcherSSRC_t(void);
    virtual ~switcherSSRC_t(void);

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

};

#endif

