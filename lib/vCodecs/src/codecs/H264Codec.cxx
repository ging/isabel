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
// $Id: H264Codec.cxx 22044 2011-02-21 17:06:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <signal.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <getopt.h>

#ifdef _MSC_VER
#include <io.h>     /* _setmode() */
#include <fcntl.h>  /* _O_BINARY */
#endif

#ifdef AVIS_INPUT
#include <windows.h>
#include <vfw.h>
#endif

#ifdef MP4_OUTPUT
#include <gpac/m4_author.h>
#endif

#include "H264Codec.h"

// include x264 (which requires stdint.h)
#include <stdint.h>
extern "C" {
#include <x264.h>
}
#ifndef _MSC_VER
//#include "h264/config.h"
#endif

//---------------------------------------------
// Forward declarations
//---------------------------------------------
void H264_delete_coder(void *context);

int H264_encode(void          *context,
                unsigned char *inBuff,
                int            inBuffLen,
                unsigned char *outBuff,
                int            outBuffLen
               );

void H264_get_coder_params(void *context, vCoderArgs_t *params);

//---------------------------------------------
// Functions
//---------------------------------------------

vCoder_t *
H264_new_coder(u32 fmt, vCoderArgs_t *params)
{
    assert ( (params->frameRate > 0)
            && "H264_new_coder: frame rate must be positive"
           );

    assert ( (params->width % 16 == 0 && params->height % 16 == 0)
            &&
            "H264_new_coder::geometry must be %%16"
           );

    x264_param_t H264param;
    x264_t       *h= NULL;

    vCoder_t *H264Coder = new vCoder_t;

    printf("X264_BUILD= %d\n", X264_BUILD);

    x264_param_default(&H264param);

#if X264_BUILD < 86
    // set params as if profile=high, preset=veryfast and tune=zerolatency
    // those params allowed in this x264 library version
    // I copied the params from X264_BUILD 112, and hold those that compile
    
    // preset "veryfast"
    H264param.analyse.i_me_method = X264_ME_HEX;
    H264param.analyse.i_subpel_refine = 2;
    H264param.i_frame_reference = 1;
    H264param.analyse.b_mixed_references = 0;
    H264param.analyse.i_trellis = 0;
#if X264_BUILD > 78
    H264param.analyse.i_weighted_pred = X264_WEIGHTP_BLIND;
#endif

    // set in "veryfast", also in "zerolatency"
    //H264param.rc.i_lookahead = 10;

    // tune "zerolatency"
#if X264_BUILD > 68
    H264param.rc.i_lookahead = 0;
#endif
#if X264_BUILD > 70
    H264param.i_sync_lookahead = 0;
#endif
    H264param.i_bframe = 0;
#if X264_BUILD > 79
    H264param.b_sliced_threads = 1;
#endif
#if X264_BUILD > 80
    H264param.b_vfr_input = 0;
#endif
#if X264_BUILD > 68
    H264param.rc.b_mb_tree = 0;
#endif

    // profile "high"
    // nothing to do :-)

#else
    const char *preset= "veryfast";
    const char *tune= "zerolatency";
    const char *profile= "high";

    if (x264_param_default_preset(&H264param, preset, tune) < 0)
    {
        printf("H264_new_coder: failed to set preset [%s,%s]\n",
               preset,
               tune
              );
        return NULL;
    }
    if (x264_param_apply_profile(&H264param, profile) < 0)
    {
        printf("H264_new_coder: failed to apply profile [%s,%s]\n",
               preset,
               tune
              );
        return NULL;
    }
#endif

    // log level
    //H264param.i_log_level= X264_LOG_ERROR;
    H264param.i_log_level= X264_LOG_NONE;
    // default X264_LOG_INFO

    // one thread consumes less CPU
    //H264param.i_threads= 1;
    //H264param.b_deterministic= 0;
    H264param.i_frame_total= 0; // total number of frames is unknown

    H264param.i_width     = params->width;
    H264param.i_height    = params->height;

    H264param.i_keyint_max= params->maxInter;
    //H264param.i_keyint_min= 1;

#if X264_BUILD < 81
    H264param.i_fps_num= 1000*params->frameRate; // this IS correct
    H264param.i_fps_den= 1000;
#else
    H264param.i_fps_num= H264param.i_timebase_num= params->frameRate; // this IS correct
    H264param.i_fps_den= H264param.i_timebase_den= 1;
#endif

#if X264_BUILD > 59
    H264param.i_bframe_adaptive = X264_B_ADAPT_FAST;
#endif
    H264param.i_bframe= 0;

#if X264_BUILD > 70
    H264param.i_slice_max_size= 900;
#endif

#if 0
    H264param.rc.i_bitrate= params->bitRate / 1000 ; // kbps in libx264
    H264param.rc.i_rc_method= X264_RC_ABR; // Average Bit Rate
    //H264param.rc.f_qcompress= 0.0; // 0.0 -> CBR, 1.0 -> constant Q
#endif
// should be else?
#if 1
    H264param.rc.i_vbv_max_bitrate= params->bitRate / 1000;
    H264param.rc.i_vbv_buffer_size= params->bitRate / params->frameRate / 1000;
#endif

#if X264_BUILD > 80
    H264param.b_intra_refresh= 1;
#endif

    H264param.rc.b_stat_write= 1;

    if ((h = x264_encoder_open(&H264param )) == NULL )
    {
        printf("H264_new_coder: x264_encoder_open failed\n" );
        return NULL;
    }

    H264Coder->encoderContext = (void*)h;
    H264Coder->Delete         = H264_delete_coder;
    H264Coder->Encode         = H264_encode;
    H264Coder->GetParams      = H264_get_coder_params;
    H264Coder->format         = fmt;

    // accepts only YUV420P
    params->format= I420P_FORMAT;

    memcpy(&H264Coder->params, params, sizeof(vCoderArgs_t));

    return H264Coder;
}

void
H264_delete_coder(void *context)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);
    x264_t *h = static_cast<x264_t *>(coder->encoderContext);

    x264_encoder_close(h);
    delete coder;
}

static int pts=0;

int
H264_encode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           )
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);
    x264_t *h = static_cast<x264_t *>(coder->encoderContext);

    unsigned size = coder->params.width * coder->params.height;

    x264_picture_t pic;
#if X264_BUILD >= 98
    x264_picture_init(&pic);
#endif

    pic.i_type       = X264_TYPE_AUTO;
#if X264_BUILD < 110
    pic.i_qpplus1    = 0;
#else
    pic.i_qpplus1    = X264_QP_AUTO;
#endif
    pic.i_pts        = 0; // pts++;
#if X264_BUILD > 79
    pic.param        = NULL;
#endif
    pic.img.i_csp    = X264_CSP_I420;
    pic.img.i_plane  = 3;
    pic.img.plane[0] = inBuff;
    pic.img.plane[1] = inBuff + size;
    pic.img.plane[2] = inBuff + size + size / 4;
    pic.img.i_stride[0] = coder->params.width;
    pic.img.i_stride[1] = coder->params.width / 2;
    pic.img.i_stride[2] = coder->params.width / 2;

    x264_picture_t pic_out;
    x264_nal_t *nals;
    int i_nal, i_frame_size;
    i_frame_size= x264_encoder_encode(h, &nals, &i_nal, &pic, &pic_out);
    if (i_frame_size < 0)
    {
        printf("H264_encode:: x264 encoder_encode failed\n");
        return -1;
    }

#if X264_BUILD <= 75
    int ret = 0;

    for (int i= 0; i < i_nal; i++)
    {
        int i_data= outBuffLen, i_size;
        i_size= x264_nal_encode(outBuff, &i_data, 1, &nals[i]);
        if (i_size < 0)
        {
            return -1;
        }
        outBuff += i_size;
        ret     += i_size;
    }

    return ret;
#else
#if 0
    printf("x264 i_frame_size= %d, nals= %d\n", i_frame_size, i_nal);

    int ret = 0;
    
    for (int i= 0; i < i_nal; i++)
    {
        memcpy(outBuff, nals[i].p_payload, nals[i].i_payload);
        outBuff += nals[i].i_payload;
        ret     += nals[i].i_payload;
    }
    
    return ret;

#else
    if (i_frame_size)
    {
        memcpy(outBuff, nals[0].p_payload, i_frame_size);
    }

    return i_frame_size;
#endif
#endif
}

void
H264_get_coder_params(void *context, vCoderArgs_t *params)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    memcpy(params, &coder->params, sizeof(vCoderArgs_t));
}

