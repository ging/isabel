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
// $Id: netSink.cc 21890 2011-01-18 17:31:24Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include "netSink.hh"
#include "videoRTPSession.hh"

netSink_t::netSink_t(u32 nchId, int BW, stats_t *myS)
: chId(nchId),
  coder(NULL),

  avgBandwidth(BW),

  codecMode(NULL)
{
    lineFmt= MPEG4_FORMAT;
#ifdef MPEG4_NOT_AVAILABLE
    lineFmt= XVID_FORMAT;
#endif

    RTPRandom_t random;
    seqNumber = (u16)random.random32();

    coderArgs.frameRate= 25;
    coderArgs.quality= 80;
    coderArgs.maxInter= 2*25;
    coderArgs.bitRate= BW;

    myStats= myS;
    myStats->setDesiredBW(BW);
}

netSink_t::~netSink_t(void)
{
    if (codecMode != NULL)
    {
        free(codecMode);
    }

    if (coder != NULL)
    {
        delete coder;
        coder= NULL;
    }

    myStats= NULL;
}

void
netSink_t::setLineFmt(u32 newFmt)
{
#ifdef MPEG4_NOT_AVAILABLE
    if (newFmt == MPEG4_FORMAT)
    {
        newFmt= XVID_FORMAT;
    }
#endif
    lineFmt= newFmt;
}

void
netSink_t::setFrameRate(double fr)
{
    if ((fr <= 0) || (fr > 30))
    {
        NOTIFY("netSink_t::setFrameRate: frame rate out of range\n");
        return;
    }

    coderArgs.frameRate= fr;

    if (coder != NULL)
    {
        coder->setCoderArgs(&coderArgs);
    }
}

void
netSink_t::setCodecQuality(int quality)
{
    if ((quality <= 0) || (quality > 100))
    {
        NOTIFY("netSink_t::setCodecQuality: quality out of range\n");
        return;
    }

    coderArgs.quality= quality;

    if (coder != NULL)
    {
        coder->setCoderArgs(&coderArgs);
    }

    myStats->setQuality(quality);
}

void
netSink_t::setCodecMode(const char *mode)
{
    if ((mode == NULL) || (strlen(mode) == 0))
    {
        NOTIFY("netSink_t::setCodecMode: invalid mode\n");
        return;
    }

    if (codecMode != NULL)
    {
        free(codecMode);
    }
    codecMode= strdup(mode);

    if (coder != NULL)
    {
        NOTIFY("Hay que poner el MODE\n");
        //coder->setPropertyValue("MODE", mode);
    }
}

void
netSink_t::setCodecPBetI(int pbeti)
{
    if ((pbeti <= 0) || (pbeti > 100))
    {
        NOTIFY("netSink_t::setCodecPBetI: pbeti out of range\n");
        return;
    }

    coderArgs.maxInter= pbeti;

    if (coder != NULL)
    {
        coder->setCoderArgs(&coderArgs);
    }
}

void
netSink_t::setBandwidth(double newBandwidth)
{
    avgBandwidth = newBandwidth;

    coderArgs.bitRate= avgBandwidth;

    if (coder != NULL)
    {
        coder->setCoderArgs(&coderArgs);
    }

    myStats->setDesiredBW(avgBandwidth);
}

void
netSink_t::sendFrame(image_t *img)
{
    u32 imgFmt= img->getFormat();

    unsigned width = img->getWidth();
    unsigned height= img->getHeight();

    if (   (coder == NULL)
        || (imgFmt != coder->getInputFormat())
        || (lineFmt != coder->getOutputFormat())
       )
    {

        if (coder != NULL)
        {
            delete coder;
            coder= NULL;
        }

        try
        {
            coderArgs.width = width;
            coderArgs.height= height;

            coder= new coderWrapper_t(imgFmt, lineFmt, &coderArgs);

            NOTIFY("Built coderWrapper %s -> %s\n",
                   vGetFormatNameById(imgFmt),
                   vGetFormatNameById(lineFmt)
                  );
        }
        catch (const char *error)
        {
            NOTIFY("Encoder not created due to an error: %s\n", error);
            coder= NULL;
        }

        if (coder != NULL)
        {
NOTIFY("Hay que poner MODE\n");
            //if(codecMode != NULL)
            //{
            //    coder->setPropertyValue("MODE", codecMode);
            //}

            myStats->setCodecInUse(vGetFormatNameById(lineFmt));
        }
    }

    if (coder == NULL)
    {
        NOTIFY("netSink_t::nextFrame: cannot code image, returning\n");
        delete img;
        return;
    }

    if (coderArgs.width != width || coderArgs.height != height)
    {
        coderArgs.width = width;
        coderArgs.height = height;
        if ( ! coder->setCoderArgs(&coderArgs))
        {
            NOTIFY("netSink_t::nextFrame: setSize went wrong, returning\n");
            delete img;
            return;
        }
    }

    //int bufSize= vGetSize4EncodedBuffer(width, height);
    int bufSize= img->getWidth()*img->getHeight()*4;
if (bufSize < 17000) bufSize= 17000;

    u8 *buf= new u8[bufSize];

    u8 *src= img->getBuff();
    int numBytes= coder->encode(buf, bufSize, src, width, height);

    if (numBytes <= 0)
    {
        NOTIFY("netSink_t::nextFrame: "
               "could not encode %dx%d image "
               "encoder returns %d numbytes, skipping\n",
               width, height, numBytes
              );

        delete []buf;
        delete img;
        return;
    }

    myStats->accountCodecBytes(numBytes);

    u8 linePT= fragmenter.setFrame(buf, numBytes, lineFmt, width, height);

    int n = 0;
    int size = rtpSession->getMaxPacketSize(chId, 0);
    u8 *fragment= new u8[3*size];

    u32 TS = (u32)(img->getTimestamp() / 1000000.0 / VIDEO_RTP_TSU);

    while ((n = fragmenter.getFragment(fragment, size)) >= 0 )
    {
        myStats->accountSentBytes(size);
        rtpSession->sendData(chId,
                             fragment,
                             size,
                             linePT,
                             n == 0,
                             seqNumber,
                             TS
                            );
        seqNumber++;
    }

    delete []buf;
    delete []fragment;

    delete img;

    myStats->accountSentFrame();
}


