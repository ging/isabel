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
// $Id: inet_ntop.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__inet_ntop_hh__
#define __icf2__inet_ntop_hh__

#ifdef WIN32

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 106
#endif
#ifndef ENOSPC
#define ENOSPC 28
#endif

#ifndef IN6ADDRSZ
# define IN6ADDRSZ 16
#endif

#ifndef INT16SZ
# define INT16SZ 2
#endif

#ifdef SPRINTF_CHAR
# define SPRINTF(x) strlen(sprintf x)
#else
# define SPRINTF(x) ((size_t)sprintf x)
#endif

char* inet_ntop(int af,unsigned char* src, char* dst,size_t size);

#endif

#endif

