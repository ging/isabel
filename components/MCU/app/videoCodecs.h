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
// $Id: videoCodecs.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __MCU_VIDEO_CODECS_H__
#define __MCU_VIDEO_CODECS_H__

#include <rtp/defragmenter.hh>
#include <vCodecs/codecs.h>

#define VIDEO_FORMAT I420P_FORMAT
#define IMAGE_FACTOR 3/2
#define MAX_VIDEO_SOURCES 0xff

class vDecoderPack_t
{
public:
    //video defragment
    defragmenter_t *defragmenter;
    vDecoder_t *Decoder;

    u32   SSRC;
    int    PT;

    vDecoderPack_t(int newSSRC);
    ~vDecoderPack_t(void);

    // check (and rebuild) the Payload Type
    bool checkPT(int newPT);

    VCimage_t *process(RTPPacket_t *pkt);
};

#endif

