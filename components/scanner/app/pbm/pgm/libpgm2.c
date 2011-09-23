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
/* libpgm2.c - pgm utility library part 2
**
** Copyright (C) 1989 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include "pgm.h"
#include "libpgm.h"

#if __STDC__
void
pgm_writepgminit( FILE* file, int cols, int rows, gray maxval, int forceplain )
#else /*__STDC__*/
void
pgm_writepgminit( file, cols, rows, maxval, forceplain )
    FILE* file;
    int cols, rows;
    gray maxval;
    int forceplain;
#endif /*__STDC__*/
    {
#ifdef PBMPLUS_RAWBITS
    if ( maxval <= 255 && ! forceplain )
	fprintf(
	    file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, RPGM_MAGIC2,
	    cols, rows, maxval );
    else
	fprintf(
	    file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, PGM_MAGIC2,
	    cols, rows, maxval );
#else /*PBMPLUS_RAWBITS*/
    fprintf(
	file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, PGM_MAGIC2,
	cols, rows, maxval );
#endif /*PBMPLUS_RAWBITS*/
    }

static void
putus( n, file )
    unsigned short n;
    FILE* file;
    {
    if ( n >= 10 )
	putus( (unsigned short)(n / 10), file );
    (void) putc( n % 10 + '0', file );
    }

#ifdef PBMPLUS_RAWBITS
static void
pgm_writepgmrowraw( file, grayrow, cols, maxval )
    FILE* file;
    gray* grayrow;
    int cols;
    gray maxval;
    {
    register int col;
    register gray* gP;

    for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
	{
#ifdef DEBUG
	if ( *gP > maxval )
	    pm_error( "value out of bounds (%u > %u)", *gP, maxval );
#endif /*DEBUG*/
	(void) putc( *gP, file );
	}
    }
#endif /*PBMPLUS_RAWBITS*/

static void
pgm_writepgmrowplain( file, grayrow, cols, maxval )
    FILE* file;
    gray* grayrow;
    int cols;
    gray maxval;
    {
    register int col, charcount;
    register gray* gP;

    charcount = 0;
    for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
	{
	if ( charcount >= 65 )
	    {
	    (void) putc( '\n', file );
	    charcount = 0;
	    }
	else if ( charcount > 0 )
	    {
	    (void) putc( ' ', file );
	    ++charcount;
	    }
#ifdef DEBUG
	if ( *gP > maxval )
	    pm_error( "value out of bounds (%u > %u)", *gP, maxval );
#endif /*DEBUG*/
	putus( (unsigned short) *gP, file );
	charcount += 3;
	}
    if ( charcount > 0 )
	(void) putc( '\n', file );
    }

#if __STDC__
void
pgm_writepgmrow( FILE* file, gray* grayrow, int cols, gray maxval, int forceplain )
#else /*__STDC__*/
void
pgm_writepgmrow( file, grayrow, cols, maxval, forceplain )
    FILE* file;
    gray* grayrow;
    int cols;
    gray maxval;
    int forceplain;
#endif /*__STDC__*/
    {
#ifdef PBMPLUS_RAWBITS
    if ( maxval <= 255 && ! forceplain )
	pgm_writepgmrowraw( file, grayrow, cols, maxval );
    else
	pgm_writepgmrowplain( file, grayrow, cols, maxval );
#else /*PBMPLUS_RAWBITS*/
    pgm_writepgmrowplain( file, grayrow, cols, maxval );
#endif /*PBMPLUS_RAWBITS*/
    }

#if __STDC__
void
pgm_writepgm( FILE* file, gray** grays, int cols, int rows, gray maxval, int forceplain )
#else /*__STDC__*/
void
pgm_writepgm( file, grays, cols, rows, maxval, forceplain )
    FILE* file;
    gray** grays;
    int cols, rows;
    gray maxval;
    int forceplain;
#endif /*__STDC__*/
    {
    int row;

    pgm_writepgminit( file, cols, rows, maxval, forceplain );

    for ( row = 0; row < rows; ++row )
	 pgm_writepgmrow( file, grays[row], cols, maxval, forceplain );
    }
