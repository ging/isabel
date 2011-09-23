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
// $Id: inputDriver.hh 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#ifndef __INPUT__DRIVER__
#define __INPUT__DRIVER__

#include <icf2/general.h>
#include <icf2/task.hh>
#include <icf2/stdTask.hh>

class inputDriver_t: public threadedTask_t {
   private:
       threadedTask_t *__outTask;
   public:
       inputDriver_t(io_ref &io, threadedTask_t *outTask);
       virtual ~inputDriver_t();

       virtual bool dispatchMsg(tlMsg_t *tlMsg);
       virtual void IOReady (io_ref &io);

       virtual const char *className() const { return "inputDriver_t";}
};

#endif
