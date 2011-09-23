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
/* pbm.h - header file for libpbm portable bitmap library
*/

#ifndef _PBM_H_
#define _PBM_H_

#include "pbmplus.h"

typedef unsigned char bit;
#define PBM_WHITE 0
#define PBM_BLACK 1


/* Magic constants. */

#define PBM_MAGIC1 'P'
#define PBM_MAGIC2 '1'
#define RPBM_MAGIC2 '4'
#define PBM_FORMAT (PBM_MAGIC1 * 256 + PBM_MAGIC2)
#define RPBM_FORMAT (PBM_MAGIC1 * 256 + RPBM_MAGIC2)
#define PBM_TYPE PBM_FORMAT


/* Macro for turning a format number into a type number. */

#define PBM_FORMAT_TYPE(f) ((f) == PBM_FORMAT || (f) == RPBM_FORMAT ? PBM_TYPE : -1)


/* Declarations of routines. */

void pbm_init ARGS(( int* argcP, char* argv[] ));

#define pbm_allocarray( cols, rows ) ((bit**) pm_allocarray( cols, rows, sizeof(bit) ))
#define pbm_allocrow( cols ) ((bit*) pm_allocrow( cols, sizeof(bit) ))
#define pbm_freearray( bits, rows ) pm_freearray( (char**) bits, rows )
#define pbm_freerow( bitrow ) pm_freerow( (char*) bitrow )

bit** pbm_readpbm ARGS(( FILE* file, int* colsP, int* rowsP ));
void pbm_readpbminit ARGS(( FILE* file, int* colsP, int* rowsP, int* formatP ));
void pbm_readpbmrow ARGS(( FILE* file, bit* bitrow, int cols, int format ));

void pbm_writepbm ARGS(( FILE* file, bit** bits, int cols, int rows, int forceplain ));
void pbm_writepbminit ARGS(( FILE* file, int cols, int rows, int forceplain ));
void pbm_writepbmrow ARGS(( FILE* file, bit* bitrow, int cols, int forceplain ));

#endif /*_PBM_H_*/
