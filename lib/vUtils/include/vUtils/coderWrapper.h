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
// $Id: coderWrapper.hh 11883 2007-10-15 15:56:38Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vutils__coder_wrapper_hh__
#define __vutils__coder_wrapper_hh__

#include <vCodecs/codecs.h>

#include <vUtils/imgTranslator.h>

//
// coder usually produces a CBR stream of 'br' bits per second
// unless br <= 0, which produces VBR stream of 'quality'
// Note that some formats (MJPEG) always produces VBR
//
class coderWrapper_t
{
private:
    u32 inFmt;
    u32 outFmt;

    vCoder_t        *coder;
    imgTranslator_t *translator;

    unsigned char *transBuff;
    int transSize;

public:
    coderWrapper_t(u32 ninFmt, u32 noutFmt, vCoderArgs_t *args);
    ~coderWrapper_t(void);

    int encode(unsigned char *dst,
               int dstlen,
               unsigned char *src,
               int srcWidth,
               int srcHeight
              );

    u32 getInputFormat(void) const;
    u32 getOutputFormat(void) const;

    bool setCoderArgs(vCoderArgs_t *args);
    void getCoderArgs(vCoderArgs_t *args);
};

#endif
