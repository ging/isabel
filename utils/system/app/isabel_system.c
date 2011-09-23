/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void
handler(int)
{
    kill(-getpid(), SIGTERM);
}


int
main(int argc, char *argv[])
{
   pid_t pid;

   if(argc> 1) {
    pid = getpid();
    setpgid(pid,pid);
    signal(SIGTERM, handler);
    signal(SIGCHLD, handler);
    system(argv[1]);
   }
   return 0;
}

