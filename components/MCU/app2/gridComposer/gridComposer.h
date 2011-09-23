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
// $Id: gridComposer.h 8173 2006-03-13 08:58:31Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_GRID_COMPOSER_H_
#define _MCU_GRID_COMPOSER_H_

#include "../general.h"

struct layout_t{
	int    maxSize;
	double dW, dH;  // % of surface
	double posX,posY;
	maskType_e mask;
};

class gridComposer_t
{
protected:
	
	enum entryType_e { EMPTY, LEGAL, DELETED };

	struct gridInfo_t
	{
		int         flowID;
		entryType_e kind;
		window_t    window;
	};

	layout_t * layout;

	gridInfo_t gridInfo[MAX_VIDEO_SOURCES];
	gridMode_e mode;

	unsigned   Width;
	unsigned   Height;

	virtual int search (int flowID)=0;
	
public:

	gridComposer_t(gridMode_e mode,unsigned w,unsigned h);		
	virtual ~gridComposer_t(void);
    virtual HRESULT addFlowID(int flowID)=0;
	virtual HRESULT delFlowID(int flowID)=0;
	virtual HRESULT getWindow(int flowID, window_t &window);	
	virtual maskType_e getMask(int flowID);
};

#endif

