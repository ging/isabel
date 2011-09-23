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
#ifndef AVM_DEFAULT_H
#define AVM_DEFAULT_H

#include <icf2/general.h>

#define AVM_BEGIN_NAMESPACE namespace avm {
#define AVM_END_NAMESPACE   }

#define __attribute__(x)

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    #include <stdint.h>
    #include <inttypes.h>
#elif defined(__BUILD_FOR_WINXP)
/* '<Unknown'> has C-linkage specified, but returns UDT 'basic_string<char, struct ...' */
 #pragma warning (disable: 4190)
/* 'std::_Tree(std::basic_string<... ': identifier was truncated to '255' characters in the debug information */
 #pragma warning (disable: 4786)
/* 'unsigned char': forcing value to bool 'true' or 'false' */
#pragma warning (disable: 4800)
    typedef __int64  int64_t;
    typedef __int32  int32_t;
    typedef __int16  int16_t;
    typedef __int8   int8_t;
    typedef unsigned __int64  __uint64;
    typedef unsigned __int32  __uint32;
    typedef unsigned __int16  __uint16;
    typedef unsigned __int8   __uint8;
    typedef __uint64 uint64_t;
    typedef __uint32 uint32_t;
    typedef __uint16 uint16_t;
    typedef __uint8  uint8_t;
#endif    
#define E_ERROR -2

typedef uint32_t fourcc_t;
typedef unsigned int uint_t;    /* use as generic type */
typedef unsigned int framepos_t;
typedef uint_t streamid_t;	/* \obsolete use uint_t */
typedef uint_t Unsigned; 	/* \obsolete use uint_t */

#ifdef X_DISPLAY_MISSING
typedef int Display;
#endif


/**
 * \namespace avm
 *
 * encupsulates functions & classes from the avifile library
 *
 * \author Zdenek Kabelac (kabi@users.sourceforge.net)
 * \author Eugene Kuznetsov (divx@euro.ru)
 */

#define AVM_COMPATIBLE  /* define & build backward compatible code */

#endif /* AVIFILE_DEFAULT_H */
