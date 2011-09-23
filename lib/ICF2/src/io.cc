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
// $Id: io.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <icf2/general.h>
#include <icf2/io.hh>


io_t::io_t(int h): __prot__handle(new __prot__handle_t(h))
{
    ;
}

io_t::io_t(const io_t &orig)
{
    __prot__handle= orig.__prot__handle;
}

io_t::~io_t(void)
{
    __prot__handle= NULL;
}

int
io_t::read(void *b, int n)
{
    return ::read(__prot__handle->sysHandle, b, n);
}

int
io_t::write(const void *b, int n)
{
    return ::write(__prot__handle->sysHandle, b, n);
}

io_t &
io_t::operator =(const io_t &orig)
{
    __prot__handle= orig.__prot__handle;

    return *this;
}


