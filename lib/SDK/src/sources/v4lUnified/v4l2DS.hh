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
// $Id: v4l2DS.hh 10216 2007-05-25 14:38:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __v4l2_dev_stat_hh__
#define __v4l2_dev_stat_hh__

#include <linux/types.h>
#include <linux/videodev2.h>

#include <icf2/smartReference.hh>

#include <Isabel_SDK/sourceDescriptor.hh>
#include <Isabel_SDK/sourceDefinition.hh>

#include "VideoHandler.hh"

//
// global status
//

struct bufptr
{
    int length;
    unsigned char *start;
};

class v4l2DS_t: public VideoHandler_t
{
private:
    unsigned char    *deviceBuffer;
    unsigned          deviceBufferLen;

    char              numbuffers;
    char              method;

    unsigned          maxw;
    unsigned          maxh;
    unsigned          minw;
    unsigned          minh;

    unsigned          framesizestype;

    int               satindex;
    int               hueindex;
    int               contrastindex;
    int               brightnessindex;

    bufptr *buff;

    v4l2_format format;

    v4l2_requestbuffers vmb;
    v4l2_capability     vcap;

    // video norms
    v4l2_standard standard[256];
    char          numstandards;
    std::string   standardList; // cached for fast access

    // channels
    v4l2_input  vchann[256];
    char        numchannels;
    int         selectedChannel;
    std::string inputPortsList; // cached for fast access

    // allowed grab sizes
    v4l2_frmsizeenum framesizes[256];
    int              numframesizes;

    unsigned char *frame;

    void mmap_init(void);
    void uptr_init(void);
    void read_init(void);
    unsigned char *mmap_capture(u32 *timestamp, u32 *bufLen);
    unsigned char *uptr_capture(u32 *timestamp, u32 *bufLen);
    unsigned char *read_capture(u32 *timestamp, u32 *bufLen);
    void mmap_close(void);
    void uptr_close(void);
    void read_close(void);

    bool pollframesizes(u32 pixformat); // true if successful
    void printframesizes(void);

    void printGrabFormats(void);

public:
    v4l2DS_t(char const *path);
    virtual ~v4l2DS_t(void);

    const char *getName(void) const { return (char *)vcap.card; }

    void startGrabbing(void);
    void stopGrabbing(void);
    u8 *capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h);

    bool setGrabSize(unsigned w, unsigned h);
    void getGrabSize(unsigned *w, unsigned *h);

    bool           setGrabFormat(u32 fmt);
    u32            getGrabFormat(void);

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

    // set/get video norm
    std::string getStandardList(void);
    bool        setStandard(const char *z);
    const char *getStandard(void);

    // set/get video flicker frequency
    std::string getFlickerFreqList(void);
    bool        setFlickerFreq(const char *z);
    const char *getFlickerFreq(void);
};

#endif
