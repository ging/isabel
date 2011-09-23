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
// $Id: fileIO.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <sys/types.h>

#include <icf2/general.h>
#include <icf2/fileIO.hh>


fileIO_t::fileIO_t(const char *fnam, fMode_e m, fAction_e a, int access)
:io_t(::open(fnam, a|m, access))
{
    ;
}

fileIO_t::~fileIO_t(void)
{
    ::close(__prot__handle->sysHandle);
}

off_t
fileIO_t::seek(off_t pos) const
{
    return lseek(__prot__handle->sysHandle, pos, SEEK_SET);
}

off_t
fileIO_t::tell(void) const
{
    return lseek(__prot__handle->sysHandle, 0L, SEEK_CUR);
}

