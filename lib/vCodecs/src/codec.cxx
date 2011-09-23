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
// $Id: codec.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifdef __SYMBIAN32__
#include "../include/vCodecs/codec.h"
#else
#include <vCodecs/codec.h>
#endif

__EXPORT void
vDeleteCoder(vCoder_t *coder)
{
    coder->Delete(coder);
}

__EXPORT void
vDeleteDecoder(vDecoder_t *decoder)
{
    decoder->Delete(decoder);
}

__EXPORT int
vEncode(vCoder_t      *coder,
        unsigned char *inBuff,
        int            inBuffLen,
        unsigned char *outBuff,
        int            outBuffLen
       )
{
    return coder->Encode(coder,
                         inBuff,
                         inBuffLen,
                         outBuff,
                         outBuffLen
                        );
}

__EXPORT int
vDecode(vDecoder_t    *decoder,
        unsigned char *inBuff,
        int            inBuffLen,
        unsigned char *outBuff,
        int            outBuffLen
       )
{
    return decoder->Decode(decoder,
                           inBuff,
                           inBuffLen,
                           outBuff,
                           outBuffLen
                          );
}

__EXPORT void
vGetParams(vCoder_t *coder, vCoderArgs_t *params)
{
    coder->GetParams(coder, params);
}

__EXPORT void
vGetParams(vDecoder_t *decoder, vDecoderArgs_t *params)
{
    decoder->GetParams(decoder, params);
}

