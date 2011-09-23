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
//////////////////////////////////////////////////////////////////////
//
// $Id: ctrlProtocol.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include <icf2/general.h>
#include <icf2/stdTask.hh>

#include "ctrlProtocol.hh"
#include "titlesApp.hh"

binding_t<titles_InterfaceDef> titles_methodBinding[]=
{
    //
    // titles stuff
    //
    { "titles_virtual_display", &titles_InterfaceDef::titles_virtual_display},
    { "titles_load_font",       &titles_InterfaceDef::titles_load_font      },
    { "titles_text_color",      &titles_InterfaceDef::titles_text_color     },
    { "titles_set_text",        &titles_InterfaceDef::titles_set_text       },
    { "titles_erase_text",      &titles_InterfaceDef::titles_erase_text     },
    { "titles_erase_all",       &titles_InterfaceDef::titles_erase_all      },

    //
    // common stuff
    //
    { "titles_query_id",        &titles_InterfaceDef::titles_query_id      },
    { "titles_nop",             &titles_InterfaceDef::titles_nop           },
    { "titles_bye",             &titles_InterfaceDef::titles_bye           },
    { "titles_quit",            &titles_InterfaceDef::titles_quit          },
    { NULL, NULL }
};


#define APP ((titlesApp_t *)((titles_Interface_t *)this)->get_owner())


char const *
titles_InterfaceDef::titles_virtual_display(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 3);

    int width = atoi(argv[1]);
    int height= atoi(argv[2]);

    if (width <= 0)
    {
        return "ERROR: width should be positive\n";
    }
    if (height <= 0)
    {
        return "ERROR: height should be positive\n";
    }

    APP->theTxtWinMgr->virtualDisplaySize(width, height);

    return "OK\n";
}



char const *
titles_InterfaceDef::titles_load_font(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 2);

    const char *fontname= argv[1];

    if (strlen(fontname) == 0)
    {
        return "ERROR: empty font name not allowed\n";
    }

    if ( ! APP->theTxtWinMgr->LoadFont(fontname) )
    {
        sprintf(retVal, "ERROR: cannot load font <%s>\n", fontname);
        return retVal;
    }

    return "OK\n";
}



char const *
titles_InterfaceDef::titles_text_color(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 2);

    const char *color= argv[1];

    if (strlen(color) == 0)
    {
        return "ERROR: empty color not allowed\n";
    }

    if ( ! APP->theTxtWinMgr->setTextColor(color) )
    {
        sprintf(retVal, "ERROR: cannot set color <%s>\n", color);
        return retVal;
    }

    return "OK\n";
}



char const *
titles_InterfaceDef::titles_set_text(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 5);

    const char *msg= argv[1];
    int x= atoi(argv[2]);
    int y= atoi(argv[3]);
    const char *alignstr= argv[4];

    txtAlign_e align;

    if (strlen(msg) == 0)
    {
        return "ERROR: empty message not allowed\n";
    }

    if (strcasecmp(alignstr, "center") == 0)
    {
        align= TXT_CENTER;
    }
    else if (strcasecmp(alignstr, "left") == 0)
    {
        align= TXT_LEFT;
    }
    else if (strcasecmp(alignstr, "right") == 0)
    {
        align= TXT_RIGHT;
    }
    else
    {
        return "ERROR: bad alignment specification\n";
    }

    int res= APP->theTxtWinMgr->setText(msg, x, y, align);

    sprintf(retVal, "%d\n", res);

    return retVal;
}



char const *
titles_InterfaceDef::titles_erase_text(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 2);

    int msgId= atoi(argv[1]);

    if ( ! APP->theTxtWinMgr->EraseText(msgId) )
    {
        return "ERROR: bad identifier\n";
    }

    return "OK\n";
}



char const *
titles_InterfaceDef::titles_erase_all(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

    APP->theTxtWinMgr->EraseAllTexts();

    return "OK\n";
}



//
// common stuff
//
char const *
titles_InterfaceDef::titles_query_id(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

    sprintf(retVal, "0x%x\n", interID);
    return retVal;
}


char const *
titles_InterfaceDef::titles_nop(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

    return "OK\n";
}


char const *
titles_InterfaceDef::titles_bye(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

    (*APP)-(static_cast<titles_Interface_t *>(this));
    return (char const *)-1;
}


char const *
titles_InterfaceDef::titles_quit(int argc, char **argv)
{
    static char retVal[512];

    INTER_VRFY_ARGC(retVal, 1);

    APP->shutdown();

    return (char const *)-1;
}

