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
   <name>fragmenter.h</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
//</head>
**/

#ifndef FRAGMENTER_MPEG4_H
#define FRAGMENTER_MPEG4_H

#include "general.h"
#include "MPEGimp.h"
#include "H263Imp.h"

/**
 <class> 
   <name>fragmenter_t</name> 
   <descr>
	This class is used by sender_t to fragment frames $/
	using each standard defined by the IETF:
   </descr>
**/
class fragmenter_t : public item_t
{
private:

	encodedImage_t * frame;
	BYTE* fragment;
	int  fragmentSize;
	long frameCount;
	int  payLoad;
	
	//Optional parameters
	int  Width;
	int  Height;

public:

	fragmenter_t();
	virtual ~fragmenter_t();
	void setFrame(BYTE *pBuffer, long BufferLen,int payLoad,int Width = 0, int Height = 0);
	int  getFragment(BYTE ** fragment,int * size,int offset);
	long getOffset(void);
	

};
#endif