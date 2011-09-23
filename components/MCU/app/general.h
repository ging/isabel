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
// $Id: general.h 20759 2010-07-05 10:30:36Z gabriel $
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

#ifdef __BUILD_FOR_INTEL_LINUX
// some basic types for LINUX
typedef int HRESULT;
#endif

// network defines
#define MAX_PKT_LEN  0xffff
#define MTU_SIZE     900 // Ethernet

// fec PayLoadType
const unsigned short FEC_PT = 46;

enum audioMode_e
{
    AUDIO_SWITCH_MODE,
    AUDIO_SWITCH_SSRC_MODE,
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
    VIDEO_SSRC_CHANGE_MODE,
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
    FLOW_SWITCH_SSRC,
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
    int            SSRC;

    audioInfo_t(void);
    ~audioInfo_t(void);
};


class flow_t
{
public:
    int PT;
    ql_t<u16> portList;
    u16 ID;

    flow_t(void);
    ~flow_t(void);

    bool operator==(flow_t inFlow);
    bool operator==(flow_t *inFlow);

    bool samePorts(flow_t inFlow);
};

#if 0
bool
addrEquals(sockaddr_storage const &IP1, sockaddr_storage const &IP2);
#endif

bool
containsPort(ql_t<u16> portList, int port);

static inline u16
getIndex(inetAddr_t const &addr)
{
    // hash table
    u16 position = 0;

    sockaddr_in  * IP4 = NULL;
    sockaddr_in6 * IP6 = NULL;

    switch (addr.family)
    {
    case AF_INET:
        {
            IP4 = (sockaddr_in *)(&addr.saddr.ssin);
            position = (u16)((((u8*)&IP4->sin_addr.s_addr)[0])<<0) ^
                       (u16)((((u8*)&IP4->sin_addr.s_addr)[1])<<1 | (((u8*)&IP4->sin_addr.s_addr)[1])>>7)^
                       (u16)((((u8*)&IP4->sin_addr.s_addr)[2])<<2 | (((u8*)&IP4->sin_addr.s_addr)[2])>>6)^
                       (u16)((((u8*)&IP4->sin_addr.s_addr)[3])<<3 | (((u8*)&IP4->sin_addr.s_addr)[3])>>5);
            break;
        }
    case AF_INET6:
        {
            IP6 = (sockaddr_in6 *)(&addr.saddr.ssin6);
            position = (u16)(IP6->sin6_addr.s6_addr[0x0])       ^
                       (u16)(IP6->sin6_addr.s6_addr[0x1]<< 1   | IP6->sin6_addr.s6_addr[0x1] >>7)^
                       (u16)(IP6->sin6_addr.s6_addr[0x2]<< 2   | IP6->sin6_addr.s6_addr[0x2] >>6)^
                       (u16)(IP6->sin6_addr.s6_addr[0x3]<< 3   | IP6->sin6_addr.s6_addr[0x3] >>5)^
                       (u16)(IP6->sin6_addr.s6_addr[0x4]<< 4   | IP6->sin6_addr.s6_addr[0x4] >>4)^
                       (u16)(IP6->sin6_addr.s6_addr[0x5]<< 5   | IP6->sin6_addr.s6_addr[0x5] >>3)^
                       (u16)(IP6->sin6_addr.s6_addr[0x6]<< 6   | IP6->sin6_addr.s6_addr[0x6] >>2)^
                       (u16)(IP6->sin6_addr.s6_addr[0x7]<< 7   | IP6->sin6_addr.s6_addr[0x7] >>1)^
                       (u16)(IP6->sin6_addr.s6_addr[0x8]<< 0   | IP6->sin6_addr.s6_addr[0x8] >>0)^
                       (u16)(IP6->sin6_addr.s6_addr[0x9]<< 1   | IP6->sin6_addr.s6_addr[0x9] >>7)^
                       (u16)(IP6->sin6_addr.s6_addr[0xa]<< 2   | IP6->sin6_addr.s6_addr[0xa] >>6)^
                       (u16)(IP6->sin6_addr.s6_addr[0xb]<< 3   | IP6->sin6_addr.s6_addr[0xb] >>5)^
                       (u16)(IP6->sin6_addr.s6_addr[0xc]<< 4   | IP6->sin6_addr.s6_addr[0xc] >>4)^
                       (u16)(IP6->sin6_addr.s6_addr[0xd]<< 5   | IP6->sin6_addr.s6_addr[0xd] >>3)^
                       (u16)(IP6->sin6_addr.s6_addr[0xe]<< 6   | IP6->sin6_addr.s6_addr[0xe] >>2)^
                       (u16)(IP6->sin6_addr.s6_addr[0xf]<< 7   | IP6->sin6_addr.s6_addr[0xf] >>1);
             break;
        }
    default:
        NOTIFY("getIndex::Unknown family [%d]\n",addr.family);
        abort();
    }
    return position;
}

#endif

