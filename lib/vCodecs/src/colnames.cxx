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
// $Id: colnames.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <vCodecs/codec.h>

struct colorSpaceMap_t
{
    const char *name;
    u32         id;
};

colorSpaceMap_t colorSpaceMap[] =
{
    {"RAW::RGB24", RGB24_FORMAT  },
    {"RAW::BGR24", BGR24_FORMAT  },
    {"YUV::422i",  I422i_FORMAT  },
    {"YUV::422P",  I422P_FORMAT  },
    {"YUV::420P",  I420P_FORMAT  },
    {"YUV::411P",  I411P_FORMAT  },

    // {"GIF::GIF", PNG_FORMAT    },
    // {"GIF::GIF", TIFF_FORMAT   },
    // {"GIF::GIF", GIF_FORMAT    },
    // {"JPEG::JPEG", JPEG_FORMAT },
    // {"CMP::JPEG",  JPEG_FORMAT },

    {"CELLB",     CELB_FORMAT    },
    {"MJPEG",     MJPEG_FORMAT   },
    {"MPEG-1",    MPEG1_FORMAT   },
    {"MPEG-2",    MPEG2_FORMAT   },
    {"H263",      H263_FORMAT    },
    {"H263+",     H263P_FORMAT   },
    {"H264",      H264_FORMAT    },
    {"MPEG-4",    MPEG4_FORMAT   },
    {"XVID",      XVID_FORMAT    },
    {"DIVX",      DIVX_FORMAT    }
};

const int MAX_COLORSPACE= ((sizeof(colorSpaceMap)/sizeof(colorSpaceMap_t)));

__EXPORT const char*
vGetFormatNameById(u32 id)
{
    int i = 0;
    for (i = 0; i < MAX_COLORSPACE; i++)
    {
        if (colorSpaceMap[i].id == id)
        {
            return colorSpaceMap[i].name;
        }
    }
    throw "vGetFormatNameById::No such identifier";
}

__EXPORT u32
vGetFormatIdByName(const char *fmtname)
{
    int i = 0;
    for (i = 0; i < MAX_COLORSPACE; i++)
    {
        if (strcmp(colorSpaceMap[i].name, fmtname) == 0)
        {
            return colorSpaceMap[i].id;
        }
    }
    throw "vGetFormatIdByName::No such name";
}

__EXPORT int
vFrameSize(int width, int height, u32 fmt)
{
    int base = height * width;
    switch (fmt)
    {
        case RGB24_FORMAT:
        case BGR24_FORMAT:
            return base*3;
        case I422i_FORMAT:
        case I422P_FORMAT:
            return base*2;
        case I420P_FORMAT:
        case I411P_FORMAT:
            return base*3/2;
        case MJPEG_FORMAT:
            return base*4 + 20000;
        default:
            throw "vFrameSize:: unknown format";
    }
}
