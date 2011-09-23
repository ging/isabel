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
// $Id: configFile.hh 8486 2006-05-11 14:31:50Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#ifndef __irouter__config__file__
#define __irouter__config__file__

#include <icf2/item.hh>
#include <icf2/stdTask.hh>

class transApp_t;


class confIrouter_InterfaceDef
{
public:
    virtual ~confIrouter_InterfaceDef(void) {
       // got error if virtual methods and no virtual destructor
    }

    virtual char const *irouter_ctrl_port(int, char **)=0;

     // Multicast parameters
     virtual char const *irouter_multicast(int, char **)=0;
     virtual char const *irouter_multicast_ttl(int, char **)=0;
     virtual char const *irouter_multicast_bw(int, char **)=0;
     virtual char const *irouter_multicast_group(int, char **)=0;

     // Connection to/from other flowservers
     virtual char const *irouter_link_mgr(int, char **)=0;
     virtual char const *irouter_connect_flowserver(int, char **)=0;
     virtual char const *irouter_down_stream_bw(int, char **)=0;
     virtual char const *irouter_up_stream_bw(int, char **)=0;

     // Flow definition
     virtual char const *irouter_define_flow(int, char **)=0;

     // Local client configuration
     virtual char const *irouter_define_local_client(int, char **)=0;
};


extern binding_t<confIrouter_InterfaceDef> confIrouter_methodBinding[];


typedef interface_t<confIrouter_InterfaceDef,
                    confIrouter_methodBinding>  __confIrouter_Interface_t;

class confIrouter_Interface_t: public virtual __confIrouter_Interface_t
{
    private:
        transApp_t *__app;
    public:
        confIrouter_Interface_t(transApp_t *app, char *fileName)
        :__confIrouter_Interface_t(fileName), __app(app) {}

        transApp_t *getApp() { return __app;}

        virtual char const *irouter_ctrl_port(int, char **);

        // Multicast parameters
        virtual char const *irouter_multicast(int, char **);
        virtual char const *irouter_multicast_ttl(int, char **);
        virtual char const *irouter_multicast_bw(int, char **);
        virtual char const *irouter_multicast_group(int, char **);

        // Connection to/from other flowservers
        virtual char const *irouter_link_mgr(int, char **);
        virtual char const *irouter_connect_flowserver(int, char **);
        virtual char const *irouter_down_stream_bw(int, char **);
        virtual char const *irouter_up_stream_bw(int, char **);

        // Flow definition
        virtual char const *irouter_define_flow(int, char **);

        // Local client configuration
        virtual char const *irouter_define_local_client(int, char **);
};


#endif
