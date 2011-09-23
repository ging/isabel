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
#ifndef AVIFILE_MMX_H
#define AVIFILE_MMX_H

/********************************************************
 *
 *      Miscellaneous MMX-accelerated routines
 *      Copyright 2000 Eugene Kuznetsov (divx@euro.ru)
 *
 ********************************************************/

// generic scaling function for 16, 24 or 32 bit data
void zoom(uint16_t* dest, const uint16_t* src, int dst_w, int dst_h, int src_w, int src_h, int bpp, int xdim=0);

// converter from 555 to 565 color depth
extern void (*v555to565)(uint16_t* dest, const uint16_t* src, int w, int h);

// fast scaler by-2 ( e.g. 640x480->320x240 ) for 16-bit colors */
extern void (*zoom_2_16)(uint16_t* dest, const uint16_t* src, int w, int h);

// scaler & converter
extern void (*zoom_2_16_to565)(uint16_t *dest, const uint16_t* src, int w, int h);

// scaler by-2 for 32-bit colors
extern void (*zoom_2_32)(uint32_t* dest, const uint32_t* src, int w, int h);

#endif // AVIFILE_MMX_H
