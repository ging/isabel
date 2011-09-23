/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: jpegmgr.hh 10576 2007-07-12 16:42:47Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __jpegmgr_hh__
#define __jpegmgr_hh__

#include <icf2/general.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include <jpeglib.h>
}

#define OUTPUT_BUF_SIZE  (704*576*3)

typedef struct {
  struct jpeg_source_mgr pub;
  u8 *data;
  u32 size;
  // width no lo uso ahora pero se podria usar para ir rellenando solo una linea cada vez en lugar de hacerlo de golpe
  u32 width;
} JpegDataSource;


typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */
  u32 *size;
  u8 *buffer;                /* start of buffer */
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

