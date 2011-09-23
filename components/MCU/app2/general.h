/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/////////////////////////////////////////////////////////////////////////
//
// $Id: general.h 8195 2006-03-14 08:54:07Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_GENERAL_H_
#define _MCU_GENERAL_H_

#include <icf2/general.h>
#include <icf2/sockIO.hh>
#include <icf2/notify.hh>
#include <icf2/ql.hh>

#ifdef WIN32
#include <windows.h>
#endif

#if defined(WIN32)
#define RAND() rand()
#elif defined(__BUILD_FOR_LINUX)
#define RAND() random()
#else
#error "Please, initialize audioTransProcessor_t for your OS"
#endif

#ifdef __BUILD_FOR_INTEL_LINUX
// some basic types for LINUX
typedef int HRESULT;
#endif

// video constants
const int MAX_VIDEO_SOURCES= 0xff;

// network defines
const int MAX_PKT_LEN= 0xffff;
const int MTU_SIZE= 900; // Ethernet

// fec PayLoadType
const unsigned short FEC_PT = 46;

enum audioMode_e
{
    AUDIO_SWITCH_MODE,
    AUDIO_MIXER_MODE
};

enum rateMode_e
{
    CBR,
    VBR
};

enum videoMode_e
{
    VIDEO_SWITCH_MODE,
    VIDEO_TRANS_MODE,
    VIDEO_GRID_MODE,
    VIDEO_GRID2_MODE
};

enum gridMode_e
{
    GRID_AUTO,
    GRID_BORDER,
    GRID_BORDER_TR,
    GRID_PIP,
    GRID_CIRCLE
};

enum flowProcessor_e
{
    FLOW_SWITCH,
    FLOW_MIX,
    FLOW_GRID,
    FLOW_GRID2,
    FLOW_AUDIO_TRANS,
    FLOW_VIDEO_TRANS
};

enum maskType_e
{
    MASK_NONE,
    MASK_NORMAL,
    MASK_INVERTED
};

struct window_t
{
    unsigned x;
    unsigned y;
    unsigned w;
    unsigned h;
};

class videoInfo_t
{
public:

    videoMode_e videoMode;
    gridMode_e  gridMode;
    rateMode_e  rateMode;

    int         PT;
    long        BW;
    u8          Q;
    u8          FR;
    u32         SSRC;
    u16         Width;
    u16         Height;

    videoInfo_t(void);
    ~videoInfo_t(void);
};

class audioInfo_t
{
public:
    audioMode_e audioMode;
    int         PT;

    audioInfo_t(void);
    ~audioInfo_t(void);
};


class flow_t
{
public:
    int PT;
    sockaddr_storage IP;
    ql_t<u16> portList;
    u16 ID;

    flow_t(void);
    ~flow_t(void);

    bool operator==(flow_t inFlow);
    bool operator==(flow_t *inFlow);

    bool samePorts(flow_t inFlow);
};

bool addrEquals(sockaddr_storage const &IP1, sockaddr_storage const &IP2);

u16 getIndex(sockaddr_storage const &IP);

#endif

