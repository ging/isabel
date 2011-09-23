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
// $Id: general.cc 7260 2005-08-29 09:43:45Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/notify.hh>

#include "general.h"

flow_t::flow_t(void)
{
    PT= -1;
    memset(&IP,0,sizeof(sockaddr_storage));
}
flow_t::~flow_t(void)
{

}

bool
flow_t::operator==(flow_t inFlow)
{
    if (addrEquals(IP,inFlow.IP) &&
        samePorts(inFlow) &&
        inFlow.PT   == PT)
    {
        return true;
    }else{
        return false;
    }
}

bool
flow_t::operator==(flow_t *inFlow)
{
    if (addrEquals(IP,inFlow->IP) &&
        samePorts(*inFlow) &&
        inFlow->PT   == PT)
    {
        return true;
    }else{
        return false;
    }
}

bool
flow_t::samePorts(flow_t inFlow)
{
    for (ql_t<u16>::iterator_t i = inFlow.portList.begin();
         i != inFlow.portList.end();
         i++
        )
    {
        bool portFound = false;
        u16 port = static_cast<u16>(i);
        for (ql_t<u16>::iterator_t j = portList.begin();
             j != portList.end();
             j++
            )
        {
            u16 port2= static_cast<u16>(i);
            if (port2 == port)
            {
                portFound = true;
                break;
            }
        }
        if ( ! portFound)
        {
            return false;
        }
    }

    return true;
}

bool
addrEquals(sockaddr_storage const & IP1, sockaddr_storage const & IP2)
{
    sockaddr_in  *IP4  = NULL;
    sockaddr_in  *_IP4 = NULL;
    sockaddr_in6 *IP6  = NULL;
    sockaddr_in6 *_IP6 = NULL;

    if (IP1.ss_family != IP2.ss_family)
    {
        NOTIFY("addrEquals:: comparing two different families [%d,%d]. Aborting...!\n",
               IP1.ss_family,IP2.ss_family);
        NOTIFY("Please, work only with one family address at the same session!\n");
        abort();
    }

    switch (IP1.ss_family)
    {
    case AF_INET:
        {
            IP4  = (sockaddr_in *)(&IP1);
            _IP4 = (sockaddr_in *)(&IP2);
            if (_IP4->sin_addr.s_addr == IP4->sin_addr.s_addr)
            {
                return true;
            }
            break;
        }
    case AF_INET6:
        {
            IP6  = (sockaddr_in6 *)(&IP1);
            _IP6 = (sockaddr_in6 *)(&IP2);
            if (memcmp(&_IP6->sin6_addr,
                       &IP6->sin6_addr,
                       sizeof(in6_addr)) == 0
               )
            {
                return true;
            }
            break;
        }
    default:
        {
            NOTIFY("addrEquals::Unknown family [%d]. Aborting... \n",
                   IP1.ss_family
                  );
            abort();
        }
    }

    return false;
}

u16
getIndex(sockaddr_storage const &IP)
{
    //hash table
    u16 position = 0;

    sockaddr_in  * IP4 = NULL;
    sockaddr_in6 * IP6 = NULL;

    switch (IP.ss_family)
    {
    case AF_INET:
        {
            IP4 = (sockaddr_in *)(&IP);
            position = (u16)((((u8*)&IP4->sin_addr.s_addr)[0])<<0) ^
                       (u16)((((u8*)&IP4->sin_addr.s_addr)[1])<<1 | (((u8*)&IP4->sin_addr.s_addr)[1])>>7)^
                       (u16)((((u8*)&IP4->sin_addr.s_addr)[2])<<2 | (((u8*)&IP4->sin_addr.s_addr)[2])>>6)^
                       (u16)((((u8*)&IP4->sin_addr.s_addr)[3])<<3 | (((u8*)&IP4->sin_addr.s_addr)[3])>>5);
            break;
        }
    case AF_INET6:
        {
            IP6 = (sockaddr_in6 *)(&IP);
            position = (u16)(IP6->sin6_addr.s6_addr[0x0])       ^
                       (u16)(IP6->sin6_addr.s6_addr[0x1]<< 1   | IP6->sin6_addr.s6_addr[0x1] >>15)^
                       (u16)(IP6->sin6_addr.s6_addr[0x2]<< 2   | IP6->sin6_addr.s6_addr[0x2] >>14)^
                       (u16)(IP6->sin6_addr.s6_addr[0x3]<< 3   | IP6->sin6_addr.s6_addr[0x3] >>13)^
                       (u16)(IP6->sin6_addr.s6_addr[0x4]<< 4   | IP6->sin6_addr.s6_addr[0x4] >>12)^
                       (u16)(IP6->sin6_addr.s6_addr[0x5]<< 5   | IP6->sin6_addr.s6_addr[0x5] >>11)^
                       (u16)(IP6->sin6_addr.s6_addr[0x6]<< 6   | IP6->sin6_addr.s6_addr[0x6] >>10)^
                       (u16)(IP6->sin6_addr.s6_addr[0x7]<< 7   | IP6->sin6_addr.s6_addr[0x7] >>9)^
                       (u16)(IP6->sin6_addr.s6_addr[0x8]<< 8   | IP6->sin6_addr.s6_addr[0x8] >>8)^
                       (u16)(IP6->sin6_addr.s6_addr[0x9]<< 9   | IP6->sin6_addr.s6_addr[0x9] >>7)^
                       (u16)(IP6->sin6_addr.s6_addr[0xa]<< 10  | IP6->sin6_addr.s6_addr[0xa] >>6)^
                       (u16)(IP6->sin6_addr.s6_addr[0xb]<< 11  | IP6->sin6_addr.s6_addr[0xb] >>5)^
                       (u16)(IP6->sin6_addr.s6_addr[0xc]<< 12  | IP6->sin6_addr.s6_addr[0xc] >>4)^
                       (u16)(IP6->sin6_addr.s6_addr[0xd]<< 13  | IP6->sin6_addr.s6_addr[0xd] >>3)^
                       (u16)(IP6->sin6_addr.s6_addr[0xe]<< 14  | IP6->sin6_addr.s6_addr[0xe] >>2)^
                       (u16)(IP6->sin6_addr.s6_addr[0xf]<< 15  | IP6->sin6_addr.s6_addr[0xf] >>1);
             break;
        }
    default:
        NOTIFY("getIndex::Unknown family [%d]\n",IP.ss_family);
        abort();
    }
    return position;
}


