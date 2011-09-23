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
// $Id: pnmImpl.hh 10573 2007-07-12 16:06:28Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __pnm_implementation__hh__
#define __pnm_implementation__hh__

#include <vUtils/image.h>

// P1, P4: portable bitmap file format (PBM), 1 bit per pixel
// P2, P5: portable graymap file format (PGM), 8 bits per pixel
// P3, P6: portable pixmap file format (PPM), 24 bits per pixel

// P1 ascii bitmap
// P2 ascii greymap
// P3 ascii pixmap
// P4 raw bitmap
// P5 raw greymap
// P6 raw pixmap

image_t *pnmLoadFile(FILE *f);

#endif
