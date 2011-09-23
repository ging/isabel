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
/* ppmcmap.h - header file for colormap routines in libppm
*/

/* Color histogram stuff. */

typedef struct colorhist_item* colorhist_vector;
struct colorhist_item
    {
    pixel color;
    int value;
    };

typedef struct colorhist_list_item* colorhist_list;
struct colorhist_list_item
    {
    struct colorhist_item ch;
    colorhist_list next;
    };

colorhist_vector ppm_computecolorhist ARGS(( pixel** pixels, int cols, int rows, int maxcolors, int* colorsP ));
/* Returns a colorhist *colorsP long (with space allocated for maxcolors. */

void ppm_addtocolorhist ARGS(( colorhist_vector chv, int* colorsP, int maxcolors, pixel* colorP, int value, int position ));

void ppm_freecolorhist ARGS(( colorhist_vector chv ));


/* Color hash table stuff. */

typedef colorhist_list* colorhash_table;

colorhash_table ppm_computecolorhash ARGS(( pixel** pixels, int cols, int rows, int maxcolors, int* colorsP ));

int
ppm_lookupcolor ARGS(( colorhash_table cht, pixel* colorP ));

colorhist_vector ppm_colorhashtocolorhist ARGS(( colorhash_table cht, int maxcolors ));
colorhash_table ppm_colorhisttocolorhash ARGS(( colorhist_vector chv, int colors ));

int ppm_addtocolorhash ARGS(( colorhash_table cht, pixel* colorP, int value ));
/* Returns -1 on failure. */

colorhash_table ppm_alloccolorhash ARGS(( void ));

void ppm_freecolorhash ARGS(( colorhash_table cht ));
