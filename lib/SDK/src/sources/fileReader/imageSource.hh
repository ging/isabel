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

#include <icf2/general.h>

#include <vUtils/image.h>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

//
// Source (Grabber)
//

class imageSource_t: public source_t
{
private:

    sourceDescriptor_ref sourceDescriptor;

    char *theFileName;
    image_t *theImg;

public:
    imageSource_t(sourceDescriptor_ref sdr, const char *fileName);

    virtual ~imageSource_t(void);

    image_t *getImage(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);

    bool setGrabSize(unsigned /*width*/, unsigned /*height*/);
    void getGrabSize(unsigned *w, unsigned *h);

    bool setGrabFormat(u32 fcc);
    u32  getGrabFormat(void);

    const char *getInputPort(void);

    sourceDescriptor_ref getDescriptor(void) const
    {
        return sourceDescriptor;
    }

    virtual const char *className(void) const { return "imageSource_t"; };
};

