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
// $Id: CellBCodec.cxx 12635 2008-02-14 17:30:10Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CellBCodec.h"
#include "CellB/CellBWrapper.hh"

int
CELLB_encode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            );

void
CELLB_delete_coder(void *context);

int
CELLB_decode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            );

void
CELLB_delete_decoder(void *context);

void
CELLB_get_coder_params(void *context, vCoderArgs_t *params);

void
CELLB_get_decoder_params(void *context, vDecoderArgs_t *params);


//---------------------------------------------
// Local functions declaration
//---------------------------------------------
int
CELLB_encode(unsigned char *inBuff,
             unsigned int   inBuffLen,
             unsigned char *outBuff,
             unsigned int   outBuffLen
            );

int
CELLB_decode(unsigned char *inBuff,
             unsigned int   inBuffLen,
             unsigned char *outBuff,
             unsigned int   outBuffLen
            );

//---------------------------------------------
// Functions
//---------------------------------------------

vCoder_t *
CELLB_new_coder(u32 fmt, vCoderArgs_t *params)
{
    vCoder_t *CELLBCoder = new vCoder_t;

    cellbEncoder_t *CELLBEnc= new cellbEncoder_t;

    CELLBCoder->encoderContext = (void*)CELLBEnc;
    CELLBCoder->Delete         = CELLB_delete_coder;
    CELLBCoder->Encode         = CELLB_encode;
    CELLBCoder->GetParams      = CELLB_get_coder_params;
    CELLBCoder->format         = fmt;

    // accepts only RGB24
    params->format= RGB24_FORMAT;

    memcpy(&CELLBCoder->params, params, sizeof(vCoderArgs_t));

    return CELLBCoder;
}

void
CELLB_delete_coder(void *context)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    cellbEncoder_t *CELLBEnc=
        static_cast<cellbEncoder_t *>(coder->encoderContext);

    coder->encoderContext= NULL;
    delete CELLBEnc;
    delete coder;
}

vDecoder_t *
CELLB_new_decoder(u32 fmt, vDecoderArgs_t *params)
{
    vDecoder_t *CELLBDecoder = new vDecoder_t;

    cellbDecoder_t *CELLBDec= new cellbDecoder_t;

    CELLBDecoder->decoderContext = (void*)CELLBDec;
    CELLBDecoder->Delete         = CELLB_delete_decoder;
    CELLBDecoder->Decode         = CELLB_decode;
    CELLBDecoder->GetParams      = CELLB_get_decoder_params;
    CELLBDecoder->format         = fmt;

    // accepts only RGB24
    params->format= RGB24_FORMAT;

    memcpy(&CELLBDecoder->params, params, sizeof(vDecoderArgs_t));

    return CELLBDecoder;
}

void
CELLB_delete_decoder(void *context)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    cellbDecoder_t *CELLBDec=
        static_cast<cellbDecoder_t*>(decoder->decoderContext);

    decoder->decoderContext= NULL;
    delete CELLBDec;
    delete decoder;
}

int
CELLB_encode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            )
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    cellbEncoder_t *CELLBEnc=
        static_cast<cellbEncoder_t *>(coder->encoderContext);

    int res = CELLBEnc->codifica(coder->params.width,
                                 coder->params.height,
                                 coder->params.quality,
                                 inBuff,  inBuffLen,
                                 outBuff, outBuffLen
                                );

    return res;
}

int
CELLB_decode(void          *context,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            )
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    cellbDecoder_t *CELLBDec=
        static_cast<cellbDecoder_t *>(decoder->decoderContext);

    int len = decoder->params.width * decoder->params.height * 3;

    if (outBuffLen < len)
    {
        return len;
    }

    int res= CELLBDec->decodifica(decoder->params.width,
                                  decoder->params.height,
                                  inBuff,  inBuffLen,
                                  outBuff, outBuffLen
                                 );

    if (res < 0) return res;

    return len;
}

void
CELLB_get_coder_params(void *context, vCoderArgs_t *params)
{
    vCoder_t *coder = static_cast<vCoder_t *>(context);

    memcpy(params, &coder->params, sizeof(vCoderArgs_t));
}

void
CELLB_get_decoder_params(void *context, vDecoderArgs_t *params)
{
    vDecoder_t *decoder = static_cast<vDecoder_t *>(context);

    memcpy(params, &decoder->params, sizeof(vDecoderArgs_t));
}

