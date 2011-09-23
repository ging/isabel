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


#include <icf2/general.h>
#include <icf2/stdTask.hh>


class titles_InterfaceDef
{
private:
    int interID;

public:
    titles_InterfaceDef(void) { interID= (int)this; };

    //
    // title stuff
    //
    char const *titles_virtual_display(int, char **);
    char const *titles_load_font(int, char **);
    char const *titles_text_color(int, char **);
    char const *titles_set_text(int, char **);
    char const *titles_erase_text(int, char **);
    char const *titles_erase_all(int, char **);

    //
    // common stuff
    //
    char const *titles_query_id(int, char **);
    char const *titles_nop(int, char **);
    char const *titles_bye(int, char **);
    char const *titles_quit(int, char **);
};


extern binding_t<titles_InterfaceDef> titles_methodBinding[];

typedef interface_t
    <titles_InterfaceDef, titles_methodBinding> titles_Interface_t;

#endif
