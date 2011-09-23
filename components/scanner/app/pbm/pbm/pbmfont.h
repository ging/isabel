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
/* pbmfont.h - header file for font routines in libpbm
*/

bit** pbm_defaultfont ARGS(( int* fcolsP, int* frowsP ));

void pbm_dissectfont ARGS(( bit** font, int frows, int fcols, int* char_heightP, int* char_widthP, int* char_aheightP, int* char_awidthP, int char_row0[], int char_col0[] ));

void pbm_dumpfont ARGS(( bit** font, int fcols, int frows ));
