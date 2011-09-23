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
// $Id: shmGrabber.cc 10216 2007-05-25 14:38:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <icf2/notify.hh>

#include "shmGrabber.hh"

shmGrabber_t::shmGrabber_t(sourceDescriptor_ref ndesc, int cmdKey, int cmdSize)
: sourceDescriptor(ndesc)
{
    int newshmid;

    NOTIFY("SHMKEY=%d\nSHMSIZE=%d\n", cmdKey, cmdSize);

    shmid = shmget(cmdKey, sizeof(imageInfo_t) + cmdSize, 0777 | IPC_CREAT | IPC_EXCL);

    if (shmid == -1)
    {
        if (errno == EEXIST)
        {
           NOTIFY("shmGrabber_t:: shared memory segment already exists\n");
        }
        else
        {
            NOTIFY("shmGrabber_t:: shget error (%d): %s\n",
                   errno,
                   strerror(errno)
                  );
            exit(1);
        }

        NOTIFY("shmGrabber_t::shmGrabber_t: getting ID to attach: ");
        newshmid = shmget(cmdKey, cmdSize, 0);
        if (newshmid == -1)
        {
            NOTIFY("shmGrabber_t:: shmget error (%d): %s\n",
                   errno,
                   strerror(errno)
                  );
            exit(1);
        }
        shmid= newshmid;
        NOTIFY("OK\n");
    }
    else
    {
        NOTIFY("shmGrabber_t::shmGrabber_t: Created shMemory\n");
    }

    shmPtr = (char *)shmat(shmid, 0, 0);
    if (shmPtr == (char *) -1)
    {
        NOTIFY("shmGrabber_t:: shmat error (%d): %s\n",errno, strerror(errno));
        shmdt(shmPtr);
        exit(1);
    }

    imgInfo= (imageInfo_t*)shmPtr;
}

shmGrabber_t::~shmGrabber_t(void)
{
    struct shmid_ds cont;

    shmdt(shmPtr);
    shmctl(shmid, IPC_STAT, &cont);
    if (cont.shm_nattch == 0)
    {
        // zero currently attached segments, destroying shm\n");
        shmctl(shmid, IPC_RMID, &cont);
    }
}

bool
shmGrabber_t::setFrameRate(double fps)
{
    return false;
}

double
shmGrabber_t::getFrameRate(void)
{
    return -1;
}

bool
shmGrabber_t::setGrabSize(unsigned w, unsigned h)
{
    return false;
}

void
shmGrabber_t::getGrabSize(unsigned *w, unsigned *h)
{
    *w= imgInfo->width;
    *h= imgInfo->height;
}

bool
shmGrabber_t::setGrabFormat(u32 fcc)
{
    return false;
}

u32
shmGrabber_t::getGrabFormat(void)
{
    return RGB24_FORMAT;
}

const char *
shmGrabber_t::getInputPort(void)
{
    return "SHM";
}

image_t*
shmGrabber_t::getImage(void)
{
    u32 timestamp;

    struct timeval currentTime;

    if (imgInfo->shmCapture != 237855)
    {
        return NULL;
    }

    gettimeofday (&currentTime, NULL);
    timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

    //memcpy(&imgInfo, shmPtr, sizeof(imageInfo_t));

    int size= imgInfo->width * imgInfo->height * 3;

    image_t *img= new image_t((u8*)(shmPtr+sizeof(imageInfo_t)),
                              size,
                              RGB24_FORMAT,
                              imgInfo->width,
                              imgInfo->height,
                              timestamp
                             );

    return img;
}

sourceDescriptor_ref
shmGrabber_t::getDescriptor(void) const
{
    return sourceDescriptor;
}

source_ref
shmGrabber_t::createSource(sourceDescriptor_ref desc, int key, int size)
{
    shmGrabber_t *retVal= new shmGrabber_t(desc, key, size);

    return retVal;
}

sourceFactory_ref shmGrabberFactory_t::__singleFactory;

shmGrabberFactory_t::shmGrabberFactory_t(void)
{
}

shmGrabberFactory_t::~shmGrabberFactory_t(void)
{
}

bool
shmGrabberFactory_t::ready(void)
{
    return true;
}

sourceFactory_ref
shmGrabberFactory_t::createFactory(void)
{
    if (__singleFactory.isValid())
        return __singleFactory;

    shmGrabberFactory_t *f= new shmGrabberFactory_t();
    if (f)
    {
        __singleFactory= f;
         if(f->ready())
         {
             return __singleFactory;
         }
        __singleFactory= NULL;
    }
    return __singleFactory;
}

source_ref
shmGrabberFactory_t::createSource(sourceDescriptor_ref desc,
                                  const char *inPort
                                 )
{
    int shmKey;
    int shmSize;

    if (sscanf(inPort, "%d %d", &shmKey, &shmSize) != 2)
    {
        return NULL;
    }

    if ((shmKey <= 0) || (shmSize <= 0))
    {
        return NULL;
    }

    return shmGrabber_t::createSource(desc, shmKey, shmSize);
}


