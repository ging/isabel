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
/**
 <head> 
   <name>NamedGuid.h</name> 
   <author>Microsoft</author>
   <descr>
   DirectShow sample code - helps in converting GUIDs to strings
   </descr>
//</head>
**/

#ifndef NAMEDGUID_H_DEFINED
#define NAMEDGUID_H_DEFINED

#include "general.h"

// Function prototypes
void GetGUIDString(TCHAR *szString, GUID *pGUID);
void GetFormatString(TCHAR *szFormat, AM_MEDIA_TYPE *pType, int nFormatSize);

struct NamedGuid
{
    const GUID *pguid;
    const TCHAR *psz;
};

extern const NamedGuid rgng[];

#endif