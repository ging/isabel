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
// $Id: videoCodecs.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <rtp/RTPUtils.hh>

#include "videoCodecs.h"


vDecoderPack_t::vDecoderPack_t(int newSSRC)
: defragmenter(new defragmenter_t),
  Decoder(NULL),
  SSRC(newSSRC),
  PT(-1)
{
}

vDecoderPack_t::~vDecoderPack_t(void)
{
    if ( ! Decoder )
    {
        delete Decoder;
    }
    if ( ! defragmenter )
    {
        delete defragmenter;
    }
}

bool
vDecoderPack_t::checkPT(int nPT)
{
    vDecoderArgs_t params;

    if (nPT != PT)
    {
        if (Decoder)
        {
            // delete previous decoder if exists
            vDeleteDecoder(Decoder);
        }

        // Fill the param struct
        params.height  = 0;  // not needed, decoders get it from frames
        params.width   = 0;  // not needed, decoders get it from frames
        params.format  = VIDEO_FORMAT;

        Decoder = vGetDecoderByFmt(getFmtByPT(nPT), &params);

        if ( ! Decoder)
        {
            NOTIFY("vDecoderPack_t::checkPT: can't get decoder PT= %d\n", nPT);

            return false;
        }
    }

    PT = nPT;
    return true;
}

VCimage_t*
vDecoderPack_t::process(RTPPacket_t *pkt)
{
    //--------------------------------------------------
    // Defragment frame
    //--------------------------------------------------
    VCimage_t *image = defragmenter->process(pkt);

return image;

    if (image)
    {
        //--------------------------------------------------
        // Decode video
        //--------------------------------------------------

        // Check if decoder has changed
        vDecoderArgs_t params;
        if ( ! checkPT(pkt->getPayloadType()))
        {
            NOTIFY("grid_t::deliver: can't get decoder PT=%d\n", PT);
            delete[] image->buffer;
            delete image;
            return NULL;
        }

        vGetParams(Decoder, &params);
        if (image->h &&
            image->w &&
            params.height != image->h &&
            params.width  != image->w
           )
        {
            params.height = image->h;
            params.width  = image->w;
        }

        if (params.height && ! image->h) image->h = params.height;
        if (params.width  && ! image->w) image->w = params.width;

        if (image->h * image->w)
        {
            unsigned int imageSize = image->h * image->w * 3;
            u8 *buffer = new u8[imageSize];
            imageSize = vDecode(Decoder,
                                image->buffer,
                                image->numBytes,
                                buffer,
                                imageSize
                               );

            if (imageSize <= 0 ||
                imageSize > image->h * image->w * IMAGE_FACTOR
               )
            {
                NOTIFY("grid2_t::deliver: vDecoder returns %d\n", imageSize);
                delete[] buffer;
                delete[] image->buffer;
                delete image;
                return NULL;
            }
        }
    }

    return image;
}

