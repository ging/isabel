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
// $Id: centralSwitch.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __central_switch_hh__
#define __central_switch_hh__

#include <icf2/item.hh>
#include <icf2/dictionary.hh>

#include "pkt.hh"
#include "link.hh"
#include "clientModule.hh"


//
//! linkList_t -- a list of links
//
typedef ql_t<link_t*> linkList_t;


//
//! clientModuleList_t -- a list of clients
//
typedef ql_t<clientModule_t*> clientModuleList_t;



//
//! downstreamRoutingInfo_t
//
/*! specifies, for every payload type (flowId_e), defined the apropiate link
   and scheduling priority. All downstreamRoutingInfo is intended
   to be kept at a downstreamRoutingDict which is keyed by
   the payloadType (flowId_e).
*/

class downstreamRoutingInfo_t
{
public:
    linkList_t links;   /*!< link list, to copy output data */

    //! downstreamRoutingInfo_t constructor
    /*!
        \param pr flow priority
    */
    downstreamRoutingInfo_t(void) { }
};
typedef dictionary_t<u32, downstreamRoutingInfo_t *> downstreamRoutingDict_t;



//
//! upstreamRoutingInfo_t
//
struct upstreamRoutingInfo_t
{
    clientModuleList_t clients; /*!< local client list to deliver data */
};
typedef dictionary_t<u32, upstreamRoutingInfo_t *> upstreamRoutingDict_t;



//
//! central switch -- the heart of the beast
//
/*
   Route data between different entities
*/
class centralSwitch_t: public virtual item_t
{
private:
    downstreamRoutingDict_t downstreamRoutingDict; /*!< downstream info */
    upstreamRoutingDict_t   upstreamRoutingDict;   /*!< upstream info   */
public:
    transApp_t *myApp;                        /*!< pointer to scheduler */

    //! centralSwitch_t constructor
    /*!
        \param app pointer to scheduler
    */
    centralSwitch_t(transApp_t *app);


    //! centralSwitch_t destructor
    virtual ~centralSwitch_t(void);


    //
    // Funciones de control del encaminamiento
    //

    //! Insert downstreamInfo_t associated to a flowId.
    /*
        \param flowId flow type
        \param dri downstreamRoutingInfo which contains the linkList
               to copy output data
        \return bool
        \sa downstreamRoutingInfo_t
    */
    bool insertDri(int flowId, downstreamRoutingInfo_t *dri);


    //! Remove downstreamInfo_t associated to a flowId.
    /*
        \param flowId flow type
        \return bool
        \sa downstreamRoutingInfo_t
    */
    bool removeDri(int flowId);


    //! Add a new link to the listLink of a downstreamInfo_t
    /*
        \param flowId flow type
        \param l new link to copy output data
        \return bool
        \sa downstreamRoutingInfo_t link_t
    */
    bool addLinkToDri(int flowId, link_t *l);


    //! Remove link from the listLink of a downstreamInfo_t
    /*
        \param flowId flow type
        \param l link to delete from listLink
        \return bool
        \sa downstreamRoutingInfo_t link_t
    */
    bool removeLinkFromDri(int flowId, link_t *l);


    //
    // Funciones para la gestion de clientes (uri)
    //

    //! Insert upstreamInfo_t associated to a flowId.
    /*
        \param flowId flow type
        \param dri upstreamRoutingInfo which contains the clientList
               to copy the data locally.
        \return bool
        \sa upstreamRoutingInfo_t
    */
    bool insertUri(int flowId, upstreamRoutingInfo_t *uri);


    //! Remove upstreamInfo_t associated to a flowId.
    /*
        \param flowId flow type
        \return bool
        \sa upstreamRoutingInfo_t
    */
    bool removeUri(int flowId);


    //! Get upstreamInfo_t associated to a flow type (flowId_e)
    /*
        \param flowId flow type
        \return upstreamRoutingInfo_t
    */
    upstreamRoutingInfo_t *getUri(int flowId) const
    {
         return upstreamRoutingDict.lookUp(flowId);
    }


    //! Copy packet to local client
    /*
        \param pkt data received from local client
    */
    virtual void pktToClient(sharedPkt_t * &pkt);

    //! Process data from local client and
    //! copy to the links associated to this flow type
    //! (downstreamRoutingInfo_t)
    /*
        \param pkt data received from local client
    */
    virtual void clientData(sharedPkt_t * &pkt);


    //! Process data from network and
    //! copy to the links associated to this flow type
    //! (downstreamRoutingInfo_t) and to the local clientList
    //! (upstreamRoutingInfo_t)
    /*
        \param pkt data received from network
    */
    virtual void networkData(sharedPkt_t * &pkt);
    virtual void audioToMix(sharedPkt_t * &pkt);

    //! For ICF2 debugging rites...
    /*!
        class name string (centralSwitch_t) to be printed when using
        debug messages
        \return class name string
    */
    virtual const char *className(void) const { return "centralSwitch_t"; };
};

#endif

