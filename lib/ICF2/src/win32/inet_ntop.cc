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
// $Id: inet_ntop.cc 20756 2010-07-05 09:57:09Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/inet_ntop.hh>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ws2tcpip.h>

//
// forward declarations
//
char * inet_ntop4 ( char* dst,size_t size,unsigned char* src);
char * inet_ntop6 (unsigned char* src, char* dst,size_t size);

/* char *
 * inet_ntop(af, src, dst, size)
 *  convierte formato network a presentacion	
 * return:
 *	puntero al formato de representacion
 */

char*
inet_ntop (int af,unsigned char* src, char* dst,size_t size)
{
	
	switch (af) {
	case AF_INET:
		return (inet_ntop4(dst, size,src));
	case AF_INET6:
	
		return (inet_ntop6(src, dst, size));

	default:
		errno = EAFNOSUPPORT;
		return (NULL);
	}

}

/* const char *
 * inet_ntop4(src, dst, size)
 */

char *
inet_ntop4( char* dst,size_t size,unsigned char* src = NULL)
{
	static const char fmt[] = "%u.%u.%u.%u";
	static char tmp[sizeof "255.255.255.255"];
	if (SPRINTF((tmp, fmt, src[0], src[1], src[2], src[3])) > size) {
		errno = ENOSPC;
		return (NULL);
	}
	strcpy(dst, tmp);
	return (dst);
	
}

/* const char *
 * inet_ntop6(src, dst, size)
 */

char *
inet_ntop6(unsigned char* src, char* dst,size_t size)
{
	static char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
        char *tp;
	struct { int base, len; } best, cur;
	unsigned int words[IN6ADDRSZ / INT16SZ];
	int i;

	memset(words, '\0', sizeof words);
	for (i = 0; i < IN6ADDRSZ; i++)
		words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	tp = tmp;
	for (i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		if (i != 0)
			*tp++ = ':';
		if (i == 6 && best.base == 0 &&
		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4( tp, sizeof tmp - (tp - tmp),src+12))
				return (NULL);
			tp += strlen(tp);
			break;
		}
		tp += SPRINTF((tp, "%x", words[i]));
	}
	if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';

	if ((size_t)(tp - tmp) > size) {
		errno = ENOSPC;
		return (NULL);
	}
	strcpy(dst, tmp);
	return (dst);
};

 
