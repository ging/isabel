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
// $Id: sourceDefinition.hh 21928 2011-01-25 16:32:02Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __isabel_sdk__source_definition_hh__
#define __isabel_sdk__source_definition_hh__

#include <icf2/general.h>
#include <icf2/smartReference.hh>
#include <icf2/configurableItem.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/sourceDescriptor.hh>

class source_t: public virtual collectible_t, public ConfigurableItem_t
{
public:
    virtual image_t *getImage(void)= 0;

    virtual void startGrabbing(void) {}; /* MUST be abstract */
    virtual void stopGrabbing(void) {}; /* MUST be abstract */

    //
    // setFrameRate: try to set the frame rate to get images
    //    returns true if succedded, i.e., the source can return
    //        images at 'fps' images per second
    //    return false if frame rate does not apply (still images,...)
    //        or the camera does not accept such rate
    // getFrameRate: get the current frame rate
    //     returns < 0 if frame rate does not apply,
    //         for example, image as a source, synthetic image generators, ...
    //     return 0 if no images can be obtained yet
    //         for example, an movie player without movie yet
    //     Otherwise, getFrameRate returns the frame rate at which apps
    //         should obtain images
    virtual bool   setFrameRate(double fps)= 0;
    virtual double getFrameRate(void)= 0;

    virtual bool setGrabSize(unsigned width, unsigned height)= 0;
    virtual void getGrabSize(unsigned *width, unsigned *height)= 0;

    virtual bool setGrabFormat(u32 fcc)= 0;
    virtual u32  getGrabFormat(void)= 0;

    virtual const char *getInputPort(void) { return NULL; }

    virtual sourceDescriptor_ref getDescriptor(void) const= 0;

    virtual const char *className(void) const { return "source_t"; }

    friend class smartReference_t<source_t>;
};

typedef smartReference_t<source_t> source_ref;

#endif

