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
//#include <stdio.h>

#include "utils.h"
#include "avm_fourcc.h"
#include "formats.h"

const char* avm_fcc_name(fourcc_t fcc) 
{
    switch (fcc)
    {
    case fccdiv3:
    case fccDIV3: return "DIV3 (divx)";
    case fccdiv4:
    case fccDIV4: return "DIV4 (divx)";
    case fccdiv5:
    case fccDIV5: return "DIV5 (divx)";
    case fccdiv6:
    case fccDIV6: return "DIV6 (divx)";
    case fccmp41:
    case fccMP41: return "MP41 (divx)";
    case fccMP43: return "MP43 (divx)";

    case fccMP42:
    case fccmp42: return "MP42 (MS MPEG-4)";
    case fccmp43: return "MP43 (MS MPEG-4)";
    case fccmpg4:
    case fccMPG4: return "MPG4 (MS MPEG-4)";

    case fccWMV1:
    case fccwmv1: return "WMV1 (WinMP)";
    case fccWMV2:
    case fccwmv2: return "WMV2 (WinMP)";

    case fccIV32: return "IV32 (Indeo)";
    case fccIV41: return "IV41 (Indeo)";
    case fccIV50: return "IV50 (Indeo)";
    case fccI263: return "I263 (Indeo)";

    case fcccvid: return "CVID (Cinepak)";
    case fccVCR2: return "VCR2";
    case fccMJPG: return "MLPG";

    case fccYUV:  return "YUV";
    case fccYUY2: return "YUV2";
    case fccYV12: return "YV12";
    case fccIYUV: return "IYUV";
    case fccUYVY: return "UYVY";
    case fccYVYU: return "YVYU";
    }

    return "unknown";
}

struct dts {
    fourcc_t fmt;
    const char* name;
};

static const struct dts data[] =
{
    { IMG_FMT_BGR32, "BGR 32-bit" },
    { IMG_FMT_BGR24, "BGR 24-bit" },
    { IMG_FMT_BGR16, "BGR 16-bit" },
    { IMG_FMT_BGR15, "BGR 15-bit" },
    { IMG_FMT_BGR8,  "BGR 8-bit" },

    { IMG_FMT_RGB32, "RGB 32-bit" },
    { IMG_FMT_RGB24, "RGB 24-bit" },
    { IMG_FMT_RGB16, "RGB 16-bit" },
    { IMG_FMT_RGB15, "RGB 15-bit" },
    { IMG_FMT_RGB8,  "RGB 8-bit" },

    { IMG_FMT_CLPL, "Planar CLPL" },
    { IMG_FMT_I420, "Planar I420" },
    { IMG_FMT_I422, "Planar I422" },
    { IMG_FMT_I444, "Planar I444" },
    { IMG_FMT_IF09, "Planar IF09" },
    { IMG_FMT_IYUV, "Planar IYUV" },
    { IMG_FMT_Y8,   "Planar Y8" },
    { IMG_FMT_Y800, "Planar Y800" },
    { IMG_FMT_YV12, "Planar YV12" },
    { IMG_FMT_YVU9, "Planar YVU9" },

    { IMG_FMT_CLJR, "Packed CLJR" },
    { IMG_FMT_IUYV, "Packed IUYV" },
    { IMG_FMT_IY41, "Packed IY41" },
    { IMG_FMT_IYU1, "Packed IYU1" },
    { IMG_FMT_IYU2, "Packed IYU2" },
    { IMG_FMT_UYNV, "Packed UYNV" },
    { IMG_FMT_UYVP, "Packed UYVP" },
    { IMG_FMT_UYVY, "Packed UYVY" },
    { IMG_FMT_V422, "Packed V422" },
    { IMG_FMT_V655, "Packed V655" },
    { IMG_FMT_Y211, "Packed Y211" },
    { IMG_FMT_Y41P, "Packed Y41P" },
    { IMG_FMT_Y41T, "Packed Y41T" },
    { IMG_FMT_Y422, "Packed Y422" },
    { IMG_FMT_Y42T, "Packed Y42T" },
    { IMG_FMT_YUNV, "Packed YUNV" },
    { IMG_FMT_YUVP, "Packed YUVP" },
    { IMG_FMT_YUY2, "Packed YUY2" },
    { IMG_FMT_YVYU, "Packed YVYU" },
    { IMG_FMT_cyuv, "Packed CYUV" },

    { IMG_FMT_MPEGPES, "Mpeg PES" },
    { 0, "Unknown" }
};

const char *avm_img_format_name(fourcc_t fmt)
{
    const struct dts* t = data;

    while (t->fmt && t->fmt != fmt)
        t++;

    return t->name;
}
