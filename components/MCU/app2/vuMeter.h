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
// $Id: vuMeter.h 7506 2005-10-20 13:29:33Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_VUMETER_H_
#define _MCU_VUMETER_H_

#include "general.h"

class vuMeter_t
{
private:

    // to eval REC Power
    double evalPower(u8 *buffer, unsigned bufferLen);
    double dbMedia;
    unsigned dbCount;

public:

    vuMeter_t(void);

    virtual ~vuMeter_t(void);
    bool     checkSilence(u8 *pBuffer, long BufferLen, int SL);
};

#endif

