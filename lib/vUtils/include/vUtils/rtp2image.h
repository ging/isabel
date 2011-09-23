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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vutils__rtp2image_h__
#define __vutils__rtp2image_h__

#include <vCodecs/codecs.h>

#include <vUtils/image.h>

#include <rtp/RTPPacket.hh>
#include <rtp/defragmenter.hh>

class rtp2image_t
{
private:
    u32             lastFmt;
    defragmenter_t *defragmenter;
    vDecoderArgs_t  decoderArgs;
    vDecoder_t     *decoder;

    // received, compressed frame
    u8 *newFrameBuf;
    int newFrameLen;

public:
    // maxFrames: size of reception buffer, in number of frames
    rtp2image_t(int maxFrames);

    virtual ~rtp2image_t(void);

    // returns an image when available, NULL otherwise
    // caller must delete image
    image_t *addPkt(RTPPacket_t *pkt);
};

#endif
