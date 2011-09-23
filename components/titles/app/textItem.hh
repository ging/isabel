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
// $Id: textItem.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __text_item_hh__
#define __text_item_hh__

#include <string>

using namespace std;

#include <string.h>

#include <icf2/ql.hh>

class textItem_t
{
private:
    ql_t<string> *text;

public:

    textItem_t(const char *msg)
    {
        char *msgdup= strdup(msg);
        char *a= msgdup, *b;

        text= new ql_t<string>;

        while ((b= strchr(a, ':')) != NULL)
        {
            b[0]='\0';
            b++;
            text->insert(string(a));
            a= b;
        }
        text->insert(string(a));

        free(msgdup);
    }

    virtual ~textItem_t(void)
    {
    }

    ql_t<string> *getText (void) { return text; }
};

#endif
