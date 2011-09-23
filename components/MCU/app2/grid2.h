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
// $Id: grid2.h 7474 2005-10-17 11:59:19Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_GRID2_H_
#define _MCU_GRID2_H_

#include <map>

#include <rtp/fragmenter.hh>
#include <rtp/defragmenter.hh>

#include <vUtils/rtp2image.h>

#include "flowProcessor.h"
#include "grid.h"

class grid2_t: public flowProcessor_t
{
    std::map<u32, rtp2image_t *> vDecoderArr;

    gridProcessor_t *gridProcessor;

    // to create new processors
    gridProcessor_t *getValidProcessor(flow_t outFlow,
                                       vector_t<outFlow_t *> *outFlowArray
                                      );

public:

    grid2_t(void);
    virtual ~grid2_t(void);

    //to define inFlow-outFlow relationShip
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

