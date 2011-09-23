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
// $Id: jpegHeaders.hh 10580 2007-07-13 16:35:04Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __jpeg_headers_h__
#define __jpeg_headers_h__

#include <icf2/general.h>

//
// Tables and functions defined in RFC 2435
//

// RTP JPEG Header from RFC 2435 (obsoletes 2035)
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   | Type specific |              Fragment Offset                  |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |      Type     |       Q       |     Width     |     Height    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct JPEGHeader_t
{
    u32 typeSpecific:8;
    u32 offset:24;

    u32 type:8;
    u32 Q:8;
    u32 width:8;
    u32 height:8;
};


// From RFC 2435 (obsoletes RFC 2035)
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |      MBZ      |   Precision   |             Length            |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Quantization Table Data                    |
//   |                              ...                              |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct QuantizationHeader_t
{
    unsigned int mbz:8;
    unsigned int precision:8;
    unsigned int length:16;
};

// From RFC 2435 (obsoletes RFC 2035)
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |       Restart Interval        |F|L|       Restart Count       |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct RestartHeader_t
{
    unsigned int restartInterval:16;
    unsigned int F:1;
    unsigned int L:1;
    unsigned int restartCount:14;
};

/*
 * Table K.1 from JPEG spec.
 */


static unsigned char jpeg_luma_quantizer[64] ={
  16, 11, 10, 16, 24, 40, 51, 61,
  12, 12, 14, 19, 26, 58, 60, 55,
  14, 13, 16, 24, 40, 57, 69, 56,
  14, 17, 22, 29, 51, 87, 80, 62,
  18, 22, 37, 56, 68, 109, 103, 77,
  24, 35, 55, 64, 81, 104, 113, 92,
  49, 64, 78, 87, 103, 121, 120, 101,
  72, 92, 95, 98, 112, 100, 103, 99
};


/*
 * Table K.2 from JPEG spec.
 */


static unsigned char jpeg_chroma_quantizer[64] ={
  17, 18, 24, 47, 99, 99, 99, 99,
  18, 21, 26, 66, 99, 99, 99, 99,
  24, 26, 56, 99, 99, 99, 99, 99,
  47, 66, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99
};


void MakeTables(int q, unsigned char *lqt, unsigned char *cqt);

u8 * MakeQuantHeader(u8 *p, u8 *qt, int tableNo);

u8 * MakeDRIHeader (u8 *p, unsigned short dri);

u8 *
MakeHuffmanHeader(u8 *p,
                  u8 *codelens,
                  int ncodes,
                  u8 *symbols,
                  int nsymbols,
                  int tableNo,
                  int tableClass
                 );

int
MakeHeaders (unsigned char *p,
             int type,
             int w,
             int h,
             unsigned char *lqt,
             unsigned char *cqt,
             unsigned short dri
            );


#endif

