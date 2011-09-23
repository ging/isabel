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
// $Id: ctrlProtocol.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __control_protocol_hh__
#define __control_protocol_hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/stdTask.hh>


//
//! webirouter_InterfaceDef -- application control interface
//!                            abstract definition
//
/*!
   Set of control functions to configure irouter application
*/

class webirouter_InterfaceDef
{
public:
    int  interID; /*! internal control interface identifier (still not used)*/

    //! webirouter_InterfaceDef constructor
    webirouter_InterfaceDef(void) { interID= (unsigned long)this; };

    virtual ~webirouter_InterfaceDef(void) {
       // got error if virtual methods and no virtual destructor
    }

    //! Return internal interface identifier
    virtual char const *ir7_query_id(int, char **)=0;


    //! Return control interface usage
    virtual char const *ir7_help (int, char **)=0;


    //
    // link management
    //

    //! Define a new gateway link
    virtual char const *ir7_define_gateway  (int, char **)=0;

    //! Define a new link
    virtual char const *ir7_define_link  (int, char **)=0;


    //! Remove a link
    virtual char const *ir7_delete_link  (int, char **)=0;



#ifdef __FULL_IR7_INTERFACE

    // Define a new flow
    virtual char const *ir7_define_flow (int, char **)=0;


    //! Remove a flow
    virtual char const *ir7_delete_flow (int, char **)=0;


    //! Define a target associated to a link (not yet implemented)
    virtual char const *ir7_define_target(int, char **)=0;


    //! Define a new local client
    virtual char const *ir7_define_client(int, char **)=0;


    //! Define a new link as local client task (not yet implemented)
    virtual char const *ir7_link_client  (int, char **)=0;


    //! xxx_define_flow (not yet implemented)
    virtual char const *xxx_define_flow(int, char **)=0;

#endif

    //
    // meta/santi
    //

    //! Set bandwidth for traffic shaping function
    virtual char const *ir7_x_set_bw    (int, char **)=0;


    //! Add target to a link for unicast mesh with traffic regulation
    //! (deprecated)
    virtual char const *ir7_x_add_target(int, char **)=0;


    //! Remove target from a link for unicast mesh with traffic regulation
    //! (deprecated)
    virtual char const *ir7_x_del_target(int, char **)=0;


    //! Print the irouter version
    virtual char const *ir7_version     (int, char **)=0;

    //
    // protecting flows
    //

    //! Get irouter links & fec params & stats
    virtual char const *ir7_get_links       (int, char **)=0;
    virtual char const *ir7_get_links_fec   (int, char **)=0;
    virtual char const *ir7_get_links_stats (int, char **)=0;


    //! Protect a flow in a link
    virtual char const *ir7_protect_flow   (int, char **)=0;
    virtual char const *ir7_unprotect_flow (int, char **)=0;


    //
    // mixing audio
    //

    //! Mix audio flow
    virtual char const *ir7_mix_audio    (int, char **)=0;
    virtual char const *ir7_no_mix_audio (int, char **)=0;


    //
    // generating statistics
    //


    //! Activate irouter statistics
    virtual char const *ir7_do_stats     (int, char **)=0;


    //
    // video transcoder
    //
    virtual char const *ir7_video_transcode (int, char **)=0;

    //
    // save measures on/off
    //

    virtual char const *ir7_start_measures(int, char **)=0;
    virtual char const *ir7_stop_measures(int, char **)=0;


    //
    // common stuf
    //

    //! NOP -> no operation
    virtual char const *ir7_nop (int, char **)=0;


    //! Close this control interface.
    virtual char const *ir7_bye (int, char **)=0;


    //! Kill irouter application
    virtual char const *ir7_quit(int, char **)=0;
};


//
//! webirouter_methodBinding[] -- array to link control function name
//!                               to the pointer to function
//
extern binding_t<webirouter_InterfaceDef> webirouter_methodBinding[];


//
//! _priv_webirouter_Interface -- internal use
//
/*
    template to link the array webirouter_methodBinding to the concrete
    class functions.
*/
typedef interface_t
    <webirouter_InterfaceDef,
     webirouter_methodBinding> _priv_webirouter_Interface;



//
//! webirouter_Interface -- control interface implementation
//
class webirouter_Interface: public _priv_webirouter_Interface
{
public:

    //! webirouter_Interface constructor
    /*!
        \param io stream control socket
    */
    webirouter_Interface(io_ref &io): _priv_webirouter_Interface(io) {};


    //! For ICF2 debugging rites...
    /*!
        class name string (webIrouter_Interface) to be printed when using
        debug messages
        \return class name string
    */
    virtual char const *className() const { return "webirouter_Interface"; };


    //! Return internal interface identifier
    /*!
        Invoked when using the following control interface command:
        ir7_query_id()
    */
    virtual char const *ir7_query_id(int, char **);


    //! Return control interface usage
    /*!
        Invoked when using the following control interface command:
        ir7_help()
    */
    virtual char const *ir7_help(int, char **);

    //
    // link management stuff
    //

    //! Define a new gateway link
    /*!
        Invoked when using the following control interface command:
        ir7_define_gateway(linkName)
    */
    virtual char const *ir7_define_gateway(int, char **);

    //! Define a new link
    /*!
        Invoked when using the following control interface command:
        ir7_define_link(linkName, linkClass, bw)
    */
    virtual char const *ir7_define_link(int, char **);


    //! Remove a link
    /*!
        Invoked when using the following control interface command:
        ir7_delete_link(linkName)
    */
    virtual char const *ir7_delete_link(int, char **);


#ifdef __FULL_IR7_INTERFACE

    // Define a new flow
    /*!
        Invoked when using the following control interface command:
        ir7_define_flow(flowName, flowId, flowPrio)
    */
    virtual char const *ir7_define_flow(int, char **);


    // Remove a flow
    /*!
        Invoked when using the following control interface command:
        ir7_delete_flow(flowName)
    */
    virtual char const *ir7_delete_flow(int, char **);


    //! Define a target associated to a link (not yet implemented)
    virtual char const *ir7_define_target(int, char **);


    //! Define a new local client
    /*!
        Invoked when using the following control interface command:
        ir7_define_client(flowName, localPort [, remotePort])
    */
    virtual char const *ir7_define_client(int, char **);



    //! Define a new local client associated to a link (not yet implemented)
    virtual char const *ir7_link_client(int, char **);


    //
    // serious stuff
    //

    //! xxx_define_flow (not yet implemented)
    virtual char const *xxx_define_flow(int, char **);
#endif

    //
    // meta/santi
    //

    //! Set bandwidth for traffic shaping function
    /*!
        Invoked when using the following control interface command:
        ir7_x_set_bw(linkName, bw)
    */
    virtual char const *ir7_x_set_bw(int, char **);


    //! Add target to a link  for unicast mesh with traffic regulation
    //! (deprecated)
    /*!
        Invoked when using the following control interface command:
        ir7_x_add_target(linkName, dstTarget)
    */
    virtual char const *ir7_x_add_target(int, char **);



    //! Remove target from a link for unicast mesh with traffic regulation
    //! (deprecated)
    /*!
        Invoked when using the following control interface command:
        ir7_x_del_target(idTgt)
    */
    virtual char const *ir7_x_del_target(int, char **);



    //! Print the irouter version
    /*!
        Invoked when using the following control interface command:
        ir7_version()
    */
    virtual char const *ir7_version     (int, char **);


    virtual char const *ir7_get_links       (int, char **);
    virtual char const *ir7_get_links_fec   (int, char **);
    virtual char const *ir7_get_links_stats (int, char **);

    virtual char const *ir7_protect_flow    (int, char **);
    virtual char const *ir7_unprotect_flow  (int, char **);

    virtual char const *ir7_mix_audio       (int, char **);
    virtual char const *ir7_no_mix_audio    (int, char **);



    //
    // generating statistics
    //


    //! Activate irouter statistics
    /*!
        Invoked when using the following control interface command:
        ir7_do_stats(1|0)
    */
    virtual char const *ir7_do_stats     (int, char **);

    //
    // video transcoder
    //

    //! Activate irouter video transcoder
    /*!
        param: SSRC, dest, BW, Codec
    */
    virtual char const *ir7_video_transcode (int, char **);

    //
    // save measures on/off
    //

    virtual char const *ir7_start_measures(int, char **);
    virtual char const *ir7_stop_measures(int, char **);

    //
    // common stuf
    //


    //! NOP -> no operation
    /*!
        Invoked when using the following control interface command:
        ir7_nop()
    */
    virtual char const *ir7_nop(int, char **);



    //! Close this control interface.
    /*!
        Invoked when using the following control interface command:
        ir7_bye()
    */
    virtual char const *ir7_bye(int, char **);



    //! Kill irouter application
    /*!
        Invoked when using the following control interface command:
        ir7_quit()
    */
    virtual char const *ir7_quit(int, char **);
};

#endif

