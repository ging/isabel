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
// $Id: soundCodecs.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "soundCodecs.h"

codecPool_t::codecPool_t(void)
{
     memset(codecArray,0,CODEC_LEN*sizeof(Codec*));
     //initialize codec array
	 for (u16 i = 0; GetCodecFactory()[i].name != NULL;i++)
     {        
        codecArray[GetCodecFactory()[i].pt] = CodecNew(GetCodecFactory()[i].pt);
        //init codecs
        CodecInit(codecArray[GetCodecFactory()[i].pt]);
     }
}

codecPool_t::~codecPool_t(void)
{
    for(u16 i = 0;i<CODEC_LEN;i++)
    {
        if (codecArray[i])
        {
            CodecDelete(codecArray[i]);
        }
    }
}

Codec * 
codecPool_t::getCodec(int PT)
{
    if (PT<127)
    {
        return codecArray[PT];
    }
    return NULL;
}

Codec * 
codecPool_t::operator[](int PT)
{
    if (PT>=0 &&
        PT<127)
    {
        return codecArray[PT];
    }
    return NULL;
}

