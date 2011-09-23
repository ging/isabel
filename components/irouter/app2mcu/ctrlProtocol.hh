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
// $Id: ctrlProtocol.hh 10646 2007-08-24 12:42:18Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2007. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __control_protocol_hh__
#define __control_protocol_hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/stdTask.hh>

//Modified version for MCU adapter layer
//Many options removed
//
//
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
    // meta/santi
    //

    //! Set bandwidth for traffic shaping function
    virtual char const *ir7_x_set_bw    (int, char **)=0;

    //! Print the irouter version
    virtual char const *ir7_version     (int, char **)=0;

    //
    // protecting flows
    //

    //! Show irouter links
//    virtual char const *ir7_get_links   (int, char **)=0;


    //! Protect a flow in a link
  //  virtual char const *ir7_protect_flow (int, char **)=0;
  //  virtual char const *ir7_unprotect_flow (int, char **)=0;

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
    // meta/santi
    //

    //! Set bandwidth for traffic shaping function
    /*!
        Invoked when using the following control interface command:
        ir7_x_set_bw(linkName, bw)
    */
    virtual char const *ir7_x_set_bw(int, char **);

    //! Print the irouter version
    /*!
        Invoked when using the following control interface command:
        ir7_version()
    */
    virtual char const *ir7_version     (int, char **);

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
