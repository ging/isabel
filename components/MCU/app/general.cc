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
// $Id: general.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "general.h"
#include <icf2/notify.hh>

flow_t::flow_t(void)
{
    PT= -1;
    ID= -1;
}
flow_t::~flow_t(void)
{

}

bool
flow_t::operator==(flow_t inFlow)
{
    return inFlow.ID == ID && samePorts(inFlow) && inFlow.PT == PT;
}

bool
flow_t::operator==(flow_t *inFlow)
{
    return inFlow->ID == ID && samePorts(*inFlow) && inFlow->PT == PT;
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

#if 0
bool
addrEquals(sockaddr_storage const & IP1, sockaddr_storage const & IP2)
{
     sockaddr_in  * IP4  = NULL;
     sockaddr_in  * _IP4 = NULL;
     sockaddr_in6 * IP6  = NULL;
     sockaddr_in6 * _IP6 = NULL;

     if (IP1.ss_family != IP2.ss_family)
     {
         NOTIFY("addrEquals:: comparing two different families [%d,%d]. Aborting...!\n",
                IP1.ss_family,IP2.ss_family);
         NOTIFY("Please, work only with one family address at the same session!\n");
         //abort();
         return false;
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
                               sizeof(in6_addr))==0)
                    {
                        return true;
                    }
                    break;
                }
                default:
                {
                    NOTIFY("addrEquals::Unknown family [%d]. Aborting... \n",
                            IP1.ss_family);
                    //abort();
                }
            }
            return false;
}
#endif

bool
containsPort(ql_t<u16> portList,int port)
{
    for (ql_t<u16>::iterator_t i = portList.begin();
         i != portList.end();
         i++
        )
    {
        u16 participantPort = static_cast<u16>(i);
        if (participantPort == port)
            return true;
    }
    return false;
}


