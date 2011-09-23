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

#ifndef __ctrl_protocol_hh__
#define __ctrl_protocol_hh__


#include <icf2/stdTask.hh>

class shmSrc_InterfaceDef_t
{
public:

    int interID;

    shmSrc_InterfaceDef_t(void) { interID= (int)this; };

    virtual ~shmSrc_InterfaceDef_t(void) {
       // got error if virtual methods and no virtual destructor
    }

    //
    // sources
    //
    virtual char const *source_list                  (int, char**);
    virtual char const *source_select                (int, char**);
    virtual char const *source_get_current           (int, char**);

    virtual char const *source_frame_rate            (int, char**);

    virtual char const *source_grab_size             (int, char**);

    virtual const char *source_list_norms            (int, char**);
    virtual char const *source_set_norm              (int, char**);
    virtual char const *source_get_norm              (int, char**);
    virtual char const *source_set_saturation        (int, char**);
    virtual char const *source_get_saturation        (int, char**);
    virtual char const *source_set_brightness        (int, char**);
    virtual char const *source_get_brightness        (int, char**);
    virtual char const *source_set_hue               (int, char**);
    virtual char const *source_get_hue               (int, char**);
    virtual char const *source_set_contrast          (int, char**);
    virtual char const *source_get_contrast          (int, char**);

    //
    // common stuf
    //
    virtual char const *source_query_id              (int, char**);
    virtual char const *source_nop                   (int, char**);
    virtual char const *source_bye                   (int, char**);
    virtual char const *source_quit                  (int, char**);
};


extern binding_t<shmSrc_InterfaceDef_t> shmSrc_methodBinding[];

typedef interface_t
    <shmSrc_InterfaceDef_t, shmSrc_methodBinding> shmSrcInterface_t;

#endif
