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
// $Id: cftpApp.h 20656 2010-06-23 14:13:58Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __CftpAPP_H__
#define __CftpAPP_H__

#include <icf2/sched.hh>
#include <icf2/stdTask.hh>

#include "cftpRTPSession.h"
#include "fchannelMgr.h"

class cftpApp_t : public application_t 
{
public:
   cftpApp_t(int &argc, argv_t &argv);
   virtual   ~cftpApp_t(void);

   fchannelMgr_t *theChanMgr; 
};

#endif
