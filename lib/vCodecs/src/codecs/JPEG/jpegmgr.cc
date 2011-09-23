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
// $Id: jpegmgr.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "jpegmgr.h"

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */
void
init_source (j_decompress_ptr cinfo)
{
  JpegDataSource *src;

  src = (JpegDataSource *) cinfo->src;
  src -> pub.bytes_in_buffer = 0;
  src -> pub.next_input_byte = NULL;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 */
boolean
fill_input_buffer (j_decompress_ptr cinfo)
{
  JpegDataSource *src;
  
  src = (JpegDataSource *) cinfo -> src;
  /* The buffer is external and already filled */
  /* We just set the necesary fields */
  src -> pub.next_input_byte = src -> data;
  src -> pub.bytes_in_buffer = src -> size;
  return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 */
void
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  JpegDataSource *src;
  
  src = (JpegDataSource *) cinfo -> src;
  
  if( num_bytes <= 0)
    return;
  while( num_bytes > (long)src -> pub.bytes_in_buffer) {
    num_bytes -= (long) src -> pub.bytes_in_buffer;
    (void)fill_input_buffer(cinfo);
  }
  src -> pub.bytes_in_buffer -= (size_t) num_bytes;
  src -> pub.next_input_byte += (size_t) num_bytes;
}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 */
void
term_source (j_decompress_ptr cinfo)
{
  JpegDataSource *src;
  
  src = (JpegDataSource *) cinfo -> src;
  free(src -> data);
}

/*
 * Prepare for input from a buffer.
 * the memory should be already allocated
 */
void
jpeg_src_buffer(j_decompress_ptr cinfo,
                unsigned char* buffer,
                unsigned int size
                /*, unsigned int width*/
               )
{
  JpegDataSource *src;
  
  if (cinfo -> src == NULL) {	/* first time for this JPEG object? */
    cinfo -> src = (struct jpeg_source_mgr *)(*cinfo -> mem -> alloc_small) ((j_common_ptr) cinfo, 
								       JPOOL_PERMANENT,
								       sizeof(JpegDataSource));
    src = (JpegDataSource *) cinfo -> src;
    src -> pub.init_source = init_source;
    src -> pub.fill_input_buffer = fill_input_buffer;
    src -> pub.skip_input_data = skip_input_data;
    // Default jpeg method
    src -> pub.resync_to_restart = jpeg_resync_to_restart;
    src -> pub.term_source = term_source;
  }
  src = (JpegDataSource *) cinfo -> src;
  src -> pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src -> pub.next_input_byte = NULL; /* until buffer loaded */
  
  src -> data=(JOCTET *)malloc(size);
  memcpy(src -> data, buffer, size);
  src -> size = size; 
}

/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */
void init_destination(j_compress_ptr cinfo) {
  JpegDataDestination *dest;

  dest = (JpegDataDestination *) cinfo -> dest;
  
  *(dest -> size) = 0;
  
  // public fields
  dest -> pub.next_output_byte = dest -> buffer;
  dest -> pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */
boolean
empty_output_buffer(j_compress_ptr cinfo)
{
  JpegDataDestination *dest;

  dest = (JpegDataDestination *) cinfo -> dest;
  
  dest -> pub.next_output_byte = dest->buffer;
  dest -> pub.free_in_buffer = OUTPUT_BUF_SIZE;
  
  return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_isa_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */
void
term_destination(j_compress_ptr cinfo)
{
  JpegDataDestination *dest;

  dest = (JpegDataDestination *) cinfo -> dest;
  
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
  *(dest -> size) += datacount;
}


/*
 * Prepare for output to a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing compression.
 */
void
jpeg_dst_buffer(j_compress_ptr cinfo,
                unsigned char *buffer,
                unsigned int *buffersize
               )
{
  JpegDataDestination *dest;

  dest = (JpegDataDestination *) cinfo -> dest;

  /* The destination object is made permanent so that multiple JPEG images
   * can be written to the same file without re-executing jpeg_stdio_isa_dest.
   * This makes it dangerous to use this manager and a different destination
   * manager serially with the same JPEG object, because their private object
   * sizes may be different.  Caveat programmer.
   */
  if (cinfo -> dest == NULL) {	/* first time for this JPEG object? */
    cinfo -> dest = (struct jpeg_destination_mgr *)
      (*cinfo -> mem->alloc_small) ((j_common_ptr) cinfo, 
				  JPOOL_PERMANENT,
				  sizeof(JpegDataDestination));
    dest = (JpegDataDestination *) cinfo->dest;
    dest -> pub.init_destination = init_destination;
    dest -> pub.empty_output_buffer = empty_output_buffer;
    dest -> pub.term_destination = term_destination;
  }
  dest = (JpegDataDestination *) cinfo->dest;
  dest -> buffer = buffer;
  dest -> size = buffersize;
  *(dest -> size) = 0;
}

