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
// $Id$
//
/////////////////////////////////////////////////////////////////////////

#ifndef __image_buf_hh__
#define __image_buf_hh__

#include <icf2/vector.hh>

#include <vUtils/image.h>

class imageBuf_t
{
private:
    static const int MAX_ELEMENTS= 4;

    vector_t<image_t *> imgQ;

    pthread_cond_t  notFull;
    pthread_cond_t  notEmpty;
    pthread_mutex_t mutex;

public:
    imageBuf_t(void);

    ~imageBuf_t(void);

    void put(image_t *newImg);

    image_t *get(void);
};

#endif
