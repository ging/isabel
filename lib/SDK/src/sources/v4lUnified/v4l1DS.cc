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
// $Id: v4l1DS.cc 10326 2007-06-07 15:48:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <icf2/notify.hh>

#include "v4l1DS.hh"
#include "v4luDioctl.hh"

#if DEBUG_IOCTL
int
Dioctlv4l1 (int fd, unsigned long cmd, void *arg)
{
    int rc = ioctl(fd, cmd, arg);
    prnv4l1ioctl(cmd, arg, rc);
    return rc;
}
#define Dioctl Dioctlv4l1
#else
#define Dioctl ioctl
#endif

v4l1DS_t::v4l1DS_t(const char *path)
: VideoHandler_t(path),
  deviceBuffer(NULL),
  deviceBufferLen(0),
  selectedChannel(0),
  grab_number(0)
{
    deviceHandle= open(getPath(), O_RDWR, 0000);
    if (deviceHandle < 0)
    {
        NOTIFY("---cannot open '%s': %s\n", getPath(), strerror(errno));
        NOTIFY("---bailing out\n");
        return; // builtOK is false
    }

    NOTIFY("+++Open device: %s\n", getPath());

    //
    // v4l capabilities
    //
    if (Dioctl(deviceHandle, VIDIOCGCAP, &vcap) == -1)
    {
        if (errno == ENOMEDIUM)
        {
            NOTIFY("---VIDIOCGCAP error: %s, trying to continue\n",
                   strerror(errno)
                  );
        }
        else
        {
            NOTIFY("---it seems Video4Linux is not supported: %s\n",
                   strerror(errno)
                  );
            NOTIFY("---bailing out\n");
            return; // builtOK is false
        }
    }
    else
    {
        NOTIFY("+++v4l1 supported\n");
        NOTIFY("+++\n");
    }

    NOTIFY("+++Description of device\n");
    NOTIFY("+++=====================\n");
    NOTIFY("+++name\t: %s\n", vcap.name);
    NOTIFY("+++type\t: %s\n", parsev4l1VideoType(vcap.type));
    NOTIFY("+++n_channels\t: %d\n", vcap.channels);
    NOTIFY("+++n_audios\t: %d\n", vcap.audios);
    NOTIFY("+++min_width\t: %d\n", vcap.minwidth);
    NOTIFY("+++min_height\t: %d\n", vcap.minheight);
    NOTIFY("+++max_width\t: %d\n", vcap.maxwidth);
    NOTIFY("+++max_height\t: %d\n", vcap.maxheight);

    if ( ! (vcap.type & VID_TYPE_CAPTURE))
    {
        NOTIFY("---ERROR: device does not support capture, bailing out\n");
        return; // builtOK is false
    }

    //
    // channels description
    //
    for (int i= 0; i < vcap.channels; i++)
    {
        vchann[i].channel= i;
        Dioctl(deviceHandle, VIDIOCGCHAN, vchann+i);
        NOTIFY("+++..description for channel %d\n", i);
        NOTIFY("+++....identifier: %d\n", vchann[i].channel);
        NOTIFY("+++....name\t: %s\n", vchann[i].name);
        NOTIFY("+++....tuners\t: %d\n", vchann[i].tuners);
        NOTIFY("+++....flags\t: %s\n", parsev4l1ChannelFlags(vchann[i].flags));
        NOTIFY("+++....type\t: %s\n", parsev4l1ChannelType(vchann[i].type));
        NOTIFY("+++....norm\t: %s\n", parsev4l1Norm(vchann[i].norm));

    }

    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == 0)
    {
        NOTIFY("+++....brightness: %d\n", vpic.brightness);
        NOTIFY("+++....hue\t: %d\n",      vpic.hue);
        NOTIFY("+++....colour\t: %d\n",   vpic.colour);
        NOTIFY("+++....contrast\t: %d\n", vpic.contrast);
        NOTIFY("+++....whiteness\t: %d\n",vpic.whiteness);
        NOTIFY("+++....depth\t: %d\n",    vpic.depth);
        NOTIFY("+++....palette\t: %s\n",  parsev4l1Palette(vpic.palette));
    }

    if (Dioctl(deviceHandle, VIDIOCGWIN, &vwin) == 0)
    {
        NOTIFY("+++..window description\n");
        NOTIFY("+++....x: %d\n",         vwin.x);
        NOTIFY("+++....y\t: %d\n",       vwin.y);
        NOTIFY("+++....width\t: %d\n",   vwin.width);
        NOTIFY("+++....height\t: %d\n",  vwin.height);
        NOTIFY("+++....chromakey: %d\n", vwin.chromakey);
        NOTIFY("+++....flags\t: %d\n",   vwin.flags);
    }

    NOTIFY("+++\n");
    Dioctl(deviceHandle, VIDIOCGCHAN, vchann+selectedChannel);
    Dioctl(deviceHandle, VIDIOCSCHAN, vchann+selectedChannel);
    Dioctl(deviceHandle, VIDIOCGCHAN, vchann+selectedChannel);
    NOTIFY("+++Selected Channel= %d %s %s\n",
           selectedChannel,
           parsev4l1ChannelType(vchann[selectedChannel].type),
           parsev4l1Norm(vchann[selectedChannel].norm)
          );
    NOTIFY("+++\n");

    //
    // read method
    //
    useMMapGrab= mmap_init();

    if ( ! useMMapGrab )
    {
        NOTIFY("---mmaped frames failed or not supported, trying read()\n");

        if ( ! read_init() )
        {
            NOTIFY("---read() method failed, bailing out\n");

            return; // builtOK is false
        }
    }

    // check supported formats
    grabFormats= allowedGrabFormats();

    if ( ( ! grabFormats.isValid()) || (grabFormats->len() == 0))
    {
        NOTIFY("---Could not detect valid formats, bailing out\n");
        return; // builtOK is false
    }

    u32 defFmt= static_cast<u32>(grabFormats->begin());
    if (useMMapGrab)
    {
        for (int i= 0; i < vmb.frames; i++)
        {
            vmmap[i].frame = i;
            vmmap[i].width = vcap.maxwidth;
            vmmap[i].height= vcap.maxheight;
            vmmap[i].format= getV4LIdByFCC(defFmt);
        }
    }
    else
    {
        // got default values, not need to set them
        int v4lFmt= getV4LIdByFCC(defFmt);
        vpic.depth   = v4lId2bpp(v4lFmt);
        vpic.palette = v4lFmt;
        Dioctl(deviceHandle, VIDIOCSPICT, &vpic);
        Dioctl(deviceHandle, VIDIOCGPICT, &vpic);

        Dioctl(deviceHandle, VIDIOCGWIN, &vwin);
        vwin.width = vcap.maxwidth;
        vwin.height= vcap.maxheight;
        Dioctl(deviceHandle, VIDIOCSWIN, &vwin);
    }

    //
    // input ports info in the form "(name1,name2,...)"
    //
    {
        inputPortsStr= new char[vcap.channels * 512];
        char buf[512];

        strcpy(inputPortsStr, "(");
        for (int i= 0; i< vcap.channels; i++)
        {
            sprintf(buf, "%s,", vchann[i].name);
            strcat(inputPortsStr, buf);
        }

        // remove last comma, if exists
        char *lastComma= strrchr(inputPortsStr, ',');
        if (lastComma) *lastComma= '\0';
        strcat(inputPortsStr, ")");
    } // end input ports info

    if (useMMapGrab)
    {
        mmap_close();
    }
    else
    {
        read_close();
    }

    setDeviceFree();

    builtOK= true;

    NOTIFY("+++Device %s correctly opened!!\n", getPath());
    NOTIFY("+++\n");
}

v4l1DS_t::~v4l1DS_t(void)
{
    stopGrabbing();

    if (deviceHandle >= 0)
    {
        close(deviceHandle);
    }

    delete []inputPortsStr;
}

bool
v4l1DS_t::mmap_init(void)
{
    assert (deviceBuffer == NULL && "mmap_init: deviceBuffer must be NULL");

        // Epiphan driver requires the following ioctl to restart
        // do not know other cameras, as v4l is deprecated
        if (Dioctl(deviceHandle, VIDIOCGCAP, &vcap) == -1)
        {
            if (errno == ENOMEDIUM)
            {
                NOTIFY("---VIDIOCGCAP error: %s, trying to continue\n",
                       strerror(errno)
                      );
            }
            else
            {
                NOTIFY("---device stopped working, VIDIOCGCAP failed: %s\n",
                       strerror(errno)
                      );
                NOTIFY("---bailing out\n");
                builtOK= false;

                return false;
            }
        }

        NOTIFY("+++%s start grabbing\n", vcap.name);
        NOTIFY("+++type\t: %s\n", parsev4l1VideoType(vcap.type));
        NOTIFY("+++n_channels\t: %d\n", vcap.channels);
        NOTIFY("+++n_audios\t: %d\n", vcap.audios);
        NOTIFY("+++min_width\t: %d\n", vcap.minwidth);
        NOTIFY("+++min_height\t: %d\n", vcap.minheight);
        NOTIFY("+++max_width\t: %d\n", vcap.maxwidth);
        NOTIFY("+++max_height\t: %d\n", vcap.maxheight);

    if (Dioctl(deviceHandle, VIDIOCGMBUF, &vmb) == -1)
    {
        NOTIFY("---mmapped frames not supported\n");

        return false;
    }

    NOTIFY("+++mmapped buffers supported: %d frame%s, %d bytes\n",
           vmb.frames,
           vmb.frames > 1 ? "s" : "",
           vmb.size
          );

    deviceBufferLen= vmb.size;

    //
    // mmap buffer and lock it
    //
    deviceBuffer= (u8 *)mmap(NULL,
                             deviceBufferLen,
                             PROT_READ|PROT_WRITE,
                             MAP_SHARED,
                             deviceHandle,
                             0
                            );
    if (deviceBuffer == MAP_FAILED)
    {
        NOTIFY("---mmap failed! (%s), reverting to read()\n",
               strerror(errno)
              );

        return false;
    }

    for (int i= 0; i < vmb.frames; i++)
    {
        vmmap[i].frame = i;
        vmmap[i].width = vcap.maxwidth;
        vmmap[i].height= vcap.maxheight;
        // the following should be set the first time mmap_init() is called,
        // after polling allowed formats, done in constructor
        //vmmap[i].format= getV4LIdByFCC(defFmt);
    }

    if (vmb.frames > 1)
    {
        // if driver allows several driver, we prepare
        // the capture of a first frame, alternating buffers
        // if only one buffer is available, CMCAPTURE and VIDIOCSYNC
        // go secuentially
   
        if (Dioctl(deviceHandle, VIDIOCMCAPTURE, &vmmap[grab_number]) == -1)
        {
            NOTIFY("---First image capture failed, trying to continue, "
                   "VIDIOCMCAPTURE failed: %s\n",
                   strerror(errno)
                  );
            return true;
        }
    }

    NOTIFY("+++using mmapped buffers for capture\n");

    return true;
}


bool
v4l1DS_t::read_init(void)
{
    assert (deviceBuffer == NULL && "read_init: deviceBuffer must be NULL");

    deviceBufferLen= vcap.maxwidth * vcap.maxheight * 4;
    deviceBuffer= (unsigned char*)malloc(deviceBufferLen);

    if ( ! deviceBuffer )
    {
        NOTIFY("---alloc memory failed: %s\n", strerror(errno));
        NOTIFY("---bailing out\n");

        return false;
    }

    return true;
}

void
v4l1DS_t::mmap_close(void)
{
    assert (deviceBuffer != NULL && "mmap_close: deviceBuffer is NULL");

    // get current image
    if (vmb.frames > 1)
    {
        Dioctl(deviceHandle, VIDIOCSYNC, &vmmap[grab_number].frame);
    }

    munmap((char *)deviceBuffer, deviceBufferLen);

    deviceBuffer= NULL;
}


void
v4l1DS_t::read_close(void)
{
    assert (deviceBuffer != NULL && "read_close: deviceBuffer is NULL");

    free(deviceBuffer);

    deviceBuffer= NULL;
}

u8 *
v4l1DS_t::read_capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h)
{
    int n= read(deviceHandle, deviceBuffer, deviceBufferLen);

    if (n < 0)
    {
        builtOK= false;
        return NULL;
    }

    struct timeval currentTime;
    gettimeofday (&currentTime, NULL);
    *timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

    *bufLen = n;

    return deviceBuffer;
}

u8 *
v4l1DS_t::mmap_capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h)
{
if (strstr(vcap.name, "VGA2USB") != NULL)
{
    // Epiphan VGA2USB returns the same min and max value
    // when working ok, and returns different value when no VGA signal
    if ( (vcap.minwidth != vcap.maxwidth)
       )
    {
        NOTIFY("---VGA2USB grabber, NO VGA signal yet\n");
        builtOK= false;
        return NULL;
    }
}

    // prepare capture for next image
    if (Dioctl(deviceHandle,
               VIDIOCMCAPTURE,
               &vmmap[(grab_number+1) % vmb.frames]
              ) == -1
       )
    {
        builtOK= false;
        return NULL;
    }

    // get current image
    if (Dioctl(deviceHandle, VIDIOCSYNC, &vmmap[grab_number].frame) == -1)
    {
        builtOK= false;
        return NULL;
    }

    // get time
    struct timeval currentTime;
    gettimeofday (&currentTime, NULL);
    *timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;

    *bufLen=   vmmap[grab_number].width
             * vmmap[grab_number].height
             * v4lId2bpp(vmmap[grab_number].format);

    *w= vmmap[grab_number].width;
    *h= vmmap[grab_number].height;

    unsigned char *theBuffer=
      deviceBuffer + vmb.offsets[vmmap[grab_number].frame];

    grab_number= (grab_number + 1) % vmb.frames;

    return theBuffer;
}

// private
formatList_ref
v4l1DS_t::allowedGrabFormats(void)
{
    NOTIFY("+++detect grab formats for '%s'\n", getPath());

    video_mmap tryvmmap;
 
    tryvmmap.frame = 0;
    tryvmmap.width = vcap.minwidth;
    tryvmmap.height= vcap.minheight;

    formatList_ref okflr= new ql_t<u32>;

    u32 fmtList[]=
        {
            I420P_FORMAT, I411P_FORMAT,
            I422P_FORMAT, I422i_FORMAT,
            RGB24_FORMAT, BGR24_FORMAT
        };
    int fmtListLen= sizeof(fmtList) / sizeof(u32);

    for (int i= 0; i < fmtListLen; i++)
    {
        u32 fmt= fmtList[i];

        NOTIFY("   probing %s:", vGetFormatNameById(fmt));

        int format= getV4LIdByFCC(fmt);
        assert (format >= 0 && "no translation for this format");

        if (useMMapGrab)
        {
            tryvmmap.format= format;

            // capture first frame
            if (Dioctl(deviceHandle, VIDIOCMCAPTURE, &tryvmmap) == -1)
            {
                NOTIFY_ND(" failed");
            }
            else
            {
                NOTIFY_ND(" OK ");

                // get current image, do not care for errors
                NOTIFY_ND("+");
                Dioctl(deviceHandle, VIDIOCSYNC, &tryvmmap.frame);
                okflr->insert(fmt);
            }
        }
        else
        {
            int n= read(deviceHandle, deviceBuffer, deviceBufferLen);

            if (n < 0)
            {
                NOTIFY_ND(" failed");
            }
            else
            {
                NOTIFY_ND(" OK ");
                okflr->insert(fmt);
            }
        }

        NOTIFY_ND("\n");
    }

    return okflr;
}

void
v4l1DS_t::stopGrabbing(void)
{
    if ( ! isStreamingNow )
    {
        return;
    }

    if (useMMapGrab)
    {
        mmap_close();
    }
    else
    {
        read_close();
    }

    isStreamingNow= false;
}

void
v4l1DS_t::startGrabbing(void)
{
    if (isStreamingNow)
    {
        return;
    }

    if (useMMapGrab)
    {
        if ( ! mmap_init() )
        {
            return;
        }
    }
    else
    {
        if ( ! read_init() )
        {
            return;
        }
    }

    builtOK= true;  // was retried, and this time successfully
    isStreamingNow= true;
}

u8 *
v4l1DS_t::capture(u32 *timestamp, u32 *bufLen, unsigned *w, unsigned *h)
{
    u8 *theBuffer= NULL;

    if (useMMapGrab)
    {
        theBuffer= mmap_capture(timestamp, bufLen, w, h);
    }
    else
    {
        theBuffer= read_capture(timestamp, bufLen, w, h);
    }

    return theBuffer;
}

//
// check a list of formats and returns a list with the acceptable ones
//
formatList_ref
v4l1DS_t::getGrabFormats(void)
{
    if ( ! grabFormats.isValid())
    {
       NOTIFY("\n\nWARNING: no formats available, continuing\n\n");
    }
    else
    {
        NOTIFY("\nDetected formats for %s: \n", getPath());
        for(ql_t<u32>::iterator_t i= grabFormats->begin();
            i != grabFormats->end();
            i++
           )
        {
            u32 fmt= static_cast<u32>(i);

            NOTIFY("0x%x %s ", fmt, vGetFormatNameById(fmt));
        }
        NOTIFY("\n");
    }

    return grabFormats;
}

bool
v4l1DS_t::setGrabSize(unsigned w, unsigned h)
{
#if 0
NOTIFY("v4l1DS_t::setGrabSize: min=%dx%d, max=%dx%d, req=%dx%d\n",
        vcap.minwidth, vcap.minheight,
        vcap.maxwidth, vcap.maxheight,
        w, h);
#endif

    // verify parameters

    if (w < (unsigned)vcap.minwidth) { w= (unsigned)vcap.minwidth; }
    if (w > (unsigned)vcap.maxwidth) { w= (unsigned)vcap.maxwidth; }

    if (h < (unsigned)vcap.minheight) { h= (unsigned)vcap.minheight; }
    if (h > (unsigned)vcap.maxheight) { h= (unsigned)vcap.maxheight; }

    bool wasGrabbing= isStreamingNow;

    stopGrabbing();

    if (useMMapGrab)
    {
        for (int i= 0; i < vmb.frames; i++)
        {
            vmmap[i].width = w;
            vmmap[i].height= h;
        }
    }
    else
    {
        vwin.width = w;
        vwin.height= h;
        Dioctl(deviceHandle, VIDIOCSWIN, &vwin);
    }

    if (wasGrabbing)
    {
        startGrabbing();
    }

    return true;
}

void
v4l1DS_t::getGrabSize(unsigned *w, unsigned *h)
{
    if (useMMapGrab)
    {
        *w= vmmap[grab_number].width;
        *h= vmmap[grab_number].height;
    }
    else
    {
        *w= vwin.width;
        *h= vwin.height;
    }
}

bool
v4l1DS_t::setGrabFormat(u32 fmt)
{
    int V4LFmt= -1;

    if (useMMapGrab)
    {
        if (getV4LIdByFCC(fmt) == vmmap[grab_number].format)
        {
            return true;
        }
    }
    else
    {
        if (fmt == vpic.palette)
        {
            return true;
        }
    }

    for (ql_t<u32>::iterator_t i= grabFormats->begin();
         i != grabFormats->end();
         i++
        )
    {
        u32 cfi= static_cast<u32>(i);

        if (fmt == cfi) // found!
        {
            V4LFmt= getV4LIdByFCC(fmt);
            break;
        }
    }

    if (V4LFmt == -1)
    {
        return false;
    }

    bool wasGrabbing= isStreamingNow;

    stopGrabbing();

    if (useMMapGrab)
    {
        for (int i= 0; i < vmb.frames; i++)
        {
            vmmap[i].format= V4LFmt;
        }
    }
    else
    {
        vpic.depth   = v4lId2bpp(V4LFmt);
        vpic.palette = V4LFmt;
        if (Dioctl(deviceHandle, VIDIOCSPICT, &vpic) == -1)
        {
            return false;
        }
    }

    if (wasGrabbing)
    {
        startGrabbing();
    }

    return true;
}

u32
v4l1DS_t::getGrabFormat(void)
{
    int fmt;

    if (useMMapGrab)
    {
        fmt= vmmap[grab_number].format;
    }
    else
    {
        if (Dioctl(deviceHandle, VIDIOCSPICT, &vpic) == -1)
        {
            throw "VIDIOCSPICT error, cannot determine grab format";
        }
        fmt= vpic.palette;
    }

    return getFCCByV4LId(fmt);
}

//
// returns input ports names in the form "(name1,name2,...)"
//
const char *
v4l1DS_t::getInputPorts(void) const
{
    return inputPortsStr;
}

//
// return the current input port name
//
const char *
v4l1DS_t::getInputPortName(void) const
{
    return vchann[selectedChannel].name;
}

//
// return the current input port id
//
int
v4l1DS_t::getInputPortId(void) const
{
    return selectedChannel;
}

//
// select input port
//
bool
v4l1DS_t::setInputPort(const char *inputPort)
{
    if ((inputPort == NULL) || (inputPort[0] == '\0'))
    {
        return NULL;
    }

    for (int i= 0; i < vcap.channels; i++)
    {
        if ((inputPort == vchann[i].name)  // usually NULL
           || (strcmp(inputPort, vchann[i].name) == 0))
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
v4l1DS_t::setChannel(int newChannel)
{
    if (newChannel < 0)
        return false;

    if (newChannel >= vcap.channels)
        return false;

    selectedChannel= newChannel;

    Dioctl(deviceHandle, VIDIOCSCHAN, vchann+selectedChannel);
    Dioctl(deviceHandle, VIDIOCGCHAN, vchann+selectedChannel);

    NOTIFY("+++Selected Channel= %d %s %s\n",
           selectedChannel,
           parsev4l1ChannelType(vchann[selectedChannel].type),
           parsev4l1Norm(vchann[selectedChannel].norm)
          );

    return true;
}

int
v4l1DS_t::getMinWidth(void)
{
    return vcap.minwidth;
}

int
v4l1DS_t::getMaxWidth(void)
{
    return vcap.maxwidth;
}

int
v4l1DS_t::getMinHeight(void)
{
    return vcap.minheight;
}

int
v4l1DS_t::getMaxHeight(void)
{
    return vcap.maxheight;
}


bool
v4l1DS_t::setSaturation(int z)
{
    // get current values
    memset(&vpic, 0, sizeof(video_picture));
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return false;
    }

    // set proper range
    z= (int) ( (float)z * 655.35 );

    if (z< 0)     z= 0;
    if (z> 65535) z= 65535;

    // set the requested value
    vpic.colour= z;
    if (Dioctl(deviceHandle, VIDIOCSPICT, &vpic) == -1)
    {
        return false;
    }
    return true;
}

int
v4l1DS_t::getSaturation(void)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return -1;
    }

    int z= vpic.colour;
    z= (int) ( (float)z / 655.35 );
    return z;
}

bool
v4l1DS_t::setBrightness(int z)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return false;
    }

    // set proper range
    z= (int) ( (float)z * 655.35 );

    if (z< 0)     z= 0;
    if (z> 65535) z= 65535;

    // set the requested value
    vpic.brightness= z;
    if (Dioctl(deviceHandle, VIDIOCSPICT, &vpic) == -1)
    {
        return false;
    }
    return true;
}

int
v4l1DS_t::getBrightness(void)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return -1;
    }

    int z= vpic.brightness;
    z= (int) ( (float)z / 655.35 );
    return z;
}

bool
v4l1DS_t::setHue(int z)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return false;
    }

    // set proper range
    z= (int) ( (float)z * 655.35 );

    if (z< 0)     z= 0;
    if (z> 65535) z= 65535;

    // set the requested value
    vpic.hue= z;
    if (Dioctl(deviceHandle, VIDIOCSPICT, &vpic) == -1)
    {
        return false;
    }
    return true;
}

int
v4l1DS_t::getHue(void)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return -1;
    }

    int z= vpic.hue;
    z= (int) ( (float)z / 655.35 );
    return z;
}

bool
v4l1DS_t::setContrast(int z)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return false;
    }

    // set proper range
    z= (int) ( (float)z * 655.35 );

    if (z< 0)     z= 0;
    if (z> 65535) z= 65535;

    // set the requested value
    vpic.contrast= z;
    if (Dioctl(deviceHandle, VIDIOCSPICT, &vpic) == -1)
    {
        return false;
    }
    return true;
}

int
v4l1DS_t::getContrast(void)
{
    if (Dioctl(deviceHandle, VIDIOCGPICT, &vpic) == -1)
    {
        return -1;
    }

    int z= vpic.contrast;
    z= (int) ( (float)z / 655.35 );
    return z;
}

std::string
v4l1DS_t::getStandardList(void)
{
    return std::string("PAL,NTSC,SECAM,AUTO");
}

bool
v4l1DS_t::setStandard(const char *z)
{
    int norm= -1;

    try
    {
        norm= getv4l1Norm(z);
    }
    catch (const char *e)
    {
        return false;
    }

    vchann[selectedChannel].norm= norm;
    Dioctl(deviceHandle, VIDIOCSCHAN, vchann+selectedChannel);

    return true;
}

const char *
v4l1DS_t::getStandard(void)
{
    return parsev4l1Norm(vchann[selectedChannel].norm);
}

std::string
v4l1DS_t::getFlickerFreqList(void)
{
    return std::string();
}

bool
v4l1DS_t::setFlickerFreq(const char *z)
{
    return false;
}

const char *
v4l1DS_t::getFlickerFreq(void)
{
    return NULL;
}

