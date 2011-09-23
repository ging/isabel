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
// $Id: netSink.hh 21890 2011-01-18 17:31:24Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __net_sink_hh__
#define __net_sink_hh__

#include <vUtils/coderWrapper.h>

#include <rtp/fragmenter.hh>

#include <vUtils/image.h>

#include "stats.hh"

#define INITIAL_BW              500000

class netSink_t
{
private:
    u32             chId;
    fragmenter_t    fragmenter;
    coderWrapper_t *coder;

    u32 lineFmt;

    u16 seqNumber;
    int avgBandwidth;

    vCoderArgs_t  coderArgs;
    char         *codecMode;

    stats_t *myStats;

public:
    netSink_t(u32 nchId, int BW, stats_t *myS);

    virtual ~netSink_t(void);

    void setFrameRate    (double fr);
    void setLineFmt      (u32 newFmt);
    void setCodecQuality (int quality);
    void setCodecMode    (const char *mode);
    void setCodecPBetI   (int pbeti);
    void setBandwidth    (double newBandwidth);

    void sendFrame       (image_t *img);
};

#endif

