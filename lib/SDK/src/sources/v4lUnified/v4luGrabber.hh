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
// $Id: v4luGrabber.hh 21928 2011-01-25 16:32:02Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __v4lu_grabber_hh__
#define __v4lu_grabber_hh__

#include <string.h>

#include <icf2/general.h>
#include <icf2/lockSupport.hh>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>
#include <Isabel_SDK/sourceFactory.hh>

#include "VideoHandler.hh"

//
// our source descriptor -- yeah!
//
static unsigned char const MAX_DEVICE_FOR_V4LU = 4;



class v4luGrabberDescriptor_t: public sourceDescriptor_t
{
private:
    char *ID;
    char *inputPorts;

public:
    v4luGrabberDescriptor_t(const char *nID, const char *nchannels);

    virtual ~v4luGrabberDescriptor_t(void);

    const char *getID        (void) const { return ID;         }
    const char *getInputPorts(void) const { return inputPorts; }

    friend class v4luGrabberImplementation_t;
};


//
// our source object -- yeah! yeah!
//
class v4luGrabberImplementation_t: public source_t, public item_t, public lockedItem_t
{
private:
    sourceDescriptor_ref  sourceDescriptor;
    VideoHandler_t       *devStat;

    u32       grabFmt;                // CACHED
    unsigned  grabWidth, grabHeight;  // CACHED

    v4luGrabberImplementation_t(sourceDescriptor_ref  ndesc,
                                VideoHandler_t       *ndevStat,
                                const char           *inPort
                               );

    virtual ~v4luGrabberImplementation_t(void);

public:
    void stopGrabbing();
    void startGrabbing();

    image_t *getImage(void); 
    bool     setFrameRate(double fps);
    double   getFrameRate(void);

    bool setGrabSize(unsigned width, unsigned height);
    void getGrabSize(unsigned *width, unsigned *height);

    bool setGrabFormat(u32 fmt);
    u32  getGrabFormat(void);

    const char *getInputPort(void);

    // video parameters (saturation, brightness, hue, constrast)
    // in the range [0 .. 100], for user convenience
    bool setSaturationCB(int newZ, int oldZ);
    int  getSaturationCB(void);
    bool setBrightnessCB(int newZ, int oldZ);
    int  getBrightnessCB(void);
    bool setHueCB(int newZ, int oldZ);
    int  getHueCB(void);
    bool setContrastCB(int newZ, int oldZ);
    int  getContrastCB(void);

    bool        setStandardListCB(std::string newZ, std::string oldZ);
    std::string getStandardListCB(void);
    bool        setStandardCB(std::string newZ, std::string oldZ);
    std::string getStandardCB(void);

    bool        setFlickerFreqListCB(std::string newZ, std::string oldZ);
    std::string getFlickerFreqListCB(void);
    bool        setFlickerFreqCB(std::string newZ, std::string oldZ);
    std::string getFlickerFreqCB(void);

public:
    sourceDescriptor_ref getDescriptor(void) const;

    const char *className(void) const { return "v4luGrabberImplementation_t"; }

    friend class v4luGrabberFactory_t;
};

typedef smartReference_t< ql_t<sourceDescriptor_ref> > srcDescList_ref;

//
// our source factory-- yeah! yeah! yeah!!
//
class v4luGrabberFactory_t: public sourceFactory_t
{
private:

    static unsigned int usedSlots;
    static sourceFactory_ref __singleFactory;
    static VideoHandler_t *slots[MAX_DEVICE_FOR_V4LU];

    v4luGrabberFactory_t(void);
    virtual ~v4luGrabberFactory_t(void);

    static void pollDevices(void);
    static void freeDevices(void);
    static void repollDevices(void);

public:
    static unsigned int getNumDevices(void);
    static VideoHandler_t *getVideoHandler(unsigned n);

    static sourceFactory_ref createFactory(void);

    source_ref createSource(sourceDescriptor_ref desc,
                            const char *inPort= NULL
                           );

    friend class smartReference_t<v4luGrabberFactory_t>;
};

#endif
