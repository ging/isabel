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
// $Id: soundCodecs.cc 6363 2005-03-18 16:31:53Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include <rtp/RTPPayloads.hh>

#include "soundCodecs.h"

codecPool_t::codecPool_t(void)
{
}

codecPool_t::~codecPool_t(void)
{
}

aCoder_t*
codecPool_t::getCoder(u8 PT, u32 SSRC)
{
    if (PT < 127)
    {
        int fmt = getFmtByPT(PT);

        codersMap_t::iterator it = coders.find(SSRC);
        if (it != coders.end())
        {
            if (aGetFormat(it->second) == fmt)
            {
                return it->second;
            }
            else
            {
                aDeleteCoder(it->second);
                coders.erase(it);
            }
        }
        aCoder_t *coder = aGetCoder(fmt);
        if (coder == NULL)
        {
            return NULL;
        }

        coders.insert(codersMap_t::value_type(SSRC, coder));

        return coder;
    }

    return NULL;
}

aDecoder_t *
codecPool_t::getDecoder(u8 PT, u32 SSRC)
{
    if (PT < 127)
    {
        int fmt = getFmtByPT(PT);

        decodersMap_t::iterator it = decoders.find(SSRC);
        if (it != decoders.end())
        {
            if (aGetFormat(it->second) == fmt)
            {
                return it->second;
            }
            else
            {
                aDeleteDecoder(it->second);
                decoders.erase(it);
            }
        }
        aDecoder_t *decoder = aGetDecoder(fmt);
        if (decoder == NULL)
        {
            return NULL;
        }

        decoders.insert(decodersMap_t::value_type(SSRC, decoder));

        return decoder;
    }

    return NULL;
}

