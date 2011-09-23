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
// $Id: v4luDioctl.hh 10216 2007-05-25 14:38:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __v4luDtioctl_hh__
#define __v4luDtioctl_hh__

//
// Debug ioctl
//

// ioctl debug level
// 0: no debug
// 1: all ioctl debug but VIDIOCMCAPTURE (v4l1) and VIDIOCSYNC (v4l2)
// 2: all ioctl
#define DEBUG_IOCTL 0

#ifdef HAVE_V4L1
//
// V4L1 header functions and Debugging ioctl
//
const char *parsev4l1VideoType(int t);
const char *parsev4l1ChannelFlags(int f);
const char *parsev4l1ChannelType(int t);
const char *parsev4l1Palette(int p);

const char *parsev4l1Norm(int m);
int getv4l1Norm(const char *n);

int v4lId2bpp(int i);

u32 getFCCByV4LId(int i);
int getV4LIdByFCC(u32 fmt);


void prnv4l1ioctl(unsigned long cmd, void *arg, int rc);
#endif

//
// V4L2 header functions and Debugging ioctl
//

const char *parsev4l2ChannelType(int t);
const char *parseV4L2Field(int ff);
const char *parseV4L2Colorspace(int ff);
const char *parseV4L2BufType(int ff);
const char *parseV4L2FlickerFreq(int ff);
int         getV4L2FlickerFreq(const char *ff);
u32         getFCCByV4L2Id(u32 i);
u32         getV4L2IdByFCC(u32 fmt);

void prnv4l2ioctl(unsigned long cmd, void *arg, int rc);

#endif
