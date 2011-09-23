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
/////////////////////////////////////////////////////////////////////////
//
// $Id: soundCodecs.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////


#ifndef __MCU_SOUND_CODECS_H__
#define __MCU_SOUND_CODECS_H__

#include <aCodecs/codecs.h>

#include "general.h"

#define CODEC_LEN 128


class codecPool_t
{
private:

    Codec * codecArray[CODEC_LEN]; //array of all known audio codecs
    
public:

    codecPool_t(void);
    ~codecPool_t(void);

    Codec * getCodec(int);
    Codec * operator[](int);
};
#endif

