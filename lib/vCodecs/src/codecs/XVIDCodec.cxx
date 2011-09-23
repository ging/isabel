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
// $Id: XVIDCodec.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include "xvid/src/xvid.h"
#else
#include <xvid.h>
#endif

#include "XVIDCodec.h"

int
XVID_encode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           );

void
XVID_delete_coder(void *context);

int
XVID_decode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           );

void
XVID_delete_decoder(void *context);

void
XVID_get_coder_params(void *context, vCoderArgs_t *params);

void
XVID_get_decoder_params(void *context, vDecoderArgs_t *params);

#undef READ_PNM

/*****************************************************************************
 *                            Quality presets
 ****************************************************************************/

static const int motion_presets[] = {
    /* quality 0 */
    0,

    /* quality 1 */
    XVID_ME_ADVANCEDDIAMOND16,

    /* quality 2 */
    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16,

    /* quality 3 */
    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
    XVID_ME_ADVANCEDDIAMOND8  | XVID_ME_HALFPELREFINE8,

    /* quality 4 */
    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
    XVID_ME_ADVANCEDDIAMOND8  | XVID_ME_HALFPELREFINE8  |
    XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

    /* quality 5 */
    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
    XVID_ME_ADVANCEDDIAMOND8  | XVID_ME_HALFPELREFINE8  |
    XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

    /* quality 6 */
    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 | XVID_ME_EXTSEARCH16 |
    XVID_ME_ADVANCEDDIAMOND8  | XVID_ME_HALFPELREFINE8  | XVID_ME_EXTSEARCH8  |
    XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

};
#define ME_ELEMENTS (sizeof(motion_presets)/sizeof(motion_presets[0]))

static const int vop_presets[] = {
    /* quality 0 */
    0,

    /* quality 1 */
    0,

    /* quality 2 */
    XVID_VOP_HALFPEL,

    /* quality 3 */
    XVID_VOP_HALFPEL | XVID_VOP_INTER4V,

    /* quality 4 */
    XVID_VOP_HALFPEL | XVID_VOP_INTER4V,

    /* quality 5 */
    XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
    XVID_VOP_TRELLISQUANT,

    /* quality 6 */
    XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
    XVID_VOP_TRELLISQUANT | XVID_VOP_HQACPRED,

};
#define VOP_ELEMENTS (sizeof(vop_presets)/sizeof(vop_presets[0]))

/*****************************************************************************
 *                     Command line global variables
 ****************************************************************************/


/*****************************************************************************
 *                     Default variables
 ****************************************************************************/

#define MAX_ZONES 64

static xvid_enc_zone_t ZONES[MAX_ZONES];
static int NUM_ZONES = 0;

//---------------------------------------------
// Local functions declaration
//---------------------------------------------
int
XVID_encode(unsigned char *inBuff,
            unsigned int   inBuffLen,
            unsigned char *outBuff,
            unsigned int   outBuffLen
           );

int
XVID_decode(unsigned char *inBuff,
            unsigned int   inBuffLen,
            unsigned char *outBuff,
            unsigned int   outBuffLen
           );

//---------------------------------------------
// Functions
//---------------------------------------------
int
XVID_global_init(void)
{
    xvid_gbl_init_t xvid_gbl_init;

    /*------------------------------------------------------------------------
     * XviD core initialization
     *----------------------------------------------------------------------*/

    /* Set version -- version checking will done by xvidcore */
    memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
    xvid_gbl_init.version = XVID_VERSION;
    xvid_gbl_init.debug = 0;
    xvid_gbl_init.cpu_flags = 0;//XVID_CPU_FORCE;

    /* Initialize XviD core -- Should be done once per __process__ */
    xvid_global(NULL, XVID_GBL_INIT, &xvid_gbl_init, NULL);
    return 0;
}

vCoder_t *
XVID_new_coder(u32 fmt, vCoderArgs_t *params)
{
    assert ( (params->frameRate > 0)
            && "XVID_new_coder: frame rate must be positive"
           );

    assert ( (params->width % 2 == 0 && params->height % 2 == 0)
            &&
            "XVID_new_coder::geometry must be %%2"
           );

    vCoder_t *XVIDCoder = new vCoder_t;

    //--------------------------------------------
    // coder creation
    //--------------------------------------------

    xvid_enc_plugin_t plugins[1];
    memset(&plugins[0],0,sizeof(xvid_enc_plugin_t));
    xvid_plugin_single_t *single = new xvid_plugin_single_t;

    xvid_enc_create_t *xvid_enc_create = new xvid_enc_create_t; //encoder context
    memset(xvid_enc_create, 0, sizeof(xvid_enc_create_t));


    /* Retrieve version*/
    xvid_enc_create->version = XVID_VERSION;

    /* Width and Height of input frames */
    xvid_enc_create->width = params->width;
    xvid_enc_create->height = params->height;
    xvid_enc_create->profile = XVID_PROFILE_S_L0;

    /* init plugins  */
    xvid_enc_create->zones = ZONES;
    xvid_enc_create->num_zones = NUM_ZONES;

    xvid_enc_create->plugins = plugins;
    xvid_enc_create->num_plugins = 1;

    memset(single, 0, sizeof(xvid_plugin_single_t));
    single->version = XVID_VERSION;
    single->bitrate = params->bitRate;
    plugins[0].func  = xvid_plugin_single;
    plugins[0].param = single;

    /* No fancy thread tests */
    xvid_enc_create->num_threads = 0;

    /* Frame rate - Do some quick float fps = fincr/fbase hack */
    xvid_enc_create->fincr = 1000;
    xvid_enc_create->fbase = 1000*params->frameRate;
    xvid_enc_create->frame_drop_ratio = 0;

    /* Maximum key frame interval */
    xvid_enc_create->max_key_interval = params->maxInter;

    /* Bframes settings */
    xvid_enc_create->max_bframes = 0;
    xvid_enc_create->bquant_ratio = 0;
    xvid_enc_create->bquant_offset = 0;

    /* Global encoder options */
    xvid_enc_create->global = 0;

    /* I use a small value here, since will not encode whole movies, but short clips */
    xvid_encore(NULL, XVID_ENC_CREATE, xvid_enc_create, NULL);

    XVIDCoder->encoderContext = (void*)xvid_enc_create;
    XVIDCoder->Delete         = XVID_delete_coder;
    XVIDCoder->Encode         = XVID_encode;
    XVIDCoder->GetParams      = XVID_get_coder_params;
    XVIDCoder->format         = fmt;

    switch (params->format)
    {
    case I420P_FORMAT:
    case BGR24_FORMAT:
        break;    // accepted formats
    default:
        // other formats are not accepted, we impose I420P
        params->format= I420P_FORMAT;
    }

    memcpy(&XVIDCoder->params, params, sizeof(vCoderArgs_t));

    return XVIDCoder;
}

void
XVID_delete_coder(void *context)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);
    xvid_enc_create_t *XVIDContext =
         static_cast<xvid_enc_create_t *>(coder->encoderContext);

    /* Destroy the encoder instance */
    xvid_encore(XVIDContext->handle, XVID_ENC_DESTROY, NULL, NULL);

    delete XVIDContext;
    delete coder;
}

vDecoder_t *
XVID_new_decoder(u32 fmt, vDecoderArgs_t *params)
{
    vDecoder_t *XVIDDecoder = new vDecoder_t;

    //--------------------------------------------
    // decoder creation
    //--------------------------------------------

    xvid_dec_create_t *xvid_dec_create = new xvid_dec_create_t;
    memset(xvid_dec_create,0,sizeof(xvid_dec_create_t));

    /* Version */
    xvid_dec_create->version = XVID_VERSION;

    /*
     * Image dimensions -- set to 0, xvidcore will resize when ever it is
     * needed
     */
    xvid_dec_create->width = 0;//params->Width;
    xvid_dec_create->height = 0;//params->Height;

    xvid_decore(NULL, XVID_DEC_CREATE, xvid_dec_create, NULL);

    XVIDDecoder->decoderContext = (void *)xvid_dec_create;
    XVIDDecoder->Delete         = XVID_delete_decoder;
    XVIDDecoder->Decode         = XVID_decode;
    XVIDDecoder->GetParams      = XVID_get_decoder_params;
    XVIDDecoder->format         = fmt;

    switch (params->format)
    {
    case I420P_FORMAT:
    case BGR24_FORMAT:
        break;    // accepted formats
    default:
        // other formats are not accepted, we impose I420P
        params->format= I420P_FORMAT;
    }

    memcpy(&XVIDDecoder->params, params, sizeof(vDecoderArgs_t));

    return XVIDDecoder;
}

void
XVID_delete_decoder(void *context)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    xvid_dec_create_t *XVIDContext =
         static_cast<xvid_dec_create_t *>(decoder->decoderContext);

    xvid_decore(XVIDContext->handle, XVID_DEC_DESTROY, NULL, NULL);

    delete XVIDContext;
    delete decoder;
}

int
XVID_encode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           )
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);
    xvid_enc_create_t *XVIDContext =
         static_cast<xvid_enc_create_t *>(coder->encoderContext);

    xvid_enc_frame_t xvid_enc_frame;
    xvid_enc_stats_t xvid_enc_stats;

    /* Version for the frame and the stats */
    memset(&xvid_enc_frame, 0, sizeof(xvid_enc_frame));
    xvid_enc_frame.version = XVID_VERSION;

    memset(&xvid_enc_stats, 0, sizeof(xvid_enc_stats));
    xvid_enc_stats.version = XVID_VERSION;

    /* Bind output buffer */
    xvid_enc_frame.bitstream = outBuff;
    xvid_enc_frame.length = outBuffLen;

    int BPP=1;
    /* Initialize input image fields */
    if (inBuff)
    {
        switch(coder->params.format)
        {
        case I420P_FORMAT:
            BPP=1;
            xvid_enc_frame.input.csp = XVID_CSP_I420;
            xvid_enc_frame.input.plane[0] = inBuff;
            xvid_enc_frame.input.stride[0] = XVIDContext->width*BPP;
            xvid_enc_frame.input.plane[1] = inBuff + XVIDContext->height*XVIDContext->width*BPP;
            xvid_enc_frame.input.stride[1] = XVIDContext->width*BPP/2;
            xvid_enc_frame.input.plane[2] = inBuff + XVIDContext->height*XVIDContext->width*BPP*5/4;
            xvid_enc_frame.input.stride[2] = XVIDContext->width*BPP/2;

            break;
        case BGR24_FORMAT:
            BPP=3;
            xvid_enc_frame.input.csp = XVID_CSP_BGR;
            xvid_enc_frame.input.plane[0] = inBuff;
            xvid_enc_frame.input.stride[0] = XVIDContext->width*BPP;

            break;
        default:
            fprintf(stderr,
                    "XVID_encode: cannot handle format [%d]\n",
                    coder->params.format
                   );
            abort();
        }
    }
    else
    {
        xvid_enc_frame.input.csp = XVID_CSP_NULL;
    }

    /* Set up core's general features */
    xvid_enc_frame.vol_flags = 0;

    /* Set up core's general features */
    xvid_enc_frame.vop_flags = vop_presets[0];

    /* Frame type -- let core decide for us */
    xvid_enc_frame.type = XVID_TYPE_AUTO;

    /* Force the right quantizer -- It is internally managed by RC plugins */
    xvid_enc_frame.quant = 0;

    /* Set up motion estimation flags */
    xvid_enc_frame.motion = motion_presets[0];

    /* We don't use special matrices */
    xvid_enc_frame.quant_intra_matrix = NULL;
    xvid_enc_frame.quant_inter_matrix = NULL;

    /* Encode the frame */
    int ret = xvid_encore(XVIDContext->handle,
                          XVID_ENC_ENCODE,
                          &xvid_enc_frame,
                          &xvid_enc_stats
                         );

    return ret;
}

int
XVID_decode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           )
{
    int ret;

    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    xvid_dec_create_t *XVIDContext =
         static_cast<xvid_dec_create_t *>(decoder->decoderContext);

    xvid_dec_stats_t xvid_dec_stats;
    memset(&xvid_dec_stats, 0, sizeof(xvid_dec_stats_t));

    xvid_dec_frame_t xvid_dec_frame;
    memset(&xvid_dec_frame, 0, sizeof(xvid_dec_frame_t));

    /* Set version */
    xvid_dec_frame.version = XVID_VERSION;
    xvid_dec_stats.version = XVID_VERSION;


    /* No general flags to set */
    xvid_dec_frame.general          = 0;

    /* Input stream */
    xvid_dec_frame.bitstream        = inBuff;
    xvid_dec_frame.length           = inBuffLen;
decore:

    int BPP = 1;
    double Factor = 0;
    /* Output frame structure */
    switch(decoder->params.format)
    {
    case I420P_FORMAT:
        BPP = 1;
        Factor = 1.5;

        xvid_dec_frame.output.csp = XVID_CSP_I420;
        xvid_dec_frame.output.plane[0]  = &outBuff[0];
        xvid_dec_frame.output.stride[0] = XVIDContext->width*BPP;
        xvid_dec_frame.output.plane[1]  = &outBuff[XVIDContext->height*XVIDContext->width*BPP];
        xvid_dec_frame.output.stride[1] = XVIDContext->width*BPP/2;
        xvid_dec_frame.output.plane[2]  = &outBuff[XVIDContext->height*XVIDContext->width*BPP*5/4];
        xvid_dec_frame.output.stride[2] = XVIDContext->width*BPP/2;
        xvid_dec_frame.output.plane[3]  = 0;
        xvid_dec_frame.output.stride[3] = 0;

        break;
    case BGR24_FORMAT:
        BPP = 3;
        Factor = 3;

        xvid_dec_frame.output.csp = XVID_CSP_BGR;
        xvid_dec_frame.output.plane[0]  = outBuff;
        xvid_dec_frame.output.stride[0] = XVIDContext->width*BPP;

        break;
    default:
        fprintf(stderr,
                "XVID_decoder: cannot handle format [%d]\n",
                decoder->params.format
               );
        abort();
    }

    ret = xvid_decore(XVIDContext->handle, XVID_DEC_DECODE, &xvid_dec_frame, &xvid_dec_stats);

    if (xvid_dec_stats.data.vol.height > 0 &&
        xvid_dec_stats.data.vol.width  > 0)
    {
        XVIDContext->height = decoder->params.height = xvid_dec_stats.data.vol.height;
        XVIDContext->width  = decoder->params.width  = xvid_dec_stats.data.vol.width;

        xvid_dec_frame.bitstream = &inBuff[ret];
        xvid_dec_frame.length   -= ret;

        if (xvid_dec_frame.length>0)
        {
            goto decore;
        }
    }

    return int(XVIDContext->width*XVIDContext->height*Factor);
}

void
XVID_get_coder_params(void *context, vCoderArgs_t *params)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    memcpy(params, &coder->params, sizeof(vCoderArgs_t));
}

void
XVID_get_decoder_params(void *context, vDecoderArgs_t *params)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    memcpy(params, &decoder->params, sizeof(vDecoderArgs_t));
}

