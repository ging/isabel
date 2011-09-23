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
// $Id: gridAuto.cc 7403 2005-10-03 13:25:06Z sirvent $
//
/////////////////////////////////////////////////////////////////////////

#include "gridAuto.h"
#include "../returnCode.h"

layout_t AutoLayout[]= { { 1, 100.00, 100.00},
                         { 2,  50.00,  60.00},
                         { 4,  50.00,  50.00},
                         { 6,  33.33,  50.00},
                         { 9,  33.33,  33.33},
                         {12,  25.00,  33.33},
                         {16,  25.00,  25.00},
                         {25,  20.00,  20.00},
                         {-1,     -1,     -1}
                       };

int 
gridAuto_t::numSrcs(void) 
{
	int total= 0;
	for (int i= 0; i < AutoLayout[actualLY].maxSize; i++) {
		if (gridInfo[i].kind == LEGAL)
			total++;
	}
	return total;
}

gridAuto_t::~gridAuto_t(void)
{

}

void 
gridAuto_t::reconfigureLY(int size) 
{
	double posX = 0, posY = 0;
	double absDeltaW= AutoLayout[actualLY].dW * Width  / 100.0;
	double absDeltaH= AutoLayout[actualLY].dH * Height / 100.0;
	int newSize= AutoLayout[actualLY].maxSize;

    // calculates new layout
	for (int i= 0; i < newSize; i++) {
		gridInfo[i].window.w= (int)absDeltaW;
		gridInfo[i].window.h= (int)absDeltaH;
		gridInfo[i].window.x= (int)posX;
		gridInfo[i].window.y= (int)posY;
		posX += absDeltaW;
		if (posX > 0.99*Width) {
			posX = 0;
			posY += absDeltaH;
		}
	}

	// saves old SSRC - only if newSize < size
	// that is, only if we are reducing the layout,
	// we reallocate windows.
	// If we are expanding the layout, there is no need
	// to reallocate windows
	for (int i= newSize; i < size; i++) {
		if(gridInfo[i].kind == LEGAL) {
			int pos= search(gridInfo[i].flowID);
			gridInfo[pos].kind= LEGAL;
			gridInfo[pos].flowID= gridInfo[i].flowID;
		}
		gridInfo[i].kind= EMPTY;
	}
}

int 
gridAuto_t::search(int flowID) 
{
	int pos= -1;
	int maxSize = AutoLayout[actualLY].maxSize;
	
	for (int i= 0; i < maxSize; i++) {
		if (gridInfo[i].flowID == flowID)
			return i;
		if (gridInfo[i].kind != LEGAL)
			pos = i;
	}
	return pos;
}


gridAuto_t::gridAuto_t(unsigned w,
					   unsigned h)
:gridComposer_t(GRID_AUTO,w,h)
{
		actualLY= 0;
		gridInfo[0].flowID= -1;
		gridInfo[0].kind= EMPTY;
		gridInfo[0].window.w= Width;
		gridInfo[0].window.h= Height;
}

HRESULT 
gridAuto_t::addFlowID(int flowID) 
{
	int pos= search (flowID);
	if (pos == -1) 
	{
		int size= AutoLayout[actualLY].maxSize;
		actualLY++;
		reconfigureLY(size);
		gridInfo[size].flowID= flowID;
		gridInfo[size].kind= LEGAL;
	} else {
		gridInfo[pos].flowID= flowID;
		gridInfo[pos].kind= LEGAL;
	}
	return S_OK;
}

HRESULT 
gridAuto_t::delFlowID(int flowID) 
{
	int pos= search(flowID);
    if (pos == -1) 
	{
		NOTIFY("gridComposer_t::delflowID: flowID %d unknown!", flowID);
		return E_ERROR;
	}

	gridInfo[pos].kind= DELETED;
	// if there is a layout with less sources, recalculate
	if ((actualLY > 0) &&
		(numSrcs() <= AutoLayout[actualLY-1].maxSize) ) {
			int size= AutoLayout[actualLY].maxSize;
			actualLY--;
			reconfigureLY(size);
	}
	return S_OK;
}
