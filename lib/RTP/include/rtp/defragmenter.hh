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
// $Id: defragmenter.hh 10860 2007-09-19 15:01:35Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __isabel_sdk__defragmenter_hh__
#define __isabel_sdk__defragmenter_hh__

#include <vCodecs/codec.h>

#include <icf2/ql.hh>

#include <rtp/RTPPacket.hh>

class RTPBuffer_t;

class defragmenter_t
{
private:
    int frameBufferSize; // number of images to be buffered

    ql_t<RTPBuffer_t *> frameBuffer;

    // some streams do not carry this info in all frames
    int lastW, lastH;

public:

    defragmenter_t(int initFrameBuferSize);
    virtual ~defragmenter_t(void);

    //----------------------------------
    // insert fragment :
    // in : fragments, defragmenter deletes them when
    //      entire frame is returned.
    // out: entire frames,
    //      defragmenter user must allocate memory.
    //----------------------------------
    int process(u8 *outBuf,
                int outBufLen,
                bool *gotFrame,
                u32 *fmt,
                int *widht,
                int *height,
                RTPPacket_t *pkt
               );
};

#endif

