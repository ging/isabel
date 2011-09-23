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
// $Id: isabelSDK.hh 11675 2007-10-03 14:52:16Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __isabel_sdk__hh__
#define __isabel_sdk__hh__

int  registerSyntheticGrabber(void);
void releaseSyntheticGrabber(void);

int  registerFileReader(void);
void releaseFileReader(void);

#if defined(__BUILD_FOR_LINUX)
int  registerV4LuGrabber(void);
void releaseV4LuGrabber(void);
#endif

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
int  registerShmGrabber(void);
void releaseShmGrabber(void);

int  registerWinGrabber(void);
void releaseWinGrabber(void);

int  registerURLGrabber(void);
void releaseURLGrabber(void);
#endif

#if defined(__BUILD_FOR_WINXP)
int  registerRfbSource(void);
void releaseRfbSource(void);

int  registerDSGrabber(void);
void releaseDSGrabber(void);
#endif

#endif

