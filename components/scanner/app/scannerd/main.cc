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
#include <stdio.h>
#include <stdlib.h>

#include <icf/general.h>
#include <icf/io.h>
#include <icf/task.h>
#include <icf/sched.h>
#include <icf/stdTask.h>


#include "scanner.hh"
#include "scannerApp.hh"
#include "ctrlProtocol.hh"




int
main(int argc, char **argv)
{
    scannerApp_t theApp(argc, argv);


    if(theApp.serverSock) {
        struct  sockaddr_in sin;
        int len= sizeof(sin);

        getsockname(
            theApp.serverSock->handle->sysHandle,
            (struct sockaddr *)&sin, &len
        );
 
        printf("%d\n", ntohs(sin.sin_port)); fflush(stdout);

#if 0
        if(fork()) 
            exit(0);

        else {
            freopen("/dev/tty", "a+", stdout);
            freopen("/dev/tty", "a+", stderr);
        }
#endif


    }



    theApp.run();

    return 0;
}
