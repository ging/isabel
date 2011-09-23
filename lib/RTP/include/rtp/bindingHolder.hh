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
// $Id: bindingHolder.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_binding_holder_hh__
#define __rtp_binding_holder_hh__

#include <icf2/vector.hh>
#include <icf2/sockIO.hh>

typedef unsigned int bindId_t;

class bindDescriptor_t
{
private:
    int cref;

    char     *hostId;
    char     *portId;
    bindId_t  bindId;

    inetAddr_t sin;

    int  getCRef(void) { return cref; }
    void incCRef(void) { cref++; }
    void decCRef(void) { cref--; }

public:
    bindDescriptor_t (bindId_t bId, const char *hostId, const char *portId);

    ~bindDescriptor_t(void);

    char     *getHostId(void) { return hostId; }
    char     *getPortId(void) { return portId; }
    bindId_t  getBindId(void) { return bindId; }

    inetAddr_t *getInetAddr(void) { return &sin; }

    const char *toString(char *s) const;

    friend class bindingHolder_t;
};


class bindingHolder_t
{
private:
    vector_t<bindDescriptor_t*> bindArray;

    bindId_t lastBindId;

public:
    bindingHolder_t(void);

    ~bindingHolder_t(void);

    bindId_t bind(const char *hId, const char *portId);
    bool unbind(bindId_t bId);

    const vector_t<bindDescriptor_t*> *getBindings(void) const;

    char* showBindings (void);
};

#endif

