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
// $Id: ctrlProtocol.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __CTRL_PROTOCOL_HH__
#define __CTRL_PROTOCOL_HH__

#include <icf2/stdTask.hh>

class xutils_InterfaceDef_t
{
public:

    int interID;

    xutils_InterfaceDef_t(void) {
        interID= (int)this;
    };

    char const *xutils_query_id           (int, char **);

    char const *xutils_open_display       (int, char **);
    char const *xutils_close_display      (int, char **);

    char const *xutils_get_winid          (int, char **);

    char const *xutils_map_window         (int, char **);
    char const *xutils_unmap_window       (int, char **);

    char const *xutils_set_geometry       (int, char **);

    char const *xutils_nop                (int, char **);
    char const *xutils_quit               (int, char **);
    char const *xutils_bye                (int, char **);

};


extern binding_t<xutils_InterfaceDef_t> xutils_methodBinding[];

typedef interface_t
    <xutils_InterfaceDef_t,  xutils_methodBinding> xutils_Interface_t;

#endif

