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
// $Id: v4l2DS.cc 10333 2007-06-07 17:11:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <icf2/notify.hh>

#include "v4l2DS.hh"
#include "v4luDioctl.hh"

#ifdef HAVE_LIBV4L
#include <libv4l2.h>
#else
#define v4l2_fd_open(fd, flags) (fd)
#define v4l2_open open
#define v4l2_close close
#define v4l2_dup dup
#define v4l2_ioctl ioctl
#define v4l2_read read
#define v4l2_mmap mmap
#define v4l2_munmap munmap
#endif

#if DEBUG_IOCTL
int
Dioctlv4l2 (int fd, unsigned long cmd, void *arg)
{
    int rc = v4l2_ioctl(fd, cmd, arg);
    prnv4l2ioctl(cmd, arg, rc);
    return rc;
}
#define Dioctl Dioctlv4l2
#else
#define Dioctl v4l2_ioctl
#endif


v4l2DS_t::v4l2DS_t(char const *path)
: VideoHandler_t(path),
  deviceBuffer(NULL),
  deviceBufferLen(0),
  buff(NULL),
  selectedChannel(-1),
  numframesizes(-1),  // no support for frame sizes
  frame(NULL)
{
    int i= 0;

    memset (&vmb, 0, sizeof (v4l2_requestbuffers));

    numchannels = 0;
    numstandards = 0;

    method = 0;

// libv4l fails to open AverMedia AVerTV Hybrid Volar HX
// in my opinion, the driver does not initialize properly
// but if we FIRST open the device and the initialice with
// v4l2_fd_open, things look ok, so... waiting for next error
    deviceHandle= open(getPath(), O_RDWR, 0000);
    if (deviceHandle < 0)
    {
        NOTIFY("---cannot open '%s': %s\n", getPath(), strerror(errno));
        return; // builtOK is false
    }

#ifdef HAVE_LIBV4L
    if (v4l2_fd_open(deviceHandle, 0) == -1)
    {
        NOTIFY("---v4l2_fd_open failed due to: %s\n", strerror(errno));
        close(deviceHandle);
        return; // builtOK is false
    }
#endif

    NOTIFY("+++Open device: %s\n", getPath());

    //
    // test v4l2 interface
    //
    memset (&vcap, 0, sizeof (v4l2_capability));
    if (Dioctl(deviceHandle, VIDIOC_QUERYCAP, &vcap) == -1)
    {
        NOTIFY("---it seems Video4Linux2 is not supported: %s\n",
               strerror(errno)
              );
        return; // builtOK is false
    }
    else
    {
        NOTIFY("+++v4l2 supported\n");
        NOTIFY("+++\n");
    }

    NOTIFY("+++Description of device\n");
    NOTIFY("+++=====================\n");
    NOTIFY("+++device: %s\n", vcap.card);
    NOTIFY("+++driver: %s\n", vcap.driver);
    NOTIFY("+++version: %d.%d.%d\n",
           (vcap.version >> 16) & 0xFF,
           (vcap.version >>  8) & 0xFF,
           (vcap.version      ) & 0xFF
          );
    NOTIFY("+++bus: %s\n", vcap.bus_info);
    NOTIFY("+++supports:\n");
    if (vcap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        NOTIFY("+++...V4L2_CAP_VIDEO_CAPTURE\n");
    }
    if (vcap.capabilities & V4L2_CAP_VBI_CAPTURE)
    {
        NOTIFY("+++...V4L2_CAP_VBI_CAPTURE\n");
    }
    if (vcap.capabilities & V4L2_CAP_RDS_CAPTURE)
    {
        NOTIFY("+++...V4L2_CAP_RDS_CAPTURE\n");
    }
    if (vcap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
    {
        NOTIFY("+++...V4L2_CAP_VIDEO_OUTPUT\n");
    }
    if (vcap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
    {
        NOTIFY("+++...V4L2_CAP_VIDEO_OVERLAY\n");
    }
    if (vcap.capabilities & V4L2_CAP_VBI_OUTPUT)
    {
        NOTIFY("+++...V4L2_CAP_VBI_OUTPUT\n");
    }
    if (vcap.capabilities & V4L2_CAP_TUNER)
    {
        NOTIFY("+++...V4L2_CAP_TUNER\n");
    }
    if (vcap.capabilities & V4L2_CAP_AUDIO)
    {
        NOTIFY("+++...V4L2_CAP_AUDIO\n");
    }
    if (vcap.capabilities & V4L2_CAP_READWRITE)
    {
        NOTIFY("+++...V4L2_CAP_READWRITE\n");
    }
    if (vcap.capabilities & V4L2_CAP_ASYNCIO)
    {
        NOTIFY("+++...V4L2_CAP_ASYNCIO\n");
    }
    if (vcap.capabilities & V4L2_CAP_STREAMING)
    {
        //method= V4L2_MEMORY_USERPTR;
        method= V4L2_MEMORY_MMAP;
        NOTIFY("+++...V4L2_CAP_STREAMING\n");
    }

    inputPortsList= "(";
    for (i= 0; i < 256; i++)
    {
        memset (&vchann[i], 0, sizeof (v4l2_input));
        vchann[i].index= i;
        if (Dioctl(deviceHandle, VIDIOC_ENUMINPUT, vchann+i) == -1)
        {
            break;
        }
        NOTIFY("+++..description for channel %d\n", i);
        NOTIFY("+++....identifier: %d\n", vchann[i].index);
        NOTIFY("+++....name\t: %s\n", vchann[i].name);
        NOTIFY("+++....type\t: %s\n", parsev4l2ChannelType(vchann[i].type));
        NOTIFY("+++....tuners\t: %d\n", vchann[i].tuner);
        NOTIFY("+++....flags\t: 0x%x\n", vchann[i].status);
        NOTIFY("+++....audioset: %d\n", vchann[i].audioset);

        inputPortsList = inputPortsList +
            ((i > 0) ? "," : "") +
            (char*)vchann[i].name;
    }
    inputPortsList += ")";
    numchannels= i;
    selectedChannel= 0;
    if (i == 256) NOTIFY("WARNING: REACHED LIMIT FOR NUMBER OF CHANNELS!\n");

    //
    // TV standard
    //
    NOTIFY("+++\n");
    NOTIFY("+++..List of supported standards:\n");
    for (i= 0; ; i++)
    {
        memset (&standard[i], 0, sizeof (v4l2_standard));
        standard[i].index= i;
        if (Dioctl(deviceHandle, VIDIOC_ENUMSTD, &standard[i]) == -1)
        {
            break;
        }

        if (standard[i].id == 0)
        {
            // it is no legal, but some webcams set it
            NOTIFY("----WARNING: illegal standard (0x0), skipping detection\n");
            break;
        }

        NOTIFY("+++....index: %d\n", standard[i].index);
        NOTIFY("+++....id: %d\n", standard[i].id);
        NOTIFY("+++....name: %s\n", standard[i].name);
        NOTIFY("+++....frames: %d:%d\n",
               standard[i].frameperiod.numerator,
               standard[i].frameperiod.denominator
              );
        NOTIFY("+++....framelines: %d\n", standard[i].framelines);

        standardList = standardList +
            ((i > 0) ? "," : "") +
            (char*)standard[i].name;
    }
    numstandards= i;

    if (numstandards == 0)
    {
        NOTIFY("---....NO video standard in use\n");
    }
    else
    {
        v4l2_std_id std;

        NOTIFY("+++..Currently using: ");
        if (Dioctl(deviceHandle, VIDIOC_G_STD, &std) == -1)
        {
            NOTIFY_ND("WARNING: cannot get current standard: %d %s\n",
                      errno,
                      strerror(errno)
                     );
        }
        for (i= 0; i < numstandards; i++)
        {
            if (standard[i].id & std)
            {
                NOTIFY_ND("%s [%d]\n", standard[i].name, i);
                break;
            }
        }
        if (i == numstandards)
        {
            NOTIFY_ND("WARNING: cannot guess standard in use\n");
        }
    }
    NOTIFY("+++\n");

#if 0
    v4l2_cropcap cropcap;
    memset (&cropcap, 0, sizeof (v4l2_cropcap));

    cropcap.type= V4L2_BUF_TYPE_VIDEO_CAPTURE;
    NOTIFY("+++..Cropping Capabilities:\n");
    if (ioctl (deviceHandle, VIDIOC_CROPCAP, &cropcap) == -1)
    {
        NOTIFY("---......Cannot do Cropping\n");
    }
    else
    {
        NOTIFY("+++......Bounds:\n");
        NOTIFY("+++.........Up: %d\n", cropcap.bounds.top);
        NOTIFY("+++.........Left: %d\n", cropcap.bounds.left);
        NOTIFY("+++.........Width: %d\n", cropcap.bounds.width);
        NOTIFY("+++.........Height: %d\n", cropcap.bounds.height);
        NOTIFY("+++......DefaultRect:\n");
        NOTIFY("+++.........Up: %d\n", cropcap.defrect.top);
        NOTIFY("+++.........Left: %d\n", cropcap.defrect.left);
        NOTIFY("+++.........Width: %d\n", cropcap.defrect.width);
        NOTIFY("+++.........Height: %d\n", cropcap.defrect.height);
        NOTIFY("+++......Pix Rel: %d/%d\n",
               cropcap.pixelaspect.numerator,
               cropcap.pixelaspect.denominator
              );
    }
    NOTIFY("+++\n");
#endif

    v4l2_queryctrl qcontrols;
    memset (&qcontrols, 0, sizeof (qcontrols));

    NOTIFY("+++..List of controls:\n");
    qcontrols.id= V4L2_CID_BASE;
    while (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &qcontrols) != -1)
    {
        if (qcontrols.flags & V4L2_CTRL_FLAG_DISABLED)
        {
            qcontrols.id++;
            continue;
        }
        if (qcontrols.step == 0)
        {
            break;
        }

        v4l2_control qcntrl;
        memset (&qcntrl, 0, sizeof (qcntrl));

        qcntrl.id= qcontrols.id;
        if (Dioctl(deviceHandle, VIDIOC_G_CTRL, &qcntrl) == -1)
        {
            NOTIFY("Control error %d %s\n", errno, strerror(errno));
            break;
        }
        qcntrl.value= (qcontrols.maximum + qcontrols.minimum)/2;

        if (Dioctl(deviceHandle, VIDIOC_S_CTRL, &qcntrl) == -1)
        {
            NOTIFY("Control error %d %s\n", errno, strerror(errno));
        }

        NOTIFY("+++...%s:\n", qcontrols.name);
        NOTIFY("+++....id: %d:\n", qcontrols.id);
        NOTIFY("+++....type: %d:\n", qcontrols.type);
        NOTIFY("+++....name: %s:\n", qcontrols.name);
        NOTIFY("+++....min: %d:\n", qcontrols.minimum);
        NOTIFY("+++....max: %d:\n", qcontrols.maximum);
        NOTIFY("+++....step: %d:\n", qcontrols.step);
        NOTIFY("+++....default: %d:\n", qcontrols.default_value);
        NOTIFY("+++....current: %d:\n",qcntrl.value);
        NOTIFY("+++....flags: 0x%X:\n", qcontrols.flags);

        qcontrols.id++;
    }
    NOTIFY("+++\n");

    //
    // print grab formats
    //
    printGrabFormats();
    NOTIFY("+++\n");

    //
    // get default configuration
    //
    memset (&format, 0, sizeof (v4l2_format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (Dioctl(deviceHandle, VIDIOC_G_FMT, &format) == -1)
    {
        NOTIFY("---Cannot get device image format.\n");
        NOTIFY("---Bailing out\n");
        return; // builtOK is false
    }

    NOTIFY("+++..Default capture values:\n");
    NOTIFY("+++...type: %s\n", parseV4L2BufType(format.type));
    NOTIFY("+++...WxH: %dx%d, bytesperline: %d, sizeimage: %d\n",
           format.fmt.pix.width,
           format.fmt.pix.height,
           format.fmt.pix.bytesperline,
           format.fmt.pix.sizeimage
          );
    NOTIFY("+++...field: %s\n", parseV4L2Field(format.fmt.pix.field));
    NOTIFY("+++...pixelformat: %c%c%c%c, colorspace: %s\n",
           (format.fmt.pix.pixelformat      ) & 0xFF,
           (format.fmt.pix.pixelformat >>  8) & 0xFF,
           (format.fmt.pix.pixelformat >> 16) & 0xFF,
           (format.fmt.pix.pixelformat >> 24) & 0xFF,
           parseV4L2Colorspace(format.fmt.pix.colorspace)
          );

    //
    // grab sizes
    //
    NOTIFY("+++..Grab sizes for default capture: ");
    if (pollframesizes(format.fmt.pix.pixelformat))
    {
        printframesizes();
    }
    else
    {
        numframesizes= -1;

        NOTIFY_ND("unknown, trying to guess min/max grab size\n");
        minw= minh= 0;
        maxw= maxh= 2000;

        setGrabSize(1, 1);
        getGrabSize(&minw, &minh);
        setGrabSize(1024, 768);
        getGrabSize(&maxw, &maxh);
    }

    NOTIFY("+++...MIN/MAX sizes: (%dx%d)..(%dx%d)\n", minw, minh, maxw, maxh);

    setGrabSize(320, 240);

    setGrabFormat(I420P_FORMAT); // required, if unavailable, conversion is set

    setDeviceFree();

    builtOK= true;
}

v4l2DS_t::~v4l2DS_t(void)
{
    stopGrabbing();

    if (deviceHandle >= 0)
    {
        v4l2_close(deviceHandle);
    }
}

void
v4l2DS_t::printGrabFormats(void)
{
    NOTIFY("+++..List of grab formats for '%s'\n", getPath());

    for (int index= 0; ; index++)
    {
        struct v4l2_fmtdesc fmtdesc;
        memset (&fmtdesc, 0, sizeof (fmtdesc));

        fmtdesc.index= index;
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (Dioctl(deviceHandle, VIDIOC_ENUM_FMT, &fmtdesc) == -1)
        {
            break;
        }

        NOTIFY("+++...Index %d de VIDEO_CAPTURE\n", index);
        NOTIFY("+++......type: %s\n", parseV4L2BufType(fmtdesc.type));
        NOTIFY("+++......flags: 0x%X (%s)\n",
               fmtdesc.flags,
               fmtdesc.flags & V4L2_FMT_FLAG_COMPRESSED
                   ? "compressed" : "uncompressed"
              );
        NOTIFY("+++......description: %s\n", fmtdesc.description);
        NOTIFY("+++......format: [0x%x] %c%c%c%c\n",
               fmtdesc.pixelformat,
               (fmtdesc.pixelformat      ) & 0xFF,
               (fmtdesc.pixelformat >>  8) & 0xFF,
               (fmtdesc.pixelformat >> 16) & 0xFF,
               (fmtdesc.pixelformat >> 24) & 0xFF
              );

        if (pollframesizes(fmtdesc.pixelformat))
        {
            NOTIFY("+++......grab sizes: ");
            printframesizes();
        }
        else
        {
            NOTIFY("---......grab sizes: unknown\n");
        }
    }
}

void
v4l2DS_t::stopGrabbing(void)
{
    if ( ! isStreamingNow)
    {
        return;
    }
    isStreamingNow= false;

    switch (method)
    {
    case V4L2_MEMORY_USERPTR:
        if (Dioctl(deviceHandle, VIDIOC_STREAMOFF, &(vmb.type)) == -1)
        {
            NOTIFY("v4l2DS_t::stopGrabbing: Error streaming Off %d %s\n",
                   errno,
                   strerror(errno)
                  );
        }
        uptr_close();
        break;
    case V4L2_MEMORY_MMAP:
        if (Dioctl(deviceHandle, VIDIOC_STREAMOFF, &(vmb.type)) == -1)
        {
            NOTIFY("v4l2DS_t::stopGrabbing: Error streaming Off %d %s\n",
                   errno,
                   strerror(errno)
                  );
        }
        mmap_close();
        break;
    default:
        read_close();
    }

    assert(frame != NULL);
    free(frame);
    frame= NULL;
}

void
v4l2DS_t::startGrabbing(void)
{
    if (isStreamingNow)
    {
        return;
    }
    switch (method)
    {
    case V4L2_MEMORY_USERPTR:
        uptr_init();
        break;
    case V4L2_MEMORY_MMAP:
        mmap_init();
        break;
    default:
        read_init();
    }

    assert (frame == NULL);

    frame = (unsigned char *)malloc(format.fmt.pix.sizeimage);

    isStreamingNow= true;
}

void
v4l2DS_t::read_init(void)
{
    deviceBufferLen= format.fmt.pix.sizeimage;
}

void
v4l2DS_t::mmap_init(void)
{
    vmb.count = 2;
    vmb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vmb.memory = V4L2_MEMORY_MMAP;
    if (Dioctl(deviceHandle, VIDIOC_REQBUFS, &vmb) == -1)
    {
        NOTIFY("---Error during buffer request: %s\n", strerror(errno));
        abort();
    }

    buff = (bufptr *)calloc (vmb.count, sizeof (struct bufptr));
    if (buff == NULL)
    {
        NOTIFY("---Not enought memory for Buffers\n");
        abort();
    }

    for (unsigned i = 0; i < vmb.count; i++)
    {
        struct v4l2_buffer buffers;
        memset (&buffers, 0, sizeof (struct v4l2_buffer));
        buffers.index = i;
        buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffers.memory = V4L2_MEMORY_MMAP;
        if (Dioctl(deviceHandle, VIDIOC_QUERYBUF, &buffers) == -1)
        {
            NOTIFY("mmap_init: Error querying buffer %d %s, bailing out\n",
                   errno,
                   strerror(errno)
                  );
            abort();
        }

        deviceBufferLen = buffers.length;
        buff[i].length = buffers.length;
        buff[i].start = (unsigned char*)v4l2_mmap(NULL,
                                                  buffers.length,
                                                  PROT_READ | PROT_WRITE,
                                                  MAP_SHARED,
                                                  deviceHandle,
                                                  buffers.m.offset
                                                 );

        if (buff[i].start == MAP_FAILED)
        {
            NOTIFY("---Memory Mapping error, bailing out\n");
            abort();
        }
        if (Dioctl(deviceHandle, VIDIOC_QBUF, &buffers) == -1)
        {
            NOTIFY("mmap_init: Error queuing buffer %d %s, bailing out\n",
                   errno,
                   strerror(errno)
                  );
            abort();
        }
    }
    if (Dioctl(deviceHandle, VIDIOC_STREAMON, &(vmb.type)) == -1)
    {
        NOTIFY("mmap_init: VIDEO_STREAMON FAILED!!! %d %s, bailing out\n",
               errno,
               strerror(errno)
              );
        abort();
    }
}

void
v4l2DS_t::uptr_init()
{
    vmb.count = 2;
    vmb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vmb.memory = V4L2_MEMORY_USERPTR;
    if (Dioctl(deviceHandle, VIDIOC_REQBUFS, &vmb) == -1)
    {
        method= V4L2_MEMORY_MMAP;
        mmap_init();
        return;
    }

    buff = (bufptr *)calloc (vmb.count, sizeof (*buff));
    deviceBufferLen= format.fmt.pix.sizeimage;

    if (buff == NULL)
    {
        NOTIFY("---Not enough memory for buffers, bailing out\n");
        abort();
    }

    for (unsigned i = 0; i < vmb.count; i++)
    {
        buff[i].length = deviceBufferLen;
        buff[i].start = (unsigned char *) malloc(deviceBufferLen);
        if (buff[i].start == NULL)
        {
            NOTIFY("---Malloc error: not enough memory, bailing out\n");
            abort();
        }
        memset(buff[i].start, 0, deviceBufferLen);
    }

    for (unsigned i = 0; i < vmb.count; i++)
    {
        struct v4l2_buffer buffers;
        memset (&buffers, 0, sizeof (buffers));

        buffers.index = i;
        buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffers.memory = V4L2_MEMORY_USERPTR;
        buffers.length = buff[i].length;
        buffers.m.userptr = (long unsigned int) buff[i].start;

        if (Dioctl(deviceHandle, VIDIOC_QBUF, &buffers) == -1)
        {
            NOTIFY("v4l2DS_t::uptr_init: VIDIOC_QBUF failed %d %s, bailing out\n",
                   errno, strerror(errno)
                  );
        abort();
        }
    }
    if (Dioctl(deviceHandle, VIDIOC_STREAMON, &(vmb.type)) == -1)
    {
        NOTIFY("uptr_init: error VIDEO_STREAMON FAILED!!! %d %s, bailing out\n",
               errno,
               strerror(errno)
              );
        abort();
    }
}

unsigned char *
v4l2DS_t::read_capture(u32 *timestamp, u32 *bufLen)
{
    int n= v4l2_read(deviceHandle, frame, format.fmt.pix.sizeimage);

    if (n < 0)
    {
        builtOK= false;
        return NULL;
    }

    struct timeval currentTime;
    gettimeofday (&currentTime, NULL);
    *timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

    *bufLen = format.fmt.pix.sizeimage;

    return frame;
}

unsigned char *
v4l2DS_t::mmap_capture(u32 *timestamp, u32 *bufLen)
{
    v4l2_buffer currentBuf;

    memset(&currentBuf, 0, sizeof(struct v4l2_buffer));
    currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    currentBuf.memory = V4L2_MEMORY_MMAP;

    if (Dioctl(deviceHandle, VIDIOC_DQBUF, &currentBuf) == -1)
    {
        NOTIFY("v4l2DS_t::mmap_capture: VIDIOC_DQBUF failed %d %s\n",
               errno, strerror(errno)
              );
        if (errno == ENODEV)
        {
            builtOK= false;
        }
    }

    if (builtOK)
    {
        *timestamp= (currentBuf.timestamp.tv_sec*1000000)
                    + currentBuf.timestamp.tv_usec;

        *bufLen= currentBuf.bytesused;

        memcpy(frame, buff[currentBuf.index].start, currentBuf.bytesused);
    }
    else // there was an error, frame invalid, try to continue
    {
        *bufLen= 0;
    }

    if (Dioctl(deviceHandle, VIDIOC_QBUF, &currentBuf) == -1)
    {
        NOTIFY("v4l2DS_t::mmap_capture: VIDIOC_QBUF failed %d %s\n",
               errno, strerror(errno)
              );
        if (errno == ENODEV)
        {
            builtOK= false;
        }
    }

    return builtOK ? frame : NULL;
}

unsigned char *
v4l2DS_t::uptr_capture(u32 *timestamp, u32 *bufLen)
{
    v4l2_buffer currentBuf;

    memset(&currentBuf, 0, sizeof(currentBuf));
    currentBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    currentBuf.memory = V4L2_MEMORY_USERPTR;

    if (Dioctl(deviceHandle, VIDIOC_DQBUF, &currentBuf) == -1)
    {
        NOTIFY("v4l2DS_t::uptr_capture: VIDIOC_DQBUF failed %d %s\n",
               errno, strerror(errno)
              );
        if (errno == ENODEV)
        {
            builtOK= false;
        }
    }
#if 0
NOTIFY("buffer:\n");
NOTIFY("   index: %d\n", currentBuf.index);
NOTIFY("   type: %d\n",parseV4L2BufType(currentBuf.type));
NOTIFY("   bytesused: %d\n", currentBuf.bytesused);
NOTIFY("   flags: 0x%X\n", currentBuf.flags);
NOTIFY("   field: 0x%X\n", currentBuf.field);
NOTIFY("   sequence: %d\n", currentBuf.sequence);
NOTIFY("   memory: 0x%X\n", currentBuf.memory);
NOTIFY("   offset: 0x%X\n", currentBuf.m.offset);
NOTIFY("   userptr: 0x%X\n", currentBuf.m.userptr);
NOTIFY("   length: %d\n", currentBuf.length);
NOTIFY("   input: %d\n", currentBuf.input);
#endif

    if (builtOK)
    {
        *timestamp= (currentBuf.timestamp.tv_sec*1000000)
                    + currentBuf.timestamp.tv_usec;

        *bufLen= currentBuf.bytesused;

        memcpy(frame, (const void *)currentBuf.m.userptr, currentBuf.bytesused);
    }
    else // there was an error, frame invalid, try to continue
    {
        *bufLen= 0;
    }

    if (Dioctl(deviceHandle, VIDIOC_QBUF, &currentBuf) == -1)
    {
        NOTIFY("v4l2DS_t::uptr_capture: VIDIOC_QBUF failed %d %s\n",
               errno, strerror(errno)
              );
        if (errno == ENODEV)
        {
            builtOK= false;
        }
    }

    return builtOK ? frame : NULL;
}

u8 *
v4l2DS_t::capture(u32 *timestamp, u32 *buffLen, unsigned *w, unsigned *h)
{
    u8 *theBuffer= NULL;
    switch (method)
    {
    case V4L2_MEMORY_MMAP:
        theBuffer= mmap_capture(timestamp, buffLen);
        break;
    case V4L2_MEMORY_USERPTR:
        theBuffer= uptr_capture(timestamp, buffLen);
        break;
    default:
        theBuffer= read_capture(timestamp, buffLen);
        break;
    }

    return theBuffer;
}

void
v4l2DS_t::read_close(void)
{
}

void
v4l2DS_t::mmap_close(void)
{
    assert(buff != NULL);

    //
    // unmap buffers
    //
    for (unsigned i = 0; i < vmb.count; ++i)
    {
        if (v4l2_munmap (buff[i].start, buff[i].length) == -1)
        {
            NOTIFY("v4l2DS_t::mmap_close: munmap failed %d %s\n",
                   errno, strerror(errno)
                  );
        }
    }

    //
    // unrequest buffers (not all drivers support it; those drivers
    // that not support unrequest buffers, usually do not need it)
    //
    struct v4l2_requestbuffers req;

    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (Dioctl(deviceHandle, VIDIOC_REQBUFS, &req) == -1)
    {
        NOTIFY("v4l2DS_t::mmap_close: unrequest buffers failed: %s\n",
               strerror(errno)
              );
        NOTIFY("v4l2DS_t::mmap_close: not mandatory, ignoring\n");
    }

    free(buff);
    buff= NULL;
}

void
v4l2DS_t::uptr_close(void)
{
    assert(buff != NULL);

    for (unsigned i = 0; i < vmb.count; ++i)
    {
        free (buff[i].start);
    }

    //
    // unrequest buffers (not all drivers support it; those drivers
    // that not support unrequest buffers, usually do not need it)
    // I AM NOT SURE IF THIS IS NEEDED IN USERPTR MODE!!
    struct v4l2_requestbuffers req;

    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;
    if (Dioctl(deviceHandle, VIDIOC_REQBUFS, &req) == -1)
    {
        NOTIFY("v4l2DS_t::uptr_close: unrequest buffers failed: %s\n",
               strerror(errno)
              );
        NOTIFY("v4l2DS_t::uptr_close: not mandatory, ignoring\n");
    }

    free(buff);
    buff= NULL;
}

bool
v4l2DS_t::setGrabFormat(u32 fmt)
{
    bool retVal= true;
    bool wasRunning= isStreamingNow;
    u32 v4l2Fmt;

    NOTIFY("v4l2DS_t::setGrabFormat: trying to set format %s\n",
           vGetFormatNameById(fmt)
          );

#if 0
if ( (strcmp((char*)vcap.driver, "saa7134") == 0) && (fmt == I420P_FORMAT) )
{
    NOTIFY("WARNING: I420P_FORMAT not included for saa7134\n");
    return false;
}
#endif
#if 1
if ( (strcmp((char*)vcap.driver, "Vision Driver") == 0) && (fmt != RGB565_FORMAT) )
{
    NOTIFY("WARNING: grab format set to RGB565 for Vision Driver\n");
    fmt= RGB565_FORMAT;
}
#endif

    try
    {
        v4l2Fmt= getV4L2IdByFCC(fmt);
    }
    catch (char const *e)
    {
        NOTIFY("v4l2DS_t::setGrabFormat: cannot dealt with format\n");
        return false;
    }

    NOTIFY("v4l2DS_t::setGrabFormat: V4L2 ID=[0x%x] %c%c%c%c\n",
           v4l2Fmt,
           (v4l2Fmt      ) & 0xFF,
           (v4l2Fmt >>  8) & 0xFF,
           (v4l2Fmt >> 16) & 0xFF,
           (v4l2Fmt >> 24) & 0xFF
          );

    v4l2_format tryFormat;
    memset (&tryFormat, 0, sizeof (v4l2_format));
    tryFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (Dioctl(deviceHandle, VIDIOC_G_FMT, &tryFormat) == -1)
    {
        NOTIFY("v4l2DS_t:setGrabFormat: "
               "VIDIOC_G_FMT failed: error %d %s\n",
               errno, strerror(errno));
        NOTIFY("v4l2DS_t:setGrabFormat: trying to continue anyway\n");
    }
    else
    {
        NOTIFY("setGrabFormat: default capture params:\n"
               "\ttype=%s,width=%d,height=%d,\n"
               "\tpixelformat=%c%c%c%c,field=%s,bytesperline=%d,\n"
               "\tsizeimage=%d,colorspace=%s\n",
               parseV4L2BufType(tryFormat.type),
               tryFormat.fmt.pix.width,
               tryFormat.fmt.pix.height,
               (tryFormat.fmt.pix.pixelformat      ) & 0xFF,
               (tryFormat.fmt.pix.pixelformat >>  8) & 0xFF,
               (tryFormat.fmt.pix.pixelformat >> 16) & 0xFF,
               (tryFormat.fmt.pix.pixelformat >> 24) & 0xFF,
               parseV4L2Field(tryFormat.fmt.pix.field),
               tryFormat.fmt.pix.bytesperline,
               tryFormat.fmt.pix.sizeimage,
               parseV4L2Colorspace(tryFormat.fmt.pix.colorspace)
              );
    }

    tryFormat.fmt.pix.pixelformat= v4l2Fmt;

    // TO DO: ask for both frames and merge them
    tryFormat.fmt.pix.field = V4L2_FIELD_INTERLACED;
    tryFormat.fmt.pix.field = V4L2_FIELD_ANY;

    if (Dioctl(deviceHandle, VIDIOC_TRY_FMT, &tryFormat) == -1)
    {
        NOTIFY("v4l2DS_t:setGrabFormat: "
               "VIDIOC_TRY_FMT failed: error %d %s\n",
               errno, strerror(errno));
        NOTIFY("v4l2DS_t:setGrabFormat: trying to continue anyway\n");
    }
    else
    {
        NOTIFY("setGrabFormat: "
               "VIDIOC_TRY_FMT accepted params:\n"
               "\ttype=%s,width=%d,height=%d,\n"
               "\tpixelformat=%c%c%c%c,field=%s,bytesperline=%d,\n"
               "\tsizeimage=%d,colorspace=%s\n",
               parseV4L2BufType(tryFormat.type),
               tryFormat.fmt.pix.width,
               tryFormat.fmt.pix.height,
               (tryFormat.fmt.pix.pixelformat      ) & 0xFF,
               (tryFormat.fmt.pix.pixelformat >>  8) & 0xFF,
               (tryFormat.fmt.pix.pixelformat >> 16) & 0xFF,
               (tryFormat.fmt.pix.pixelformat >> 24) & 0xFF,
               parseV4L2Field(tryFormat.fmt.pix.field),
               tryFormat.fmt.pix.bytesperline,
               tryFormat.fmt.pix.sizeimage,
               parseV4L2Colorspace(tryFormat.fmt.pix.colorspace)
              );
    }

    if (isStreamingNow)
    {
        stopGrabbing();
    }

    if (Dioctl(deviceHandle, VIDIOC_S_FMT, &tryFormat) == -1)
    {
        NOTIFY("v4l2DS_t:setGrabFormat: "
               "VIDIOC_S_FMT failed: error %d %s\n",
               errno, strerror(errno)
              );
        retVal= false;
    }
    if (Dioctl(deviceHandle, VIDIOC_G_FMT, &format) == -1)
    {
        NOTIFY("v4l2DS_t::setGrabFormat: "
               "VIDIOC_G_FMT failed: error %d %s\n",
               errno, strerror(errno)
              );
    }
    else
    {
        NOTIFY("setGrabFormat: "
               "VIDIOC_G_FMT returned params:\n"
               "\ttype=%s,width=%d,height=%d,\n"
               "\tpixelformat=%c%c%c%c,field=%s,bytesperline=%d,\n"
               "\tsizeimage=%d,colorspace=%s\n",
               parseV4L2BufType(format.type),
               format.fmt.pix.width,
               format.fmt.pix.height,
               (format.fmt.pix.pixelformat      ) & 0xFF,
               (format.fmt.pix.pixelformat >>  8) & 0xFF,
               (format.fmt.pix.pixelformat >> 16) & 0xFF,
               (format.fmt.pix.pixelformat >> 24) & 0xFF,
               parseV4L2Field(format.fmt.pix.field),
               format.fmt.pix.bytesperline,
               format.fmt.pix.sizeimage,
               parseV4L2Colorspace(format.fmt.pix.colorspace)
              );
    }

    if (wasRunning)
    {
        startGrabbing();
    }

    return retVal;
}

static bool doneForWebCam_SCB_0370N= false;

bool
v4l2DS_t::setGrabSize(unsigned w, unsigned h)
{
    bool wasRunning= isStreamingNow;
    bool retVal= true;

    assert ((w * h) > 0 && "setGrabSize error, w and h must be > 0");

if (strcmp((const char*)vcap.card, "WebCam SCB-0370N") == 0)
{
    NOTIFY("v4l2DS_t::setGrabSize: setting 640x480 for WebCam SCB-0370N\n");
    w= 640;
    h= 480;

    if (doneForWebCam_SCB_0370N) // already set, do not rey again
       return true;
    doneForWebCam_SCB_0370N= true;
}

    if (isStreamingNow)
    {
        stopGrabbing();
    }

    if (w < minw)
    {
        w= minw;
        NOTIFY("Requested width shorter than minimum -> Applied: %d\n", w);
    }
    if (w > maxw)
    {
        w= maxw;
        NOTIFY("Requested width larger than maximum -> Applied: %d\n", w);
    }
    if (h < minh)
    {
        h= minh;
        NOTIFY("Requested height shorter than minimum -> Applied: %d\n", h);
    }
    if (h > maxh)
    {
        h= maxh;
        NOTIFY("Requested height larger than maximum -> Applied: %d\n", h);
    }

    if (pollframesizes(format.fmt.pix.pixelformat))
    {
        switch (framesizestype)
        {
            case V4L2_FRMSIZE_TYPE_DISCRETE:
            {
                unsigned nw= framesizes[0].discrete.width;
                unsigned nh= framesizes[0].discrete.height;
                unsigned reqArea= w * h;
                unsigned lastArea= nw * nh;
                unsigned curW, curH, curArea;

                for (int i = 1; i < numframesizes; i++)
                {
                    // if size is greater than requested
                    // and smaller than previously considered
                    // then choose new candidate
                    // up to 5% of considered area is allowed
                    curW= framesizes[i].discrete.width;
                    curH= framesizes[i].discrete.height;
                    curArea= curW * curH;
                    if ( (curW == w) && (curH == h) )
                    {
                        nw = curW;
                        nh = curH;
                        break;
                    }
                    if (lastArea <= reqArea)
                    {
                        // I have a good candidate, better than last one?
                        if (curArea <= reqArea && curArea > lastArea)
                        { // smaller than requested and bigger than last one
                            nw = curW;
                            nh = curH;
                            lastArea= curArea;
                        }
                    }
                    else
                    {
                        // I do not have a candidate, choose smaller
                        if (curArea < lastArea)
                        {
                            nw = curW;
                            nh = curH;
                            lastArea= curArea;
                        }
                    }
                }
                w= nw;
                h= nh;
                break;
            }
            case V4L2_FRMSIZE_TYPE_STEPWISE:
            {
                w = w - (w % framesizes[0].stepwise.step_width);
                h = h - (h % framesizes[0].stepwise.step_height);
                break;
            }
            case V4L2_FRMSIZE_TYPE_CONTINUOUS:
            {
                break;
            }
            default:
            {
                NOTIFY("setGrabSize: unsupported FRMSIZE_TYPE, bailing out\n");
                abort();
            }
        }
    }

    memset(&format, 0, sizeof (format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (Dioctl(deviceHandle, VIDIOC_G_FMT, &format) == -1)
    {
        NOTIFY("v4l2DS_t::setGrabProperty: VIDIOC_G_FMT failed:  %d %s\n",
               errno, strerror(errno)
              );
        return false;
    }

    format.fmt.pix.width = w;
    format.fmt.pix.height= h;

    // not really needed, useful for debugging
    if (Dioctl(deviceHandle, VIDIOC_TRY_FMT, &format) == -1)
    {
        NOTIFY("v4l2DS_t:setGrabSize: "
               "VIDIOC_TRY_FMT failed: error %d %s\n",
               errno, strerror(errno));
        NOTIFY("v4l2DS_t:setGrabSize: trying to continue anyway\n");
    }
    if (Dioctl(deviceHandle, VIDIOC_S_FMT, &format) == -1)
    {
        NOTIFY("v4l2DS_t:setGrabSize: "
               "VIDIOC_S_FMT failed: error %d %s\n",
               errno, strerror(errno));
        retVal= false;
    }
    if (Dioctl(deviceHandle, VIDIOC_G_FMT, &format) == -1)
    {
        NOTIFY("v4l2DS_t::setGrabSize: "
               "VIDIOC_G_FMT failed: error %d %s\n",
               errno, strerror(errno)
              );
    }

    if (wasRunning)
    {
        startGrabbing();
    }

    return retVal;
}


void
v4l2DS_t::getGrabSize(unsigned *w, unsigned *h)
{
    *w= format.fmt.pix.width;
    *h= format.fmt.pix.height;
}

u32
v4l2DS_t::getGrabFormat(void)
{
    // return the format we are grabbing.
    // if fails, maybe we are performing a conversion,
    // then it returns I410P
    // else, rethrows the exception
    try
    {
        return getFCCByV4L2Id(format.fmt.pix.pixelformat);
    }
    catch (char const *e)
    {
        throw e;
    }
}

//
// returns input ports names in the form "(name1,name2,...)"
//
const char *
v4l2DS_t::getInputPorts(void) const
{
    return inputPortsList.c_str();
}

//
// return the current input port name
//
const char *
v4l2DS_t::getInputPortName(void) const
{
    return (const char*)vchann[selectedChannel].name;
}

//
// return the current input port id
//
int
v4l2DS_t::getInputPortId(void) const
{
    return selectedChannel;
}

//
// select input port
//
bool
v4l2DS_t::setInputPort(const char *inputPort)
{
    if ((inputPort == NULL) || (inputPort[0] == '\0'))
    {
        return NULL;
    }
    for (int i= 0; i < numchannels; i++)
    {
        if ((inputPort == (const char *)vchann[i].name)
           || (strcmp(inputPort, (const char *)vchann[i].name) == 0))
        {

            if (setChannel(i))
            {
                return true;
            }
        }
    }
    return false;
}

bool
v4l2DS_t::setChannel(int newChannel)
{
    if (newChannel < 0)
        return false;

    if (newChannel >= numchannels)
        return false;

    selectedChannel= newChannel;

    if (Dioctl(deviceHandle, VIDIOC_S_INPUT, vchann+selectedChannel) == -1)
    {
        NOTIFY("---Cannot select channel %d\n", newChannel);
        return false;
    }

    NOTIFY("+++Selected Channel= %d %s\n",
           selectedChannel,
           parsev4l2ChannelType(vchann[selectedChannel].type)
          );

    return true;
}

int
v4l2DS_t::getMinWidth(void)
{
    return minw;
}

int
v4l2DS_t::getMaxWidth(void)
{
    return maxw;
}

int
v4l2DS_t::getMinHeight(void)
{
    return minh;
}

int
v4l2DS_t::getMaxHeight(void)
{
    return maxh;
}

bool
v4l2DS_t::setSaturation(int z)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_SATURATION;
    cntrl.id   = V4L2_CID_SATURATION;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("setSaturation: error %d %s\n", errno, strerror(errno));
        if (errno == ENODEV)
        {
            builtOK= false;
        }
        return false;
    }

    if (controls.step == 0)
    {
        NOTIFY("setSaturation: error step is 0. Invalid value\n");
        return false;
    }

    int min= controls.minimum;
    int max= controls.maximum;

    // range [0..100] -> [min..max]
    z= int(((float)max - min) * z / 100 + min);

    if (z< controls.minimum) z= controls.minimum;
    if (z> controls.maximum) z= controls.maximum;

    cntrl.value= z - (z % controls.step);

    if (Dioctl(deviceHandle, VIDIOC_S_CTRL, &cntrl) == -1)
    {
        NOTIFY("setSaturation: error %d %s\n", errno, strerror(errno));
        return false;
    }
    return true;
}

int
v4l2DS_t::getSaturation(void)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_SATURATION;
    cntrl.id   = V4L2_CID_SATURATION;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("getSaturation: error %d %s\n", errno, strerror(errno));
        return -1;
    }

    if (controls.step == 0)
    {
        NOTIFY("getSaturation: error step is 0. Invalid value\n");
        return -1;
    }

    if (Dioctl(deviceHandle, VIDIOC_G_CTRL, &cntrl) == -1)
    {
        NOTIFY("getSaturation: error %d %s\n", errno, strerror(errno));
        return  -1;
    }

    int min= controls.minimum;
    int max= controls.maximum;
    int z  = cntrl.value;

    z= int( ((float)z - min) * 100 / (max - min) );

    return z;
}


bool
v4l2DS_t::setBrightness(int z)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_BRIGHTNESS;
    cntrl.id   = V4L2_CID_BRIGHTNESS;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("setBrightness: error %d %s\n", errno, strerror(errno));
        return false;
    }

    if (controls.step == 0)
    {
        NOTIFY("setBrightness: step is 0. Invalid value\n");
        return false;
    }

    int min= controls.minimum;
    int max= controls.maximum;

    // range [0..100] -> [min..max]
    z= int(((float)max - min) * z / 100 + min);

    if (z< controls.minimum) z= controls.minimum;
    if (z> controls.maximum) z= controls.maximum;

    cntrl.value= z - (z % controls.step);

    if (Dioctl(deviceHandle, VIDIOC_S_CTRL, &cntrl) == -1)
    {
        NOTIFY("setBrightness: error %d %s\n", errno, strerror(errno));
        return false;
    }
    return true;
}

int
v4l2DS_t::getBrightness(void)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_BRIGHTNESS;
    cntrl.id   = V4L2_CID_BRIGHTNESS;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("getBrightness: error %d %s\n", errno, strerror(errno));
        return -1;
    }

    if (controls.step == 0)
    {
        NOTIFY("getBrightness: step is 0. Invalid value\n");
        return -1;
    }

    if (Dioctl(deviceHandle, VIDIOC_G_CTRL, &cntrl) == -1)
    {
        NOTIFY("getBrightness: error %d %s\n", errno, strerror(errno));
        return -1;
    }

    int min= controls.minimum;
    int max= controls.maximum;
    int z  = cntrl.value;

    z= int( ((float)z - min) * 100 / (max - min) );

    return z;
}

bool
v4l2DS_t::setHue(int z)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_HUE;
    cntrl.id   = V4L2_CID_HUE;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("setHue: hue error %d %s\n", errno, strerror(errno));
        return false;
    }

    if (controls.step == 0)
    {
        NOTIFY("setHue: error step is 0. Invalid value\n");
        return false;
    }

    int min= controls.minimum;
    int max= controls.maximum;

    // range [0..100] -> [min..max]
    z= int(((float)max - min) * z / 100 + min);

    if (z< controls.minimum) z= controls.minimum;
    if (z> controls.maximum) z= controls.maximum;

    cntrl.value= z - (z % controls.step);

    if (Dioctl(deviceHandle, VIDIOC_S_CTRL, &cntrl) == -1)
    {
        NOTIFY("setHue: hue error %d %s\n", errno, strerror(errno));
        return false;
    }
    return true;
}

int
v4l2DS_t::getHue(void)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_HUE;
    cntrl.id   = V4L2_CID_HUE;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("getHue: hue error %d %s\n", errno, strerror(errno));
        return -1;
    }

    if (controls.step == 0)
    {
        NOTIFY("getHue: error step is 0. Invalid value\n");
        return -1;
    }

    if (Dioctl(deviceHandle, VIDIOC_G_CTRL, &cntrl) == -1)
    {
        NOTIFY("getHue: error %d %s\n", errno, strerror(errno));
        return -1;
    }

    int min= controls.minimum;
    int max= controls.maximum;
    int z  = cntrl.value;

    z= int( ((float)z - min) * 100 / (max - min) );

    return z;
}

bool
v4l2DS_t::setContrast(int z)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_CONTRAST;
    cntrl.id   = V4L2_CID_CONTRAST;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("setContrast: error %d %s\n", errno, strerror(errno));
        return false;
    }

    if (controls.step == 0)
    {
        NOTIFY("setContrast: error step is 0. Invalid value\n");
        return false;
    }

    int min= controls.minimum;
    int max= controls.maximum;

    // range [0..100] -> [min..max]
    z= int(((float)max - min) * z / 100 + min);

    if (z< controls.minimum) z= controls.minimum;
    if (z> controls.maximum) z= controls.maximum;

    cntrl.value= z - (z % controls.step);

    if (Dioctl(deviceHandle, VIDIOC_S_CTRL, &cntrl) == -1)
    {
        NOTIFY("setContrast: error %d %s\n", errno, strerror(errno));
        return false;
    }
    return true;
}

int
v4l2DS_t::getContrast(void)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_CONTRAST;
    cntrl.id   = V4L2_CID_CONTRAST;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("getContrast: error %d %s\n", errno, strerror(errno));
        return -1;
    }

    if (controls.step == 0)
    {
        NOTIFY("getContrast: error step is 0. Invalid value\n");
        return -1;
    }

    if (Dioctl(deviceHandle, VIDIOC_G_CTRL, &cntrl) == -1)
    {
        NOTIFY("getContrast: error %d %s\n", errno, strerror(errno));
        return -1;
    }

    int min= controls.minimum;
    int max= controls.maximum;
    int z  = cntrl.value;

    z= int( ((float)z - min) * 100 / (max - min) );

    return z;
}

std::string
v4l2DS_t::getStandardList(void)
{
    return standardList;
}

bool
v4l2DS_t::setStandard(const char *z)
{
    int i= 0;
    v4l2_std_id std_id;

    for (i= 0; i < numstandards; i++)
    {
        if (strcmp((const char*)standard[i].name, z) == 0)
        {
            break; // found
        }
    }

    if (i == numstandards) // not found
    {
        NOTIFY("v4l2DS_t::setStandard: [%d] standards\n", numstandards);
        return false;
    }

    std_id= standard[i].id;
    if (Dioctl(deviceHandle, VIDIOC_S_STD, &std_id) == -1)
    {
        NOTIFY("v4l2DS_t::setStandard: cannot set norm: %d %s\n",
               errno,
               strerror(errno)
              );
        return false;
    }

    return true;
}

const char *
v4l2DS_t::getStandard(void)
{
    v4l2_std_id std_id;

    if (numstandards == 0) // surely, an USB device
    {
        return NULL;
    }

    if (Dioctl(deviceHandle, VIDIOC_G_STD, &std_id) == -1)
    {
        NOTIFY("v4l2DS_t::getStandard: cannot get current standard: %d %s\n",
               errno,
               strerror(errno)
              );
        return NULL;
    }

    for (int i= 0; i < numstandards; i++)
    {
        if (standard[i].id & std_id)
        {
            //NOTIFY("v4l2DS_t::getStandard: found %s!\n", standard[i].name);
            return (const char*)standard[i].name;
        }
    }

    // not found, strange, but
    NOTIFY("v4l2DS_t::getStandard: error checking standard\n");

    return NULL;
}

std::string
v4l2DS_t::getFlickerFreqList(void)
{
    return std::string("50Hz,60Hz,NoFlicker");
}

#ifndef V4L2_CID_POWER_LINE_FREQUENCY
// only needed if kernel < 2.6.25
#define V4L2_CID_POWER_LINE_FREQUENCY (V4L2_CID_BASE+24)
#endif
bool
v4l2DS_t::setFlickerFreq(const char *z)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_POWER_LINE_FREQUENCY;
    cntrl.id   = V4L2_CID_POWER_LINE_FREQUENCY;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("setFlickerFreq: error %d %s\n", errno, strerror(errno));
        return false;
    }

    if (controls.step == 0)
    {
        NOTIFY("setFlickerFreq: error step is 0. Invalid value\n");
        return false;
    }

    cntrl.value= getV4L2FlickerFreq(z);

    if (Dioctl(deviceHandle, VIDIOC_S_CTRL, &cntrl) == -1)
    {
        NOTIFY("setFlickerFreq: error %d %s\n", errno, strerror(errno));
        return false;
    }
    return true;
}

const char *
v4l2DS_t::getFlickerFreq(void)
{
    v4l2_queryctrl controls;
    v4l2_control   cntrl;

    memset(&controls, 0, sizeof(controls));
    memset(&cntrl,    0, sizeof(cntrl));

    controls.id= V4L2_CID_POWER_LINE_FREQUENCY;
    cntrl.id   = V4L2_CID_POWER_LINE_FREQUENCY;

    if (Dioctl(deviceHandle, VIDIOC_QUERYCTRL, &controls) == -1)
    {
        NOTIFY("getFlickerFreq: error %d %s\n", errno, strerror(errno));
        return NULL;
    }

    if (controls.step == 0)
    {
        NOTIFY("getFlickerFreq: error step is 0. Invalid value\n");
        return NULL;
    }

    if (Dioctl(deviceHandle, VIDIOC_G_CTRL, &cntrl) == -1)
    {
        NOTIFY("getFlickerFreq: error %d %s\n", errno, strerror(errno));
        return NULL;
    }

    return parseV4L2FlickerFreq(cntrl.value);
}

bool
v4l2DS_t::pollframesizes(u32 pixformat)
{
    int i;

    memset(&framesizes[0], 0, sizeof(v4l2_frmsizeenum));
    framesizes[0].index= 0;
    framesizes[0].pixel_format= pixformat;

    if (Dioctl (deviceHandle, VIDIOC_ENUM_FRAMESIZES, &framesizes[0]) == -1)
    {
        numframesizes= -1;

        return false;
    }

    numframesizes= 1;
    framesizestype = framesizes[0].type;

    switch (framesizestype)
    {
    case V4L2_FRMSIZE_TYPE_DISCRETE:
        minw = maxw = framesizes[0].discrete.width;
        minh = maxh = framesizes[0].discrete.height;
        for (i= 1; i < 256; i++)
        {
            memset(&framesizes[i], 0, sizeof(v4l2_frmsizeenum));
            framesizes[i].index= i;
            framesizes[i].pixel_format= pixformat;
            if (Dioctl(deviceHandle,
                       VIDIOC_ENUM_FRAMESIZES,
                       framesizes + i
                      ) < 0
               )
            {
                break;
            }
            if (framesizes[i].discrete.width < minw)
            {
                minw = framesizes[i].discrete.width;
            }
            if (framesizes[i].discrete.width > maxw)
            {
                maxw = framesizes[i].discrete.width;
            }
            if (framesizes[i].discrete.height < minh)
            {
                minh = framesizes[i].discrete.height;
            }
            if (framesizes[i].discrete.height > maxh)
            {
                maxh = framesizes[i].discrete.height;
            }
            numframesizes++;
        }
        if (i == 256) NOTIFY("WARNING: REACHED LIMIT FOR ENUM_FRAMESIZES!\n");
        break;
    case V4L2_FRMSIZE_TYPE_STEPWISE:
        minw= framesizes[0].stepwise.min_width;
        minh= framesizes[0].stepwise.min_height;
        maxw= framesizes[0].stepwise.max_width;
        maxh= framesizes[0].stepwise.max_height;
        break;
    case V4L2_FRMSIZE_TYPE_CONTINUOUS:
        minw= framesizes[0].stepwise.min_width;
        minh= framesizes[0].stepwise.min_height;
        maxw= framesizes[0].stepwise.max_width;
        maxh= framesizes[0].stepwise.max_height;
        break;
    default:
        NOTIFY("pollframesizes: unsupported FRMSIZE_TYPE, bailing out\n");
        abort();
    }

    return true;
}

void
v4l2DS_t::printframesizes(void)
{
    framesizestype = framesizes[0].type;
    switch (framesizestype)
    {
    case V4L2_FRMSIZE_TYPE_DISCRETE:
        NOTIFY_ND("%d discrete values: ", numframesizes);
        for (int i= 0; i < numframesizes; i++)
        {
            NOTIFY_ND("%dx%d ",
                      framesizes[i].discrete.width,
                      framesizes[i].discrete.height
                     );
        }
        NOTIFY_ND("\n");
        break;
    case V4L2_FRMSIZE_TYPE_STEPWISE:
        NOTIFY_ND("stepwise range: width [%d - %d] by %d ",
                  framesizes[0].stepwise.min_width,
                  framesizes[0].stepwise.max_width,
                  framesizes[0].stepwise.step_width
                 );
        NOTIFY_ND("height [%d - %d] by %d\n",
                  framesizes[0].stepwise.min_height,
                  framesizes[0].stepwise.max_height,
                  framesizes[0].stepwise.step_height
                 );
        break;
    case V4L2_FRMSIZE_TYPE_CONTINUOUS:
        NOTIFY_ND("continuous range from %dx%d to %dx%d\n",
                  framesizes[0].stepwise.min_width,
                  framesizes[0].stepwise.min_height,
                  framesizes[0].stepwise.max_width,
                  framesizes[0].stepwise.max_height
                 );
        break;
    default:
        NOTIFY_ND(" printframesizes: unknown FRMSIZE_TYPE, bailing out\n");
        abort();
    }
}

