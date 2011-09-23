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
// $Id: coderWrapper.cc 11883 2007-10-15 15:56:38Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vCodecs/codec.h>

#include <vUtils/coderWrapper.h>

coderWrapper_t::coderWrapper_t(u32 ninFmt, u32 noutFmt, vCoderArgs_t *args)
: inFmt(ninFmt),
  outFmt(noutFmt),
  coder(NULL),
  translator(NULL),
  transBuff(NULL),
  transSize(-1)
{
    assert(args != NULL && "set coder wrapper parameters, please!");

    printf("BUILDING CODER FOR %s -> %s\n",
           vGetFormatNameById(inFmt),
           vGetFormatNameById(outFmt)
          );

    args->format= inFmt; // just in the case the user forgot it
    coder = vGetCoderByFmt(outFmt, args);
    if (coder == NULL)
    {
        printf("CANNOT BUILT THE CODER!!\n");
        printf("coderWrapper_t:: codec not found for %s!!\n",
               vGetFormatNameById(outFmt)
              );
        throw "No coder found";
    }

    if (args->format != inFmt)
    {
        try
        {
            translator = new imgTranslator_t(inFmt, args->format);
        }
        catch (const char *e)
        {
            printf("coderWrapper_t:: cannot build translator %s -> %s"
                   ", error= %s\n",
                   vGetFormatNameById(inFmt),
                   vGetFormatNameById(args->format),
                   e
                  );
            exit(1);
        }
        printf("coderWrapper_t:: built translator %s -> %s\n",
               vGetFormatNameById(inFmt),
               vGetFormatNameById(args->format)
              );
        transSize= 1024 * 768 * 3;
        transBuff= new unsigned char[transSize];
    }
    else
    {
        printf("coderWrapper_t:: no translator needed\n");
    }

    printf("CODER BUILT CORRECTLY\n");
}

coderWrapper_t::~coderWrapper_t(void)
{
    vDeleteCoder(coder);

    if (translator)
    {
        delete translator;
    }
    if (transBuff)
    {
        delete []transBuff;
    }
}

int
coderWrapper_t::encode(unsigned char *dst,
                       int dstlen,
                       unsigned char* src,
                       int srcWidth,
                       int srcHeight
                      )
{
    int inSize;
    unsigned char *inBuff;

    if (translator == NULL)
    {
        // if not translation, input for coder is the src image

        inSize = vFrameSize(srcWidth, srcHeight, inFmt);
        inBuff= src;
    }
    else
    {
        // if translation, input for coder is the translated image
        
        int dstWidth=-1, dstHeight= -1;

        inSize = vFrameSize(srcWidth, srcHeight, translator->getOutputFormat());

        if (inSize > transSize) // reallocation if needed
        {
            delete []transBuff;
            transSize= inSize;
            transBuff= new unsigned char[transSize];
            if ( ! transBuff )
            {
                printf("coderWrapper_t::encode: run out of memory\n");
                printf("coderWrapper_t::encode: skipping\n");
                transSize = -1;
                return -1;
            }
        }
 
        translator->translate(transBuff,
                              dstWidth, dstHeight,
                              src,
                              srcWidth, srcHeight
                             );

        inSize= vFrameSize(dstWidth, dstHeight, translator->getOutputFormat());
        inBuff= transBuff;
    }

    int n1 = vEncode(coder,
                     inBuff,
                     inSize,
                     dst,
                     dstlen
                    );

    if (n1 <= 0)
    {
        printf("coderWrapper_t::encode: vEncode error\n");
        printf("coderWrapper_t::encode: skipping\n");
        return -2;
    }

    if (dstlen < n1)
    {
        return 0;
    }

    return n1;
}

u32
coderWrapper_t::getInputFormat(void) const
{
    return inFmt;
}

u32
coderWrapper_t::getOutputFormat(void) const
{
    return outFmt;
}

bool
coderWrapper_t::setCoderArgs(vCoderArgs_t *args)
{
    vCoder_t *oldCoder= coder;

    // if there is a coder, try to keep old values in case
    // the new values fail

    coder = vGetCoderByFmt(outFmt, args);

    if (coder == NULL)
    {
        coder= oldCoder;
        return false;
    }

    vDeleteCoder(oldCoder);
    return true;
}

void
coderWrapper_t::getCoderArgs(vCoderArgs_t *args)
{
    vGetParams(coder, args);
}

