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
#ifndef __DSHOW_UTILS_H__
#define __DSHOW_UTILS_H__

#include <vector>
#include <Dshow.h>
#include "DSGrabberDesc.hh"

namespace DSUtils
{

#define DELETE_PTR(x) { if(x!=NULL) { delete x; x = NULL; } }
#define DELETE_ARR(x) { if(x!=NULL) { delete[] x; x = NULL; } }
#define RELEASE_FILTER(x) { if (x!=NULL) { x->Release(); x = NULL; }}
#define REMOVE_FILTER(graph,filter) { if (filter != NULL) { if (graph != NULL){ graph->RemoveFilter(filter); } filter->Release(); filter=NULL; }}

 void errorCheck(int hr);
 HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
 HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
 HRESULT GetConnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
 std::vector<CDSGrabberDesc> EnumVideoSources(void);
 HRESULT InitSource(CDSGrabberDesc desc,IBaseFilter ** pSource);

}

#endif
