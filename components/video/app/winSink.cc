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
// $Id: winSink.cc 21890 2011-01-18 17:31:24Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <icf2/notify.hh>

#include <Isabel_SDK/systemRegistry.hh>

#include "winSink.hh"

paintTask_t::paintTask_t(u32 nchId, renderer_t *rr, int nmaxTTL)
: simpleTask_t(1000000),
  chId(nchId),
  theRenderer(rr),
  avatarImage(NULL),
  imagesPainted(false),
  lastTS(0),
  disorderFrames(0),
  imageTimeout(NULL),
  ttl(0),
  maxTTL(nmaxTTL),
  acro(NULL),
  followSize(false)
{
}

paintTask_t::~paintTask_t(void)
{
    this->set_period(0);
    if (imageTimeout.isValid())
    {
        del_alarm(imageTimeout);
        imageTimeout = NULL;
    }
    while (imageList.len() != 0)
    {
        image_t *img= imageList.head();
        imageList.behead();
        delete img;
    }

    if (acro != NULL)
    {
        free(acro);
    }

    if (avatarImage == NULL)
    {
        delete avatarImage;
        avatarImage= NULL; // paranoid
    }

    theRenderer= NULL; // paranoid
}

void
paintTask_t::setFollowSize(bool nfollowSize)
{
    followSize= nfollowSize;
}

void
paintTask_t::setAcronym(const char *nacro)
{
    if ( acro )  free(acro);

    acro = nacro  ? strdup(nacro)  : NULL ;
}

void
paintTask_t::setAvatarImage(image_t *img)
{
    assert (img != NULL && "setAvatarImage: invalid image");

    if (avatarImage == NULL)
    {
        delete avatarImage;
    }

    avatarImage= new image_t(*img);
}

void
paintTask_t::setAvatarTimeout(unsigned lapse)
{
    maxTTL = lapse;

    if (maxTTL == 0)
    {
        paintAvatar();
    }
}

void
paintTask_t::unmapWindow(void)
{
    imagesPainted= false;
    lastTS= 0;
    if (imageTimeout.isValid())
    {
        del_alarm(imageTimeout);
        imageTimeout = NULL;
    }
    while (imageList.len() != 0)
    {
        image_t *img= imageList.head();
        imageList.behead();
        delete img;
    }
}

void
paintTask_t::paintImage (image_t *img)
{
    locker_t locker = lock();

    ttl= 0;

    if ( ! imagesPainted)
    {
        imagesPainted = true;
        gettimeofday (&timeToPaint, NULL);
        gettimeofday (&lastPaintTime, NULL);
        lastTS = img->getTimestamp();
        assert((!imageTimeout.isValid()) && "ni la primera");

        theRenderer->paint(img, followSize);
    }
    else
    {
        double diffTS = 0;
        diffTS = ((double)img->getTimestamp()) - (double)lastTS;
        double diffTSab = fabs(diffTS);
        if (diffTSab > MAX_ARRIVAL_DIFF)
        {
            while (imageList.len() != 0)
            {
                image_t *first= imageList.head();
                imageList.behead();
                delete first;
            }
            imagesPainted = false;
            disorderFrames = 0;
            if (imageTimeout.isValid())
                del_alarm(imageTimeout);
            imageTimeout= NULL;

            NOTIFY("paintTask_t::paintImage: too long diff, discarding\n");
            delete img;
            return;
        }

        if (diffTS < 0)
        {
            disorderFrames++;
            if (disorderFrames > MAX_DISORDER)
            {
                while (imageList.len() != 0)
                {
                    image_t *first= imageList.head();
                    imageList.behead();
                    delete first;
                }
                imagesPainted = false;
                disorderFrames = 0;
                if (imageTimeout.isValid())
                    del_alarm(imageTimeout);
                imageTimeout= NULL;
            }
            NOTIFY("paintTask_t::paintImage: too much disorder, discarding\n");
            delete img;
            return;
        }
        else
        {
            disorderFrames = 0;
        }

        u32 generationDiff = (u32)(diffTS);
        while (generationDiff >= 1000000)
        {
            timeToPaint.tv_sec++;
            generationDiff -= 1000000;
        }
        timeToPaint.tv_usec += generationDiff;
        img->setTimeToPaint(timeToPaint);

        if (imageList.len() != 0)
        {
            while (imageList.len() != 0)
            {
                image_t *first= imageList.head();
                if ( (img->getTimestamp() - first->getTimestamp()) > MAX_BUFFER_DIFF)
                {
                    imageList.behead();
                    delete first;
                }
                else
                {
                    break;
                }
            }
            imageList.insert(img);
        }
        else
        {
            double diffTime = 0;
            struct timeval currentTime;
            gettimeofday (&currentTime, NULL);
            long diffsec = currentTime.tv_sec - lastPaintTime.tv_sec;
            long diffusec = currentTime.tv_usec - lastPaintTime.tv_usec;
            while (diffusec < 0)
            {
                diffsec--;
                diffusec += 1000000;
            }
            diffTime = (((double)diffsec)*1000000.0) + (double)diffusec;

            if ( (diffTime - diffTS) >= 0)
            {
                lastTS = img->getTimestamp();
                gettimeofday (&lastPaintTime, NULL);
                theRenderer->paint(img, followSize);
            }
            else
            {
                u32 period = (u32)(diffTS - diffTime);
                imageList.insert(img);
                assert((!imageTimeout.isValid()) && ("HABIA UNA ALARMA 1"));
                imageTimeout= set_alarm (period, NULL);
            }
        }
    }
}


void
paintTask_t::timeOut(const alarm_ref &alarm)
{
    locker_t locker = lock();

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    imageTimeout= NULL;

    assert((imageList.len() > 0) && ("empty queue in timeOut 1, IMPOSSIBLE"));

    image_t *currentImg = imageList.head();
    imageList.behead();

    while (imageList.len() != 0)
    {
        struct timeval nextTimeToPaint = imageList.head()->getTimeToPaint();
        long diffsec = currentTime.tv_sec - nextTimeToPaint.tv_sec;
        long diffusec = currentTime.tv_usec - nextTimeToPaint.tv_usec;
        while (diffusec < 0)
        {
            diffsec--;
            diffusec += 1000000;
        }
        u32 diff = (u32)((diffsec*1000000) + diffusec);
        if (diff > 0)
        {
            assert((imageList.len() > 0)
                   && ("empty queue in timeOut 2, IMPOSSIBLE"));
            delete currentImg; // forget previous one
            currentImg = imageList.head();
            imageList.behead();
        }
        else
        {
            struct timeval currentTimeToPaint = currentImg->getTimeToPaint();
            long diffsec = nextTimeToPaint.tv_sec - currentTimeToPaint.tv_sec;
            long diffusec= nextTimeToPaint.tv_usec - currentTimeToPaint.tv_usec;
            while (diffusec < 0)
            {
                diffsec--;
                diffusec += 1000000;
            }
            u32 period = (u32)((diffsec*1000000) + diffusec);
            assert((!imageTimeout.isValid()) && ("HABIA UNA ALARMA 2"));
            imageTimeout= set_alarm (period, NULL);
            break;
        }
    }

    lastTS = currentImg->getTimestamp();
    gettimeofday(&lastPaintTime, NULL);

    theRenderer->paint(currentImg, followSize);
}


void
paintTask_t::heartBeat(void)
{
    locker_t locker = lock();

    if (imageList.len() > 1)
    {
        image_t *first= imageList.head();
        imageList.behead();
        delete first;
    }

    if (ttl >= maxTTL)
    {
        paintAvatar();
    }
    else
    {
        ttl++;
    }
}


void
paintTask_t::paintAvatar(void)
{
    if (theRenderer->isMapped())
    {
        if (avatarImage != NULL)
        {
            theRenderer->paint(new image_t(*avatarImage), false);
            theRenderer->drawMsg(acro);
        }
        else
        {
            theRenderer->clear();
            theRenderer->drawMsg(acro);
        }
    }
}


//
// maxTTL should be 0 for local video, 5 (recommended) for remote video
//
winSink_t::winSink_t(sched_t *ctx, u32 nchId, stats_t *myS, int maxTTL)
: context(ctx),
  chId(nchId),
  imgTranslator(NULL)
{
    myStats= myS;

    theRenderer= new renderer_t (chId, myStats);

    paintTask = new paintTask_t (chId, theRenderer, maxTTL);
    (*context)<< paintTask;
}

winSink_t::~winSink_t(void)
{
    (*context) - paintTask;

    if (imgTranslator != NULL)
    {
        delete imgTranslator;
    }

    paintTask= NULL;

    delete theRenderer;

    myStats= NULL;
}

void
winSink_t::mapWindow(void)
{
    theRenderer->mapWindow();
}

void
winSink_t::unmapWindow(void)
{
    theRenderer->unmapWindow();
    paintTask->unmapWindow();
}

bool
winSink_t::isMapped(void)
{
    return theRenderer->isMapped();
}

void
winSink_t::setWindowGeometry(const char *geom)
{
    theRenderer->setGeometry(geom);
}

void
winSink_t::setWindowOverrideRedirect(bool ORFlag)
{
    theRenderer->setOverrideRedirect(ORFlag);
}

void
winSink_t::setWindowTitle(const char *acronym, const char *title)
{
    myStats->setAcronym(acronym);

    theRenderer->setTitle(title);
    paintTask->setAcronym(acronym);
}

u32
winSink_t::getWinId(void)
{
    return theRenderer->getWinId();
}

void
winSink_t::setAvatarImage(image_t *img)
{
    paintTask->setAvatarImage(img);
}

void
winSink_t::setAvatarTimeout(unsigned lapse)
{
    paintTask->setAvatarTimeout(lapse);
}

void
winSink_t::setFollowSize(bool nfollowSize)
{
    paintTask->setFollowSize(nfollowSize);
}

void
winSink_t::paintFrame(image_t *img)
{
    u32 imgFmt= img->getFormat();

    if (imgFmt == RGB24_FORMAT)
    {
        // colorspace translation not needed
        if (imgTranslator != NULL)
        {
            delete imgTranslator;
            imgTranslator= NULL;
        }
    }
    else
    {
        // colorspace translation needed
        if (imgTranslator != NULL)   // old translator is valid?
        {
            if (imgTranslator->getInputFormat() != imgFmt)
            {
                delete imgTranslator;
                imgTranslator= NULL;
            }
        }
        if (imgTranslator == NULL)   // build it up
        {
            try
            {
                imgTranslator = new imgTranslator_t(img->getFormat(), RGB24_FORMAT);
            }
            catch (char const *e)
            {
                NOTIFY("winSink_t::paintFrame: "
                       "cannot build imgTranslator_t, error= %s\n",
                       e
                      );
                exit(1);
            }
        }
    }

    if (imgTranslator != NULL)
    {
        int srcWidth = img->getWidth();
        int srcHeight= img->getHeight();
        int dstWidth= -1, dstHeight= -1;

        u8 *src= img->getBuff();

        int transSize = vFrameSize(srcWidth, srcHeight, RGB24_FORMAT);
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

        image_t *transImg= new image_t(transBuff,
                                       transSize,
                                       RGB24_FORMAT,
                                       dstWidth,
                                       dstHeight,
                                       img->getTimestamp()
                                      );

        delete [] transBuff;

        delete img;

        img= transImg;
    }

    paintTask->paintImage(img);
}

