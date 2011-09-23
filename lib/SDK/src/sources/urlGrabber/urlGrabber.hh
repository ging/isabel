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
// $Id: urlGrabber.hh 20999 2010-07-30 15:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __url_grabber_hh__
#define __url_grabber_hh__

#include <string.h>

#include <icf2/general.h>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>
#include <Isabel_SDK/sourceFactory.hh>

#include "ffWrapper.hh"

//
//
// Source descriptor
//
static const char *URL_ID= "URL Grabber";

class urlGrabberDescriptor_t: public sourceDescriptor_t
{
public:
    urlGrabberDescriptor_t(void) { }

    const char *getID         (void) const { return URL_ID; }
    const char *getInputPorts (void) const { return NULL;   }
};

//
// our source object -- yeah! yeah!
//
class urlGrabber_t: public source_t
{
private:
    sourceDescriptor_ref sourceDescriptor;

    URLStreamState_t *urlss;

public:
    urlGrabber_t(sourceDescriptor_ref ndesc, const char *urlString);

    ~urlGrabber_t(void);

    image_t *getImage(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);

    bool setGrabSize(unsigned width, unsigned height);
    void getGrabSize(unsigned *width, unsigned *height);

    bool setGrabFormat(u32 fcc);
    u32  getGrabFormat(void);

    const char *getInputPort(void);

    sourceDescriptor_ref getDescriptor(void) const;

    const char *className(void) const { return "urlGrabber_t"; }
};

//
// Source Factory
//
class urlGrabberFactory_t: public sourceFactory_t
{
public:

    urlGrabberFactory_t(void)  {}
    virtual ~urlGrabberFactory_t(void) {}

    source_ref createSource(sourceDescriptor_ref sdr, const char *inPort)
    {
        try
        {
            return new urlGrabber_t(sdr, inPort);
        }
        catch (const char *)
        {
            // unrecognizeble image
        }

        return NULL;
    }

};

#endif

