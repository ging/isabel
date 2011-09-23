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
// $Id: H263imp.h 10746 2007-09-06 09:11:51Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __h263_imp_h__
#define __h263_imp_h__

#define MAXMEM16CIF     1622016
#define MAXMEM16CIF_4    405504

typedef enum {SQCIF=1, QCIF=2, CIF=3, CIF4=4, CIF16=5, OTRO=7} H263Sizes;

// Common part of H263 RTP headers (See RFC 2190)
//
struct Mode_Header_t
{
    u8 ebit:3;
    u8 sbit:3;
    u8 P:1;
    u8 F:1;
};

// From RFC 2190: Mode A Header
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |F|P|SBIT |EBIT | SRC |I|U|S|A|R      |DBQ| TRB |    TR         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct H263_AHeader_t
{
    u8 ebit:3;
    u8 sbit:3;
    u8 P:1;
    u8 F:1;

    u8 R1:1;
    u8 A:1;
    u8 S:1;
    u8 U:1;
    u8 I:1;
    u8 src:3;

    u8 trb:3;
    u8 dbq:2;
    u8 R:3;

    u8 tr:8;
};


// From RFC 2190: Mode B Header
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |F|P|SBIT |EBIT | SRC | QUANT   |  GOBN   |   MBA           |R  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |I|U|S|A| HMV1        | VMV1        | HMV2        | VMV2        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct H263_BHeader_t
{
    u8 ebit: 3;
    u8 sbit: 3;
    u8 F: 1;
    u8 P: 1;

    u8 quant: 5;
    u8 src: 3;

    u16 R: 2;
    u16 mba: 9;
    u16 gobn: 5;

    u32 vmv2: 7;
    u32 hmv2: 7;
    u32 vmv1: 7;
    u32 hmv1: 7;
    u32 A: 1;
    u32 S: 1;
    u32 U: 1;
    u32 I: 1;
};

// from RFC 2190: Mode C Header NOT SUPPORTED
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |F|P|SBIT |EBIT | SRC | QUANT   |  GOBN   |   MBA           |R  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |I|U|S|A| HMV1        | VMV1        | HMV2        | VMV2        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// | RR                                  |DBQ| TRB |    TR         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

struct H263_CHeader_t
{
    u8 ebit: 3;
    u8 sbit: 3;
    u8 F: 1;
    u8 P: 1;

    u8 quant: 5;
    u8 src: 3;

    u16 R: 2;
    u16 mba: 9;
    u16 gobn: 5;

    u32 vmv2: 7;
    u32 hmv2: 7;
    u32 vmv1: 7;
    u32 hmv1: 7;
    u32 A: 1;
    u32 S: 1;
    u32 U: 1;
    u32 I: 1;

    u32 TR: 8;
    u32 TRB: 3;
    u32 DBQ: 2;
    u32 RR: 19;
};

struct h263FragData_t
{
    u8 mode;
};

#endif

