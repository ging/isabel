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
// $Id: JPEGimp.h 10492 2007-06-29 17:02:05Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __jpeg_imp_h__
#define __jpeg_imp_h__

struct jpegFragData_t
{
    u8  lqt[64];
    u8  cqt[64];
    u16 dri;
    int quality;
    u8  type;
    u8  typeSpecific;
};

#endif

