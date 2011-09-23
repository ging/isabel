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
// $Id: demux.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_DEMUX_H_
#define _MCU_DEMUX_H_

#include <icf2/vector.hh>
#include <icf2/stdTask.hh>
#include <map>

#include "flowProcessor.h"

class flowDesc_t
{
public:
    int                      PT;   // Payload
    ql_t<flowProcessor_t *>  flowProcessorList;
    RTPFecReceiver_t        *RTPFecReceiver;

    flowDesc_t(u8 nPT, u8 fecPT);
    ~flowDesc_t(void);
};

// save all session info
class addrDesc_t
{
private:
    inetAddr_t        addr; // user IP
    u16               ID;   // user ID
    vector_t<flowDesc_t*> flowDescArray;

public:

    addrDesc_t(void);
    ~addrDesc_t(void);

    friend class portListener_t;
};

#define MAX_USERS 0xffff

class portListener_t: public simpleTask_t
{
private:
    u8 data[MAX_PKT_LEN]; // auxiliar for reading

protected:
    // Input data
    virtual void IOReady(io_ref &io);

private:
    bool natPort;
    addrDesc_t *tempAddrDesc;
    int sessionID;

    std::map<inetAddr_t,addrDesc_t*> addrList; // IP list
    dgramSocket_t *socket;

public:
    portListener_t(unsigned int port);
    portListener_t(unsigned int port, bool isForNat);
    virtual ~portListener_t(void);

    // put flow : asigns flow processor for an IP/PT pair
    // substitutes old flow processor if exists
    HRESULT putFlow(flow_t inFlow,
                    ql_t<flowProcessor_t *> flowProcessorList,
                    inetAddr_t const &addr
                   );

    HRESULT deleteFlow(flow_t inFlow, inetAddr_t const &addr);

    HRESULT joinSSMGroup(inetAddr_t const &addr, inetAddr_t const &groupAddr);
    HRESULT leaveSSMGroup(inetAddr_t const &addr, inetAddr_t const &groupAddr);

    // true if addrList is empty.
    bool isEmpty(void);
    unsigned int getPort(void);
};


class demux_t
{
private:

    vector_t<portListener_t *> portListenerArray;

public:

    demux_t(void);
    ~demux_t(void);

    // flow Works
    HRESULT putFlow(flow_t inFlow,
                    ql_t<flowProcessor_t *> flowProcessor,
                    bool flowNAT,
                    inetAddr_t const &addr
                   );

    HRESULT deleteFlow(flow_t inFlow,inetAddr_t const &addr);

    HRESULT joinSSMGroup(inetAddr_t const &addr, inetAddr_t const &groupAddr);
    HRESULT leaveSSMGroup(inetAddr_t const &addr, inetAddr_t const &groupAddr);
};

#endif

