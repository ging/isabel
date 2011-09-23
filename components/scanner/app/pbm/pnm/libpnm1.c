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
/* libpnm1.c - pnm utility library part 1
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

#include "pnm.h"

#include "ppm.h"
#include "libppm.h"

#include "pgm.h"
#include "libpgm.h"

#include "pbm.h"
#include "libpbm.h"

void
pnm_init( argcP, argv )
    int* argcP;
    char* argv[];
    {
    ppm_init( argcP, argv );
    }

xelval pnm_pbmmaxval = 1;

void
pnm_readpnminit( file, colsP, rowsP, maxvalP, formatP )
    FILE* file;
    int* colsP;
    int* rowsP;
    int* formatP;
    xelval* maxvalP;
    {
    gray gmaxval;

    /* Check magic number. */
    *formatP = pbm_readmagicnumber( file );
    switch ( PNM_FORMAT_TYPE(*formatP) )
	{
	case PPM_TYPE:
	ppm_readppminitrest( file, colsP, rowsP, (pixval*) maxvalP );
	break;

	case PGM_TYPE:
	pgm_readpgminitrest( file, colsP, rowsP, &gmaxval );
	*maxvalP = (xelval) gmaxval;
	break;

	case PBM_TYPE:
	pbm_readpbminitrest( file, colsP, rowsP );
	*maxvalP = pnm_pbmmaxval;
	break;

	default:
	pm_error( "bad magic number - not a ppm, pgm, or pbm file" );
	}
    }

#if __STDC__
void
pnm_readpnmrow( FILE* file, xel* xelrow, int cols, xelval maxval, int format )
#else /*__STDC__*/
void
pnm_readpnmrow( file, xelrow, cols, maxval, format )
    FILE* file;
    xel* xelrow;
    xelval maxval;
    int cols, format;
#endif /*__STDC__*/
    {
    register int col;
    register xel* xP;
    gray* grayrow;
    register gray* gP;
    bit* bitrow;
    register bit* bP;

    switch ( PNM_FORMAT_TYPE(format) )
	{
	case PPM_TYPE:
	ppm_readppmrow( file, (pixel*) xelrow, cols, (pixval) maxval, format );
	break;

	case PGM_TYPE:
	grayrow = pgm_allocrow( cols );
	pgm_readpgmrow( file, grayrow, cols, (gray) maxval, format );
	for ( col = 0, xP = xelrow, gP = grayrow; col < cols; ++col, ++xP, ++gP )
	    PNM_ASSIGN1( *xP, *gP );
	pgm_freerow( grayrow );
	break;

	case PBM_TYPE:
	bitrow = pbm_allocrow( cols );
	pbm_readpbmrow( file, bitrow, cols, format );
	for ( col = 0, xP = xelrow, bP = bitrow; col < cols; ++col, ++xP, ++bP )
	    PNM_ASSIGN1( *xP, *bP == PBM_BLACK ? 0: pnm_pbmmaxval );
	pbm_freerow( bitrow );
	break;

	default:
	pm_error( "can't happen" );
	}
    }

xel**
pnm_readpnm( file, colsP, rowsP, maxvalP, formatP )
    FILE* file;
    int* colsP;
    int* rowsP;
    int* formatP;
    xelval* maxvalP;
    {
    xel** xels;
    int row;

    pnm_readpnminit( file, colsP, rowsP, maxvalP, formatP );

    xels = pnm_allocarray( *colsP, *rowsP );

    for ( row = 0; row < *rowsP; ++row )
	pnm_readpnmrow( file, xels[row], *colsP, *maxvalP, *formatP );

    return xels;
    }
