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
// $Id: jpegImpl.hh 10576 2007-07-12 16:42:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __jpeg_implementation__hh__
#define __jpeg_implementation__hh__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>

#ifdef __BUILD_FOR_LINUX
#include <netinet/in.h>
#endif


#include <icf2/general.h>
#include <icf2/notify.hh>

#include <vUtils/image.h>

class jpegEncodeSession_t;
class jpegDecodeSession_t;

jpegEncodeSession_t *jpegCreateEncodeSession(void);
jpegDecodeSession_t *jpegCreateDecodeSession(void);

void jpegDestroyEncodeSession(jpegEncodeSession_t *);
void jpegDestroyDecodeSession(jpegDecodeSession_t *);


struct jpegEncodedImage_t
{
    u8           *buffer;
    unsigned      numBytes;
    unsigned      w;
    unsigned      h;
    unsigned      ts;
    bool          isCompleted;
    u8           *lqt;
    u8           *cqt;
    u16           dri;
    unsigned      bytesPopped;
    unsigned      lastOffsetPushed;
    int           quality;
    u8            type;
    u8            typeSpecific;

    ~jpegEncodedImage_t(void)
    {
        free(buffer); buffer= NULL;
        free(lqt);    lqt= NULL;
        free(cqt);    cqt= NULL;
    }
};

jpegEncodedImage_t *
jpegEncode(jpegEncodeSession_t *,
           const u8 *b,
           unsigned w,
           unsigned h,
           int quality,
           int dri
          );

u8 *jpegDecode(jpegDecodeSession_t *, jpegEncodedImage_t *);

image_t *jpegLoadFile(FILE *f);

bool jpegSaveFile(image_t *img, char *const path, bool addExt);


#endif
