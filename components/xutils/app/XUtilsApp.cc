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
// $Id: XUtilsApp.cc 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/sockIO.hh>
#include <icf2/stdTask.hh>

#include "XUtilsApp.hh"
#include "ctrlProtocol.hh"

#define __INSERT_COOKIE 0

#if __INSERT_COOKIE
class cookie_t: public simpleTask_t
{
public:
    cookie_t(void): simpleTask_t(100000) {};

    // comment out to avoid "tic-tac" messages
    //virtual void heartBeat(void) { ; };
};
#endif

xutilsApp_t::xutilsApp_t (int &argc, argv_t &argv)
: application_t(argc, argv)
{
    enum myOptions
    {
        optShell,

        optControlPort
    };

    optionDefList_t  opt;
    appParamList_t  *parList;

    opt
       << new optionDef_t("shell"       , optShell     )

       << new optionDef_t("@cport"      , optControlPort );

    parList= getOpt(opt, argc, argv);

    serverSock= NULL;

    for ( ; parList->len(); parList->behead())
    {
        switch(parList->head()->parId)
        {
        case optShell:
            *this<< new xutils_Interface_t(new shellIO_t(0, 1));
            break;
        case optControlPort: 
            {
                inetAddr_t addr("127.0.0.1", parList->head()->parValue);
                serverSock= new streamSocket_t(addr);
                if (serverSock->listen(addr) < 0)
                {
                    NOTIFY("Error tcp server socket:: system halted!\n");
                    abort();
                }

                *this<< new tcpServer_t<xutils_Interface_t> (*serverSock);

            }
            break;
        default:
            // se deberia lanzar una excepcion
            NOTIFY("---Ignoring unknown option: `%s'\n",
                   parList->head()->parValue
                  );
            fprintf(stderr,
                    "echoApp_t::echoApp_t()::"
                    "bogus parList received from application_t::getOpt()\n"
                   );
            abort();
        }
    }

#if __INSERT_COOKIE
   (*this) << new cookie_t;
#endif
}

xutilsApp_t::~xutilsApp_t(void)
{
}

