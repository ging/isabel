////////////////////////////////////////////////////////////////////////
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
// $Id: v4luGrabber.cc 21928 2011-01-25 16:32:02Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <icf2/notify.hh>

#include "v4luGrabber.hh"
#include "v4luDioctl.hh"

v4luGrabberDescriptor_t::v4luGrabberDescriptor_t(const char *nID,
                                                 const char *nchannels
                                                )
: ID(strdup(nID)),
  inputPorts(strdup(nchannels))
{
}

v4luGrabberDescriptor_t::~v4luGrabberDescriptor_t(void)
{
    free(ID);
    free(inputPorts);
}

v4luGrabberImplementation_t::v4luGrabberImplementation_t
                                 (
                                  sourceDescriptor_ref  ndesc,
                                  VideoHandler_t       *ndevStat,
                                  const char           *inPort
                                 )
: sourceDescriptor(ndesc),
  devStat(ndevStat)
{
    devStat->setDeviceBusy();

    addAttr<v4luGrabberImplementation_t, int>("Saturation", 50, &v4luGrabberImplementation_t::setSaturationCB, &v4luGrabberImplementation_t::getSaturationCB);
    addAttr<v4luGrabberImplementation_t, int>("Brightness", 50, &v4luGrabberImplementation_t::setBrightnessCB, &v4luGrabberImplementation_t::getBrightnessCB);
    addAttr<v4luGrabberImplementation_t, int>("Hue", 50, &v4luGrabberImplementation_t::setHueCB, &v4luGrabberImplementation_t::getHueCB);
    addAttr<v4luGrabberImplementation_t, int>("Contrast", 50, &v4luGrabberImplementation_t::setContrastCB, &v4luGrabberImplementation_t::getContrastCB);

    addAttr<v4luGrabberImplementation_t, std::string>("StandardList", "", &v4luGrabberImplementation_t::setStandardListCB, &v4luGrabberImplementation_t::getStandardListCB);
    addAttr<v4luGrabberImplementation_t, std::string>("Standard", "AUTO", &v4luGrabberImplementation_t::setStandardCB, &v4luGrabberImplementation_t::getStandardCB);

    addAttr<v4luGrabberImplementation_t, std::string>("FlickerFreqList", "", &v4luGrabberImplementation_t::setFlickerFreqListCB, &v4luGrabberImplementation_t::getFlickerFreqListCB);
    addAttr<v4luGrabberImplementation_t, std::string>("FlickerFreq", "AUTO", &v4luGrabberImplementation_t::setFlickerFreqCB, &v4luGrabberImplementation_t::getFlickerFreqCB);

    devStat->setInputPort(inPort);

    grabFmt= devStat->getGrabFormat();
    devStat->getGrabSize(&grabWidth, &grabHeight);

    devStat->startGrabbing();
}

v4luGrabberImplementation_t::~v4luGrabberImplementation_t(void)
{
    locker_t l= lock();

    devStat->stopGrabbing();

    devStat->setDeviceFree();
}

void
v4luGrabberImplementation_t::stopGrabbing(void)
{
    locker_t l= lock();

    devStat->stopGrabbing();
}

void
v4luGrabberImplementation_t::startGrabbing(void)
{
    locker_t l= lock();

    devStat->setGrabSize(grabWidth, grabHeight);
    devStat->getGrabSize(&grabWidth, &grabHeight);

    bool res= devStat->setGrabFormat(grabFmt);
    grabFmt= devStat->getGrabFormat();

    devStat->startGrabbing();
}

bool
v4luGrabberImplementation_t::setFrameRate(double fps)
{
    return false; // TO BE IMPLEMENTED
}

double
v4luGrabberImplementation_t::getFrameRate(void)
{
    return -1; // TO BE IMPLEMENTED
}

bool
v4luGrabberImplementation_t::setGrabSize(unsigned w, unsigned h)
{
    locker_t l= lock();

    devStat->setGrabSize(w, h);
    devStat->getGrabSize(&grabWidth, &grabHeight);

    NOTIFY("v4luGrabber::setGrabSize: requested %dx%d, got %dx%d\n",
           w, h, grabWidth, grabHeight
          );

    return true;
}

void
v4luGrabberImplementation_t::getGrabSize(unsigned *w, unsigned *h)
{
    devStat->getGrabSize(&grabWidth, &grabHeight);

    *w= grabWidth;
    *h= grabHeight;
}

bool
v4luGrabberImplementation_t::setGrabFormat(u32 fmt)
{
    locker_t l= lock();

    bool res= devStat->setGrabFormat(fmt);
    grabFmt= devStat->getGrabFormat();

    return res;
}

u32
v4luGrabberImplementation_t::getGrabFormat(void)
{
    grabFmt= devStat->getGrabFormat();
    return grabFmt;
}

const char *
v4luGrabberImplementation_t::getInputPort(void)
{
    return devStat->getInputPortName();
}

bool
v4luGrabberImplementation_t::setSaturationCB(int newZ, int oldZ)
{
    locker_t l= lock();

    return devStat->setSaturation(newZ);
}

int
v4luGrabberImplementation_t::getSaturationCB(void)
{
    locker_t l= lock();

    return devStat->getSaturation();
}

bool
v4luGrabberImplementation_t::setBrightnessCB(int newZ, int oldZ)
{
    locker_t l= lock();

    return devStat->setBrightness(newZ);
}

int
v4luGrabberImplementation_t::getBrightnessCB(void)
{
    locker_t l= lock();

    return devStat->getBrightness();
}

bool
v4luGrabberImplementation_t::setHueCB(int newZ, int oldZ)
{
    locker_t l= lock();

    return devStat->setHue(newZ);
}

int
v4luGrabberImplementation_t::getHueCB(void)
{
    locker_t l= lock();

    return devStat->getHue();
}

bool
v4luGrabberImplementation_t::setContrastCB(int newZ, int oldZ)
{
    locker_t l= lock();

    return devStat->setContrast(newZ);
}

int
v4luGrabberImplementation_t::getContrastCB(void)
{
    locker_t l= lock();

    return devStat->getContrast();
}

bool
v4luGrabberImplementation_t::setStandardListCB(std::string newZ, std::string oldZ)
{
    return false;
}

std::string
v4luGrabberImplementation_t::getStandardListCB(void)
{
    return devStat->getStandardList();
}

bool
v4luGrabberImplementation_t::setStandardCB(std::string newZ, std::string oldZ)
{
    locker_t l= lock();

    return devStat->setStandard(newZ.c_str());
}

std::string
v4luGrabberImplementation_t::getStandardCB(void)
{
    locker_t l= lock();

    const char *retVal= devStat->getStandard();
    return retVal == NULL ? std::string() : std::string(retVal);
}

bool
v4luGrabberImplementation_t::setFlickerFreqListCB(std::string newZ, std::string oldZ)
{
    return false;
}

std::string
v4luGrabberImplementation_t::getFlickerFreqListCB(void)
{
    return devStat->getFlickerFreqList();
}

bool
v4luGrabberImplementation_t::setFlickerFreqCB(std::string newZ, std::string oldZ)
{
    locker_t l= lock();

    return devStat->setFlickerFreq(newZ.c_str());
}

std::string
v4luGrabberImplementation_t::getFlickerFreqCB(void)
{
    locker_t l= lock();

    const char *retVal= devStat->getFlickerFreq();
    return retVal == NULL ? std::string() : std::string(retVal);
}

image_t*
v4luGrabberImplementation_t::getImage(void)
{
    locker_t l= lock();

    if ( ! devStat->isBuiltOK() )
    {
        return NULL;
    }

    u32 timestamp, size;

    u8 *theBuffer= devStat->capture(&timestamp, &size, &grabWidth, &grabHeight);

    if ( (theBuffer == NULL) || (size <= 0) )
    {
        NOTIFY("v4luGrabberImplementation_t::getImage: "
               "device capture returns NULL\n"
              );

        return NULL;
    }

    image_t *img= new image_t(theBuffer,
                              size,
                              grabFmt,
                              grabWidth,
                              grabHeight,
                              timestamp
                             );

    return img;
}

sourceDescriptor_ref
v4luGrabberImplementation_t::getDescriptor(void) const
{
    return sourceDescriptor;
}

sourceFactory_ref v4luGrabberFactory_t::__singleFactory;

v4luGrabberFactory_t::v4luGrabberFactory_t(void)
{
    pollDevices();
}

v4luGrabberFactory_t::~v4luGrabberFactory_t(void)
{
    freeDevices();
}

static const char *PATH_TO_DEVICE_x= "/dev/video%d";

void
v4luGrabberFactory_t::pollDevices(void)
{
    for (unsigned i= 0; i < MAX_DEVICE_FOR_V4LU; i++)
    {
        char pth[4096];

        sprintf(pth, PATH_TO_DEVICE_x, i);

        VideoHandler_t *x= VideoHandler_t::buildDS(pth);

        if (x != NULL)
        {
            if (x->isBuiltOK())
            {
                slots[usedSlots]= x;
                usedSlots++;
            }
            else
            {
                delete x;
            }
        }
    }
}

void
v4luGrabberFactory_t::freeDevices(void)
{
    VideoHandler_t *next= NULL;
    for (unsigned i= 0; i < usedSlots; i++)
    {
        next= slots[usedSlots];
        if (next != NULL)
        {
            delete next;
        }
    }

    usedSlots= 0;
}

void
v4luGrabberFactory_t::repollDevices(void)
{
    bool found;
    unsigned j;

    for (unsigned i= 0; i < MAX_DEVICE_FOR_V4LU; i++)
    {
        char pth[4096];

        sprintf(pth, PATH_TO_DEVICE_x, i);

        found= false;
        for (j= 0; j < usedSlots; j++)
        {
            if (strcmp(pth, slots[j]->getPath()) == 0)
            {
                found= true;
                break;
            }
        }

        if (found)
        {
            if ( ! slots[j]->isBuiltOK() ) // remove it
            {
                VideoHandler_t *bye= slots[j];
                slots[j]= slots[usedSlots-1];
                slots[usedSlots-1]= NULL;
                usedSlots--;
                i--;
                if (bye->isDeviceFree())
                {
                    delete bye;
                }
                else
                {
                    NOTIFY("*** LOSING MEMORY :-(, sources MUST be released "
                           "if they do not deliver images\n"
                           "*** usually, that means a ENODEV"
                          );
                }
            }
            continue;
        }

        VideoHandler_t *x= VideoHandler_t::buildDS(pth);

        if (x != NULL)
        {
            if (x->isBuiltOK())
            {
                slots[usedSlots]= x;
                usedSlots++;
            }
            else
            {
                delete x;
            }
        }
    }
}

unsigned int
v4luGrabberFactory_t::getNumDevices(void)
{
    return usedSlots;
}

VideoHandler_t *
v4luGrabberFactory_t::getVideoHandler(unsigned n)
{
    if (n >= usedSlots)
    {
        return NULL;
    }
    return slots[n];
}

sourceFactory_ref
v4luGrabberFactory_t::createFactory(void)
{
    if (__singleFactory.isValid())
    {
        repollDevices();
    }

    if (__singleFactory.isValid())
    {
        return __singleFactory;
    }

    v4luGrabberFactory_t *f= new v4luGrabberFactory_t;
    if (f)
    {
        __singleFactory= f;
         if (usedSlots > 0)
         {
             return __singleFactory;
         }
        __singleFactory= NULL;
    }
    return __singleFactory;
}

source_ref
v4luGrabberFactory_t::createSource(sourceDescriptor_ref desc,
                                   const char *inPort
                                  )
{
    for (unsigned i= 0; i < usedSlots; i++)
    {
        VideoHandler_t *devStat = v4luGrabberFactory_t::getVideoHandler(i);
        if (strcmp(devStat->getID(), desc->getID()) == 0)
        {
            if (devStat->isDeviceFree())
            {
                return new v4luGrabberImplementation_t(desc, devStat, inPort);
            }
            else
            {
                return NULL;
            }
        }
    }
    return NULL;
}

VideoHandler_t* v4luGrabberFactory_t::slots[MAX_DEVICE_FOR_V4LU];
unsigned int v4luGrabberFactory_t::usedSlots = 0;

