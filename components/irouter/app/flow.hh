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
// $Id: flow.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __flow__hh__
#define __flow__hh__

#include <string>

using namespace std;


#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>

#include "webirouterConfig.hh"

//! Different flow types routed by irouter
typedef enum {
    noFlowId         =      0,	 /*!< not assigned                   */
    audioId          =  10001,	 /*!< audio flow                     */
    vumeterId        =  20001,   /*!< audio vumeter flow             */
    videoId          =  30001,   /*!< video flow                     */
    sldFtpId         =  40001,   /*!< slidesFtp flow                 */
    pointerId        =  50001,   /*!< pointer flow                   */
    shDispId         =  60001,   /*!< shared display flow            */
    irouterCtrlReqId =  70001,   /*!< internal irouter communication */
    rtcpaudioId      =  80001,	 /*!< audio flow RTCP                */
    rtcpvumeterId    =  90001,   /*!< audio vumeter RTCP flow        */
    rtcpVideoId      = 100001,   /*!< video RTCP flow                */
    rtcpsldFtpId     = 110001,   /*!< slidesFtp RTCP flow            */
    rtcppointerId    = 120001,   /*!< pointer RTCP flow              */
    rtcpShDispId     = 130001,   /*!< shared display RTCP flow       */
} flowId_e;

extern dictionary_t<int,int> * fecById;

const char *getFlowStr(flowId_e flow);

class flowBinder_t;    // forward declared


//
//! flow_t -- flow definition
//
/*! Resumes all sensible data about a given flow. The kind of
    data included in flow_t could be anything
*/

struct flow_t: public virtual item_t
{
    char     *flowName;       /*!< Flow name                    */
    flowId_e  flowId;         /*!< Flow identifier              */

private:

    //! flow_t constructor
    /*!
        \param nam  flow name
        \param id   flow identifier
        \param prio flow priority for traffic delivery
        \param life flow time life, after this time, packet is cancelled
    */
    flow_t(const char *nam, flowId_e id);



    //! flow_t destructor
    virtual ~flow_t(void);

public:

    //! For ICF2 debugging rites...
    /*!
        class name string (flow_t) to be printed when using debug messages
        \return class name string
    */
    virtual const char *className(void) const { return "flow_t"; };

    friend class flowBinder_t;
};




//
//! flowBinder_t - flow registry
//
/*! Handles flow creation and naming. All flow are supposed to be created
    from this class, this is acomplished by having a private constructor
    in 'flow_t'. It's also supposed to be only one instance of
    this class which is global to the system, this is enforced by having the
    internal dictionaries to be class members. flowBinder_t is responsible
    for keeping both dicitionaries in sync.
*/

class flowBinder_t: public virtual item_t
{
private:
    static u32 idCounter; /*!< is an internal counter used to uniquely
                               identify each flow, users have the choice
                               to use their own flow identifiers or to
                               let the system choose (the latter is not
                               currently used by irouter)
                           */

    static dictionary_t  <flowId_e, flow_t *> byId; /*!<
                                          dictionary collects flow indexed
                                          by their flow identifier
                                        */

    static dictionary_t <string, flow_t *> byName; /*!<
                                          dictionary collects flow indexed
                                          by their flow name
                                        */

public:
    flowBinder_t(void);
    virtual ~flowBinder_t(void);


    //! Create new flow and register it if a flow with that name
    //! did not previosly exist. If the flow already existed it returns a
    //! pointer to it. A flow is a data type to be routed by irouter.
    /*!
       \param nam flow name
       \param id  flow identifier
       \param prio  flow priority
       \param life  flow lifetime (microseconds)
       \return flow_t structure containing this information
       \sa flow_t()
    */
    virtual flow_t *newFlow(const char *nam, flowId_e id);



    //! Delete a flow by name.
    /*!
       \param nam flow name
       \return a boolean. Indicates whether the deletion were possible.
       \sa flow_t()
    */
    virtual bool deleteFlow(const char *nam);



    //! Delete a flow by identifier.
    /*!
       \param id flow identifier.
       \return a boolean. Indicates whether the deletion were possible.
       \sa flow_t()
    */
    virtual bool deleteFlow(flowId_e id);


    //! Perform a look up on the dictionaries and will return
    //! a pointer to the named flow (ir NULL, if no such flow).
    /*!
       \param nam flow name.
       \return flow_t structure with this flow name.
       \sa flow_t()
    */
    virtual flow_t *lookUp(const char *nam);



    //! Perform a look up on the dictionarues and will return
    //! a pointer to the identified flow (ir NULL, if no such flow).
    /*!
       \param id flow identifier.
       \return flow_t structure with this flow identifier.
       \sa flow_t()
    */
    virtual flow_t *lookUp(flowId_e id);



    //! Get a registered flow identifier list.  This list must
    //! be deleted by the caller method when it is not used.
    /*!
       \return the registered flow identifier listn
       \sa ql_t()
    */
    ql_t <flowId_e> *getFlowList(void) const;



    //! For ICF2 debugging rites...
    /*! class name string (flowBinder_t) to be printed when
        using debug messages
    */
    virtual const char *className(void) const { return "flowBinder_t";};
};


#endif
