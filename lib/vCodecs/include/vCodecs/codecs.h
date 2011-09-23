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
// $Id: codecs.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vcodecs_codecs_h__
#define __vcodecs_codecs_h__

#ifdef __SYMBIAN32__
#include "vCodecs/codec.h"
#else
#include <vCodecs/codec.h>
#endif

__IMPORT int vCodecInit(void); //once per app

__IMPORT int vCodecRelease(void); //once per app

__IMPORT vCoder_t *vGetCoderByFmt(u32 fmt, vCoderArgs_t *args);

__IMPORT vDecoder_t *vGetDecoderByFmt(u32 fmt, vDecoderArgs_t *args);

__IMPORT int vGetNumRegisteredCoders(void);

__IMPORT int vGetNumRegisteredDecoders(void);

__IMPORT int vGetCodecFmtList(u32 *buffer, int buffer_size);

#endif
