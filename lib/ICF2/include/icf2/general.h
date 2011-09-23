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
// $Id: general.h 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__general_h__
#define __icf2__general_h__

#ifdef WIN32
#include <winsock2.h>
#pragma warning (disable:4996)
#endif

#include <icf2/hostArch.h>


// Para crear un DLL //
#define _Dll_

#ifdef WIN32
#pragma warning( disable : 4521)
#undef _Dll_
#ifdef _USRDLL
#define _Dll_ __declspec(dllexport)
#else
#define _Dll_
#endif
#endif
//
 
/*
 * Soporte de threads
 *
 */
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#ifdef __THREADED_ICF
#include <pthread.h>
#endif
#endif

/* 
 * CDECL -- solo para C++
 *
 */
#ifdef __cplusplus
# ifndef CDECL
# define CDECL "C"
# endif
#else
# ifndef CDECL
# define CDECL
# endif
#endif

/*
 * some integer types
 *
 */
typedef signed   __i8  i8;
typedef signed   __i16 i16;
typedef signed   __i32 i32;
typedef unsigned __i8  u8;
typedef unsigned __i16 u16;
typedef unsigned __i32 u32;



/*
 * handy macros -- extended syntax
 *
 */
#define KBYTES  *1024
#define MBYTES  *1024 KBYTES

#define KBytes KBYTES
#define MBytes MBYTES

#define numElems(x) (sizeof(x)/sizeof(x[0]))

#endif

