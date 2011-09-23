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
/**
 <head> 
   <name>H263.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef H263IMP_IVIDEO_H
#define H263IMP_IVIDEO_H

#include "xxxImp.h"

#define __H263	34 

typedef enum {SQCIF=1, QCIF=2, CIF=3, CIF4=4, CIF16=5, OTRO=7} SourceFormat;
// H263 structs

#define MAXMEM16CIF     1622016
#define MAXMEM16CIF_4    405504


struct Mode_Header_t{
    u8 ebit:3;
    u8 sbit:3;
    u8 P:1;
    u8 F:1;
};

struct H263_0Header_t{

    unsigned char ebit:3;
    unsigned char sbit:3;
    unsigned char P:1;
    unsigned char F:1;
    unsigned char R1:1;
    unsigned char A:1;
    unsigned char S:1;
    unsigned char U:1;
    unsigned char I:1;
    unsigned char src:3;
    unsigned char trb:3;
    unsigned char dbq:2;
    unsigned char R:3;
    unsigned char tr;
};

struct H263_1Header_t{

    u8 ebit: 3;
    u8 sbit: 3;    
    u8 F: 1;
    u8 P: 1; 
    u8 quant: 5;
    u8 src: 3;
    u8 R: 2;	
    u16 mba: 9;
    u8 gobn: 5;
    u8 vmv2: 7;
    u8 hmv2: 7;
    u8 vmv1: 7;	
    u8 hmv1: 7;
    u8 A: 1;
    u8 S: 1;
    u8 U: 1;    
    u8 I: 1; 
};

/**
 <class> 
   <name>h263EncodedImage_t</name> 
   <descr>
	This class encapsulates H263 frames.
   </descr>
**/

class h263EncodedImage_t: public encodedImage_t
{
 public:

	unsigned int  nGOBs;
    unsigned int  nMBs;
    unsigned int  nMBTotal;
    unsigned int  bytesPushed;
    unsigned int  bytesPopped;
    unsigned      GOBsPopped;
    unsigned      MBsPopped;
    u8            source;
    u8            inter;
    u8            quant;
    u8            mode;
    unsigned int *GOBp;
    unsigned int *GOBq;
    unsigned int *MBp;
    unsigned int *MBq;

	h263EncodedImage_t(void)
	{
		nGOBs = 0;
		nMBs  = 0;
	    nMBTotal = 0;
		bytesPushed = 0;
		bytesPopped = 0;
		GOBsPopped = 0;
		MBsPopped = 0;
		source = 0;
		inter = 0;
		quant = 0;
		mode = 0;
		GOBp = NULL;
		GOBq = NULL;
		MBp = NULL;
		MBq = NULL;
	}

    ~h263EncodedImage_t(void) 
	{ 
				free (GOBp);  GOBp= NULL;
				free (GOBq);  GOBq= NULL;
				free (MBp);   MBp= NULL;
				free (MBq);   MBq= NULL;
	}
};

#endif
