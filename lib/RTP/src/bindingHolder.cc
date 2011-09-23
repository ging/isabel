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
// $Id: bindingHolder.cc 20745 2010-07-02 12:42:19Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/bindingHolder.hh>

bindDescriptor_t::bindDescriptor_t (bindId_t bId,
                                    const char *hostId,
                                    const char *portId
                                   )
: sin(hostId, portId, serviceAddr_t::DGRAM)
{
    cref= 1;
    this->hostId= strdup(hostId);
    this->portId= strdup(portId);
    bindId= bId;
}

bindDescriptor_t::~bindDescriptor_t(void)
{
    free (hostId);
    free (portId); 
}

const char *
bindDescriptor_t::toString(char *s) const
{
    sprintf(s, "bId=%d(%d), [%s.%s]", bindId, cref, hostId, portId);

    return s;
}

bindingHolder_t::bindingHolder_t(void)
: lastBindId(0)
{
}

bindingHolder_t::~bindingHolder_t(void)
{
    while (bindArray.size() > 0)
    {
        int pos= bindArray.size() - 1;
        bindDescriptor_t *bd= bindArray.elementAt(pos);

        delete bd;

        bindArray.remove(pos);
    }
}

bindId_t
bindingHolder_t::bind(const char *hostId, const char *portId)
{
    bindId_t bId= lastBindId;

    for (int i= 0; i < bindArray.size(); i++)
    {
        bindDescriptor_t *bd= bindArray.elementAt(i);

        if (   (strcmp(bd->getHostId(), hostId) == 0)
            && (strcmp(bd->getPortId(), portId) == 0)
           )
        {
            bd->incCRef();
            return bd->getBindId();
        }
    }

    bindDescriptor_t *newbd= new bindDescriptor_t(bId, hostId, portId);

    lastBindId++;
    bindArray.add(newbd);

    return newbd->getBindId();
}

bool
bindingHolder_t::unbind(bindId_t bId)
{
    for (int i= 0; i < bindArray.size(); i++)
    {
        bindDescriptor_t * bd= bindArray.elementAt(i);

        if (bd->getBindId() == bId)
        {
            bd->decCRef();
            if (bd->getCRef() == 0)
            {
                delete bd;
                bindArray.remove(i);
            }
            return true;
        }
    }

    return false;
}

const vector_t<bindDescriptor_t*> *
bindingHolder_t::getBindings(void) const
{
    return &bindArray;
}

char*
bindingHolder_t::showBindings(void)
{
    char aux[256];
    char *retVal= (char*)malloc(10*1024*sizeof(char));
    retVal[0]='\0';

    for (int i= 0; i < bindArray.size(); i++)
    {
        bindDescriptor_t *bd= bindArray.elementAt(i);

        bd->toString(aux);
        if (strlen(retVal) + strlen(aux) < 10*1024)
        {
            sprintf (retVal, "%s\n %s", retVal, aux);
        }
    }

    if (strlen(retVal) == 0)
    {
        sprintf(retVal, "No bindings\n");
    }
    else
    {
        sprintf(retVal, "%s\n", retVal);
    }

    return retVal;
}

