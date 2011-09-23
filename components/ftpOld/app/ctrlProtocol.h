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
// $Id: ctrlProtocol.h 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#ifndef __CTRL_PROTOCOL_H__
#define __CTRL_PROTOCOL_H__


#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/stdTask.hh>




class cftp_InterfaceDef
{
 
  public:
    
    const char *  cftp_create_channel  (int, char **);
    const char *  cftp_delete_channel  (int, char **);

    const char *  cftp_bind            (int, char **);
    const char *  cftp_unbind          (int, char **);

    const char *  cftp_send            (int, char **);
    const char *  cftp_recv            (int, char **);

    const char *  cftp_end             (int, char **);

    const char *  cftp_bw              (int, char **);

    const char *  cftp_nop             (int, char **);
    const char *  cftp_quit            (int, char **);

};


extern binding_t<cftp_InterfaceDef> cftp_methodBinding[];

typedef interface_t <cftp_InterfaceDef,  cftp_methodBinding> cftp_Interface;


#endif
