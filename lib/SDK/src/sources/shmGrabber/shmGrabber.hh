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
// $Id: shmGrabber.hh 9345 2006-12-26 01:41:39Z ldelgado $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __shm_grabber_hh__
#define __shm_grabber_hh__

#include <string.h>

#include <icf2/general.h>
#include <icf2/ql.hh>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>
#include <Isabel_SDK/sourceFactory.hh>

//
// our source descriptor -- yeah!
//
static const char *SHM_ID= "SHM grabber";

struct imageInfo_t
{
    int width;
    int height;
    u32 fcc;
    int shmCapture;
};

class shmGrabberDescriptor_t: public sourceDescriptor_t
{
public:
    shmGrabberDescriptor_t(void) { }

    const char *getID         (void) const { return SHM_ID; }
    const char *getInputPorts (void) const { return NULL;   }
};


//
// our source object -- yeah! yeah!
//
class shmGrabber_t: public virtual source_t
{
private:
    sourceDescriptor_ref sourceDescriptor;

    char  *shmPtr;
    int    shmid;
    int    shmkey;
    int    shmsize;

    struct imageInfo_t *imgInfo; // auxiliar, = shmPtr

    shmGrabber_t(sourceDescriptor_ref ndesc, int lineId, int lineSize);

    virtual ~shmGrabber_t(void);

public:
    image_t *getImage(void);

    bool   setFrameRate(double fps);
    double getFrameRate(void);

    bool setGrabSize(unsigned width, unsigned height);
    void getGrabSize(unsigned *width, unsigned *height);

    bool setGrabFormat(u32 fcc);
    u32  getGrabFormat(void);

    const char *getInputPort(void);

    sourceDescriptor_ref getDescriptor(void) const;

    static source_ref createSource(sourceDescriptor_ref desc, int id, int size);

    const char *className(void) const { return "shmGrabber_t"; }

    friend class shmGrabberFactory_t;
};

typedef smartReference_t< ql_t<sourceDescriptor_ref> > srcDescList_ref;

class shmGrabberFactory_t: public sourceFactory_t
{
private:
    static sourceFactory_ref __singleFactory;

    shmGrabberFactory_t(void);
    virtual ~shmGrabberFactory_t(void);

    bool ready(void);

public:
    static sourceFactory_ref createFactory(void);

    source_ref createSource(sourceDescriptor_ref desc, const char *inPort);

    friend class smartReference_t<shmGrabberFactory_t>;
};

#endif

