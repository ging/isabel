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

#ifndef __rfb_source_hh__
#define __rfb_source_hh__

#include <icf2/general.h>

#include <vUtils/image.h>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

#include <vncconn.h>

class rfbSource_t: public virtual source_t
{
private:

    sourceDescriptor_ref theSourceDescriptor;
    u8 *image;
    libvnc::CVncConn *vnc;
    char *url;

public:

    rfbSource_t(sourceDescriptor_ref desc, const char *url);

    virtual ~rfbSource_t(void);

    image_t *getImage(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);

    bool setGrabSize(unsigned width, unsigned height);
    void getGrabSize(unsigned *width, unsigned *height);

    const char *getInputPort(void);

    bool setGrabFormat(u32 fmt);
    u32  getGrabFormat(void);

    sourceDescriptor_ref getDescriptor(void) const
    {
        return theSourceDescriptor;
    };

    virtual const char *className(void) const { return "rfbSource_t"; };
};

#endif

