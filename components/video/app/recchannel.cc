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
////////////////////////////////////////////////////////////////////////
//
// $Id: recchannel.cc 22392 2011-05-20 14:09:42Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <fcntl.h> // defines O_BINARY for _pipe
#endif

#include <icf2/notify.hh>
#include <icf2/ql.hh>

#include <Isabel_SDK/imageLoader.hh>
#include <Isabel_SDK/systemRegistry.hh>

#include "recchannel.hh"
#include "videoRTPSession.hh"

#include "sourceTask.hh"

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include "../../../lib/SDK/src/sources/winGrabber/winGrabber.hh"
#else
typedef u32 Window; // just to have selectByWinID available
#endif

#define SYNTHETIC_IMAGE_SOURCE "Synthetic Image Source"

////////////////////////////////////////////////////
//                IMAGE WORKER                    //
////////////////////////////////////////////////////

imageWorker_t::imageWorker_t(io_ref           io,
                             lockedImage_ref  ib,
                             winSink_t       *ws,
                             netSink_t       *ns,
                             stats_t         *myS
                            )
: imgBuff(ib),
  winSink(ws),
  netSink(ns),
  myStats(myS),
  cTop(0), cLeft(0), cBottom(0), cRight(0),
  doCrop(false),
  flipV(false),
  mirrorH(false),
  doSend(false),
  w(320), h(240),
  imgTranslator(NULL)
{
    add_IO(io);
}

imageWorker_t::~imageWorker_t(void)
{
    myStats= NULL;
    winSink= NULL;
    netSink= NULL;

    if (imgTranslator != NULL)
    {
        delete imgTranslator;
    }
}

void
imageWorker_t::send(bool sendFlag)
{
    doSend= sendFlag;
}

bool
imageWorker_t::isSending(void)
{
    return doSend;
}

void
imageWorker_t::setSize(int nw, int nh)
{
    w= nw & ~0x0F; // divisible by 16
    h= nh & ~0x0F; // divisible by 16
}

void
imageWorker_t::setFlipMode(bool flip)
{
    flipV= flip;
}

bool
imageWorker_t::getFlipMode(void)
{
    return flipV;
}

void
imageWorker_t::setMirrorMode(bool mirror)
{
    mirrorH= mirror;
}

bool
imageWorker_t::getMirrorMode(void)
{
    return mirrorH;
}

void
imageWorker_t::setCrop(double top, double left, double bottom, double right)
{
    assert (    (top >= 0)    && (top <= 100.00)
             && (left >= 0)   && (left <= 100.00)
             && (bottom >= 0) && (bottom <= 100.00)
             && (right >= 0)  && (right <= 100.00)
             && "imageWorker_t::setCrop: invalid value(s)"
           );

    cTop= top;
    cLeft= left;
    cBottom= bottom;
    cRight= right;

    doCrop= (top > 0) || (left > 0) || (bottom > 0) || (right > 0);
}


void
imageWorker_t::getCrop(double &top, double &left, double &bottom, double &right)
{
    top= cTop;
    left= cLeft;
    bottom= cBottom;
    right= cRight;
}

image_t*
imageWorker_t::convertImg(image_t *img, u32 newFmt)
{
    u32 imgFmt= img->getFormat();

    assert (imgFmt != newFmt && "convertImg: unneeded call\n");

    // colorspace translation needed
    if (imgTranslator != NULL)   // old translator is valid?
    {
        if ((imgTranslator->getInputFormat()  != imgFmt) ||
            (imgTranslator->getOutputFormat() != newFmt)
           )
        {
            delete imgTranslator;
            imgTranslator= NULL;
        }
    }
    if (imgTranslator == NULL)   // build it up
    {
        try
        {
            imgTranslator= new imgTranslator_t(imgFmt, newFmt);
            NOTIFY("imageWorker_t::convertImg: translator built for %s->%s\n",
                   vGetFormatNameById(imgFmt),
                   vGetFormatNameById(newFmt)
                  );
        }
        catch (char const *e)
        {
            //NOTIFY("winSink_t::paintFrame: "
            //       "cannot build imgTranslator_t, error= %s\n",
            //       e
            //      );
            //exit(1);
        }
        if (imgTranslator == NULL)
        {
            NOTIFY("imageWorker_t::convertImg: cannot convert "
                   "from 0x%x to 0x%x, returning NULL\n",
                   imgFmt,
                   newFmt
                  );
            return NULL;
        }
    }

    int dstWidth= -1, dstHeight= -1;
    int srcWidth = img->getWidth();
    int srcHeight= img->getHeight();
    u8 *src= img->getBuff();

    int transSize = vFrameSize(srcWidth, srcHeight, newFmt);
    u8 *transBuff= new u8[transSize];
    if ( ! transBuff )
    {
        NOTIFY("paint: run out of memory, bailing out\n");
        exit(-1);
    }

    imgTranslator->translate(transBuff,
                             dstWidth, dstHeight,
                             src,
                             srcWidth, srcHeight
                            );

    image_t *newImg= new image_t(transBuff,
                                 transSize,
                                 newFmt,
                                 dstWidth,
                                 dstHeight,
                                 img->getTimestamp()
                                );

    delete [] transBuff;

    return newImg;
}

void
imageWorker_t::doWork(void)
{
    image_t *img= imgBuff->getImage();

    if (img == NULL)
    {
        // if loaded CPU, sourceTask may put images faster than
        // we got them, resulting in more than one "GO" messages
        // for just one frame, the last one
        return ;
    }

    if (img->getFormat() != I420P_FORMAT)
    {
        image_t *newImg= convertImg(img, I420P_FORMAT);

        delete img;

        if (newImg == NULL)
        {
            NOTIFY("imageWorker_t::doWork: "
                   "invalid converted image, returning\n"
                  );
            return ;
        }

        img= newImg;
    }

    if ( flipV )
    {
        img->flipV();
    }

    if ( mirrorH )
    {
        img->mirrorH();
    }

    if ( doCrop )
    {
        int imgW= img->getWidth();
        int imgH= img->getHeight();

        int croppedX= int(imgW * cLeft / 100);
        int croppedY= int(imgH * cTop  / 100);

        int croppedW= int( (100.0 - cLeft - cRight) * imgW / 100);
        int croppedH= int( (100.0 - cTop - cBottom) * imgH / 100);

        croppedW &= ~0x0F; // dividible by 16
        croppedH &= ~0x0F; // dividible by 16

        img->crop(croppedX, croppedY, croppedW, croppedH);
    }

    if ((w > 0) && (h > 0) &&
        ( (1.10 * w * h) < ((float)img->getWidth() * img->getHeight()) )
       )
    {
        img->zoom(w, h);
    }

    img->crop16();

    myStats->setImageSize(img->getWidth(), img->getHeight());

    if (doSend)
    {
        image_t *sendImg= new image_t(*img);

        netSink->sendFrame(sendImg);
    }
    winSink->paintFrame(img);
}

void
imageWorker_t::IOReady(io_ref &io)
{
    char res[8];

    memset(res, 0, 8);

    int nn= io->read(res, sizeof(res));

    if (nn < 0)
    {
        NOTIFY("imageWorker_t::IOReady: read returned < 0!\n");
        return;
    }
    if (nn == 0)
    {
        NOTIFY("imageWorker_t::IOReady: read returned 0!\n");
        // hold_IO(io);
        // get_owner()->remoteTask(this);
        return;
    }

    doWork();
}

////////////////////////////////////////////////////
//               SOURCE CHANNEL                   //
////////////////////////////////////////////////////

recChannel_t::recChannel_t(sched_t *ctx, u32 chId, const char *compId)
: channel_t(ctx, chId, channel_t::REC_CHANNEL, compId),
  FR(25.0),
  grabW(352),
  grabH(288)
{
    inetAddr_t addr0("127.0.0.1", NULL, SOCK_DGRAM);
    inetAddr_t addr1("127.0.0.1", NULL, SOCK_DGRAM);
    dgramSocket_t* s0 = new dgramSocket_t(addr0); // abrimos las conexiones
    dgramSocket_t* s1 = new dgramSocket_t(addr1,addr0);
    io_ref pipe0= (io_t*)s0;
    io_ref pipe1= (io_t*)s1;

    netSink= new netSink_t(chId, INITIAL_BW, myStats);

    lockedImage_ref imgBuff= new lockedImage_t;

    sourceTask= new sourceTask_t(pipe1, imgBuff);

    imgWorker= new imageWorker_t(pipe0, imgBuff, winSink, netSink, myStats);

    (*context) << imgWorker;
    sourceTask_t *stp= sourceTask;
    (*context) << stp;

    rtpSession->assignSender(chId);
}

recChannel_t::~recChannel_t(void)
{
    sourceTask->set_period(0);

    sourceTask_t *stp= sourceTask;
    context->removeTask(stp);
    sourceTask= NULL;

    winSink= NULL;

    netSink= NULL;

    context->removeTask(imgWorker);

    context->removeTask(dynamic_cast<simpleTask_t*>(this));

    rtpSession->deleteSender (this->getId(), "Channel deleted");
}

void
recChannel_t::mapWindow(void)
{
    channel_t::mapWindow();

    if (sourceTask->get_period() == 0)
    {
        setFrameRate(FR);
    }
}

void
recChannel_t::unmapWindow(void)
{
    channel_t::unmapWindow();

    if ( ! imgWorker->isSending())
    {
        sourceTask->set_period(0);
    }
}

std::string
recChannel_t::interactiveSelect (void)
{
    std::string res;

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    if ( ! theSource.isValid())
    {
        res= "ERROR: source not available";
        NOTIFY("recChannel_t::interactiveSelect: %s\n", res.c_str());
        return res;
    }

    const char *desc= theSource->getDescriptor()->getID();
    if (strcmp(desc, "Window grabber") != 0)
    {
        res= "cannot select, unsuitable source= ";
        res += desc;
        NOTIFY("recChannel_t::interactiveSelect: %s \n", res.c_str());
        return res;
    }

    source_t *s= theSource;
    windowGrabber_t *gi= dynamic_cast<windowGrabber_t*>(s);

    gi->selectWindowByClicking();

    res= "OK";
#else
    res= "NOT IMPLEMENTED";
    NOTIFY("recChannel_t::interactiveSelect: %s\n", res.c_str());
#endif

    return res;
}

std::string
recChannel_t::selectByWinId (u32 winId)
{
    std::string res;

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    if ( ! theSource.isValid())
    {
        res= "ERROR: source not available";
        NOTIFY("recChannel_t::interactiveSelect: %s\n", res.c_str());
        return res;
    }

    const char *desc= theSource->getDescriptor()->getID();
    if (strcmp(desc, "Window grabber") != 0)
    {
        res= "unsuitable source= ";
        res += desc;
        NOTIFY("recChannel_t::selectByWinId: %s\n", res.c_str());
        return res;
    }

    source_t *s= theSource;
    windowGrabber_t *gi= dynamic_cast<windowGrabber_t*>(s);
    gi->selectWindowByWinId(winId);

    res= "OK";
#else
    res= "NOT IMPLEMENTED";
    NOTIFY("recChannel_t::selectByWinId: %s\n", res.c_str());
#endif

    return res;
}

bool
recChannel_t::setSourceFormat(u32 sourceFmt)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setSourceFormat: source not available\n"); 
        return false;
    }

    theSource->setGrabFormat(sourceFmt);

    return true;
}

void
recChannel_t::setGrabSize(unsigned w, unsigned h)
{
    NOTIFY("recChannel_t::setGrabSize: trying to set to %dx%d\n", w, h);

    if ((w == 0) || (h == 0))
    {
        NOTIFY("recChannel_t::setGrabSize: "
               "width or height are 0, bailing out\n"
              );
        return ;
    }

    grabW= w;
    grabH= h;

    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setGrabSize: source not available\n");
        return ;
    }

    theSource->setGrabSize(w, h);

    imgWorker->setSize(w, h);
}


void
recChannel_t::setFrameRate(double fr)
{
    assert(fr > 0);

    FR= fr;

    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setFramerate: source not available\n");
        return;
    }

    sourceTask->set_period(0);

    theSource->setFrameRate(FR);

    double fps, cfps= theSource->getFrameRate();

    if (cfps == 0.0)
    {
        // source is unable to deliver frames, so...
        NOTIFY("recChannel_t::setFrameRate: source is not ready, fps= 0\n");
        return;
    }
    if (cfps > 0)
    {
        // source imposes the frame rate, so...
        fps= cfps;
    }
    else
    {   // < 0, any rate may apply
        fps = FR;
    }

    sourceTask->set_period((u32)(1000000/fps));

    netSink->setFrameRate(fps);

    myStats->setDesiredFR(fr);

    NOTIFY("recChannel_t::setFrameRate:%s period=%u, TBF=%f (%f fps)\n",
           cfps > 0 ? " FORCED" : "",
           sourceTask->get_period(),
           1 / fps,
           fps
          );
}


void
recChannel_t::setVideoSaturation(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setVideoSaturation: source not available\n");
        return ;
    }

    if (theSource->getAttr("Saturation") != NULL)
    {
        theSource->getAttr("Saturation")->setValue(value);
    }

    videoConf.saturation= value;
    writeConfigFile(&videoConf);
}


int
recChannel_t::getVideoSaturation(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getVideoSaturation: source not available\n");
        return -1;
    }

    if (theSource->getAttr("Saturation") == NULL)
    {
        return -1;
    }

    int retVal;
    theSource->getAttr("Saturation")->getValue(&retVal);

    return retVal;
}


void
recChannel_t::setVideoBrightness(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setVideoBrightness: source not available\n");
        return ;
    }

    if (theSource->getAttr("Brightness") != NULL)
    {
        theSource->getAttr("Brightness")->setValue(value);
    }

    videoConf.brightness= value;
    writeConfigFile(&videoConf);
}


int
recChannel_t::getVideoBrightness(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getVideoBrightness: source not available\n");
        return -1;
    }

    if (theSource->getAttr("Brightness") == NULL)
    {
        return -1;
    }

    int retVal;
    theSource->getAttr("Brightness")->getValue(&retVal);

    return retVal;
}


void
recChannel_t::setVideoHue(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setVideoHue: source not available\n");
        return ;
    }

    if (theSource->getAttr("Hue") != NULL)
    {
        theSource->getAttr("Hue")->setValue(value);
    }

    videoConf.hue= value;
    writeConfigFile(&videoConf);
}


int
recChannel_t::getVideoHue(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getVideoHue: source not available\n");
        return -1;
    }

    if (theSource->getAttr("Hue") == NULL)
    {
        return -1;
    }

    int retVal;
    theSource->getAttr("Hue")->getValue(&retVal);

    return retVal;
}


void
recChannel_t::setVideoContrast(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setVideoContrast: source not available\n");
        return ;
    }

    if (theSource->getAttr("Contrast") != NULL)
    {
        theSource->getAttr("Contrast")->setValue(value);
    }

    videoConf.contrast= value;
    writeConfigFile(&videoConf);
}


int
recChannel_t::getVideoContrast(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getVideoContrast: source not available\n");
        return -1;
    }

    if (theSource->getAttr("Contrast") == NULL)
    {
        return -1;
    }

    int retVal;
    theSource->getAttr("Contrast")->getValue(&retVal);

    return retVal;
}


std::string
recChannel_t::getVideoStandardList(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getVideoStandardList: source not available\n");
        return std::string();
    }

    if (theSource->getAttr("StandardList") == NULL)
    {
        return std::string();
    }

    std::string retVal;
    theSource->getAttr("StandardList")->getValue(&retVal);

    return retVal;
}


bool
recChannel_t::setVideoStandard(const char *norm)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setVideoStandard: source not available\n");
        return false;
    }

    if (theSource->getAttr("Standard") == NULL)
    {
        return false;
    }

    if ( ! theSource->getAttr("Standard")->setValue(std::string(norm)))
    {
        return false;
    }

    sprintf(videoConf.norm, "%s", norm);
    writeConfigFile(&videoConf);

    return true;
}


std::string
recChannel_t::getVideoStandard(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getVideoStandard: source not available\n");
        return std::string();
    }

    if (theSource->getAttr("Standard") == NULL)
    {
        return std::string();
    }

    std::string retVal;
    theSource->getAttr("Standard")->getValue(&retVal);

    return retVal;
}


std::string
recChannel_t::getFlickerFreqsList(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getFlickerFreqsList: source not available\n");
        return std::string();
    }

    if (theSource->getAttr("FlickerFreqList") == NULL)
    {
        return std::string();
    }

    std::string retVal;
    theSource->getAttr("FlickerFreqList")->getValue(&retVal);

    return retVal;
}


bool
recChannel_t::setFlickerFreq(const char *ffreq)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::setFlickerFreq: source not available\n");
        return false;
    }

    if (theSource->getAttr("FlickerFreq") == NULL)
    {
        return false;
    }

    if ( ! theSource->getAttr("FlickerFreq")->setValue(std::string(ffreq)))
    {
        return false;
    }

    sprintf(videoConf.flickerfreq, "%s", ffreq);
    writeConfigFile(&videoConf);

    return true;
}


std::string
recChannel_t::getFlickerFreq(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getFlickerFreq: source not available\n");
        return std::string();
    }

    if (theSource->getAttr("FlickerFreq") == NULL)
    {
        return std::string();
    }

    std::string retVal;
    theSource->getAttr("FlickerFreq")->getValue(&retVal);

    return retVal;
}


void
recChannel_t::setFlipMode(bool flip)
{
    imgWorker->setFlipMode(flip);
}


bool
recChannel_t::getFlipMode(void)
{
    return imgWorker->getFlipMode();
}


void
recChannel_t::setMirrorMode(bool mirror)
{
    imgWorker->setMirrorMode(mirror);
}


bool
recChannel_t::getMirrorMode(void)
{
    return imgWorker->getMirrorMode();
}

void
recChannel_t::setCrop(double top, double left, double bottom, double right)
{
    return imgWorker->setCrop(top, left, bottom, right);
}


void
recChannel_t::getCrop(double &top, double &left, double &bottom, double &right)
{
    return imgWorker->getCrop(top, left, bottom, right);
}

//
// if srcDesc == NULL, then select the first valid source
bool
recChannel_t::setSource(const char *srcDesc, const char *inputPort)
{
    NOTIFY("recChannel_t::setSource(%s,%s)\n", srcDesc, inputPort);

    theSource= NULL;
    sourceTask->stopAndRelease();

    if ((srcDesc != NULL) && strcmp(srcDesc, "NONE") == 0)
    {
        return true;
    }

    std::string srcDescStr = "";
    if (srcDesc != NULL)
    {
        srcDescStr = srcDesc;
    }

    source_ref s, validSource;
    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        if (srcDesc == NULL) // keep the first valid
        {
            if ( ! validSource.isValid() )
            {
                validSource= sf->createSource(sd, NULL);
            }
            continue;
        }
        else if (strcmp(srcDesc, sd->getID()) == 0)
        {
            s= sf->createSource(sd, inputPort);

            if (s.isValid())
                break;
        }
    }
    if ( ! s.isValid())
    {
        if ( ! validSource.isValid() )
        {
            NOTIFY("recChannel_t::setSource: Grabber NOT found\n");
            return false;
        }
        const char *validName= validSource->getDescriptor()->getID();
        if (strcmp(validName, "Synthetic Image Source") == 0)
        {
            // do not select Synthetic Image Source by default
            return false;
        }
        s= validSource;
        srcDescStr = validSource->getDescriptor()->getID();
        inputPort= validSource->getInputPort();
    }

    theSource= s;
    theSource->setGrabFormat(I420P_FORMAT);

    readConfigFile(srcDescStr.c_str(), videoConf);

    sprintf(videoConf.inputport, "%s", inputPort == NULL ? "" : inputPort);

    // old settings
    NOTIFY("recChannel_t::recChannel_t: old settings for %s (%s)\n",
           srcDescStr.c_str(),
           inputPort != NULL ? inputPort : ""
          );

    if (theSource->getAttr("Brightness") != NULL)
        theSource->getAttr("Brightness")->setValue(videoConf.brightness);
    if (theSource->getAttr("Saturation") != NULL)
        theSource->getAttr("Saturation")->setValue(videoConf.saturation);
    if (theSource->getAttr("Hue") != NULL)
        theSource->getAttr("Hue")->setValue(videoConf.hue);
    if (theSource->getAttr("Contrast") != NULL)
        theSource->getAttr("Contrast")->setValue(videoConf.contrast);
    if (theSource->getAttr("Standard") != NULL)
        theSource->getAttr("Standard")->setValue((std::string)videoConf.norm);
    if (theSource->getAttr("FlickerFreq") != NULL)
        theSource->getAttr("FlickerFreq")->setValue((std::string)videoConf.flickerfreq);

    writeConfigFile(&videoConf); // write it, to avoid get older

    sourceTask->setSource(theSource);

    setFrameRate(FR);
    setGrabSize(grabW, grabH);

    return true;
}


const char*
recChannel_t::getSourceDesc(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getSourceDesc: source not available\n");
        return NULL;
    }

    return theSource->getDescriptor()->getID();
}


const char *
recChannel_t::getInputPort(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("recChannel_t::getInputPort: source not available\n");
        return NULL;
    }

    return theSource->getInputPort();
}


void
recChannel_t::setCodecQuality(int quality)
{
    netSink->setCodecQuality(quality);
}


void
recChannel_t::setH263Mode(const char *mode)
{
    netSink->setCodecMode(mode);
}


void
recChannel_t::setMpegPBetweenI(int pbeti)
{
    netSink->setCodecPBetI(pbeti);
}


void
recChannel_t::sendFlow(bool sendFlag)
{
    imgWorker->send(sendFlag);

    if (sendFlag)
    {
        if (sourceTask->get_period() == 0)
        {
            setFrameRate(FR);
        }
    }
    else
    {
        if ( ! winSink->isMapped())
        {
            sourceTask->set_period(0);
        }
    }
}

void
recChannel_t::setChannelBandwidth (double bw)
{
    netSink->setBandwidth(bw);
}


void
recChannel_t::setLineFmt(u32 newFmt)
{
    netSink->setLineFmt(newFmt);
}


