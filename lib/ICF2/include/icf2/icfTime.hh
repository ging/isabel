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
// $Id: icfTime.hh 22763 2011-08-03 13:59:57Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__icftime_hh__
#define __icf2__icftime_hh__

#include <icf2/general.h>

#if defined(WIN32)

#include <time.h>
#include <sys/timeb.h>

int gettimeofday(struct timeval *time, struct timezone *tz);

#elif defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)

#include <sys/time.h>

#else
#error "Please, define gettimeofday for your O.S."
#endif

#endif
