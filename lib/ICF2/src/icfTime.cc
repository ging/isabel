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
// $Id: icfTime.cc 22764 2011-08-03 14:00:23Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/icftime.hh>

#if defined(WIN32)
int gettimeofday(timeval *time, struct timezone *tz)
{
	_timeb __the_time;
	_ftime(&__the_time);
	time->tv_sec = __the_time.time;
	time->tv_usec = __the_time.millitm*1000;

    return 0; // success
}
#elif defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
// nothing to do
#else
#error "Please, check if you need to implement gettimeofday in your OS"
#endif

