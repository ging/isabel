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
// $Id: CellBWrapper.hh 10673 2007-08-30 10:54:34Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2007. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __cellb_wrapper_hh__
#define __cellb_wrapper_hh__

#include "CellBImpl.hh"

//
// codec rgb24 --> cellb
//
class cellbEncoder_t
{
private:
    cellbEncodeSession_t *session;

public:
    cellbEncoder_t(void)
    : session(cellbCreateEncodeSession())
    {
    }

    virtual ~cellbEncoder_t(void) {
        //
        // eliminar sesion, etc
        //
        cellbDestroyEncodeSession(session);
    }

    int
    codifica(unsigned int   width,
             unsigned int   height,
             int            quality,
             unsigned char *inBuff,
             int            inBuffLen,
             unsigned char *outBuff,
             int            outBuffLen
            )
    {
        cellbEncodedImage_t *fr= cellbEncode(session, inBuff, width, height);

        if (fr == NULL)
        {
            return -1;
        }

        int numBytes= fr->numBytes;

        if (numBytes > outBuffLen)
        {
            // user should alloc more bytes
            return fr->numBytes;
        }

        memcpy(outBuff, fr->buffer, numBytes);

        delete fr;

        return numBytes;
    }
};



//
// codec cellb --> rgb24
//
class cellbDecoder_t
{
private:

    cellbDecodeSession_t *session;

public:
    cellbDecoder_t(void)
    : session(cellbCreateDecodeSession())
    {
    }

    virtual ~cellbDecoder_t(void) {
        //
        // eliminar sesion, etc
        //
        cellbDestroyDecodeSession(session);
    }

    int
    decodifica(unsigned width,
               unsigned height,
               unsigned char *inBuff,
               int            inBuffLen,
               unsigned char *outBuff,
               int            outBuffLen
              )
    {
        int res= cellbDecode(session,
                             width, height,
                             outBuff, outBuffLen,
                             inBuff, inBuffLen
                            );

        return res;
    }
};

#endif

