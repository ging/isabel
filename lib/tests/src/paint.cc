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
// $Id: grabberTest.cc 11674 2007-10-03 14:48:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <icf2/notify.hh>

#include "paint.h"

void
paint(imgTranslator_t *trans, displayTask_t::window_t *win, image_t *img)
{
    assert(img != NULL);

    u8 *paintBuff= NULL, *transBuff= NULL;

    int dstWidth= -1, dstHeight= -1;

    int srcWidth = img->getWidth();
    int srcHeight= img->getHeight();
    u8 *src= img->getBuff();

    if (trans == NULL)
    {
        // if not translation, input for painter is src image

        dstWidth = srcWidth;
        dstHeight= srcHeight;
        paintBuff= src;
    }
    else
    {
        // if translation, input for painter is translated image
        // translator sets dstWidth/dstHeight

        int transSize = vFrameSize(srcWidth, srcHeight, RGB24_FORMAT);
        transBuff= new u8[transSize];
        if ( ! transBuff )
        {
            NOTIFY("paint: run out of memory, bailing out\n");
            exit(-1);
        }

        trans->translate(transBuff,
                         dstWidth, dstHeight,
                         src,
                         srcWidth, srcHeight
                        );

        paintBuff= transBuff;
    }

    assert ( dstWidth != -1 && dstHeight != -1 && "translate error");

    win->putRGB24(paintBuff, dstWidth, dstHeight, -1, -1);

    if (transBuff)
    {
        delete []transBuff;
    }
}

