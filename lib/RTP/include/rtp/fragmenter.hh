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
// $Id: fragmenter.hh 10823 2007-09-17 10:22:30Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __isabel_sdk__fragmenter_hh__
#define __isabel_sdk__fragmenter_hh__

#include <icf2/general.h>

class fragmenter_t
{
private:
    u8  *buffer;   // borrowed, do not free. Besides, it is moved,
                   // so it will not point to the beginning of the
                   // memory allocated segment
    long numBytes;

    void *frameData; // additional data for each type of frame

    long frameCount;

    u32 fcc;
    int Width;
    int Height;

private:

    void setMPEG1Frame(u8 *pBuffer, long BufferLen);
    void setMPEG2Frame(u8 *pBuffer, long BufferLen);
    void setH263Frame (u8 *pBuffer, long BufferLen);
    void setH264Frame (u8 *pBuffer, long BufferLen);
    void setJPEGFrame (u8 *pBuffer, long BufferLen);

    int  getMPEG4Fragment(u8 *fragment, int & size);
    int  getMPEG1Fragment(u8 *fragment, int & size);
    int  getMPEG2Fragment(u8 *fragment, int & size);
    int  getH263Fragment (u8 *fragment, int & size);
    int  getH264Fragment (u8 *fragment, int & size);
    int  getJPEGFragment (u8 *fragment, int & size);
    int  getCellBFragment(u8 *fragment, int & size);

public:

    fragmenter_t(void);
    virtual ~fragmenter_t(void);

    // setFrame returns the PT, maybe the app need it
    u8 setFrame(u8 *pBuffer, long BufferLen, u32 fcc, int Width, int Height);

    int  getFragment(u8 *fragment, int & size);
    long getOffset(void);
};

#endif

