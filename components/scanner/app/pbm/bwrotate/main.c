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
/*
 * djr@dit.upm.es
 * (C) 1998
 *
 * Santiago
 * (C) 1999
 * Correccion de errores.
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>


/*
 * quick hack to make bw ppm rotate
 * I will asume everything is coming from a pipe and
 * will read it 
 */


int
main( int argc, char *argv[])
{
    unsigned int val;
    unsigned int bitshift, bitshift2, bit2get;
    unsigned int leftOrRight = 0;
    int i, j, count;
    int width, padwidth, charwidth;
    int height, padheight, charheight;
    char *src, *dst, *src2;

    /* check args */
    if( argc > 1 ) {
        if( argv[1][1] == 'l' )
            leftOrRight = 1;
    }

    
    src = (char *) malloc( 256*1024*10);
    dst = (char *) malloc( 256*1024*10);
    memset( dst, 0, 256*1024*10);
    memset( src, 0, 256*1024*10);
    src2 = src;
    while( fread( src2, 1, 256*1024, stdin) == 256*1024 ) {
        src2 += 256*1024;
    }

    /* parse header */
#if 1
    src2 = strtok( src, "\n");
    src2 = strtok( NULL, "\n");
    while ( src2[0] == '#' ) {
        src2 = strtok( NULL, "\n");
    }
    sscanf(src2,"%d %d",&width,&height);
    
    /* get real image start */
    src2 += strlen( src2)+1;
#else
    src2 = strtok( src, " ");
    src2 = strtok( NULL, " ");
    width = atoi( src2);
    src2 = strtok( NULL, " ");
    height = atoi( src2);
    /* get real image start */
    src2 += strlen( src2)+1;
#endif
    charwidth = width/8 + (width%8 ? 1 : 0);
    padwidth = charwidth *8;

    charheight = height/8 + (height%8 ? 1 : 0);
    padheight = charheight*8;

    /* rotation */
    for( count = 0; count < 8; count++) {
        bit2get = 7 - count;
        bitshift = 0x01 << bit2get;
        for( j = 0; j < padheight; j++) {
            bitshift2 = leftOrRight ? 7 - ( j%8) : (j%8);
            for( i = 0; i < charwidth; i++) {
                val = (src2[ i + j*charwidth] & bitshift) >> bit2get;
                if( leftOrRight)
                    dst[ (padwidth-(i*8)-count-1)*charheight +j/8] |=
                        val << bitshift2;
                else
                    dst[ (1+i*8 + count)*charheight -j/8 -1] |=
                        val << bitshift2;
            }
        }
    }
    fprintf( stdout, "P4\n%d\t%d\n", padheight, padwidth);
    fwrite( dst, 1, padwidth*padheight/8, stdout);
    fflush( stdout);
    free( src);
    free( dst);
    exit( 0);
    return 0;
}
