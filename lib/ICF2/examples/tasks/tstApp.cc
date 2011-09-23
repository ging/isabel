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
// $Id: tstApp.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/stdTask.hh>


struct x_t: public application_t
{
    x_t(int &argc, argv_t &argv): application_t(argc, argv) {
         printf("%d %s\n", argc, argv[0]);
         argc--, argv++;

        debugMsg(dbg_App_Normal, "constructor x_t", "argc= %d", argc);
    };
};

struct y_t: public x_t
{
    y_t(int &argc, argv_t &argv): x_t(argc, argv) {
        printf("%d %s\n", argc, argv[0]);
    };
};

typedef ql_t<const char *> stringList_t;

main(int argc, char *argv[])
{
    x_t  xx(argc, argv);
    y_t  yy(argc, argv);

    xx.run();
    printf("%s\n", (stringList_t()<< "hi!"<< "bye!").head());
}
