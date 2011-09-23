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
// $Id: clientModule.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __client_module_hh__
#define __client_module_hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>
#include <icf2/stdTask.hh>

#include "webirouterConfig.hh"
#include "flow.hh"

const int ALIVE_PERIOD= 5000000; // ALIVE packet period;

class clientBinder_t;            // forward declared

//
//! clientModule_t -- local client information
//

class clientModule_t: public simpleTask_t
{
private:
    char      *_clientId;         /*!< Internal flow client identifier */
    flowId_e   flowId;            /*!< Flow type: video, audio, pointer.. */
    dgramSocket_t *my_dgramSock;  /*!< Datagram socket to read/write data
                                       flow from local clients (video, audio,
                                       pointer... etc */

    bool watchDog; //watchDog, put it true or alive packet will be sent

#ifdef WIN32
    // In windows, sender must be different than receiver
    // due to problems with select
    dgramSocket_t *my_dgramSock2;
#endif

private:

    //! clientModule_t constructor
    //! Receive data from client, _only_. (Still not used)
    /*
        Generate a simpleTask_t to read data from client.
        \param flow struct containing flow information associated to
               this client.
        \param lPort local port to receive data from client
    */
    clientModule_t(flow_t *flow, const char *lPort);

    //! clientModule_t constructor
    //! Receive/send data from/to client.
    /*
        Generate a simpleTask_t to read/write data from/to client.
        \param flow struct containing flow information associated to
               this client.
        \param lPort local port to receive data from client
        \param rPort remote port to send data to client
        \param connectedSocket if true, only data from rPort is
               received (socket is connected to rPort), all isabel
               data flows except vumeter. Else, data from any source
               port is received (socket is not connected to rPort),
               vumeter flow.
    */
    clientModule_t(flow_t *flow, const char *lPort, const char *rPort);

public:

    //! clientModule_t destructor
    virtual ~clientModule_t(void);

    //! Callback from simpleTask_t.
    //! Used to send a periodic packet and resolve
    //! NAT troubles

    virtual void heartBeat(void);

    //! Callback from simpleTask_t. It is used when data
    //! is detected in socket io.
    /*
        \param io io_ref: input/output descriptor (client socket)
    */
    virtual void IOReady(io_ref &);


    //! Deliver data (not used)
    virtual void deliver(void);


    //! Deliver data to local client
    /*
        \param b data to deliver to the local client
        \param n data length
    */
    virtual void deliver(const void *b, int n);


    //! Returns the unique client identifier
    /*
        \return char *: unique client identifier
    */
    const char *clientId(void) const { return _clientId; }


    //! For ICF2 debugging rites...
    /*!
        class name string (clientModule_t) to be printed when
        using debug messages
        \return char *: class name string
    */
    virtual const char *className(void) const { return "clientModule_t"; }

    friend class clientBinder_t;
};


//
//! clientBinder_t -- client registry
//

/*! Handles client creation and naming. All clients are supposed to be
    created from this class, this is acomplished by having a private
    constructor in clientModule_t. It's also supposed to
    be only one instance of this class which is global to the system,
    this is enforced by having the internal dictionary to be a class member.
*/

class clientBinder_t: public virtual item_t
{
private:

    dictionary_t<string, clientModule_t *> clientDict; /*!< a class variable
                                                           which contains the
                                                           actual clients */

public:

    clientBinder_t(void);
    virtual ~clientBinder_t(void);

    //! Create new clientModule_t and register it in the clienDict dictionary
    //! using an unique client identifier.
    //! Returns a pointer to new clientModule_t.
    /*!
       \param flow flow_t struct
       \param lPort local Port to read data from this client
       \param rPort remote Port to write data to this client
       \return clientModule_t
       \sa clientModule_t(void)
    */
    virtual clientModule_t *newClient(flow_t *flow,
                                      const char *lPort,
                                      const char *rPort
                                     );

    //! Remove and destroy the named client, the boolean
    //! return value indicates whether the deletion were possible.
    /*!
       \param name unique client identifier
       \return bool
    */
    virtual bool            deleteClient(const char *name);


    //! Performs a look up on the dictionary and will
    //! return a pointer to the named client (or NULL, if not such client).
    /*!
       \param name unique client identifier
       \return clientModule_t
    */
    virtual clientModule_t *lookUp(const char *name);


    //! For ICF2 debugging rites...
    /*!
        class name string (clientBinder_t) to be printed when
        using debug messages
        \return char *: class name string
    */
    virtual const char *className(void) const {return "clientBinder_t";};
};

#endif

