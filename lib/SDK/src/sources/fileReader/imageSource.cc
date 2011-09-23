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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <Isabel_SDK/imageLoader.hh>

#include "imageSource.hh"

imageSource_t::imageSource_t(sourceDescriptor_ref sdr, const char *fileName)
: sourceDescriptor(sdr),
  theFileName(NULL),
  theImg(NULL)
{
#if 0
    NOTIFY("imageSource_t:: loading image <%s> FROM FILE!!\n", fileName);
#endif

    if ( ! fileName || fileName[0] == '\0')
    {
        throw "imageSource_t:: empty filename";
    }

    theFileName= strdup(fileName);
    theImg= loadFile(theFileName);

    if (theImg == NULL)
    {
        throw "imageSource_t:: cannot load filename";
    }
}

imageSource_t::~imageSource_t(void)
{
    delete theImg;
}

image_t*
imageSource_t::getImage(void)
{
#if 0
    NOTIFY("captImage loading image <%s> FROM FILE!!\n", fileName);
#endif

    // get time
    struct timeval currentTime;
    gettimeofday (&currentTime, NULL);
    u32 timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

    theImg->setTimestamp(timestamp);

    // duplicate, sink will delete it
    image_t *retVal= new image_t(*theImg);
    return retVal;
}

bool
imageSource_t::setFrameRate(double fps)
{
    return false;
}

double
imageSource_t::getFrameRate(void)
{
    return -1;
}

bool
imageSource_t::setGrabSize(unsigned /*width*/, unsigned /*height*/)
{
    return false;
}

void
imageSource_t::getGrabSize(unsigned *w, unsigned *h)
{
    *w= theImg->getWidth();
    *h= theImg->getHeight();
}

bool
imageSource_t::setGrabFormat(u32 fcc)
{
    return false;
}

u32
imageSource_t::getGrabFormat(void)
{
    return theImg->getFormat();
}

const char *
imageSource_t::getInputPort(void)
{
    return theFileName;
}

