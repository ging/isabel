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
// $Id: CellBImpl.hh 10673 2007-08-30 10:54:34Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __cellb_implementation__hh__
#define __cellb_implementation__hh__

#include <vCodecs/general.h>

class cellbEncodeSession_t;
class cellbDecodeSession_t;

cellbEncodeSession_t *cellbCreateEncodeSession(void);
cellbDecodeSession_t *cellbCreateDecodeSession(void);

void cellbDestroyEncodeSession(cellbEncodeSession_t *);
void cellbDestroyDecodeSession(cellbDecodeSession_t *);


struct cellbEncodedImage_t
{
    u8         *buffer;
    unsigned    numBytes;
    unsigned    w;
    unsigned    h;

    ~cellbEncodedImage_t(void) { free(buffer); buffer= NULL; }
};

cellbEncodedImage_t *
cellbEncode(cellbEncodeSession_t *, const u8 *b, unsigned w, unsigned h);

int
cellbDecode(cellbDecodeSession_t *,
            unsigned w,
            unsigned h,
            unsigned char *outBuff,
            int outBuffLen,
            unsigned char *inBuff,
            int inBuffLen
           );


#endif
