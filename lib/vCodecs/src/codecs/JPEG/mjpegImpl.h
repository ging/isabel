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
// $Id: mjpegImpl.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __mjpeg_implementation__hh__
#define __mjpeg_implementation__hh__
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
 
//#include <netinet/in.h>
 
#include <setjmp.h>
 
extern "C" {
#ifdef WIN32
#include "jpeglib/jpeglib.h"
#include "jpeglib/jerror.h"
#else
#include <jpeglib.h>
#include <jerror.h>
#endif
}
 
#include "jpegmgr.h"

class mjpegEncodeSession_t;
class mjpegDecodeSession_t;

mjpegEncodeSession_t *mjpegCreateEncodeSession(void);
mjpegDecodeSession_t *mjpegCreateDecodeSession(void);

void mjpegDestroyEncodeSession(mjpegEncodeSession_t *);
void mjpegDestroyDecodeSession(mjpegDecodeSession_t *);



int
mjpegEncode(mjpegEncodeSession_t *session,
            unsigned char *outBuff,
            int            outBuffLen,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned int   width,
            unsigned int   height,
			int            quality
           );

int
mjpegDecode(mjpegDecodeSession_t *session,
            unsigned char *outBuff,
            int            outBuffLen,
            unsigned char *inBuff,
            int            inBuffLen,
            unsigned int  &width,
            unsigned int  &height
           );

#endif
