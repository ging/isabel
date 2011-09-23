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
/* libpbm4.c - pbm utility library part 4
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include "pbm.h"
#include "libpbm.h"

char
pbm_getc( file )
    FILE* file;
    {
    register int ich;
    register char ch;

    ich = getc( file );
    if ( ich == EOF )
	pm_error( "EOF / read error" );
    ch = (char) ich;
    
    if ( ch == '#' )
	{
	do
	    {
	    ich = getc( file );
	    if ( ich == EOF )
		pm_error( "EOF / read error" );
	    ch = (char) ich;
	    }
	while ( ch != '\n' && ch != '\r' );
	}

    return ch;
    }

unsigned char
pbm_getrawbyte( file )
    FILE* file;
    {
    register int iby;

    iby = getc( file );
    if ( iby == EOF )
	pm_error( "EOF / read error" );
    return (unsigned char) iby;
    }

int
pbm_getint( file )
    FILE* file;
    {
    register char ch;
    register int i;

    do
	{
	ch = pbm_getc( file );
	}
    while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );

    if ( ch < '0' || ch > '9' )
	pm_error( "junk in file where an integer should be" );

    i = 0;
    do
	{
	i = i * 10 + ch - '0';
	ch = pbm_getc( file );
        }
    while ( ch >= '0' && ch <= '9' );

    return i;
    }
