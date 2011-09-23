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
// $Id: gridBorder1.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "gridBorder1.h"
#include "../returnCode.h"
				   //W      H      X     Y
layout_t BorderLayout[] = { { 16,  60.00,  60.00, 20.00, 20.00},
                            { 16,  20.00,  20.00,  0.00,  0.00},
                	    { 16,  20.00,  20.00, 80.00,  0.00},
			    { 16,  20.00,  20.00,  0.00, 80.00},
                            { 16,  20.00,  20.00, 80.00, 80.00},
                            { 16,  20.00,  20.00, 40.00,  0.00},
                            { 16,  20.00,  20.00, 40.00, 80.00},
			    { 16,  20.00,  20.00, 0.00,  40.00},
			    { 16,  20.00,  20.00, 80.00, 40.00},
			    { 16,  20.00,  20.00, 20.00,  0.00},
			    { 16,  20.00,  20.00, 20.00, 20.00},
                            { 16,  20.00,  20.00, 60.00,  0.00},
			    { 16,  20.00,  20.00, 20.00, 80.00},
			    { 16,  20.00,  20.00, 60.00, 80.00},
			    { 16,  20.00,  20.00,  0.00, 20.00},
			    { 16,  20.00,  20.00, 80.00, 20.00},
			    { 16,  20.00,  20.00,  0.00, 60.00},
			    { 16,  20.00,  20.00, 80.00, 60.00},
                            {-1,     -1,     -1}
                          };


int 
gridBorder1_t::search (int flowID) 
{
	int pos= -1;
	int maxSize = BorderLayout[0].maxSize;
		
	for (int i= 0; i < maxSize; i++) {
		if (gridInfo[i].flowID == flowID)
			return i;
		if (gridInfo[i].kind != LEGAL && pos==-1)
			pos = i;
	}
	return pos;
}

gridBorder1_t::gridBorder1_t(unsigned int w,
							 unsigned int h)
:gridComposer_t(GRID_BORDER,w,h)
{
	for (u16 i = 0;BorderLayout[i].maxSize!=-1;i++)
	{
		gridInfo[i].flowID= -1;
		gridInfo[i].kind= EMPTY;
		gridInfo[i].window.w= (unsigned int)(BorderLayout[i].dW*Width/100.00);
		gridInfo[i].window.h= (unsigned int)(BorderLayout[i].dH*Height/100.00);
		gridInfo[i].window.x= (unsigned int)(BorderLayout[i].posX*Width/100.00);
		gridInfo[i].window.y= (unsigned int)(BorderLayout[i].posY*Height/100.00);
	}
}

gridBorder1_t::~gridBorder1_t(void)
{

}

HRESULT 
gridBorder1_t::addFlowID(int flowID) 
{
	int pos= search (flowID);
    if (pos == -1) 
	{
		NOTIFY("gridComposer_t:gridBorder:there is no room for flowID %d\n",flowID);
	}else{
		gridInfo[pos].flowID= flowID;
		gridInfo[pos].kind= LEGAL;
	}
	return S_OK;
}

HRESULT 
gridBorder1_t::delFlowID(int flowID) 
{
	int pos= search(flowID);
	if (pos == -1) {
		NOTIFY("gridBorder1_t::delflowID: flowID %d unknown!", flowID);
        return E_ERROR;
	}
	gridInfo[pos].kind= DELETED;
	return S_OK;
}



