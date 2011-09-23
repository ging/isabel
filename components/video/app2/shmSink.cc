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
// $Id: shmSink.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <icf2/notify.hh>

#include "shmSink.hh"

shmSink_t::shmSink_t(int shmId, int shmSize)
{
    int newshmid;

    creator=true;
    shmid = shmget(shmId, shmSize, 0777 | IPC_CREAT | IPC_EXCL);

    if (shmid == -1)
    {
        creator=false;
        if (errno == EEXIST)
        {
            NOTIFY("shmSink_t::shmSink_t: Shared Memory Segment "
                   "already exist (Maybe an incorrect IsabelShutdown?)\n"
                  );
        }
        else
        {
            NOTIFY("shmSink_t::shmSink_t: error creating shmget(%d): %s\n",
                   errno, strerror(errno)
                  );
            exit(1);
        }
        NOTIFY("shmSink_t::shmSink_t: Trying to get ID as a client: ");
        newshmid = shmget(shmId, shmSize, 0);
        if (newshmid == -1)
        {
            NOTIFY("FAIL\n");
            NOTIFY("shmSink_t::shmSink_t: error in shmget(%d): %s\n",
                   errno, strerror(errno)
                  );
            exit(1);
        }
        shmid= newshmid;
        NOTIFY("OK\n");
    }
    else
    {
        NOTIFY("shmSink_t::shmSink_t: Created SHMemory\n");
    }
    shmPtr = (u8 *)shmat(shmid, 0, 0);
    if (shmPtr == (u8 *) -1)
    {
        NOTIFY("shmSink_t::shmSink_t: error in shmat(%d): %s\n",
               errno, strerror(errno)
              );
        shmdt(shmPtr);
        exit(1);
    }
    NOTIFY("shmSink_t::shmSink_t: Attached correctly to ID:%08X size:%08X\n",
           shmId,
           shmSize
          );
}

shmSink_t::~shmSink_t(void)
{
    struct shmid_ds cont;

    shmdt(shmPtr);

    shmctl(shmid, IPC_STAT, &cont);
    if (cont.shm_nattch == 0)
    {
        shmctl(shmid, IPC_RMID, &cont);
    }
}

void
shmSink_t::putImage(image_t *img)
{
    struct imageProperties ip;

    ip.width = img->getWidth();
    ip.height = img->getHeight();
    ip.fmt = img->getFormat();
    ip.size = vFrameSize(ip.width, ip.height, ip.fmt);
    memcpy(shmPtr, &ip, sizeof(ip));
    memcpy(shmPtr + sizeof(ip), img->getBuff(), ip.size);
}

