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
/* libpbm.h - internal header file for libpbm portable bitmap library
*/

#ifndef _LIBPBM_H_
#define _LIBPBM_H_

/* Here are some routines internal to the pbm library. */

char pbm_getc ARGS(( FILE* file ));
unsigned char pbm_getrawbyte ARGS(( FILE* file ));
int pbm_getint ARGS(( FILE* file ));

int pbm_readmagicnumber ARGS(( FILE* file ));

void pbm_readpbminitrest ARGS(( FILE* file, int* colsP, int* rowsP ));

#endif /*_LIBPBM_H_*/
