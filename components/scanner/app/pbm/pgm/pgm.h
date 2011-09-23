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
/* pgm.h - header file for libpgm portable graymap library
*/

#ifndef _PGM_H_
#define _PGM_H_

#include "pbm.h"

#ifdef PGM_BIGGRAYS
typedef unsigned short gray;
#define PGM_MAXMAXVAL 65535
#else /*PGM_BIGGRAYS*/
typedef unsigned char gray;
#define PGM_MAXMAXVAL 255
#endif /*PGM_BIGGRAYS*/


/* Magic constants. */

#define PGM_MAGIC1 'P'
#define PGM_MAGIC2 '2'
#define RPGM_MAGIC2 '5'
#define PGM_FORMAT (PGM_MAGIC1 * 256 + PGM_MAGIC2)
#define RPGM_FORMAT (PGM_MAGIC1 * 256 + RPGM_MAGIC2)
#define PGM_TYPE PGM_FORMAT


/* Macro for turning a format number into a type number. */

#define PGM_FORMAT_TYPE(f) ((f) == PGM_FORMAT || (f) == RPGM_FORMAT ? PGM_TYPE : PBM_FORMAT_TYPE(f))


/* Declarations of routines. */

void pgm_init ARGS(( int* argcP, char* argv[] ));

#define pgm_allocarray( cols, rows ) ((gray**) pm_allocarray( cols, rows, sizeof(gray) ))
#define pgm_allocrow( cols ) ((gray*) pm_allocrow( cols, sizeof(gray) ))
#define pgm_freearray( grays, rows ) pm_freearray( (char**) grays, rows )
#define pgm_freerow( grayrow ) pm_freerow( (char*) grayrow )

gray** pgm_readpgm ARGS(( FILE* file, int* colsP, int* rowsP, gray* maxvalP ));
void pgm_readpgminit ARGS(( FILE* file, int* colsP, int* rowsP, gray* maxvalP, int* formatP ));
void pgm_readpgmrow ARGS(( FILE* file, gray* grayrow, int cols, gray maxval, int format ));

void pgm_writepgm ARGS(( FILE* file, gray** grays, int cols, int rows, gray maxval, int forceplain ));
void pgm_writepgminit ARGS(( FILE* file, int cols, int rows, gray maxval, int forceplain ));
void pgm_writepgmrow ARGS(( FILE* file, gray* grayrow, int cols, gray maxval, int forceplain ));

extern gray pgm_pbmmaxval;
/* This is the maxval used when a PGM program reads a PBM file.  Normally
** it is 1; however, for some programs, a larger value gives better results
*/

#endif /*_PGM_H_*/
