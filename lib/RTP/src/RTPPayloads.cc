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

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPayloads.hh>

#include <vCodecs/codecs.h>

#include <aCodecs/codecs.h>


u32
getFmtByPT(u8 PT)
{
    switch (PT)
    {
    case GSM_8_PT   : return GSM_8_FORMAT;
    case GSM_16_PT  : return GSM_16_FORMAT;
    case GSM_32_PT  : return GSM_32_FORMAT;

    case AMR_8_PT   : return AMR_8_FORMAT;
    case AMR_16_PT  : return AMR_16_FORMAT;
    case AMR_32_PT  : return AMR_32_FORMAT;

    case NONE_8_PT  : return NONE_8_FORMAT;
    case NONE_16_PT : return NONE_16_FORMAT;
    case NONE_22_PT : return NONE_22_FORMAT;
    case NONE_44_PT : return NONE_44_FORMAT;
    case NONE_48_PT : return NONE_48_FORMAT;

    case PCMA_8_PT  : return PCMA_8_FORMAT; // g711
    case PCMU_8_PT  : return PCMU_8_FORMAT;
    case PCMU_16_PT : return PCMU_16_FORMAT;
    case PCMU_22_PT : return PCMU_22_FORMAT;
    case PCMU_44_PT : return PCMU_44_FORMAT; // g711

    case G722_8_PT  : return G722_8_FORMAT;
    case G722_16_PT : return G722_16_FORMAT;

    case G726_8_PT  : return G726_8_FORMAT;
    case G726_16_PT : return G726_16_FORMAT;

    case MP3_8_PT   : return MP3_8_FORMAT;
    case MP3_16_PT  : return MP3_16_FORMAT;
    case MP3_32_PT  : return MP3_32_FORMAT;
    case MP3_44_PT  : return MP3_44_FORMAT;
    case MP3_48_PT  : return MP3_48_FORMAT;

    case SPEEX_8_PT : return SPEEX_8_FORMAT;
    case SPEEX_16_PT: return SPEEX_16_FORMAT;

    case CELB_PT  : return CELB_FORMAT;
    case MJPEG_PT : return MJPEG_FORMAT;
    case MPEG4_PT : return MPEG4_FORMAT;
    case XVID_PT  : return XVID_FORMAT;
    case MPEG1_PT : return MPEG1_FORMAT;
    case MPEG2_PT : return MPEG2_FORMAT;
    case H263_PT  : return H263_FORMAT;
    case H263P_PT : return H263P_FORMAT;
    case H264_PT  : return H264_FORMAT;
    }

    throw "getFmtByPT: unknown PT";
}

u8
getPTByFmt(u32 fmt)
{
    switch (fmt)
    {
    case  GSM_8_FORMAT   : return GSM_8_PT;
    case GSM_16_FORMAT   : return GSM_16_PT;
    case GSM_32_FORMAT   : return GSM_32_PT;

    case  AMR_8_FORMAT   : return AMR_8_PT;
    case AMR_16_FORMAT   : return AMR_16_PT;
    case AMR_32_FORMAT   : return AMR_32_PT;

    case  NONE_8_FORMAT  : return NONE_8_PT;
    case NONE_16_FORMAT  : return NONE_16_PT;
    case NONE_22_FORMAT  : return NONE_22_PT;
    case NONE_44_FORMAT  : return NONE_44_PT;
    case NONE_48_FORMAT  : return NONE_48_PT;

    case  PCMA_8_FORMAT  : return PCMA_8_PT;
    case  PCMU_8_FORMAT  : return PCMU_8_PT;
    case PCMU_16_FORMAT  : return PCMU_16_PT;
    case PCMU_22_FORMAT  : return PCMU_22_PT;
    case PCMU_44_FORMAT  : return PCMU_44_PT;

    case  G722_8_FORMAT  : return G722_8_PT;
    case G722_16_FORMAT  : return G722_16_PT;

    case  G726_8_FORMAT  : return G726_8_PT;
    case G726_16_FORMAT  : return G726_16_PT;

    case  MP3_8_FORMAT   : return MP3_8_PT;
    case MP3_16_FORMAT   : return MP3_16_PT;
    case MP3_32_FORMAT   : return MP3_32_PT;
    case MP3_44_FORMAT   : return MP3_44_PT;
    case MP3_48_FORMAT   : return MP3_48_PT;

    case  SPEEX_8_FORMAT : return SPEEX_8_PT;
    case SPEEX_16_FORMAT : return SPEEX_16_PT;

    case CELB_FORMAT     : return CELB_PT;
    case MJPEG_FORMAT    : return MJPEG_PT;
    case MPEG4_FORMAT    : return MPEG4_PT;
    case XVID_FORMAT     : return XVID_PT;
    case MPEG1_FORMAT    : return MPEG1_PT;
    case MPEG2_FORMAT    : return MPEG2_PT;
    case H263_FORMAT     : return H263_PT;
    case H263P_FORMAT    : return H263P_PT;
    case H264_FORMAT     : return H264_PT;

    default:
        throw "getPTByFmt: do not know how to convert";
    }
}

int
getRateByPT(u8 PT)
{
    switch (PT)
    {
    case GSM_8_PT   : return  8000;
    case GSM_16_PT  : return 16000;
    case GSM_32_PT  : return 32000;

    case AMR_8_PT   : return  8000;
    case AMR_16_PT  : return 16000;
    case AMR_32_PT  : return 32000;

    case NONE_8_PT  : return  8000;
    case NONE_16_PT : return 16000;
    case NONE_22_PT : return 22050;
    case NONE_44_PT : return 44100;
	case NONE_48_PT : return 48000;

    case PCMA_8_PT  : return  8000;
    case PCMU_8_PT  : return  8000;
    case PCMU_16_PT : return 16000;
    case PCMU_22_PT : return 22050;
    case PCMU_44_PT : return 48000;

    case G722_8_PT  : return  8000;
    case G722_16_PT : return 16000;

    case G726_8_PT  : return  8000;
    case G726_16_PT : return 16000;

    case MP3_8_PT  : return  8000;
    case MP3_16_PT : return 16000;
    case MP3_32_PT : return 32000;
    case MP3_44_PT : return 44100;
    case MP3_48_PT : return 48000;

    case SPEEX_8_PT : return  8000;
    case SPEEX_16_PT: return 16000;
    }

    throw "getRateByPT: unknown PT";
}

static const struct
{
    short PT;  // -1 to finish
    int bandwidth;
    unsigned short int rate;
    int mcs_capture;
} audioRTPParamsPerPT[]=
{
    {GSM_8_PT,    20,  8000, 40},
    {GSM_16_PT,   40, 16000, 20},
    {GSM_32_PT,   60, 32000, 10},

    {AMR_8_PT,    20,  8000, 40},
    {AMR_16_PT,   40, 16000, 20},
    {AMR_32_PT,   60, 32000, 10},

    {NONE_8_PT,  135,  8000, 20},
    {NONE_16_PT, 271, 16000, 20},
    {NONE_22_PT, 383, 22050, 20},
    {NONE_44_PT, 746, 44100, 10},
    {NONE_48_PT, 800, 48000, 10},

    {PCMA_8_PT,   72,  8000, 20},
    {PCMU_8_PT,   72,  8000, 20},
    {PCMU_16_PT, 143, 16000, 20},
    {PCMU_22_PT, 197, 22050, 20},
    {PCMU_44_PT, 394, 48000, 10},

    {G722_8_PT,   40,  8000, 20},
    {G722_16_PT,  79, 16000, 20},

    {G726_8_PT,   32,  8000, 20},
    {G726_16_PT,  63, 16000, 20},

    {MP3_8_PT,    22,  8000, 144},
    {MP3_16_PT,   43, 16000, 72},
    {MP3_32_PT,   85, 32000, 36},
    {MP3_44_PT,  117, 44100, 27},
    {MP3_48_PT,  128, 48000, 24},

    {SPEEX_8_PT,  20,  8000, 20},
    {SPEEX_16_PT, 40, 16000, 20},

    {-1, 0, 0, 0}
};

int
getMSPerPacketByPT(u8 PT)
{
    for (int i= 0; audioRTPParamsPerPT[i].PT != -1; i++)
    {
        if (audioRTPParamsPerPT[i].PT == PT)
        {
            return audioRTPParamsPerPT[i].mcs_capture;
        }
    }

    throw "getRateByPT: unknown PT";
}

int
getBWByPT(u8 PT)
{
    for (int i= 0; audioRTPParamsPerPT[i].PT != -1; i++)
    {
        if (audioRTPParamsPerPT[i].PT == PT)
        {
            return audioRTPParamsPerPT[i].bandwidth;
        }
    }

    throw "getRateByPT: unknown PT";
}

flowKind_e
getFlowByPT(u8 PT)
{
    switch (PT)
    {
    case MJPEG_PT :
    case MPEG4_PT :
    case XVID_PT  :
    case MPEG1_PT :
    case MPEG2_PT :
    case H263_PT  :
    case H263P_PT :
    case H264_PT  :
        return VIDEO_FLOW;

    case GSM_8_PT   :
    case GSM_16_PT  :
    case GSM_32_PT  :

    case AMR_8_PT   :
    case AMR_16_PT  :
    case AMR_32_PT  :

    case NONE_8_PT  :
    case NONE_16_PT :
    case NONE_22_PT :
    case NONE_44_PT :
    case NONE_48_PT :

    case PCMA_8_PT  :
    case PCMU_8_PT  :
    case PCMU_16_PT :
    case PCMU_22_PT :
    case PCMU_44_PT :

    case G722_8_PT  :
    case G722_16_PT :

    case G726_8_PT  :
    case G726_16_PT :

    case MP3_8_PT  :
    case MP3_16_PT :
    case MP3_32_PT :
    case MP3_44_PT :
    case MP3_48_PT :

    case SPEEX_8_PT :
    case SPEEX_16_PT:

        return AUDIO_FLOW;

    case VUMETER_PT:
        return VUMETER_FLOW;

    case ALIVE_PT:
        return ALIVE_FLOW;

    default:
        return UNKNOWN_FLOW;
    }
}

double
getTimestampUnitByPT(u8 PT)
{
    flowKind_e fk= getFlowByPT(PT);

    switch (fk)
    {
    case VIDEO_FLOW:
        return VIDEO_RTP_TSU;
    case AUDIO_FLOW:
        return AUDIO_RTP_TSU;
    default:
        return DEFAULT_RTP_TSU;
    }
}


