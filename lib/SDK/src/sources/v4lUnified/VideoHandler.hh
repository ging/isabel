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
// $Id: VideoHandler.hh 10216 2007-05-25 14:38:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __v4lu_Video_Handler_hh__
#define __v4lu_Video_Handler_hh__

#include <icf2/general.h>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

#include "v4luDioctl.hh"

extern const char *V4LU_NAME;

typedef smartReference_t< ql_t<u32> > formatList_ref;

struct Size_t
{
    unsigned w, h;
};

int AREA (Size_t &g);

void SWAP (Size_t &a, Size_t &b);

class VideoHandler_t
{
private:
    char *ID;
    char *devicePath;
    bool  deviceFree;

protected:
    int   deviceHandle;
    bool  builtOK;
    bool  isStreamingNow;

public:

    VideoHandler_t(const char *path);
    virtual ~VideoHandler_t(void);

    static VideoHandler_t *buildDS(const char *path);

    const char *getID(void);

    bool isDeviceFree (void) const;
    void setDeviceFree(void);
    void setDeviceBusy(void);
    bool isBuiltOK(void) const;

    const char *getPath(void) const;

    virtual const char *getName(void) const = 0;

    virtual void           startGrabbing(void) = 0;
    virtual void           stopGrabbing(void) = 0;
    virtual u8            *capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h) = 0;

    virtual bool           setGrabSize(unsigned w, unsigned h) = 0;
    virtual void           getGrabSize(unsigned *w, unsigned *h) = 0;

    virtual bool           setGrabFormat(u32 fmt) = 0;
    virtual u32            getGrabFormat(void) = 0;

    virtual const char    *getInputPorts(void) const = 0;
    virtual const char    *getInputPortName(void) const = 0;
    virtual int            getInputPortId(void) const = 0;
    virtual bool           setInputPort(const char *inputPort) = 0;
    virtual bool           setChannel  (int newChannel) = 0;

    virtual bool           setSaturation(int z) = 0;
    virtual int            getSaturation(void) = 0;
    virtual bool           setBrightness(int z) = 0;
    virtual int            getBrightness(void) = 0;
    virtual bool           setHue       (int z) = 0;
    virtual int            getHue       (void) = 0;
    virtual bool           setContrast  (int z) = 0;
    virtual int            getContrast  (void) = 0;

    virtual std::string    getStandardList(void) = 0;
    virtual bool           setStandard(const char *z) = 0;
    virtual const char    *getStandard(void) = 0;

    virtual std::string    getFlickerFreqList(void) = 0;
    virtual bool           setFlickerFreq(const char *z) = 0;
    virtual const char    *getFlickerFreq(void) = 0;
};

#endif
