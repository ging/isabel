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
// $Id: gridBorderTR.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include "gridBorderTR.h"
#include "../returnCode.h"
                   //W      H      X     Y
layout_t BorderTRLayout[] = { { 6,  66.67,  66.67,  0.00, 33.33},
                              { 6,  33.33,  33.33,  0.00,  0.00},
                              { 6,  33.33,  33.33, 33.33,  0.00},
                              { 6,  33.33,  33.33, 66.67,  0.00},
                              { 6,  33.33,  33.33, 66.67, 33.33},
                              { 6,  33.33,  33.33, 66.67, 66.67},
                              {-1,     -1,     -1}
                          };


int
gridBorderTR_t::search (int flowID)
{
    int pos= -1;
    int maxSize = BorderTRLayout[0].maxSize;

    for (int i= 0; i < maxSize; i++) {
        if (gridInfo[i].flowID == flowID)
            return i;
        if (gridInfo[i].kind != LEGAL && pos==-1)
            pos = i;
    }
    return pos;
}

gridBorderTR_t::gridBorderTR_t(unsigned int w, unsigned int h)
:gridComposer_t(GRID_BORDER,w,h)
{
    for (u16 i = 0;BorderTRLayout[i].maxSize!=-1;i++) {
        gridInfo[i].flowID= -1;
        gridInfo[i].kind= EMPTY;
        gridInfo[i].window.w= (unsigned int)(BorderTRLayout[i].dW*Width/100.00);
        gridInfo[i].window.h= (unsigned int)(BorderTRLayout[i].dH*Height/100.00);
        gridInfo[i].window.x= (unsigned int)(BorderTRLayout[i].posX*Width/100.00);
        gridInfo[i].window.y= (unsigned int)(BorderTRLayout[i].posY*Height/100.00);
    }
}

gridBorderTR_t::~gridBorderTR_t(void)
{

}

HRESULT
gridBorderTR_t::addFlowID(int flowID)
{
    int pos= search (flowID);
    if (pos == -1) {
        NOTIFY("gridBorderTR_t:addFlowID: there is no room for flowID %d\n",
               flowID
              );
    } else {
        gridInfo[pos].flowID= flowID;
        gridInfo[pos].kind= LEGAL;
    }
    return S_OK;
}

HRESULT
gridBorderTR_t::delFlowID(int flowID)
{
    int pos= search(flowID);
    if (pos == -1) {
        NOTIFY("gridBorderTR_t::delflowID: flowID %d unknown!", flowID);
        return E_ERROR;
    }
    gridInfo[pos].kind= DELETED;
    return S_OK;
}



