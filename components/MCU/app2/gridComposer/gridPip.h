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
// $Id: gridPip.h 8194 2006-03-14 08:46:11Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef _MCU_GRID_PIP_H_
#define _MCU_GRID_PIP_H_

#include "gridComposer.h"

class gridPip_t: public gridComposer_t
{
private:

	layout_t * layout;
	virtual int search (int flowID);
	
public:

	gridPip_t(gridMode_e mode,
		      unsigned int w,
			  unsigned int h);		
	virtual ~gridPip_t(void);
    virtual HRESULT addFlowID(int flowID);
	virtual HRESULT delFlowID(int flowID);	
	virtual maskType_e getMask(int flowID);

};

#endif

