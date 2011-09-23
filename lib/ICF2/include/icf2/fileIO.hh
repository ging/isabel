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
// $Id: fileIO.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__file_io_hh__
#define __icf2__file_io_hh__

#include <fcntl.h>

#include <icf2/general.h>
#include <icf2/io.hh>

class fileIO_t: public io_t
{
public:
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    enum fMode_e {
        READ_ONLY= O_RDONLY,
        WRITE_ONLY= O_WRONLY,
        READ_WRITE= O_RDWR
    };
    enum fAction_e {
        NONE= 0,
        CREATE= O_CREAT|O_TRUNC
    };
#elif defined(WIN32)
    enum fMode_e {
        READ_ONLY= _O_RDONLY,
        WRITE_ONLY= _O_WRONLY,
        READ_WRITE= _O_RDWR
    };
    enum fAction_e {
        NONE= 0,
        CREATE= _O_CREAT|_O_TRUNC
    };
#else
#error "Please, defined file access for your O.S."
#endif

    fileIO_t(const char *,
             fMode_e= READ_WRITE,
             fAction_e= NONE,
             int access= 0666
            );
    virtual ~fileIO_t(void);

    virtual off_t seek(off_t pos) const;
    virtual off_t tell(void) const;


    virtual const char *className(void) const { return "fileIO_t"; };
};
#endif

