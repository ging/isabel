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
// $Id: imgTranslator.hh 11967 2007-10-19 16:52:45Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vutils__img_translator_hh__
#define __vutils__img_translator_hh__

#include <vCodecs/codec.h>
#include <vCodecs/colorspace.h>

typedef void (colorspace_t::*TRANSFORMER)(unsigned char *, int &, int &, unsigned char *, int, int);

class imgTranslator_t
{
private:

    typedef struct
    {
        u32 inFmt;
        u32 outFmt;
        TRANSFORMER translator;
    } transformation_t;

    static transformation_t conversion[];
    static const int MAX_TRANSFORMATIONS;

    TRANSFORMER translator;
    u32 inFmt, outFmt;

public:
    static TRANSFORMER getTransformation(u32 inFormat, u32 outFormat);

    imgTranslator_t (u32 ninFmt, u32 noutFmt);
    ~imgTranslator_t(void);

    u32 getInputFormat(void);
    u32 getOutputFormat(void);

    // as a result of a colorspace translation, the resulting image
    // can be smaller than original one. Typical example is converting
    // from RGB to YUV, which must generate and even width/height image
    // (or even divisible by 4 if YUV411, etc.)
    void translate(unsigned char *dst,
                   int &dstWidth,
                   int &dstHeight,
                   unsigned char *src,
                   int srcWidth,
                   int srcHeight
                  );
};

#endif
