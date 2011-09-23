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
// $Id: image.hh 10820 2007-09-17 09:20:48Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __gen_image_hh__
#define __gen_image_hh__

#include <icf2/general.h>

#include <vUtils/image.h>

image_t*
genImageRAW24(u8 *buffer,
              unsigned width,
              unsigned height,
              unsigned &posX,
              unsigned &posY,
              unsigned side,
              int &signoX,
              int &signoY,
              bool doBGR,
              u32 timestamp
             );

image_t*
genImageYUV422i(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               );

image_t*
genImageYUV422P(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               );

image_t*
genImageYUV420P(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               );

image_t*
genImageYUV411P(u8 *buffer,
                unsigned width,
                unsigned height,
                unsigned &posX,
                unsigned &posY,
                unsigned side,
                int &signoX,
                int &signoY,
                u32 timestamp
               );

#endif

