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
// $Id: srcMgr.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <vCodecs/codec.h>

#include <icf2/notify.hh>
#include <icf2/ql.hh>

#include <Isabel_SDK/systemRegistry.hh>

#include "srcMgr.hh"

#include "sourceTask.hh"
#include "confFile.hh"

////////////////////////////////////////////////////
//                IMAGE WORKER                    //
////////////////////////////////////////////////////

imageWorker_t::imageWorker_t(io_ref io, lockedImage_ref ib, shmSink_ref ssr)
: imgBuff(ib),
  shmSink(ssr)
{
    add_IO(io);
}

imageWorker_t::~imageWorker_t(void)
{
    shmSink= NULL;
}

void
imageWorker_t::IOReady(io_ref &io)
{
    char res[1024];

    int nn;

    nn= io->read(res, sizeof(res));

    if (nn < 0)
    {
        NOTIFY("imageWorker_t::IOReady: read returned < 0!\n");
        return;
    }
    if (nn == 0)
    {
        NOTIFY("imageWorker_t::IOReady: read returned 0!\n");
        NOTIFY("imageWorker_t::IOReady: bailing out\n");
        hold_IO(io);
        get_owner()->removeTask(this);
        return;
    }

    image_t *img= imgBuff->getImage();

    if (img == NULL)
    {
        return ;
    }

    shmSink->putImage(img);
}

////////////////////////////////////////////////////
//               SOURCE CHANNEL                   //
////////////////////////////////////////////////////

srcMgr_t::srcMgr_t(sched_t *ctx, int shmId, int shmSize)
: context(ctx)
{
    int  camLen= 1024;
    char lastCamera[camLen];
    source_ref validSource= NULL;

    // select the last used camera
    memset(lastCamera, 0, camLen);
    readConfigFile(lastCamera, camLen);

    source_ref s;
    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        s= sf->createSource(sd);

        if (s.isValid())
        {
            if ( ! validSource.isValid()) // keep the first one valid
                validSource= s;

            if (strcmp(sd->getID(), lastCamera) == 0)   // found
            {
                theSource= s;
                break;
            }
        }
    }

    if ( ! theSource.isValid())
    {
        theSource= validSource;
    }

    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::srcMgr_t: grabber NOT found\n");
    }
    else
    {
        theSource->setGrabFormat(I420P_FORMAT);
        writeConfigFile(theSource->getDescriptor()->getID());
    }

    assert((pipe(notificationPipe) > 0) || "failed to open with myself\n");

    io_ref pipe0= new io_t(notificationPipe[0]);
    io_ref pipe1= new io_t(notificationPipe[1]);

    shmSink= new shmSink_t(shmId, shmSize);

    lockedImage_ref imgBuff= new lockedImage_t;

    sourceTask= new sourceTask_t(pipe1, theSource, imgBuff);

    imgWorker= new imageWorker_t(pipe0, imgBuff, shmSink);

    (*context) << imgWorker;
    (*context) << static_cast<threadedTask_t*>(sourceTask);

    double initialFR = 25.0;
    sourceTask->tlPostMsg(new startMsg_t(1/initialFR));
}

srcMgr_t::~srcMgr_t(void)
{
    sourceTask->tlPostMsg(new tlMsg_t(MSG_STOP));
    //(*context) - static_cast<threadedTask_t*>(sourceTask);
    context->removeTask(static_cast<threadedTask_t*>(sourceTask));
    sourceTask= NULL;

    shmSink= NULL;

    context->removeTask(imgWorker);
}

void
srcMgr_t::setGrabSize(unsigned w, unsigned h)
{
    debugMsg(dbg_App_Verbose,
             "setGrabSize",
             "trying to set to %dx%d\n",
             w, h
            );

    if ((w == 0) || (h == 0))
    {
        NOTIFY("srcMgr_t::setGrabSize: "
               "width or height are 0, bailing out\n"
              );
        return ;
    }

    if (theSource.isValid())
    {
        debugMsg(dbg_App_Verbose,
                 "setGrabSize",
                 "setting to %ux%u\n",
                 w, h
                );
        theSource->setGrabSize(w, h);
    }
}


void
srcMgr_t::setFrameRate(double fr)
{
    assert(fr > 0);

    sourceTask->tlPostMsg(new setTimeBetweenFramesMsg_t(1.0/fr));
}


void
srcMgr_t::setVideoSaturation(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::setVideoSaturation: cannot set Saturation to %s "
               "[invalid source]\n",
               value
              );
        return ;
    }

    if (theSource->getAttr("Saturation") != NULL)
    {
        theSource->getAttr("Saturation")->setValue(value);
    }

}


int
srcMgr_t::getVideoSaturation(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getVideoSaturation: cannot get Saturation "
               "[invalid source]\n"
              );
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
srcMgr_t::setVideoBrightness(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::setVideoBrightness: cannot set Brightness to %s "
               "[invalid source]\n",
               value
              );
        return ;
    }

    if (theSource->getAttr("Brightness") != NULL)
    {
        theSource->getAttr("Brightness")->setValue(value);
    }

}


int
srcMgr_t::getVideoBrightness(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getVideoSaturation: cannot get Brightness "
               "[invalid source]\n"
              );
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
srcMgr_t::setVideoHue(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::setVideoHue: source not available\n");
        return ;
    }

    if (theSource->getAttr("Hue") != NULL)
    {
        theSource->getAttr("Hue")->setValue(value);
    }
}


int
srcMgr_t::getVideoHue(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getVideoSaturation: source not availbale\n");
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
srcMgr_t::setVideoContrast(int value)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::setVideoContrast: source not available\n");
        return ;
    }

    if (theSource->getAttr("Contrast") != NULL)
    {
        theSource->getAttr("Contrast")->setValue(value);
    }
}


int
srcMgr_t::getVideoContrast(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getVideoSaturation: source not available\n");
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
srcMgr_t::getVideoStandardList(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getVideoStandardList: source not available\n");
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
srcMgr_t::setVideoStandard(const char *norm)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::setVideoStandard: source not available\n");
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

    return true;
}


std::string
srcMgr_t::getVideoStandard(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getVideoStandard: source not available\n");
        return NULL;
    }

    if (theSource->getAttr("Standard") == NULL)
    {
        return NULL;
    }

    std::string retVal;
    theSource->getAttr("Standard")->getValue(&retVal);

    return retVal;
}


void
srcMgr_t::setSource(char *srcInfo, char *camera)
{
    NOTIFY("srcMgr_t::setSource: %s,%s\n", srcInfo, camera);

    source_ref s;
    sourceFactoryInfoArray_ref sfia= getSourceFactoryInfoArray();

    for (int i= 0; i < sfia->size(); i++)
    {
        sourceFactoryInfo_ref sfi= sfia->elementAt(i);

        sourceFactory_ref     sf = sfi->getFactory();
        sourceDescriptor_ref  sd = sfi->getDescriptor();

        if (strcmp(srcInfo, sd->getID()) == 0)
        {
            s= sf->createSource(sd, camera);

            if (s.isValid())
                break;
        }
    }
    if ( ! s.isValid())
    {
        NOTIFY("sourceTask_t::setSource: Grabber NOT found\n");
        return;
    }

    theSource= s;
    theSource->setGrabFormat(I420P_FORMAT);

    sourceTask->tlPostMsg(new setSourceMsg_t(theSource));

    writeConfigFile(theSource->getDescriptor()->getID());
}


const char*
srcMgr_t::getSourceInfo(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getSourceInfo: source not available\n");
        return NULL;
    }

    return theSource->getDescriptor()->getID();
}


const char *
srcMgr_t::getCamera(void)
{
    if ( ! theSource.isValid())
    {
        NOTIFY("srcMgr_t::getSourceInfo: source not available\n");
        return NULL;
    }

    return theSource->getInputPort();
}


