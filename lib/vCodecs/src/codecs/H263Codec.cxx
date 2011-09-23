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
// $Id: H263Codec.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "H263Codec.h"
#include "H263/H263Encoder.hh"

int
H263_encode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           );

void
H263_delete_coder(void *context);

int
H263_decode(void           *context,
            unsigned char  *inBuff,
            int             inBuffLen,
            unsigned char  *outBuff,
            int             outBuffLen
           );

void
H263_delete_decoder(void *context);

void
H263_get_coder_params(void *context, vCoderArgs_t *params);

void
H263_get_decoder_params(void *context, vDecoderArgs_t *params);


//---------------------------------------------
// Local functions declaration
//---------------------------------------------
int
H263_encode(unsigned char *inBuff,
            unsigned int   inBuffLen,
            unsigned char *outBuff,
            unsigned int   outBuffLen
           );

int
H263_decode(unsigned char *inBuff,
            unsigned int   inBuffLen,
            unsigned char *outBuff,
            unsigned int   outBuffLen
           );

//---------------------------------------------
// Functions
//---------------------------------------------

vCoder_t *
H263_new_coder(u32 fmt, vCoderArgs_t *params)
{
    assert ( (params->frameRate > 0)
            && "H263_new_coder: frame rate must be positive"
           );

    vCoder_t *H263Coder = new vCoder_t;

    H263Encoder_t *H263E= new H263Encoder_t;

    H263Coder->encoderContext = (void*)H263E;
    H263Coder->Delete         = H263_delete_coder;
    H263Coder->Encode         = H263_encode;
    H263Coder->GetParams      = H263_get_coder_params;
    H263Coder->format         = fmt;

    // accepts only YUV::420P
    params->format= I420P_FORMAT;

    memcpy(&H263Coder->params, params, sizeof(vCoderArgs_t));

    return H263Coder;
}

void
H263_delete_coder(void *context)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    H263Encoder_t *H263E= static_cast<H263Encoder_t *>(coder->encoderContext);

    coder->encoderContext= NULL;
    delete H263E;
    delete coder;
}

vDecoder_t *
H263_new_decoder(u32 fmt, vDecoderArgs_t *params)
{
    vDecoder_t *H263Decoder = new vDecoder_t;

    H263Decoder_t *H263D= new H263Decoder_t;

    H263Decoder->decoderContext = (void*)H263D;
    H263Decoder->Delete         = H263_delete_decoder;
    H263Decoder->Decode         = H263_decode;
    H263Decoder->GetParams      = H263_get_decoder_params;
    H263Decoder->format         = fmt;

    // accepts only YUV::420P
    params->format= I420P_FORMAT;

    memcpy(&H263Decoder->params, params, sizeof(vDecoderArgs_t));

    return H263Decoder;
}

void
H263_delete_decoder(void *context)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    H263Decoder_t *H263D= static_cast<H263Decoder_t *>(decoder->decoderContext);

    decoder->decoderContext= NULL;
    delete H263D;
    delete decoder;
}

int
H263_encode(void          *context,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned char *outBuff,
            int            outBuffLen
           )
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    H263Encoder_t *H263E= static_cast<H263Encoder_t *>(coder->encoderContext);

    int res =  H263E->codifica(coder->params.width,
                               coder->params.height,
                               coder->params.bitRate,
                               coder->params.frameRate,
                               coder->params.maxInter,
                               coder->params.quality,
                               inBuff,  inBuffLen,
                               outBuff, outBuffLen
                              );

    return res;
}

int
H263_decode(void           *context,
            unsigned char  *inBuff,
            int             inBuffLen,
            unsigned char  *outBuff,
            int             outBuffLen
           )
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    H263Decoder_t *H263D= static_cast<H263Decoder_t *>(decoder->decoderContext);

    int len = decoder->params.width * decoder->params.height * 3 /2;

    if (outBuffLen < len)
    {
        return len;
    }

    int res= H263D->decodifica(decoder->params.width,
                               decoder->params.height,
                               inBuff,  inBuffLen,
                               outBuff, outBuffLen
                              );

    if (res < 0) return res;

    return len;
}

void
H263_get_coder_params(void *context, vCoderArgs_t *params)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    memcpy(params, &coder->params, sizeof(vCoderArgs_t));
}

void
H263_get_decoder_params(void *context, vDecoderArgs_t *params)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    memcpy(params, &decoder->params, sizeof(vDecoderArgs_t));
}

