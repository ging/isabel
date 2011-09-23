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
/* libpgm1.c - pgm utility library part 1
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
#include "pbm.h"
#include "libpbm.h"

void
pgm_init( argcP, argv )
    int* argcP;
    char* argv[];
    {
    pbm_init( argcP, argv );
    }

void
pgm_readpgminitrest( file, colsP, rowsP, maxvalP )
    FILE* file;
    int* colsP;
    int* rowsP;
    gray* maxvalP;
    {
    int maxval;

    /* Read size. */
    *colsP = pbm_getint( file );
    *rowsP = pbm_getint( file );

    /* Read maxval. */
    maxval = pbm_getint( file );
    if ( maxval > PGM_MAXMAXVAL )
	pm_error( "maxval is too large - try reconfiguring with PGM_BIGGRAYS" );
    *maxvalP = maxval;
    }

gray pgm_pbmmaxval = 1;

void
pgm_readpgminit( file, colsP, rowsP, maxvalP, formatP )
    FILE* file;
    int* colsP;
    int* rowsP;
    int* formatP;
    gray* maxvalP;
    {
    /* Check magic number. */
    *formatP = pbm_readmagicnumber( file );
    switch ( PGM_FORMAT_TYPE(*formatP) )
	{
        case PGM_TYPE:
	pgm_readpgminitrest( file, colsP, rowsP, maxvalP );
	break;

        case PBM_TYPE:
	pbm_readpbminitrest( file, colsP, rowsP );
	*maxvalP = pgm_pbmmaxval;
	break;

	default:
	pm_error( "bad magic number - not a pgm or pbm file" );
	}
    }

#if __STDC__
void
pgm_readpgmrow( FILE* file, gray* grayrow, int cols, gray maxval, int format )
#else /*__STDC__*/
void
pgm_readpgmrow( file, grayrow, cols, maxval, format )
    FILE* file;
    gray* grayrow;
    int cols;
    gray maxval;
    int format;
#endif /*__STDC__*/
    {
    register int col;
    register gray* gP;
    bit* bitrow;
    register bit* bP;

    switch ( format )
	{
	case PGM_FORMAT:
	for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
	    {
	    *gP = pbm_getint( file );
#ifdef DEBUG
	    if ( *gP > maxval )
		pm_error( "value out of bounds (%u > %u)", *gP, maxval );
#endif /*DEBUG*/
	    }
	break;
	
	case RPGM_FORMAT:
	for ( col = 0, gP = grayrow; col < cols; ++col, ++gP )
	    {
	    *gP = pbm_getrawbyte( file );
#ifdef DEBUG
	    if ( *gP > maxval )
		pm_error( "value out of bounds (%u > %u)", *gP, maxval );
#endif /*DEBUG*/
	    }
	break;
	
	case PBM_FORMAT:
	case RPBM_FORMAT:
	bitrow = pbm_allocrow( cols );
	pbm_readpbmrow( file, bitrow, cols, format );
	for ( col = 0, gP = grayrow, bP = bitrow; col < cols; ++col, ++gP, ++bP )
	    *gP = ( *bP == PBM_WHITE ) ? maxval : 0;
	pbm_freerow( bitrow );
	break;

	default:
	pm_error( "can't happen" );
	}
    }

gray**
pgm_readpgm( file, colsP, rowsP, maxvalP )
    FILE* file;
    int* colsP;
    int* rowsP;
    gray* maxvalP;
    {
    gray** grays;
    int row;
    int format;

    pgm_readpgminit( file, colsP, rowsP, maxvalP, &format );

    grays = pgm_allocarray( *colsP, *rowsP );

    for ( row = 0; row < *rowsP; ++row )
	pgm_readpgmrow( file, grays[row], *colsP, *maxvalP, format );

    return grays;
    }
