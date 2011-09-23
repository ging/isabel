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
// $Id: resource.hh 20756 2010-07-05 09:57:09Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__resource_hh__
#define __icf2__resource_hh__

#include   <icf2/general.h>

#include   <stdio.h>
#include   <io.h>
#include   <errno.h>
#include   <wtypes.h>
#include   <winbase.h>

#ifdef WIN32

#define RLIMIT_CPU      0   /* limit on CPU time per process */
#define RLIMIT_FSIZE    1   /* limit on file size */
#define RLIMIT_DATA     2   /* limit on data segment size */
#define RLIMIT_STACK    3   /* limit on process stack size */
#define RLIMIT_CORE     4   /* limit on size of core dump file */
#define RLIMIT_NOFILE   5   /* limit on number of open files */
#define RLIMIT_AS       6   /* limit on process total address space size */
#define RLIMIT_VMEM     RLIMIT_AS

#define RLIM_NLIMITS    7



/*
 * process resource limits definitions
 */

struct _Dll_ rlimit {
//        LARGE_INTEGER  rlim_cur;
//        LARGE_INTEGER  rlim_max;
          __int64    rlim_cur;
          __int64    rlim_max;
};

typedef struct rlimit  rlimit_t;

/*
 * Prototypes
 */
int _Dll_ getrlimit(int resource, struct rlimit *);
int _Dll_ setrlimit(int resource, const struct rlimit *);

size_t rfwrite( const void *buffer, size_t size, size_t count, FILE
*stream );
int _rwrite( int handle, const void *buffer, unsigned int count );

//
// Following are the prototypes for the real functions...
//
// size_t fwrite( const void *buffer, size_t size, size_t count, FILE*stream );
// int _write( int handle, const void *buffer, unsigned int count );

#endif

#endif

