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
// $Id: v4luDioctl.cc 10332 2007-06-07 17:11:37Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>
#ifdef HAVE_V4L1
#include <linux/videodev.h>
#endif
#include <sys/time.h>

#include <icf2/notify.hh>

#include <Isabel_SDK/systemRegistry.hh>

#include "v4luDioctl.hh"


#ifdef HAVE_V4L1
//
// a couple of utility functions
//
const char *
parsev4l1VideoType(int t)
{
    static char b[4096];

    b[0]= 0;

    const char *desc[]={
      "CAPTURE "     , "TUNER "       ,
      "TELETEXT "    , "OVERLAY "     ,
      "CHROMAKEY "   , "CLIPPING "    ,
      "FRAMERAM "    , "SCALES "      ,
      "MONOCHROME "  , "SUBCAPTURE"   ,
      "MPEG_DECODER" , "MPEG_ENCODER" ,
      "MJPEG_DECODER", "MJPEG_ENCODER"
    };

    for(int i= 0; i< 9; i++)
        if(t&(1<<i))
            strcat(b, desc[i]);

    return b;
}

const char *
parsev4l1ChannelFlags(int f)
{
    static char b[4096];

    b[0]= 0;

    const char *desc[]={
      "TUNER "   , "AUDIO "
    };

    for(int i= 0; i< 2; i++)
        if(f&(1<<i))
            strcat(b, desc[i]);

    return b;
}

const char *
parsev4l1ChannelType(int t)
{
    static char b[4096];

    b[0]= 0;

    const char *desc[]=
    {
        "TV "   , "CAMERA "
    };

    for(int i= 0; i< 2; i++)
        if(t&(1<<i))
            strcat(b, desc[i]);

    return b;
}

const char *
parsev4l1Palette(int p)
{
    switch(p)
    {
    case VIDEO_PALETTE_GREY   : return "GREY";
    case VIDEO_PALETTE_HI240  : return "HI240";
    case VIDEO_PALETTE_RGB565 : return "RGB565";
    case VIDEO_PALETTE_RGB24  : return "RGB24";
    case VIDEO_PALETTE_RGB32  : return "RGB32";
    case VIDEO_PALETTE_RGB555 : return "RGB555";
    case VIDEO_PALETTE_YUV422 : return "YUV422";
    case VIDEO_PALETTE_YUYV   : return "YUYV";
    case VIDEO_PALETTE_UYVY   : return "UYVY";
    case VIDEO_PALETTE_YUV420 : return "YUV420";
    case VIDEO_PALETTE_YUV411 : return "YUV411";
    case VIDEO_PALETTE_RAW    : return "RAW";
    case VIDEO_PALETTE_YUV422P: return "YUV422P";
    case VIDEO_PALETTE_YUV411P: return "YUV411P";
    case VIDEO_PALETTE_YUV420P: return "YUV420P";
    case VIDEO_PALETTE_YUV410P: return "YUV410P";
    default                   : return "UNKNOWN_PALETTE";
    }
}

const char *
parsev4l1Norm(int m)
{
    switch(m)
    {
    case VIDEO_MODE_PAL  :  return "PAL";
    case VIDEO_MODE_NTSC :  return "NTSC";
    case VIDEO_MODE_SECAM:  return "SECAM";
    case VIDEO_MODE_AUTO :  return "AUTO";
    default              :  return NULL;
    }
}

int
getv4l1Norm(const char *z)
{
    if (strcasecmp("PAL", z) == 0)
    {
        return VIDEO_MODE_PAL;
    }
    if (strcasecmp("NTSC", z) == 0)
    {
        return VIDEO_MODE_NTSC;
    }
    if (strcasecmp("SECAM", z) == 0)
    {
        return VIDEO_MODE_SECAM;
    }
    if (strcasecmp("AUTO", z) == 0)
    {
        return VIDEO_MODE_AUTO;
    }

    throw "getv4l1Norm: unknown norm";
}

int
v4lId2bpp(int i)
{
    // bytes per pixel
    switch(i)
    {
    case VIDEO_PALETTE_RGB24  : return 3;
    case VIDEO_PALETTE_RGB565 :
    case VIDEO_PALETTE_RGB555 :
    case VIDEO_PALETTE_YUV422 :
    case VIDEO_PALETTE_YUV422P:
    case VIDEO_PALETTE_YUV411P:
    case VIDEO_PALETTE_YUV420P: return 2;
    }
    throw "v4lId2bpp: cannot dealt with palette";
}

u32
getFCCByV4LId(int i)
{
    switch (i)
    {
    case VIDEO_PALETTE_RGB565  : return RGB565_FORMAT;
    case VIDEO_PALETTE_RGB24   : return BGR24_FORMAT; // CARE!!
    case VIDEO_PALETTE_YUV422  : return I422i_FORMAT;
    case VIDEO_PALETTE_YUV422P : return I422P_FORMAT;
    case VIDEO_PALETTE_YUV420P : return I420P_FORMAT;
    case VIDEO_PALETTE_YUV411P : return I411P_FORMAT;
    }
    throw "getFCCByV4LId: no conversion available";
}

int
getV4LIdByFCC(u32 fmt)
{
    switch (fmt)
    {
    //case RGB16_555_FORMAT : return VIDEO_PALETTE_RGB555;
    case RGB565_FORMAT     : return VIDEO_PALETTE_RGB565;
    case RGB24_FORMAT      : return VIDEO_PALETTE_RGB24;
    case BGR24_FORMAT      : return VIDEO_PALETTE_RGB24;
    case I422i_FORMAT      : return VIDEO_PALETTE_YUYV;
    case I422P_FORMAT      : return VIDEO_PALETTE_YUV422P;
    case I411P_FORMAT      : return VIDEO_PALETTE_YUV411P;
    case I420P_FORMAT      : return VIDEO_PALETTE_YUV420P;
    default:
        //NOTIFY("getV4LIdByFCC: cannot translate 0x%x (%s), sorry\n",
        //       fmt,
        //       getNameByFCC(fmt)
        //      );
        return -1;
    }
    return -1; // to shut lint off
}


// hack to make ioctl names usable in switch statement
#undef _IOC_TYPECHECK
#define _IOC_TYPECHECK(t) (sizeof(t))

void
prnv4l1ioctl(unsigned long cmd, void *arg, int rc)
{
    switch (cmd)
    {
    case VIDIOCGCAP:
    {
        video_capability *a = (video_capability*)arg;
        NOTIFY("ioctl VIDIOCGCAP(%s,type=0x%x,chan=%d,audio=%d,"
               "size=%dx%d-%dx%d)",
               a->name,a->type,a->channels,a->audios,
               a->minwidth,a->minheight,a->maxwidth,a->maxheight
              );
        break;
    }
    case VIDIOCGCHAN:
    case VIDIOCSCHAN:
    {
        video_channel *a = (video_channel*)arg;
        NOTIFY("ioctl %s(%d,%s,flags=0x%x,type=%d,norm=%d)",
               (cmd == VIDIOCGCHAN) ? "VIDIOCGCHAN" : "VIDIOCSCHAN",
               a->channel,a->name,a->flags,a->type,a->norm
              );
        break;
    }
    case VIDIOCGTUNER:
    case VIDIOCSTUNER:
    {
        video_tuner *a = (video_tuner*)arg;
        NOTIFY("ioctl %s(%d,%s,range=%ld-%ld,flags=0x%x,"
               "mode=%d,signal=%d)",
               (cmd == VIDIOCGTUNER) ? "VIDIOCGTUNER" : "VIDIOCSTUNER",
               a->tuner,a->name,a->rangelow,a->rangehigh,
               a->flags,a->mode,a->signal
              );
        break;
    }
    case VIDIOCGPICT:
    case VIDIOCSPICT:
    {
        video_picture *a = (video_picture*)arg;
        NOTIFY("ioctl %s(params=%d/%d/%d/%d/%d,depth=%d,fmt=%d)",
               (cmd == VIDIOCGPICT) ? "VIDIOCGPICT" : "VIDIOCSPICT",
               a->brightness,a->hue,a->colour,a->contrast,a->whiteness,
               a->depth,a->palette
              );
        break;
    }
    case VIDIOCGAUDIO:
    case VIDIOCSAUDIO:
    {
        video_audio *a = (video_audio*)arg;
        NOTIFY("ioctl %s(%d,%s,flags=0x%x,vol=%d,balance=%d,"
               "bass=%d,treble=%d,mode=0x%x,step=%d)",
               (cmd == VIDIOCGAUDIO) ? "VIDIOCGAUDIO" : "VIDIOCSAUDIO",
               a->audio,a->name,a->flags,a->volume,a->balance,
               a->bass,a->treble,a->mode,a->step
              );
        break;
    }
    case VIDIOCGWIN:
    case VIDIOCSWIN:
    {
        video_window *a = (video_window*)arg;
        NOTIFY("ioctl %s(win=%dx%d+%d+%d,key=%d,flags=0x%x,clips=%d)",
               (cmd == VIDIOCGWIN) ? "VIDIOCGWIN" : "VIDIOCSWIN",
               a->width,a->height,a->x,a->y,
               a->chromakey,a->flags,a->clipcount
              );
        break;
    }
    case VIDIOCGFBUF:
    case VIDIOCSFBUF:
    {
        video_buffer *a = (video_buffer*)arg;
        NOTIFY("ioctl %s(base=%p,size=%dx%d,depth=%d,bpl=%d)",
               (cmd == VIDIOCGFBUF) ? "VIDIOCGFBUF" : "VIDIOCSFBUF",
               a->base,a->width,a->height,a->depth,a->bytesperline
              );
        break;
    }
    case VIDIOCGFREQ:
    case VIDIOCSFREQ:
    {
        unsigned long *a = (unsigned long *)arg;
        NOTIFY("ioctl %s(%.3f MHz)",
               (cmd == VIDIOCGFREQ) ? "VIDIOCGFREQ" : "VIDIOCSFREQ",
               (float)*a/16
              );
        break;
    }
    case VIDIOCCAPTURE:
    {
        int *a = (int*)arg;
        NOTIFY("ioctl VIDIOCCAPTURE(%s)", *a ? "on" : "off");
        break;
    }
    case VIDIOCGMBUF:
    {
        video_mbuf *a = (video_mbuf*)arg;
        NOTIFY("ioctl VIDIOCGMBUF(size=%d,frames=%d)", a->size, a->frames);
        break;
    }
    case VIDIOCMCAPTURE:
    {
#if DEBUG_IOCTL == 2
        video_mmap *a = (video_mmap*)arg;
        NOTIFY("ioctl VIDIOCMCAPTURE(%d,fmt=%d,size=%dx%d)",
               a->frame,a->format,a->width,a->height
              );
#endif
        break;
    }
    case VIDIOCSYNC:
    {
#if DEBUG_IOCTL == 2
        int *a = (int*)arg;
        NOTIFY("ioctl VIDIOCSYNC(%d)",*a);
#endif
        break;
    }
    default:
        NOTIFY("ioctl UNKNOWN(cmd=%lu)",cmd);
        break;
    }

    NOTIFY_ND(": %s\n",(rc >= 0) ? "ok" : strerror(errno));
}
#endif // HAVE_V4L1

/*
 *
 * V4L2 api specific ioctl debug
 *
 * */



const char *
parsev4l2ChannelType(int t)
{
    switch (t)
    {
    case V4L2_INPUT_TYPE_TUNER:  return "TV";
    case V4L2_INPUT_TYPE_CAMERA: return "CAMERA";
    }

    return "UNKNOWN";
}

const char *
parseV4L2Field(int ff)
{
    switch (ff)
    {
    case V4L2_FIELD_ANY           : return "V4L2_FIELD_ANY";
    case V4L2_FIELD_NONE          : return "V4L2_FIELD_NONE";
    case V4L2_FIELD_TOP           : return "V4L2_FIELD_TOP";
    case V4L2_FIELD_BOTTOM        : return "V4L2_FIELD_BOTTOM";
    case V4L2_FIELD_INTERLACED    : return "V4L2_FIELD_INTERLACED";
    case V4L2_FIELD_SEQ_TB        : return "V4L2_FIELD_SEQ_TB";
    case V4L2_FIELD_SEQ_BT        : return "V4L2_FIELD_SEQ_BT";
    case V4L2_FIELD_ALTERNATE     : return "V4L2_FIELD_ALTERNATE";
    case V4L2_FIELD_INTERLACED_TB : return "V4L2_FIELD_INTERLACED_TB";
    case V4L2_FIELD_INTERLACED_BT : return "V4L2_FIELD_INTERLACED_BT";
    }

    return "UNKNOWN_FIELD_TYPE";
}

const char *
parseV4L2Colorspace(int ff)
{
    switch (ff)
    {
    /* ITU-R 601 -- broadcast NTSC/PAL */
    case V4L2_COLORSPACE_SMPTE170M     : return "V4L2_COLORSPACE_SMPTE170M";

    /* 1125-Line (US) HDTV */
    case V4L2_COLORSPACE_SMPTE240M     : return "V4L2_COLORSPACE_SMPTE240M";

    /* HD and modern captures. */
    case V4L2_COLORSPACE_REC709        : return "V4L2_COLORSPACE_REC709";

    /* broken BT878 extents (601, luma range 16-253 instead of 16-235) */
    case V4L2_COLORSPACE_BT878         : return "V4L2_COLORSPACE_BT878";

    /* These should be useful.  Assume 601 extents. */
    case V4L2_COLORSPACE_470_SYSTEM_M  : return "V4L2_COLORSPACE_470_SYSTEM_M";
    case V4L2_COLORSPACE_470_SYSTEM_BG : return "V4L2_COLORSPACE_470_SYSTEM_BG";

    /* I know there will be cameras that send this.  So, this is
     * unspecified chromaticities and full 0-255 on each of the
     * Y'CbCr components
     */
    case V4L2_COLORSPACE_JPEG          : return "V4L2_COLORSPACE_JPEG";

    /* For RGB colourspaces, this is probably a good start. */
    case V4L2_COLORSPACE_SRGB          : return "V4L2_COLORSPACE_SRGB";
    }

    return "UNKNOWN_COLORSPACE";
}

const char *
parseV4L2BufType(int ff)
{
    switch (ff)
    {
    case V4L2_BUF_TYPE_VIDEO_CAPTURE        : return "V4L2_BUF_TYPE_VIDEO_CAPTURE";
    case V4L2_BUF_TYPE_VIDEO_OUTPUT         : return "V4L2_BUF_TYPE_VIDEO_OUTPUT";
    case V4L2_BUF_TYPE_VIDEO_OVERLAY        : return "V4L2_BUF_TYPE_VIDEO_OVERLAY";
    case V4L2_BUF_TYPE_VBI_CAPTURE          : return "V4L2_BUF_TYPE_VBI_CAPTURE";
    case V4L2_BUF_TYPE_VBI_OUTPUT           : return "V4L2_BUF_TYPE_VBI_OUTPUT";
    case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE   : return "V4L2_BUF_TYPE_SLICED_VBI_CAPTURE";
    case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT    : return "V4L2_BUF_TYPE_SLICED_VBI_OUTPUT";
    case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY : return "V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY";
    case V4L2_BUF_TYPE_PRIVATE              : return "V4L2_BUF_TYPE_PRIVATE";
    }

    return "UNKNOWN_BUF_TYPE";
}

#ifndef V4L2_CID_POWER_LINE_FREQUENCY
// only needed if kernel < 2.6.25
#define V4L2_CID_POWER_LINE_FREQUENCY_DISABLED 0
#define V4L2_CID_POWER_LINE_FREQUENCY_50HZ     1
#define V4L2_CID_POWER_LINE_FREQUENCY_60HZ     2
#endif
const char *
parseV4L2FlickerFreq(int ff)
{
    switch (ff)
    {
    case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED: return "NoFlicker";
    case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:     return "50Hz";
    case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:     return "60Hz";
    }

    return "UNKNOWN";
}

int
getV4L2FlickerFreq(const char *ff)
{
    if (strcasecmp("50Hz", ff) == 0)
    {
        return V4L2_CID_POWER_LINE_FREQUENCY_50HZ;
    }
    if (strcasecmp("60Hz", ff) == 0)
    {
        return V4L2_CID_POWER_LINE_FREQUENCY_60HZ;
    }
    if (strcasecmp("NoFlicker", ff) == 0)
    {
        return V4L2_CID_POWER_LINE_FREQUENCY_DISABLED;
    }

    return V4L2_CID_POWER_LINE_FREQUENCY_DISABLED;
}

u32
getFCCByV4L2Id(u32 i)
{
    switch (i)
    {
    case V4L2_PIX_FMT_RGB565  : return RGB565_FORMAT;
    case V4L2_PIX_FMT_RGB24   : return RGB24_FORMAT;
    case V4L2_PIX_FMT_BGR24   : return BGR24_FORMAT;
    case V4L2_PIX_FMT_YUYV    : return I422i_FORMAT;
    case V4L2_PIX_FMT_YUV422P : return I422P_FORMAT;
    case V4L2_PIX_FMT_YUV420  : return I420P_FORMAT;
    case V4L2_PIX_FMT_YUV411P : return I411P_FORMAT;
    }
    throw "getFCCByV4L2Id: no conversion available";
}

u32
getV4L2IdByFCC(u32 fmt)
{
    switch (fmt)
    {
    //case RGB8_332_FORMAT  : return V4L2_PIX_FMT_RGB332;
    //case RGB16_555_FORMAT : return V4L2_PIX_FMT_RGB555;
    case RGB565_FORMAT    : return V4L2_PIX_FMT_RGB565;
    case RGB24_FORMAT     : return V4L2_PIX_FMT_RGB24;
    case BGR24_FORMAT     : return V4L2_PIX_FMT_BGR24;
    case I422i_FORMAT     : return V4L2_PIX_FMT_YUYV;
    case I422P_FORMAT     : return V4L2_PIX_FMT_YUV422P;
    case I420P_FORMAT     : return V4L2_PIX_FMT_YUV420;
    case I411P_FORMAT     : return V4L2_PIX_FMT_YUV411P;
    // case JPEG_FORMAT      : return V4L2_PIX_FMT_JPEG;
    case MJPEG_FORMAT     : return V4L2_PIX_FMT_MJPEG;
    //case MPEG1_FORMAT     : return V4L2_PIX_FMT_MPEG;
    default:
        NOTIFY("getV4L2IdByFCC: cannot translate 0x%x (%s), sorry\n",
               fmt,
               vGetFormatNameById(fmt)
              );
        NOTIFY("getV4L2IdByFCC: Bailing out\n");
        exit(1);
    }
    return 0; // to shut lint off
}

void
prnv4l2ioctl(unsigned long cmd, void *arg, int rc)
{
    switch (cmd)
    {
    case VIDIOC_CROPCAP:
    {
        v4l2_cropcap *a=(v4l2_cropcap*)arg;
        NOTIFY("ioctl VIDIOC_CROPCAP(type=%d)", a->type);
        break;
    }
    case VIDIOC_ENUM_FMT:
    {
        v4l2_fmtdesc *a=(v4l2_fmtdesc *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_FMT(%d,type=%d,flags=0x%X,"
               "description=%s,pixelformat=%c%c%c%c)",
               a->index, a->type, a->flags, a->description,
               (a->pixelformat      ) & 0xFF,
               (a->pixelformat >>  8) & 0xFF,
               (a->pixelformat >> 16) & 0xFF,
               (a->pixelformat >> 24) & 0xFF
              );
        break;
    }
    case VIDIOC_ENUMINPUT:
    {
        v4l2_input *a=(v4l2_input *)arg;
        NOTIFY("ioctl VIDIOC_ENUMINPUT(%d,name=%s,type=%d,"
               "audioset=%d,tuner=%d,std=%d,status=0x%X)",
               a->index, a->name, a->type, a->audioset,
               a->tuner, a->std, a->status
              );
        break;
    }
    case VIDIOC_ENUMSTD:
    {
        v4l2_standard *a=(v4l2_standard *)arg;
        NOTIFY("ioctl VIDIOC_ENUMSTD(%d,id=0x%X,name=%s,"
               "frameperiod=%d/%d,framelines=%d)",
               a->index, a->id, a->name,
               a->frameperiod.numerator, a->frameperiod.denominator,
               a->framelines
              );
        break;
    }
    case VIDIOC_G_CTRL:
    {
        v4l2_control *a=(v4l2_control *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_G_CTRL(id=%d,value=%d)", a->id, a->value);
        break;
    }
    case VIDIOC_S_CTRL:
    {
        v4l2_control *a=(v4l2_control *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_S_CTRL(id=%d,value=%d)", a->id, a->value);
        break;
    }
    case VIDIOC_G_FMT:
    {
        v4l2_format *a=(v4l2_format *)arg;
        NOTIFY("ioctl VIDIOC_G_FMT(type=%d,width=%d,height=%d,"
               "pixelformat=%c%c%c%c,field=%d,bytesperline=%d,"
               "sizeimage=%d,colorspace=%d)",
               a->type, a->fmt.pix.width, a->fmt.pix.height,
               (a->fmt.pix.pixelformat      ) & 0xFF,
               (a->fmt.pix.pixelformat >>  8) & 0xFF,
               (a->fmt.pix.pixelformat >> 16) & 0xFF,
               (a->fmt.pix.pixelformat >> 24) & 0xFF,
               a->fmt.pix.field,
               a->fmt.pix.bytesperline,
               a->fmt.pix.sizeimage,
               a->fmt.pix.colorspace
              );
        break;
    }
    case VIDIOC_TRY_FMT:
    {
        v4l2_format *a=(v4l2_format *)arg;
        NOTIFY("ioctl VIDIOC_TRY_FMT(type=%d,width=%d,height=%d,"
               "pixelformat=%c%c%c%c,field=%d,bytesperline=%d,"
               "sizeimage=%d,colorspace=%d)",
               a->type, a->fmt.pix.width, a->fmt.pix.height,
               (a->fmt.pix.pixelformat      ) & 0xFF,
               (a->fmt.pix.pixelformat >>  8) & 0xFF,
               (a->fmt.pix.pixelformat >> 16) & 0xFF,
               (a->fmt.pix.pixelformat >> 24) & 0xFF,
               a->fmt.pix.field,
               a->fmt.pix.bytesperline,
               a->fmt.pix.sizeimage,
               a->fmt.pix.colorspace
              );
        break;
    }
    case VIDIOC_S_FMT:
    {
        v4l2_format *a=(v4l2_format *)arg;
        NOTIFY("ioctl VIDIOC_S_FMT(type=%d,width=%d,height=%d,"
               "pixelformat=%c%c%c%c,field=%d,bytesperline=%d,"
               "sizeimage=%d,colorspace=%d)",
               a->type, a->fmt.pix.width, a->fmt.pix.height,
               (a->fmt.pix.pixelformat      ) & 0xFF,
               (a->fmt.pix.pixelformat >>  8) & 0xFF,
               (a->fmt.pix.pixelformat >> 16) & 0xFF,
               (a->fmt.pix.pixelformat >> 24) & 0xFF,
               a->fmt.pix.field,
               a->fmt.pix.bytesperline,
               a->fmt.pix.sizeimage,
               a->fmt.pix.colorspace
              );
        break;
    }
    case VIDIOC_G_INPUT:
    {
        int *a=(int *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_G_INPUT(input=%d)", *a);
        break;
    }
    case VIDIOC_S_INPUT:
    {
        int *a=(int *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_S_INPUT(input=%d)", *a);
        break;
    }
    case VIDIOC_G_STD:
    {
        v4l2_std_id *a=(v4l2_std_id *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_G_STD(standard=0x%X)", *a);
        break;
    }
    case VIDIOC_S_STD:
    {
        v4l2_std_id *a=(v4l2_std_id *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_S_STD(standard=0x%X)", *a);
        break;
    }
    case VIDIOC_DQBUF:
    {
#if DEBUG_IOCTL == 2
        v4l2_buffer *a = (v4l2_buffer *)arg;
        NOTIFY("ioctl VIDIOC_DQBUF(%d,type=%d,bytesused=%X,"
               "flags=0x%X,field=%d,timestamp=%d,"
               "timecode=[%d:0xX %d.%d.%d@%d],sequence=%d,"
               "memory=%d,offset=0x%X,length=0x%X,input=%d)",
               a->index, a->type, a->bytesused, a->flags, a->field,
               a->timestamp.tv_sec*1000000 + a->timestamp.tv_usec,
               a->timecode.type, a->timecode.flags,
               a->timecode.hours, a->timecode.minutes, a->timecode.seconds,
               a->timecode.frames,
               a->sequence, a->memory, a->m.offset, a->length, a->input
              );
#endif
        break;
    }
    case VIDIOC_QBUF:
    {
#if DEBUG_IOCTL == 2
        v4l2_buffer *a = (v4l2_buffer *)arg;
        NOTIFY("ioctl VIDIOC_QBUF(%d,type=%d,bytesused=%X,"
               "flags=0x%X,field=%d,timestamp=%d,"
               "timecode=[%d:0xX %d.%d.%d@%d],sequence=%d,"
               "memory=%d,offset=0x%X,length=0x%X,input=%d)",
               a->index, a->type, a->bytesused, a->flags, a->field,
               a->timestamp.tv_sec*1000000 + a->timestamp.tv_usec,
               a->timecode.type, a->timecode.flags,
               a->timecode.hours, a->timecode.minutes, a->timecode.seconds,
               a->timecode.frames,
               a->sequence, a->memory, a->m.offset, a->length, a->input
              );
#endif
        break;
    }
    case VIDIOC_QUERYBUF:
    {
        v4l2_buffer *a = (v4l2_buffer *)arg;
        NOTIFY("ioctl VIDIOC_QUERYBUF(%d,type=%d,bytesused=%X,"
               "flags=0x%X,field=%d,timestamp=%d,"
               "timecode=[%d:0xX %d.%d.%d@%d],sequence=%d,"
               "memory=%d,offset=0x%X,length=0x%X,input=%d)",
               a->index, a->type, a->bytesused, a->flags, a->field,
               a->timestamp.tv_sec*1000000 + a->timestamp.tv_usec,
               a->timecode.type, a->timecode.flags,
               a->timecode.hours, a->timecode.minutes, a->timecode.seconds,
               a->timecode.frames,
               a->sequence, a->memory, a->m.offset, a->length, a->input
              );
        break;
    }
    case VIDIOC_QUERYCAP:
    {
        v4l2_capability *a=(v4l2_capability *)arg;
        NOTIFY("ioctl VIDIOC_QUERYCAP(driver=%s,card=%s,"
               "bus_info=%s,version=%d.%d.%d,capabilities=0x%X)",
               a->driver, a->card, a->bus_info,
               (a->version >> 16) & 0xFF,
               (a->version >>  8) & 0xFF,
               (a->version      ) & 0xFF,
               a->capabilities
              );
        break;
    }
    case VIDIOC_QUERYCTRL:
    {
        v4l2_queryctrl *a=(v4l2_queryctrl *)arg;
        NOTIFY("ioctl VIDIOC_QUERYCTRL(%d,type=%d,name=%s)"
               "minimum=%d,maximum=%d,step=%d,"
               "default_value=%d,flags=0x%X)",
               a->id, a->type, a->name,
               a->minimum, a->maximum, a->step,
               a->default_value, a->flags
              );
        break;
    }
    case VIDIOC_QUERYSTD:
    {
        v4l2_std_id *a=(v4l2_std_id *)arg;
        NOTIFY("ioctl VIDIOC_QUERYSTD(standard=0x%X)", *a);
        break;
    }
    case VIDIOC_REQBUFS:
    {
        v4l2_requestbuffers *a=(v4l2_requestbuffers *)arg;
        NOTIFY("ioctl VIDIOC_REQBUFS(count=%d,type=%d,memory=%d)",
                a->count, a->type, a->memory
              );
        break;
    }
    case VIDIOC_STREAMON:
    {
        int *a = (int *)arg;
        NOTIFY("ioctl VIDIOC_STREAMON(mode=%d)",*a);
        break;
    }
    case VIDIOC_STREAMOFF:
    {
        int *a = (int *)arg;
        NOTIFY("ioctl VIDIOC_STREAMOFF(mode=%d)",*a);
        break;
    }
    case VIDIOC_ENUM_FRAMESIZES:
    {
        v4l2_frmsizeenum *a = (v4l2_frmsizeenum *)arg;
        NOTIFY("ioctl VIDIOC_ENUM_FRAMESIZES(index=%d, pixelformat=%c%c%c%c)",
               a->index,
               (a->pixel_format      ) & 0xFF,
               (a->pixel_format >>  8) & 0xFF,
               (a->pixel_format >> 16) & 0xFF,
               (a->pixel_format >> 24) & 0xFF
              );
        break;
    }
    default:
        NOTIFY("ioctl UNKNOWN(cmd=%lu)",cmd);
        break;
    }

    NOTIFY_ND(": %s\n",(rc == 0) ? "ok" : strerror(errno));
}

