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
// $Id: hostArch.h 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__host_architecure_hh__
#define __icf2__host_architecure_hh__

#ifdef __BUILD_FOR_INTEL_WINXP
# define __BUILD_FOR_WINXP
# define __BUILD_FOR_INTEL
# define __BUILD_FOR_LITTLE_ENDIAN
#endif

#ifdef __BUILD_FOR_INTEL_LINUX
# define __BUILD_FOR_LINUX
# define __BUILD_FOR_INTEL
# define __BUILD_FOR_LITTLE_ENDIAN
#endif

#ifdef __BUILD_FOR_INTEL_DARWIN
# define __BUILD_FOR_DARWIN
# define __BUILD_FOR_INTEL
# define __BUILD_FOR_LITTLE_ENDIAN
#endif

#ifdef __BUILD_FOR_INTEL_SOLARIS
# define __BUILD_FOR_SOLARIS
# define __BUILD_FOR_INTEL
# define __BUILD_FOR_LITTLE_ENDIAN
#endif

#ifdef __BUILD_FOR_SPARC_LINUX
# define __BUILD_FOR_LINUX
# define __BUILD_FOR_SPARC
# define __BUILD_FOR_BIG_ENDIAN
#endif

#ifdef __BUILD_FOR_SPARC_SOLARIS
# define __BUILD_FOR_SOLARIS
# define __BUILD_FOR_SPARC
# define __BUILD_FOR_BIG_ENDIAN
#endif

#ifdef __BUILD_FOR_MIPS_IRIX
# define __BUILD_FOR_IRIX
# define __BUILD_FOR_MIPS
# define __BUILD_FOR_BIG_ENDIAN
#endif

#ifdef __BUILD_FOR_MIPS4_IRIX
# define __BUILD_FOR_IRIX
# define __BUILD_FOR_MIPS4
# define __BUILD_FOR_BIG_ENDIAN
#endif

#ifdef __BUILD_FOR_ARM_LINUX
# define __BUILD_FOR_LINUX
# define __BUILD_FOR_ARM
# define __BUILD_FOR_BIG_ENDIAN
#endif

#ifdef __BUILD_FOR_ARM_SYMBIAN
# define __BUILD_FOR_SYMBIAN
# define __BUILD_FOR_ARM
# define __BUILD_FOR_LITTLE_ENDIAN
#endif

//
// Windows definitions
//
#ifdef __BUILD_FOR_WINXP
# ifdef __BUILD_FOR_INTEL
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
#endif

//
// Linux definitions
//
#ifdef __BUILD_FOR_LINUX
# ifdef __BUILD_FOR_INTEL
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
# ifdef __BUILD_FOR_SPARC
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
# ifdef __BUILD_FOR_ARM
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
#endif



//
// Darwin definitions
//
#ifdef __BUILD_FOR_DARWIN
# ifdef __BUILD_FOR_INTEL
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
#endif



//
// Solaris definitions
//
#ifdef __BUILD_FOR_SOLARIS
# ifdef __BUILD_FOR_INTEL
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
# ifdef __BUILD_FOR_SPARC
#  define __i8  char
#  define __i16 short
#  define __i32 int
# endif
#endif



//
// IRIX definitions
//
#ifdef __BUILD_FOR_IRIX
# define __i8  char
# define __i16 short
# define __i32 int
#endif



//
// SYMBIAN definitions
//
#ifdef __BUILD_FOR_SYMBIAN
# define __i8  char
# define __i16 short
# define __i32 int
#endif

#endif

