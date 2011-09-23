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
// $Id: RTPRandom.hh 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __rtp_random_hh_
#define __rtp_random_hh_

#include <icf2/general.h>

/**
* This class is used to generate random parameters.
*/
class  RTPRandom_t
{
public:

    /**
    * RTPRandom_t constructor.
    */
    RTPRandom_t (void);

    /**
    * RTPRandom_t destructor.
    */
    ~RTPRandom_t (void);

    /**
    * Method to get an unsigned 32 bit integer.
    * It can be used to generate the SSRC or timestamp.
    */
    u32 random32 (void);

    /**
    * Method to get an unsigned 16 bit integer.
    * It can be used to generate the sequence number.
    */
    u16 random16 (void);

private:
    int numcalls;
    u8 randomByte (void);
};

#endif

