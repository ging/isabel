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
// $Id: jpegRFC2435.cc 10685 2007-08-31 15:04:54Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "jpegHeaders.hh"

// functions defined in RFC 2435
//


static unsigned char lum_dc_codelens[] = {
        0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};

static unsigned char lum_dc_symbols[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char lum_ac_codelens[] = {
        0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d,
};

static unsigned char lum_ac_symbols[] = {
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
        0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
        0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
        0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
        0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
        0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
        0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
        0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
        0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa,
};


static unsigned char chm_dc_codelens[] = {
        0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

static unsigned char chm_dc_symbols[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static unsigned char chm_ac_codelens[] = {
        0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
};

static unsigned char chm_ac_symbols[] = {
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
        0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
        0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
        0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
        0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
        0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
        0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
        0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa,
};


void
MakeTables(int q, u8 *lqt, u8 *cqt)
{
    unsigned char qtTrans[64] = {0,  1,  8,  16, 9,  2,  3,  10,
                                 17, 24, 32, 25, 18, 11, 4,  5,
                                 12, 19, 26, 33, 40, 48, 41, 34,
                                 27, 20, 13, 6,  7,  14, 21, 28,
                                 35, 42, 49, 56, 57, 50, 43, 36,
                                 29, 22, 15, 23, 30, 37, 44, 51,
                                 58, 59, 52, 45, 38, 31, 39, 46,
                                 53, 60, 61, 54, 47, 55, 62, 63};


    for (int i=0; i < 64; i++) {
        int lq = (jpeg_luma_quantizer[qtTrans[i]]   * q + 50) / 100;
        int cq = (jpeg_chroma_quantizer[qtTrans[i]] * q + 50) / 100;

        // Limit the quantizers to 1 <= q <= 255
        if (lq < 1) lq = 1;
        else if (lq > 255) lq = 255;
        lqt[i] = (u8)lq;

        if (cq < 1) cq = 1;
        else if (cq > 255) cq = 255;
        cqt[i] = (u8)cq;
    }
}

u8 *
MakeQuantHeader(u8 *p, u8 *qt, int tableNo)
{
    *p++ = 0xff;
    *p++ = 0xdb;            /* DQT */
    *p++ = 0;               /* length msb */
    *p++ = 67;              /* length lsb */
    *p++ = tableNo;
    memcpy(p, qt, 64);
    return (p + 64);
}

u8 *
MakeDRIHeader (u8 *p, u16 dri)
{
    *p++ = 0xff;
    *p++ = 0xdd;            /* DRI */
    *p++ = 0x0;             /* length msb */
    *p++ = 4;               /* length lsb */
    *p++ = dri >> 8;        /* dri msb */
    *p++ = dri & 0xff;      /* dri lsb */
    return (p);
}

u8 *
MakeHuffmanHeader(u8 *p,
                  u8 *codelens,
                  int ncodes,
                  u8 *symbols,
                  int nsymbols,
                  int tableNo,
                  int tableClass
                 )
{
    *p++ = 0xff;
    *p++ = 0xc4;                  /* DHT */
    *p++ = 0;                     /* length msb */
    *p++ = 3 + ncodes + nsymbols; /* length lsb */
    *p++ = (tableClass << 4) | tableNo;
    memcpy(p, codelens, ncodes);
    p += ncodes;
    memcpy(p, symbols, nsymbols);
    p += nsymbols;
    return (p);
}


int
MakeHeaders (u8 *p,
             int type,
             int w,
             int h,
             u8 *lqt,
             u8 *cqt,
             u16 dri
            )
{
    unsigned char *start = p;
    w <<= 3;
    h <<= 3;
    *p++ = 0xff;     /*SOI*/
    *p++ = 0xd8;
    *p++ = 0xff;     //APP0
    *p++ = 0xe0;
    *p++ = 0;
    *p++ = 0x10;
    *p++ = 0x4a;
    *p++ = 0x46;
    *p++ = 0x49;
    *p++ = 0x46;
    *p++ = 0;
    *p++ = 1;
    *p++ = 1;
    *p++ = 0;
    *p++ = 0;
    *p++ = 1;
    *p++ = 0;
    *p++ = 1;
    *p++ = 0;
    *p++ = 0;
    p = MakeQuantHeader (p, lqt, 0);
    p = MakeQuantHeader (p, cqt, 1);
    *p++ = 0xff;    //SOF
    *p++ = 0xc0;
    *p++ = 0;
    *p++ = 0x11;
    *p++ = 8;
    *p++ = h >> 8;
    *p++ = h;
    *p++ = w >> 8;
    *p++ = w;
    *p++ = 3;
    *p++ = 1;
    if ((type == 0)||(type == 64)) {
        *p++ = 0x21;
    } else {
        *p++ = 0x22;
    }
    *p++ = 0;
    *p++ = 2;
    *p++ = 0x11;
    *p++ = 1;
    *p++ = 3;
    *p++ = 0x11;
    *p++ = 1;
    p = MakeHuffmanHeader (p,
                           lum_dc_codelens,
                           sizeof (lum_dc_codelens),
                           lum_dc_symbols,
                           sizeof (lum_dc_symbols),
                           0, 0
                          );
    p = MakeHuffmanHeader (p,
                           lum_ac_codelens,
                           sizeof (lum_ac_codelens),
                           lum_ac_symbols,
                           sizeof (lum_ac_symbols),
                           0, 1
                          );
    p = MakeHuffmanHeader (p,
                           chm_dc_codelens,
                           sizeof (chm_dc_codelens),
                           chm_dc_symbols,
                           sizeof (chm_dc_symbols),
                           1, 0
                          );
    p = MakeHuffmanHeader (p,
                           chm_ac_codelens,
                           sizeof (chm_ac_codelens),
                           chm_ac_symbols,
                           sizeof (chm_ac_symbols),
                           1, 1
                          );
    if (dri != 0){
        p = MakeDRIHeader (p, dri);
    }
    *p++ = 0xff;    //SOS
    *p++ = 0xda;
    *p++ = 0;
    *p++ = 0xc;
    *p++ = 3;
    *p++ = 1;
    *p++ = 0;
    *p++ = 2;
    *p++ = 0x11;
    *p++ = 3;
    *p++ = 0x11;
    *p++ = 0;
    *p++ = 0x3f;
    *p++ = 0;
    return (p - start);
}

