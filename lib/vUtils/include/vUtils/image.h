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
// $Id: image.h 22391 2011-05-20 14:08:39Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __vutils__image_h__
#define __vutils__image_h__

#include <vCodecs/codec.h>

#include <icf2/general.h>
#include <icf2/icfTime.hh>

//
// Documentar...
//
class image_t
{
private:
    u8  *buffer;
    u32  numBytes;
    u32  fcc;
    int  w, h;

    u32            timestamp;
    struct timeval timeToPaint;

public:
    image_t(u8  *b,
            u32  nBytes,
            u32  nfcc,
            int  width,
            int  height,
            u32  ts
           );

    image_t(const image_t &other);

    virtual ~image_t(void);

    // image info
    u8  *getBuff    (void) const;
    u32  getNumBytes(void) const;
    u32  getFormat  (void) const;
    int  getWidth   (void) const;
    int  getHeight  (void) const;

    void setTimestamp(u32 ts);
    u32  getTimestamp   (void) const;

    void setTimeToPaint(struct timeval time);
    struct timeval getTimeToPaint (void) const;

    // actions
    void flipV(void);
    void mirrorH(void);
    void crop16(void);
    void reduceBy2(void);
    void zoom(int nw, int nh);
    void smooth(int nw, int nh);
    void crop(int fromX, int fromY, int nW, int nH);
};

#endif
