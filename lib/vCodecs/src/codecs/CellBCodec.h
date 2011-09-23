/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: CellBCodec.h 12543 2008-02-01 17:42:11Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __cellb_codec_h__
#define __cellb_codec_h__

#ifdef __SYMBIAN32__
#include "../../include/vCodecs/codec.h"
#else
#include <vCodecs/codec.h>
#endif


//-------------------------------------------
// Initialize encoder for first use,
// pass all needed parameters to the codec
//-------------------------------------------

vCoder_t *
CELLB_new_coder(u32 fmt, vCoderArgs_t *params);

vDecoder_t *
CELLB_new_decoder(u32 fmt, vDecoderArgs_t *params);

#endif

