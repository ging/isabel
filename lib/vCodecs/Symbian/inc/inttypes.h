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
/* 7.8 Format conversion of integer types <inttypes.h> */

#ifndef __VCODECSSYMBIAN_INTTYPES_H_
#define __VCODECSSYMBIAN_INTTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
#ifndef	int8_t
typedef signed char int8_t;
#endif
#ifndef	uint8_t
typedef unsigned char uint8_t;
#endif
#ifndef	int16_t
typedef int int16_t;
#endif
#ifndef	uint16_t
typedef unsigned int uint16_t;
#endif 
#ifndef	int32_t
typedef long int32_t;
#endif
#ifndef	uint32_t
typedef unsigned long uint32_t;
#endif 
#ifndef	int64_t
typedef long long int64_t;
#endif
#ifndef	uint64_t
typedef unsigned long long uint64_t;
#endif 
#ifndef	intptr_t
typedef int16_t intptr_t;
#endif
#ifndef	uintptr_t
typedef uint16_t uintptr_t;
#endif
#ifndef	DCTELEM
typedef short DCTELEM;
#endif
#ifdef __cplusplus
}
#endif

#endif /* ndef _INTTYPES_H */
