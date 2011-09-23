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
// $Id: playchannel.cc 20563 2010-06-15 10:05:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <icf2/notify.hh>

#include <vUtils/rtp2image.h>

rtp2image_t::rtp2image_t(int maxFrames)
{
    assert (maxFrames > 0 && "rtp2image_t: maxFrames must be positive");

    defragmenter= new defragmenter_t(maxFrames);
    decoder= NULL;

    newFrameLen= 1024*768*4; // initial value, may grow
    newFrameBuf= new u8[newFrameLen];
}

rtp2image_t::~rtp2image_t(void)
{
    delete defragmenter;

    if (decoder != NULL)
    {
        vDeleteDecoder(decoder);
    }

    delete []newFrameBuf;
}

image_t*
rtp2image_t::addPkt(RTPPacket_t *pkt)
{
    bool gotFrame= false;
    int width=-1;
    int height= -1;
    u32 currentFmt= 0;
    int reqSize= 0;

    //NOTIFY ("rtp2image_t::addPkt: "
    //        "Channel: %d -- TS: %u -- Seq. number: %u -- LastFragment: %d\n",
    //        pkt -> getSSRC(),
    //        pkt -> getTimestamp(),
    //        pkt -> getSequenceNumber(),
    //        pkt -> getMark()
    //       );

    for (int n= 0; n < 2; n++) // retry just once, never more
    {
        reqSize= defragmenter->process(newFrameBuf,
                                       newFrameLen,
                                       &gotFrame,
                                       &currentFmt,
                                       &width,
                                       &height,
                                       pkt
                                      );

        if ( ! gotFrame && ( reqSize > newFrameLen ) )
        {
            // needs more room, never twice
            newFrameLen = 2 * reqSize;
            NOTIFY("rtp2image_t::addPkt: "
                   "requesting more space: %d bytes\n",
                   newFrameLen
                  );
            delete [] newFrameBuf;
            newFrameBuf= new u8[newFrameLen];
        }
        else
        {
            // got image, decoding error or not image yet
            break;
        }
    }

    if ( ! gotFrame )
    {
        // no frame yet
        return NULL;
    }

    // got an image, let's decode and paint
    if ((decoder != NULL) && (lastFmt != currentFmt))
    {
        // decoder is useless
        delete decoder;
        decoder= NULL;
    }
    if (decoder == NULL)
    {
        decoderArgs.width = width;
        decoderArgs.height= height;
        decoderArgs.format= I420P_FORMAT;
        decoder = vGetDecoderByFmt(currentFmt, &decoderArgs);

        if (decoder != NULL)
        {
            lastFmt= currentFmt;
            NOTIFY("rtp2image_t::addPkt: built decoder for %s\n",
                   vGetFormatNameById(currentFmt)
                  );
        }
        else
        {
            NOTIFY("rtp2image_t::addPkt: ERROR "
                   "cannot build decoder for 0x%x\n",
                   currentFmt
                  );
            return NULL;
        }
    }

    int outBuffLen= 2048*1536*4;
    u8 *outBuff= new u8[outBuffLen];

    if (outBuff == NULL)
    {
        NOTIFY("rtp2image_t::addPkt: run out of memory, bailing out\n");
        abort();
    }

    int res= vDecode(decoder,
                     newFrameBuf,
                     newFrameLen,
                     outBuff,
                     outBuffLen
                    );

    if (res < 0)
    {
        NOTIFY("rtp2image_t::addPkt: bad decoded image SSRC=%d\n",
               pkt->getSSRC()
              );

        delete []outBuff;

        return NULL;
    }

    vGetParams(decoder, &decoderArgs);

    if (res > outBuffLen)
    {
        NOTIFY("rtp2image_t::addPkt: decode image too big %dx%d!!\n",
               decoderArgs.width,
               decoderArgs.height
              );

        delete []outBuff;

        return NULL;
    }

    image_t *newImg= new image_t(outBuff,
                                 res,
                                 decoderArgs.format,
                                 decoderArgs.width,
                                 decoderArgs.height,
                                 pkt->getTimestamp()
                                );

    delete []outBuff;

    return newImg;
}

