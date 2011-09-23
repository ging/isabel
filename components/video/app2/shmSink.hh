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
// $Id: shmSink.hh 20770 2010-07-07 13:19:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __shm_sink_hh__
#define __shm_sink_hh__

#include <icf2/smartReference.hh>

#include <vUtils/image.h>

struct imageProperties
{
    int width;
    int height;
    u32 fmt;
    int size;
};

class shmSink_t: public virtual collectible_t
{

private:
    u8 *shmPtr;
    int shmid;
    bool creator;

public:
    shmSink_t(int shmId, int shmSize);

    virtual ~shmSink_t(void);

    void putImage(image_t *img);

    friend class smartReference_t<shmSink_t>;
};
typedef smartReference_t<shmSink_t> shmSink_ref;

#endif
