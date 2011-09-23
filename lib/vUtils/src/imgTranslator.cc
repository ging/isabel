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
// $Id: imgTranslator.cc 11968 2007-10-19 16:53:03Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <vCodecs/codecs.h>
#include <vCodecs/colorspace.h>

#include <vUtils/imgTranslator.h>

imgTranslator_t::transformation_t imgTranslator_t::conversion[]=
{
     {RGB24_FORMAT,  RGB24_FORMAT,  NULL                            },
     {RGB24_FORMAT,  BGR24_FORMAT,  &colorspace_t::RAW24swap        },
     {RGB24_FORMAT,  I422i_FORMAT,  &colorspace_t::RGB24toYUV422i   },
     {RGB24_FORMAT,  I422P_FORMAT,  &colorspace_t::RGB24toYUV422P   },
     {RGB24_FORMAT,  I420P_FORMAT,  &colorspace_t::RGB24toYUV420P   },
     {RGB24_FORMAT,  I411P_FORMAT,  &colorspace_t::RGB24toYUV411P   },

     {BGR24_FORMAT,  RGB24_FORMAT,  &colorspace_t::RAW24swap        },
     {BGR24_FORMAT,  BGR24_FORMAT,  NULL                            },
     {BGR24_FORMAT,  I422i_FORMAT,  &colorspace_t::BGR24toYUV422i   },
     {BGR24_FORMAT,  I422P_FORMAT,  &colorspace_t::BGR24toYUV422P   },
     {BGR24_FORMAT,  I420P_FORMAT,  &colorspace_t::BGR24toYUV420P   },
     {BGR24_FORMAT,  I411P_FORMAT,  &colorspace_t::BGR24toYUV411P   },

     {I422i_FORMAT,  RGB24_FORMAT,  &colorspace_t::YUV422itoRGB24   },
     {I422i_FORMAT,  BGR24_FORMAT,  &colorspace_t::YUV422itoBGR24   },
     {I422i_FORMAT,  I422i_FORMAT,  NULL                            },
     {I422i_FORMAT,  I422P_FORMAT,  &colorspace_t::YUV422itoYUV422P },
     {I422i_FORMAT,  I420P_FORMAT,  &colorspace_t::YUV422itoYUV420P },
     {I422i_FORMAT,  I411P_FORMAT,  NULL                            },

     {I422P_FORMAT,  RGB24_FORMAT,  &colorspace_t::YUV422PtoRGB24   },
     {I422P_FORMAT,  BGR24_FORMAT,  &colorspace_t::YUV422PtoBGR24   },
     {I422P_FORMAT,  I422i_FORMAT,  &colorspace_t::YUV422PtoYUV422i },
     {I422P_FORMAT,  I422P_FORMAT,  NULL                            },
     {I422P_FORMAT,  I420P_FORMAT,  &colorspace_t::YUV422PtoYUV420P },
     {I422P_FORMAT,  I411P_FORMAT,  &colorspace_t::YUV422PtoYUV411P },

     {I420P_FORMAT,  RGB24_FORMAT,  &colorspace_t::YUV420PtoRGB24   },
     {I420P_FORMAT,  BGR24_FORMAT,  &colorspace_t::YUV420PtoBGR24   },
     {I420P_FORMAT,  I422i_FORMAT,  &colorspace_t::YUV420PtoYUV422i },
     {I420P_FORMAT,  I422P_FORMAT,  NULL                            },
     {I420P_FORMAT,  I420P_FORMAT,  NULL                            },
     {I420P_FORMAT,  I411P_FORMAT,  &colorspace_t::YUV420PtoYUV411P },

     {I411P_FORMAT,  RGB24_FORMAT,  &colorspace_t::YUV411PtoRGB24   },
     {I411P_FORMAT,  BGR24_FORMAT,  &colorspace_t::YUV411PtoBGR24   },
     {I411P_FORMAT,  I422i_FORMAT,  NULL                            },
     {I411P_FORMAT,  I422P_FORMAT,  NULL                            },
     {I411P_FORMAT,  I420P_FORMAT,  &colorspace_t::YUV411PtoYUV420P },
     {I411P_FORMAT,  I411P_FORMAT,  NULL                            }
};

const int imgTranslator_t::MAX_TRANSFORMATIONS=
    ((sizeof(imgTranslator_t::conversion)/sizeof(imgTranslator_t::transformation_t)));

TRANSFORMER
imgTranslator_t::getTransformation(u32 ninFmt, u32 noutFmt)
{
    for (int i= 0; i < MAX_TRANSFORMATIONS; i++)
    {
        if (    (conversion[i].inFmt  == ninFmt )
             && (conversion[i].outFmt == noutFmt)
           )
        {
            return conversion[i].translator;
        }
    }

    return NULL;
}

imgTranslator_t::imgTranslator_t (u32 ninFmt, u32 noutFmt)
: translator(NULL),
  inFmt(ninFmt),
  outFmt(noutFmt)
{
    translator = getTransformation(inFmt, outFmt);

    if (translator == NULL)
    {
        throw "Translator not found";
    }
}

imgTranslator_t::~imgTranslator_t(void)
{
}

u32
imgTranslator_t::getInputFormat(void)
{
    return inFmt;
}

u32
imgTranslator_t::getOutputFormat(void)
{
    return outFmt;
}

void
imgTranslator_t::translate(unsigned char *dst,
                           int &dstWidth,
                           int &dstHeight,
                           unsigned char *src,
                           int srcWidth,
                           int srcHeight
                          )
{
    (colorspace->*translator)(dst,
                              dstWidth, dstHeight,
                              src,
                              srcWidth, srcHeight
                             );
}

