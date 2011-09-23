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
////////////////////////////////////////////////////////////////////////
//
// $Id: measures.hh 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __irouter_measures_hh__
#define __irouter_measures_hh__

#include <icf2/general.h>

#define IROUTER_SAVE_MEASURES

class measures_t
{
private:

    FILE *fp;
    char *filename;

    void __savePkt(const char *msg, u32 SSRC, u8 PT, u32 len, u32 TS, u16 SQ);

public:

    measures_t(void);
    ~measures_t(void);

#ifdef IROUTER_SAVE_MEASURES
    inline void savePkt(const char *msg, u32 SSRC, u8 PT, u32 len, u32 TS, u16 SQ)
    {
        __savePkt(msg, SSRC, PT, len, TS, SQ);
    }
#else
    inline void savePkt(const char *msg, u32 SSRC, u8 PT, u32 len, u32 TS, u16 SQ)
    { }
#endif

    void startMeasures(const char *newFilename);
    void stopMeasures(void);
};

extern measures_t *measures;

#endif
