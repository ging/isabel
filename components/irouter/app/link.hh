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
// $Id: link.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __link_hh__
#define __link_hh__

#include <string>

using namespace std;

#include <icf2/general.h>
#include <icf2/ql.hh>
#include <icf2/dictionary.hh>

#include "webirouterConfig.hh"
#include "output.hh"             // should not be here

#define NORMAL   0 //normal link, with targets
#define CONTROL  1 //control link, without tagets


class transApp_t;              // from transApp.h
class linkBinder_t;            // forward declared

typedef ql_t<target_t *> targetList_t;

//
//! link_t -- a network link as seen by end user
//
/*!
    A link is an abstraction of a connection.
    A link has a list of destination targets, they
    are the nodes where traffic will be copied.
    To implement the different connection models:
       - unicast mesh with traffic regulation:
            The target list contains all sites
            which share the traffic shaping function
            using the configured link bandwidth.
       - flowserver connection:
            The target list only contains a site.
            The flowserver root of this one.
       - multicast:
            The target list only contains the
            multicast group address.
*/

class link_t: public virtual item_t
{
public:

    //! Different link types
    enum linkClass_e {
        noWay,          /*!< no link */
        inputOnly,      /*!< only input link, only receives flows */
        outputOnly,     /*!< only output link, only sends flows   */
        inputOutput     /*!< input & output link,
                             both sends and receives flows */
    };

private:
    transApp_t *myApp;    /*!< pointer to application */
    linkClass_e lclass;
    bool type;
    bool shouldEcho; /*!< Boolean to indicate if echo is made in
                          this link, that is, packets received
                          in this link are copied in this link too
                          Only used when debugging... problems with
                          the irouter identifier to check loops */


    /*!< dictionary to find a target_t from its target identifier */
    dictionary_t<int, target_t *> byTargetId;

    /*!< dictionary to find the target list from the payload type (flowId_e) */
    dictionary_t<int, targetList_t *> byPayloadType;

public:

    //! Return pointer to application
    transApp_t *getApp(void) { return myApp;}

    //! Return shouldEcho state
    bool getShouldEcho(void) { return shouldEcho; }

//private:
    /*!
        link_t constructor
        \param app
        \param linkClass_e  (to be use in the future)
        \param echoBool when true, packets received in this link
                        are sent to this link (echo copy)
        \param bandwidth bandwidth to traffic shaping function
        \sa linkClass_e
    */
    link_t(transApp_t *app,
           linkClass_e lc,
           bool echoBool,
           int bandwidth= 0,
           bool typeFlag= NORMAL
          );



    //! link_t destructor
    /*! Delete all registered targets in this link.
        Remove the outputDriver_t (task which performs
        traffic shaping and packet lifeTime timeouts)
        in this link.
        \sa target_t outputDriver_t
    */
    virtual ~link_t(void);

public:

    //! Enqueue packets in this output link
    /*!
        For each registered destination target
        in this link, there is a copy of the packet
        in the flow queue of the output link.
        \param pkt packet to be enqueued
        \sa sharedPkt_ref
    */
    virtual void enqueue(sharedPkt_t * &pkt);
    virtual void copyPktToTargets(sharedPkt_t * &pkt);


    //! Add a destination target to copy the data.
    /*! Normally we use one destination target by each link, this
        is the FS connection or multicast connection.
        We were used some targets by each link when we used
        unicast mesh with traffic regulation

        \param flowId flow type = flowId_e
        \param addr destination target, IP address or hostname
        \param port data flow port
        \param mcastTTL Time-To-Life when multicast target is used
    */
    virtual int  addTarget(flowId_e flowId,
                           const char *addr,
                           int port,
                           int n,
                           int k,
                           i8 mcastTTL= 0
                          );


    //! Delete a destination target from this link.
    /*!
       \param id target identifier
    */
    virtual void delTarget(int id);

    virtual target_t *getTarget(flowId_e flowId, const char *addr);
    virtual target_t *getMcastTarget(flowId_e flowId);

    virtual targetList_t *getTargets(void);

    virtual void protectFlow(const char *addr,
                             flowId_e flowId,
                             const char *fecType,
                             int n,
                             int k
                            );


    // looks up dest target and creates a transcoder module
    virtual bool videoTranscode(u32 SSRC, char *dest, u32 BW, u32 lineFmt);

    // SSM
    int joinSSM(char *source);
    int leaveSSM(char *source);

    //! Get destination target address from the
    //! target identifier.
    /*!
        \param targetId destination target identifier
        \return inetAddr_t struct pointer containing the
                address associated to this target identifier
        \sa sharedPkt_ref
    */
    virtual inetAddr_t *getAddr(int targetId);

    //! Change bandwidth used by the traffic shaping function
    //virtual void adjustBandWidth(u32 bw) { outd->adjustBandWidth(bw); }


    //! For ICF2 debugging rites...
    /*! class name string (link_t) to be printed when using debug messages */
    const char *className(void) const { return "link_t"; };


    friend class webirouter_InterfaceDef;
    friend class linkBinder_t;
    friend class outputDriver_t;
};



//
//! linkClass_t -- utility class to parse link definitions (see above)
//
class linkClass_t
{
private:
    link_t::linkClass_e lClass;

public:
    linkClass_t(const char *s)
    {
        lClass= link_t::noWay;

        if(strcmp(s, "inputOnly")== 0)
            lClass= link_t::inputOnly;
        else if(strcmp(s, "outputOnly")== 0)
            lClass= link_t::outputOnly;
        else if(strcmp(s, "inputOutput")== 0)
            lClass= link_t::inputOutput;
    }

    //! casting operator
    /*! converts linkClass_t object to a link_t::linkClass_e one.*/
    operator link_t::linkClass_e(void) { return lClass; }
};



//
//! linkBinder_t -- link registry
//

/*! Handles link creation and naming. All links are supposed to be created
    from this class, this is acomplished by having a private constructor
    in 'link_t'. It's also supposed to be only one instance of
    this class which is global to the system, this is enforced by having the
    internal dictionary to be a class member.
*/

class linkBinder_t: public virtual item_t, public virtual collectible_t
{
private:
    transApp_t *myApp; /*!< is the transport application which owns
                            the linkBinder_t */
    static dictionary_t <string, link_t *> linkDict; /*!<
                            is a class variable which contains the
                            actual links */

public:
    linkBinder_t(transApp_t *app);

    //! linkBinder_t destructor
    virtual ~linkBinder_t(void);

    virtual ql_t<string> *getKeys(void) { return linkDict.getKeys(); }

    //! Create a link_t if a link with that name
    //! did not previously exist. If the link already existed
    //! it returns a pointer to it.
    /*!
       \param nam link name
       \param c link type (noWay/inputOnly/outputOnly/inputOutput)
       \param echoMode make echo, copy packets from a link to the
              same link.
       \param bw output bandwidth to implement traffic shaping
       \return a link associated to this name
       \sa link_t link_t::linkClass_e
    */
    virtual ql_t<link_t*> *getValues(void) { return linkDict.getValues(); }

    virtual link_t *newLink(const char *nam,
                            link_t::linkClass_e c,
                            bool echoMode,
                            int bw,
                            bool type= NORMAL
                           );



    //! Delete a link with this name. If a link with that
    //! name exists previously the method returns ok, otherwise
    //! returns false.
    /*!
       \param name link name
       \return a boolean. Indicates whether the deletion were possible.
       \sa link_t link_t::linkClass_e
    */
    virtual bool    deleteLink(const char *nam);


    //! Perform a look up on the dictionaries and will return
    //! a pointer to the named link (ir NULL, if no such link).
    /*!
       \param nam link name.
       \return link_t structure with this link name.
       \sa link_t(void)
    */
    virtual link_t *lookUp(const char *nam);


    //! For ICF2 debugging rites...
    /*!
        class name string (linkBinder_t) to be printed when using
        debug messages
    */
    virtual const char *className(void) const { return "linkBinder_t";};

    friend class smartReference_t<linkBinder_t>;
};

typedef smartReference_t<linkBinder_t> linkBinder_ref;

#endif

