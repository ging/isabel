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
// $Id: recchannel.hh 22392 2011-05-20 14:09:42Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __recchannel_hh__
#define __recchannel_hh__

#include <icf2/sched.hh>

#include <Isabel_SDK/sourceDefinition.hh>

#include "lockedImage.hh"
#include "sourceTask.hh"
#include "channel.hh"
#include "netSink.hh"
#include "videoConfig.hh"

class imageWorker_t: public simpleTask_t
{
private:
    lockedImage_ref  imgBuff;

    winSink_t *winSink;
    netSink_t *netSink;

    stats_t *myStats;

    // crop relative values
    double cTop, cLeft, cBottom, cRight;
    bool doCrop; // cached to simplify code

    bool flipV, mirrorH;
    bool doSend;

    int w, h;

    imgTranslator_t *imgTranslator;
    u32              lastImgFmt;

    image_t *convertImg(image_t *img, u32 newFmt);
    void doWork(void);

protected:
    virtual void IOReady(io_ref &io);

public:
    imageWorker_t(io_ref io,
                  lockedImage_ref ib,
                  winSink_t *ws,
                  netSink_t *ns,
                  stats_t *myS
                 );

    void send(bool sendFlag);
    bool isSending(void);
    void setSize(int nw, int nh);
    void setFlipMode(bool flip);
    bool getFlipMode(void);
    void setMirrorMode(bool mirror);
    bool getMirrorMode(void);

    void setCrop(double  top, double  left, double  bottom, double  up);
    void getCrop(double &top, double &left, double &bottom, double &up);

    virtual ~imageWorker_t(void);
};

class recChannel_t: public channel_t
{
private:
    sourceTask_ref sourceTask;
    source_ref     theSource; // shared with sourceTask, created here

    int notificationPipe[2];

    netSink_t     *netSink;
    imageWorker_t *imgWorker;

    double   FR;
    unsigned grabW, grabH;

    videoConfig_t videoConf;

public:
    recChannel_t(sched_t *ctx, u32 chId, const char *compId);

    virtual ~recChannel_t(void);

    //
    // presentation related methods, extends channel_t behaviour
    //
    void mapWindow(void);
    void unmapWindow(void);

    // source related parameters
    std::string    interactiveSelect(void);
    std::string    selectByWinId(u32 winId);
    bool           setSourceFormat(u32 sourceFmt);
    void           setGrabSize(unsigned w, unsigned h);
    void           setFrameRate(double fr);

    void           setVideoSaturation(int value);
    int            getVideoSaturation(void);
    void           setVideoBrightness(int value);
    int            getVideoBrightness(void);
    void           setVideoHue(int value);
    int            getVideoHue(void);
    void           setVideoContrast(int value);
    int            getVideoContrast(void);
    std::string    getVideoStandardList(void);
    bool           setVideoStandard(const char *norm);
    std::string    getVideoStandard(void);
    std::string    getFlickerFreqsList(void);
    bool           setFlickerFreq(const char *ffreq);
    std::string    getFlickerFreq(void);

    void           setFlipMode(bool flip);
    bool           getFlipMode(void);
    void           setMirrorMode(bool mirror);
    bool           getMirrorMode(void);

    void setCrop(double  top, double  left, double  bottom, double  up);
    void getCrop(double &top, double &left, double &bottom, double &up);

    bool           setSource(const char *srcDesc, const char *inputPort);
    const char    *getSourceDesc(void);
    const char    *getInputPort(void);

    // line related parameters
    void           sendFlow(bool sendFlag);
    void           setChannelBandwidth(double bw);
    void           setLineFmt(u32 newFmt);

    void           setCodecQuality(int quality);
    void           setH263Mode(const char *factor);
    void           setMpegPBetweenI(int pbeti);

    // administrivia related parameters
    virtual const char *className(void) const { return "recChannel_t"; }
};

#endif
