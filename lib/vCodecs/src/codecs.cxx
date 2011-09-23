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
// $Id: codecs.cxx 22045 2011-02-23 17:26:23Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>

#ifdef __SYMBIAN32__
#include "vCodecs/codec.h"
#include "../include/vCodecs/codecs.h"
#else
#include <vCodecs/codec.h>
#include <vCodecs/codecs.h>
#endif
#include <vCodecs/fourcc.h>

#include "codecs/CellBCodec.h"
#include "codecs/H263Codec.h"
#include "codecs/XVIDCodec.h"
#ifndef __SYMBIAN32__
#ifndef WIN32
#include "codecs/H264Codec.h"
#endif
#include "codecs/MJPEGCodec.h"
#endif

#include "codecs/ffmpegWrapper.h"

typedef struct
{
    u32 fmt; // output format, internal library id
    const char *description;
    vCoder_t* (*newCoder)(u32 fmt, vCoderArgs_t *params);
} vCoderFactory_t;

typedef struct
{
    u32 fmt; // input format, internal library id
    const char *description;
    vDecoder_t* (*newDecoder)(u32 fmt, vDecoderArgs_t *params);
} vDecoderFactory_t;


vCoderFactory_t coderFactory[] =
{
#ifndef WIN32
   {CELB_FORMAT,  "CellB Encoder",           CELLB_new_coder  },
#endif
   {H263_FORMAT,  "H263 ISABEL coder",       H263_new_coder   },
#ifndef WIN32
   {H263P_FORMAT, "FFMPEG ISABEL coder",     FFMPEG_new_coder },
   {H264_FORMAT,  "x264",                    H264_new_coder   },
   {MPEG1_FORMAT, "MPEG1 FFMPEG Encoder",    FFMPEG_new_coder },
   {MPEG2_FORMAT, "MPEG2-TS FFMPEG Encoder", FFMPEG_new_coder },
   {MPEG4_FORMAT, "MPEG4 FFMPEG Encoder",    FFMPEG_new_coder },
#endif
   {XVID_FORMAT,  "XVID Core Encoder",       XVID_new_coder   },
   {MJPEG_FORMAT, "MJPEG",                   MJPEG_new_coder  }
};

vDecoderFactory_t decoderFactory[] =
{
#ifndef WIN32
   {CELB_FORMAT,  "CellB Decoder",            CELLB_new_decoder  },
#endif
#ifdef WIN32
   {H263_FORMAT,  "H263 ISABEL Decoder",      H263_new_decoder   },
#else
   {H263_FORMAT,  "H263 ISABEL Decoder",      FFMPEG_new_decoder },
   {H263P_FORMAT, "FFMPEG ISABEL Decoder",    FFMPEG_new_decoder },
   {H264_FORMAT,  "H264 FFMPEG Decoder",      FFMPEG_new_decoder },
   {MPEG1_FORMAT, "MPEG1 FFMPEG Decoder",     FFMPEG_new_decoder },
   {MPEG2_FORMAT, "MPEG2-TS FFMPEG Decoder",  FFMPEG_new_decoder },
   {MPEG4_FORMAT, "MPEG4 FFMPEG Decoder",     FFMPEG_new_decoder },
#endif
   {XVID_FORMAT,  "XVID Core Decoder",        XVID_new_decoder   },
   {MJPEG_FORMAT, "MJPEG",                    MJPEG_new_decoder  }
};

const int MAX_CODERS   = ((sizeof(coderFactory)/sizeof(vCoderFactory_t)));
const int MAX_DECODERS = ((sizeof(decoderFactory)/sizeof(vDecoderFactory_t)));

__EXPORT int
vCodecInit(void)
{
#ifndef WIN32
    FFMPEG_global_init();
#endif

    XVID_global_init();

    return 0;
}

__EXPORT int
vCodecRelease(void)
{
    return 0;
}

__EXPORT vCoder_t *
vGetCoderByFmt(u32 fmt, vCoderArgs_t *args)
{
    for (int i = 0; i < MAX_CODERS; i++)
    {
        if (coderFactory[i].fmt == fmt)
        {
            return coderFactory[i].newCoder(fmt, args);
        }
    }
    return NULL;
}

__EXPORT vDecoder_t *
vGetDecoderByFmt(u32 fmt, vDecoderArgs_t *args)
{
    for (int i = 0; i < MAX_DECODERS; i++)
    {
        if (decoderFactory[i].fmt == fmt)
        {
            return decoderFactory[i].newDecoder(fmt, args);
        }
    }
    return NULL;
}


__EXPORT int
vGetNumRegisteredCoders(void)
{
    return MAX_CODERS;
}

__EXPORT int
vGetNumRegisteredDecoders(void)
{
    return MAX_DECODERS;
}

__EXPORT int
vGetCodecFmtList(u32 *buffer, int buffer_size)
{
    int i;

    for ( i = 0 ; i < MAX_CODERS; i++ )
    {
        if (i >= buffer_size)
        {
            // cannot put into 'buffer', simply count how many Fmts there are
            continue;
        }
        buffer[i]= coderFactory[i].fmt;
    }

    return i;
}

