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
// $Id: v4l1DS.hh 10216 2007-05-25 14:38:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __v4l1_dev_stat_hh__
#define __v4l1_dev_stat_hh__

#include <sys/types.h>
#include <linux/videodev.h>

#include <icf2/smartReference.hh>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

#include "VideoHandler.hh"

//
// global status
//

class v4l1DS_t: public VideoHandler_t
{
private:
    enum readMethod_e
    {
        MEMORY_MAPPED,
        READ
    };

    unsigned char    *deviceBuffer;
    unsigned          deviceBufferLen;

    bool              useMMapGrab;

    video_mbuf        vmb;
    video_capability  vcap;
    video_picture     vpic;
    video_window      vwin;

    video_mmap        vmmap[2];

    video_channel     vchann[256];
    int               selectedChannel;

    int               grab_number;

    formatList_ref  grabFormats;

    char           *inputPortsStr;

    bool mmap_init(void);
    bool read_init(void);

    void mmap_close(void);
    void read_close(void);

    unsigned char *mmap_capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h);
    unsigned char *read_capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h);

    formatList_ref allowedGrabFormats(void);

public:
    v4l1DS_t(char const *path);
    virtual ~v4l1DS_t(void);

    const char *getName(void) const { return (char *)vcap.name; }

    void startGrabbing(void);
    void stopGrabbing(void);

    u8 *capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h);

    // check a list of formats and returns a list with the acceptable ones
    formatList_ref getGrabFormats(void);

    bool setGrabSize(unsigned w, unsigned h);
    void getGrabSize(unsigned *w, unsigned *h);

    bool setGrabFormat(u32 fcc);
    u32  getGrabFormat(void);

    // returns channels names in the form "(name1,name2,...)"
    const char *getInputPorts(void) const;

    // return the current channel name
    const char *getInputPortName(void) const;

    // return the current channel id
    int  getInputPortId(void) const;

    // select channel
    bool setInputPort(const char *inputPort);
    bool setChannel  (int newChannel);

    int getMinWidth (void);
    int getMaxWidth (void);
    int getMinHeight(void);
    int getMaxHeight(void);

    // range [0 .. 100], for user convenience
    bool setSaturation(int z);
    int  getSaturation(void);
    bool setBrightness(int z);
    int  getBrightness(void);
    bool setHue       (int z);
    int  getHue       (void);
    bool setContrast  (int z);
    int  getContrast  (void);

    // set/get video standard
    std::string getStandardList(void);
    bool        setStandard(const char *z);
    const char *getStandard(void);

    // set/get video flicker frequency
    std::string getFlickerFreqList(void);
    bool        setFlickerFreq(const char *z);
    const char *getFlickerFreq(void);
};

#endif

