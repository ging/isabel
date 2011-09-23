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
// $Id: syntheticStub.cc 11521 2007-10-01 16:46:22Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <icf2/general.h>
#include <icf2/notify.hh>

#include <vUtils/image.h>

#include <Isabel_SDK/isabelSDK.hh>
#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "generators.hh"


const char *SYNTHETIC_ID= "Synthetic Image Source";

class syntheticGrabberDescriptor_t: public sourceDescriptor_t
{
public:
    syntheticGrabberDescriptor_t(void) { }

public:
    const char *getID         (void) const { return SYNTHETIC_ID; }
    const char *getInputPorts (void) const { return NULL;         }
};



//
// our grabber: its a wrapper of the actual source
//     all methods are delegated in real sources
//     this way, we implement a source returning different formats
//
class syntheticGrabber_t: public virtual source_t, public virtual item_t
{
private:
    sourceDescriptor_ref  theSourceDescriptor;
    syntheticSrc_t       *theImgGen;

    unsigned grabW, grabH;

    syntheticGrabber_t(sourceDescriptor_ref desc)
    : theSourceDescriptor(desc)
    {
        theImgGen= new syntheticRAW24Src_t;
        theImgGen->getImageSize (&grabW, &grabH);
    }

    virtual ~syntheticGrabber_t(void)
    {
        delete theImgGen;
        theImgGen= NULL;
    }

public:

    virtual image_t *getImage(void)
    {
        struct timeval currentTime;

        gettimeofday (&currentTime, NULL);
        u32 timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

        return theImgGen->genImage(timestamp);
    }

    virtual bool setGrabSize(unsigned width, unsigned height)
    {
        bool retVal= theImgGen->setImageSize(width, height);
        theImgGen->getImageSize(&grabW, &grabH);
        return retVal;
    }

    virtual void getGrabSize(unsigned *width, unsigned *height)
    {
        theImgGen->getImageSize(width, height);
    }

    virtual bool setGrabFormat(u32 fcc)
    {
        delete theImgGen;
        theImgGen= NULL; // paranoid

        switch (fcc)
        {
        case RGB24_FORMAT:
            theImgGen= new syntheticRAW24Src_t;
            break;
        case BGR24_FORMAT:
            theImgGen= new syntheticRAW24Src_t(true);
            break;
        case I422i_FORMAT:
            theImgGen= new syntheticYUV422iSrc_t;
            break;
        case I422P_FORMAT:
            theImgGen= new syntheticYUV422PSrc_t;
            break;
        case I420P_FORMAT:
            theImgGen= new syntheticYUV420PSrc_t;
            break;
        case I411P_FORMAT:
            theImgGen= new syntheticYUV411PSrc_t;
            break;
        default:
            NOTIFY("syntheticGrabber_t::setGrabFormat: unknown format %d,"
                   "bailing out\n",
                   fcc
                  );
            abort();
        }

        theImgGen->setImageSize(grabW, grabH);
        theImgGen->getImageSize(&grabW, &grabH);

        return true;
    }

    virtual u32 getGrabFormat(void)
    {
        return theImgGen->getGrabFormat();
    }

    virtual sourceDescriptor_ref getDescriptor(void) const
    {
        return theSourceDescriptor;
    }

    static source_ref createSource(sourceDescriptor_ref desc)
    {
        return new syntheticGrabber_t(desc);
    }

    bool setFrameRate(double fps)
    {
        return false;
    }

    double getFrameRate(void)
    {
        return -1;
    }
      
    const char *className(void) const { return "syntheticGrabber_t"; }

    friend class sisGrabberFactory_t;
};


//
// our source factory-- yeah! yeah! yeah!!
//
class syntheticGrabberFactory_t: public sourceFactory_t
{
private:
    static sourceFactory_ref singleFactory;

    syntheticGrabberFactory_t(void) { }

    ~syntheticGrabberFactory_t(void) { }

public:
    static sourceFactory_ref createFactory(void)
    {
        if(singleFactory.isValid())
            return singleFactory;

        syntheticGrabberFactory_t *f= new syntheticGrabberFactory_t;

        singleFactory= f;

        return singleFactory;
    }

    source_ref createSource(sourceDescriptor_ref desc, const char *inPort= NULL)
    {
        return syntheticGrabber_t::createSource(desc);
    }

    friend class smartReference_t<syntheticGrabberFactory_t>;
};

sourceFactory_ref syntheticGrabberFactory_t::singleFactory;

//
// housekeeping funtions
//

int
registerSyntheticGrabber(void)
{
    NOTIFY("%s: starting up\n", SYNTHETIC_ID);

    //
    // start registration
    //

    sourceDescriptor_ref desc= new syntheticGrabberDescriptor_t;
    sourceFactory_ref    fact= syntheticGrabberFactory_t::createFactory();


    if ( ! desc.isValid())
    {
        NOTIFY("---Module internal error\n");
        NOTIFY("---Bailing out\n");

        return -1;
    }
    if ( ! fact.isValid())
    {
        NOTIFY("---Cannot build synthetic factory\n");
        NOTIFY("---Bailing out\n");

        return -1;
    }

    if (registerSourceFactory(desc, fact))
    {
        NOTIFY("+++Registered grabber '%s'\n", desc->getID());
    }
    else
    {
        NOTIFY("---Unable to register grabber '%s'\n", desc->getID());
        NOTIFY("---Bailing out\n");

        return -1;
    }

    return 0;
}

void
releaseSyntheticGrabber(void)
{
    NOTIFY("%s: shutting down\n", SYNTHETIC_ID);
}

