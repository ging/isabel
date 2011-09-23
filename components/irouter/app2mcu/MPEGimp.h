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
   <name>MPEGimp.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef MPEGIMP_IVIDEO_H
#define MPEGIMP_IVIDEO_H

#include "xxxImp.h"

//codec definitions

#define __MPEG4 96
#define __MPEG1	32

// MPEG1 structs

struct MPEG1Header_t {
  
  u16 word1;

  u8 P:3;
  u8 E:1;
  u8 B:1;
  u8 S:1;
  u8 N:1;
  u8 AN:1;

  u8 FFC:3;
  u8 FFV:1;
  u8 BFC:3;
  u8 FBV:1;

};

/**
 <class> 
   <name>mpegEncodedImage_t</name> 
   <descr>
	This class encapsulates H263 frames.
   </descr>
**/
class mpegEncodedImage_t: public encodedImage_t
{
public:
    unsigned 	bytesPopped;
    unsigned 	lastOffsetPushed; 
    u16         temporal_reference;
    u8          picture_type;
    u8          FFC;

    ~mpegEncodedImage_t(void) 
	{ 
	}
};

#endif

