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
// $Id: VideoHandler.cc 9459 2007-01-30 17:24:59Z isatest $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <icf2/notify.hh>

#include <Isabel_SDK/systemRegistry.hh>

#include "VideoHandler.hh"
#ifdef HAVE_V4L1
#include "v4l1DS.hh"
#endif
#include "v4l2DS.hh"

const char *V4LU_NAME="v4lUnified";

int
AREA (Size_t &g)
{
    return g.w * g.h;
}

void
SWAP (Size_t &a, Size_t &b)
{
    int w= a.w, h= a.h;
    a.w = b.w;
    a.h = b.h;
    b.w = w;
    b.h = h;
}

VideoHandler_t::VideoHandler_t(const char *path)
: ID(NULL),
  devicePath(strdup(path)),
  deviceFree(false),
  deviceHandle(-1),
  builtOK(false),
  isStreamingNow(false)
{
}

VideoHandler_t::~VideoHandler_t(void)
{
    free(devicePath);
}

VideoHandler_t*
VideoHandler_t::buildDS(char const *path)
{
    VideoHandler_t *vh= NULL;

    int deviceHandle = open(path, O_RDWR, 0000);
    if (deviceHandle < 0)
    {
        NOTIFY("---cannot open '%s': %s\n", path, strerror(errno));
        return NULL;
    }
    close(deviceHandle);

    //
    // try v4l2 interface
    //
    vh= new v4l2DS_t(path);
    if (vh != NULL && vh->isBuiltOK())
    {
        return vh;
    }
    else
    {
        NOTIFY("---Video4Linux2 NOT supported: %s, trying Video4Linux\n", path);
        if (vh != NULL)
        {
            delete vh;
            vh= NULL;
        }
    }

#ifdef HAVE_V4L1
    //
    // try v4l interface
    //
    vh= new v4l1DS_t(path);
    if (vh != NULL && vh->isBuiltOK())
    {
        return vh;
    }
    else
    {
        if (vh != NULL)
        {
            delete vh;
            vh= NULL;
        }
    }
#endif
//
// device is neither v4l / v4l2 compatible, returning null
//
    return NULL;
}

const char*
VideoHandler_t::getID(void)
{
    if (ID == NULL)
    {
        const char *name= getName();
        const char *path= getPath();

        ID= (char*)malloc(strlen(V4LU_NAME) + strlen(name) + strlen(path) + 10);

        sprintf(ID, "%s::%s::%s", V4LU_NAME, name, path);

        char *p;
        // remove '('
        while ((p = strchr(ID, '(')) != NULL) *p= ' ';
        // remove ')'
        while ((p = strchr(ID, ')')) != NULL) *p= ' ';
    }

    return ID;
}

bool
VideoHandler_t::isDeviceFree(void) const
{
    return deviceFree;
}

void
VideoHandler_t::setDeviceFree(void)
{
    deviceFree= true;
}

void
VideoHandler_t::setDeviceBusy(void)
{
    deviceFree= false;
}

bool
VideoHandler_t::isBuiltOK(void) const
{
    return builtOK;
}

const char *
VideoHandler_t::getPath(void) const
{
    return devicePath;
}

