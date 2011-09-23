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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#include "rfbSource.hh"
#include <string>

rfbSource_t::rfbSource_t(sourceDescriptor_ref desc, const char *url)
: theSourceDescriptor(desc)
{
    if (url==NULL || strlen(url) == 0)
    {
        throw "Undefined URL";
    }

    image = NULL;
    this->url = strdup(url);
    std::string addr = url;
    std::string port;
    std::string pass;

    int port_token = addr.find(":");
    int pass_token = addr.find("@");

    // if we specify port and pass
    if (port_token != std::string::npos &&
        pass_token != std::string::npos
       )
    {
        pass = addr.substr(pass_token+1);
        port = addr.substr(port_token+1,pass_token-port_token-1);
        addr = addr.substr(0,port_token);
    }
    else if (pass_token != std::string::npos &&
             port_token == std::string::npos
            )
    {
        pass = addr.substr(pass_token+1);
        port = "0";
        addr = addr.substr(0,pass_token);
    }
    else if (port_token != std::string::npos &&
              pass_token == std::string::npos
            )
    {
        pass = "";
        port = addr.substr(port_token+1);
        addr = addr.substr(0,port_token);
    }

    int iport = atoi(port.c_str());
    vnc = new libvnc::CVncConn(addr,iport,pass);
    vnc->Run();

    if ( ! vnc->IsReady())
    {
        throw "Can't connect to RFB server";
    }
}

rfbSource_t::~rfbSource_t(void)
{
    free(url);
}

bool
rfbSource_t::setFrameRate(double fps)
{
    if (fps > 20.0)
    {
        fps = 20.0;
    }
    vnc->SetPeriod((int)1000.0/fps);

    return true;
}

double
rfbSource_t::getFrameRate(void)
{
    double fps = 1000 / vnc->GetPeriod();

    return fps;
}

bool
rfbSource_t::setGrabSize(unsigned width, unsigned height)
{
    return false;
}

void
rfbSource_t::getGrabSize(unsigned *width, unsigned *height)
{
    *width = vnc->GetImage()->GetW();
    *height = vnc->GetImage()->GetH();
}

const char *
rfbSource_t::getInputPort(void)
{
    return url;
}

bool
rfbSource_t::setGrabFormat(u32 fmt)
{
    return false;
}

u32
rfbSource_t::getGrabFormat(void)
{
    return RGB24_FORMAT;
}

image_t*
rfbSource_t::getImage(void)
{
    if (vnc->IsReady())
    {
        int len = 0;
        if (vnc->GetImage()->IsUpdated())
        {
            // free last image
            if (image)
            {
                delete[] image;
            }
            // Get image
            int len = vnc->GetImage()->GetImage(&image,libvnc::CImage::RGB24);
        }
        if (image)
        {
            // Get current time
            struct timeval currentTime;
            gettimeofday (&currentTime, NULL);
            u32 timestamp = (currentTime.tv_sec*1000000) + currentTime.tv_usec;
            image_t *img = new image_t(image,
                                       len,
                                       RGB24_FORMAT,
                                       vnc->GetImage()->GetW(),
                                       vnc->GetImage()->GetH(),
                                       timestamp
                                      );
            return img;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

