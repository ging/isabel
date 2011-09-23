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
// $Id: urlGrabber.cc 22203 2011-03-31 15:15:08Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/notify.hh>

#include <Isabel_SDK/imageLoader.hh>

#include "imageBuf.hh"
#include "urlGrabber.hh"

urlGrabber_t::urlGrabber_t(sourceDescriptor_ref sdr, const char *urlString)
: sourceDescriptor(sdr)
{
    //NOTIFY("urlGrabber_t:: loading url <%s> FROM URL!!\n", urlString);

    inetAddr_t addr0("127.0.0.1",NULL,SOCK_DGRAM);
    inetAddr_t addr1("127.0.0.1","6789",SOCK_DGRAM);
    dgramSocket_t *audSocket = new dgramSocket_t(addr0, addr1);

    if ( ! urlString || urlString[0] == '\0')
    {
        throw "urlGrabber_t:: invalid string";
    }

    AVinit();

    urlss= URLStreamOpen(urlString, audSocket);

    if (urlss == NULL)
    {
        throw "urlGrabber_t:: cannot open stream";
    }

    NOTIFY("urlGrabber_t:: Input \"%s\"\n", urlss->ic->iformat->name);
    if (urlss->video_st)
    {
        NOTIFY_ND("\tVideo Stream: frame rate %d/%d, time_base %d/%d\n",
               urlss->video_st->r_frame_rate.num,
               urlss->video_st->r_frame_rate.den,
               urlss->video_st->time_base.num,
               urlss->video_st->time_base.den
              );
    }
    else
    {
        NOTIFY_ND("\tNO Video Stream\n");
    }
    if (urlss->audio_st)
    {
        NOTIFY_ND("\tAudio Stream: sample format: %s, rate= %d \n",
#if LIBAVCODEC_VERSION_INT < ((52 << 16)+(94<<8)+3)
                  avcodec_get_sample_fmt_name(urlss->audio_st->codec->sample_fmt),
#else
                  av_get_sample_fmt_name(urlss->audio_st->codec->sample_fmt),
#endif
                  urlss->audio_st->codec->sample_rate
                 );
    }
    else
    {
        NOTIFY_ND("\tNO Audio Stream\n");
    }
}

urlGrabber_t::~urlGrabber_t(void)
{
    URLStreamClose(urlss);

    // the sockets MUST be deleted after closing source
    // as it may be sending audio
    delete urlss->audioSocket;
    urlss->audioSocket= NULL;
}

image_t*
urlGrabber_t::getImage(void)
{
    //NOTIFY("captImage loading url <%s> FROM URL!!\n", urlss->streamName);

#ifdef USA_BUFFER_VIDEO
    image_t *retVal= urlss->imgBuf.get();
#else
    pthread_mutex_lock(&urlss->frame_mutex);

    image_t *retVal= new image_t(urlss->vrawBuf,
                                 urlss->width * urlss->height * 3 / 2,
                                 I420P_FORMAT,
                                 urlss->width,
                                 urlss->height,
                                 urlss->video_pts
                                );

    pthread_mutex_unlock(&urlss->frame_mutex);
#endif

    return retVal;
}

bool
urlGrabber_t::setFrameRate(double fps)
{
    return false;
}

double
urlGrabber_t::getFrameRate(void)
{
    double fr= -1;

    if (urlss->video_st->r_frame_rate.den && urlss->video_st->r_frame_rate.num)
    {
        fr= av_q2d(urlss->video_st->r_frame_rate);
    }

#if LIBAVFORMAT_VERSION_INT >= ((52 << 16)+(41<<8)+0)
    if (urlss->video_st->avg_frame_rate.den && urlss->video_st->avg_frame_rate.num)
    {
        fr= av_q2d(urlss->video_st->r_frame_rate);
    }
#endif

    if (strcmp(urlss->ic->iformat->name, "rtsp") == 0)
    {
        NOTIFY("urlGraber_t::getFrameRate: RTSP, returning -1\n");
        return -1;
    }

    if (fr > 100)
    {
        NOTIFY("urlGraber_t::getFrameRate: fr>100, returning -1\n");
        return -1;
    }

    NOTIFY("urlGraber_t::getFrameRate: returning %f\n", fr);
    return fr;
}

bool
urlGrabber_t::setGrabSize(unsigned /*width*/, unsigned /*height*/)
{
    return false;
}

void
urlGrabber_t::getGrabSize(unsigned *w, unsigned *h)
{
    *w= urlss->width;
    *h= urlss->height;
}

bool
urlGrabber_t::setGrabFormat(u32 fcc)
{
    return false;
}

u32
urlGrabber_t::getGrabFormat(void)
{
    return I420P_FORMAT;
}

const char *
urlGrabber_t::getInputPort(void)
{
    return urlss->streamName;
}

sourceDescriptor_ref
urlGrabber_t::getDescriptor(void) const
{
    return sourceDescriptor;
}

