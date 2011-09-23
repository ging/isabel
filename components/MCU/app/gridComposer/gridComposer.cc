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
// $Id: gridComposer.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "gridComposer.h"
#include "../returnCode.h"

gridComposer_t::gridComposer_t(gridMode_e mode,
							   unsigned w,
							   unsigned h)
{
        memset(gridInfo,0,MAX_VIDEO_SOURCES*sizeof(gridInfo_t));
		this->mode = mode;
		Width  = w;
		Height = h;
		layout = NULL;
 }

gridComposer_t::~gridComposer_t(void)
{

}

HRESULT
gridComposer_t::getWindow(int flowID,
						  window_t &window)
{
	int pos= search(flowID);
	if ( (pos == -1) || (gridInfo[pos].kind != LEGAL) ) {
		return E_ERROR;
	}
	window= gridInfo[pos].window;
	return S_OK;
}

maskType_e 
gridComposer_t::getMask(int flowID)
{
	return MASK_NONE;
}
