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
/////////////////////////////////////////////////////////////////////////
//
// $Id: jpegmgr.h 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __jpegmgr_hh__
#define __jpegmgr_hh__

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#ifdef WIN32
#include "jpeglib/jpeglib.h"
#include "jpeglib/jerror.h"
#else
#include <jpeglib.h>
#include <jerror.h>
#endif
}

#define OUTPUT_BUF_SIZE  (704*576*3) 

typedef struct {
  struct jpeg_source_mgr pub;
  unsigned char *data;
  unsigned int size;
  // width no lo uso ahora pero se podria usar para ir rellenando solo una linea cada vez en lugar de hacerlo de golpe
  unsigned int  width;
} JpegDataSource;
 

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */
  unsigned int *size;
  unsigned char *buffer;                /* start of buffer */
} JpegDataDestination;


void jpeg_src_buffer(j_decompress_ptr cinfo,
                     unsigned char* buffer,
                     unsigned int size
                     /*, unsigned int width*/
                    );

void jpeg_dst_buffer(j_compress_ptr cinfo,
                     unsigned char *buffer,
                     unsigned int *buffersize
                    );

#endif

