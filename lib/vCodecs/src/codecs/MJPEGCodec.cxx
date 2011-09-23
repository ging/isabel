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
// $Id: MJPEGCodec.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "MJPEGCodec.h"
#include "JPEG/mjpegImpl.h"

int
MJPEG_encode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            );

void
MJPEG_delete_coder(void *context);

int
MJPEG_decode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            );

void
MJPEG_delete_decoder(void *context);

void
MJPEG_get_coder_params(void *context, vCoderArgs_t *params);

void
MJPEG_get_decoder_params(void *context, vDecoderArgs_t *params);


//---------------------------------------------
// Local functions declaration
//---------------------------------------------
int
MJPEG_encode(unsigned char *inBuff,
             unsigned int   inBuffLen,
             unsigned char *outBuff,
             unsigned int   outBuffLen
            );

int
MJPEG_decode(unsigned char *inBuff,
             unsigned int   inBuffLen,
             unsigned char *outBuff,
             unsigned int   outBuffLen
            );

//---------------------------------------------
// Functions
//---------------------------------------------

vCoder_t *
MJPEG_new_coder(u32 fmt, vCoderArgs_t *params)
{
    vCoder_t *MJPEGCoder = new vCoder_t;

    mjpegEncodeSession_t *session= mjpegCreateEncodeSession();

    MJPEGCoder->encoderContext = (void*)session;
    MJPEGCoder->Delete         = MJPEG_delete_coder;
    MJPEGCoder->Encode         = MJPEG_encode;
    MJPEGCoder->GetParams      = MJPEG_get_coder_params;
    MJPEGCoder->format         = fmt;

    if (params->quality > 80) params->quality = 80;

    // accepts only RGB24
    params->format= RGB24_FORMAT;

    memcpy(&MJPEGCoder->params, params, sizeof(vCoderArgs_t));

    return MJPEGCoder;
}

void
MJPEG_delete_coder(void *context)
{
}

vDecoder_t *
MJPEG_new_decoder(u32 fmt, vDecoderArgs_t *params)
{
    vDecoder_t *MJPEGDecoder = new vDecoder_t;

    mjpegDecodeSession_t *session= mjpegCreateDecodeSession();

    MJPEGDecoder->decoderContext = (void*)session;
    MJPEGDecoder->Delete         = MJPEG_delete_decoder;
    MJPEGDecoder->Decode         = MJPEG_decode;
    MJPEGDecoder->GetParams      = MJPEG_get_decoder_params;
    MJPEGDecoder->format         = fmt;

    // returns only RGB24
    params->format= RGB24_FORMAT;

    memcpy(&MJPEGDecoder->params, params, sizeof(vDecoderArgs_t));

    return MJPEGDecoder;
}

void
MJPEG_delete_decoder(void *context)
{
}

int
MJPEG_encode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            )
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    mjpegEncodeSession_t *session=
        static_cast<mjpegEncodeSession_t *>(coder->encoderContext);

    int ret =  mjpegEncode(session,
                           outBuff, outBuffLen,
                           inBuff,  inBuffLen,
                           coder->params.width,
                           coder->params.height,
                           coder->params.quality
                          );

    return ret;
}

int
MJPEG_decode(void           *context,
             unsigned char  *inBuff,
             int             inBuffLen,
             unsigned char  *outBuff,
             int             outBuffLen
            )
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    mjpegDecodeSession_t *session=
        static_cast<mjpegDecodeSession_t *>(decoder->decoderContext);

    int len = decoder->params.width * decoder->params.height * 3;

    if (outBuffLen < len)
    {
        return len;
    }

    int res= mjpegDecode(session,
                         outBuff, outBuffLen,
                         inBuff,  inBuffLen,
                         decoder->params.width,
                         decoder->params.height
                        );

    if (res < 0) return res;

    return len;
}

void
MJPEG_get_coder_params(void *context, vCoderArgs_t *params)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    memcpy(params, &coder->params, sizeof(vCoderArgs_t));
}

void
MJPEG_get_decoder_params(void *context, vDecoderArgs_t *params)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    memcpy(params, &decoder->params, sizeof(vDecoderArgs_t));
}

