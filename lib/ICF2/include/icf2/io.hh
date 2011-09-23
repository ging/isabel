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
// $Id: io.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__io_hh__
#define __icf2__io_hh__

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>

#if defined(WIN32)
#include <io.h>
#elif defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <unistd.h>
#else
#error "Please, include read/write in your O.S."
#endif

#include <sys/types.h>


class _Dll_ io_t: public virtual item_t, public virtual collectible_t
{
protected:
    class __prot__handle_t: public virtual collectible_t
    {
    public:
        int sysHandle;

        __prot__handle_t(int h) {
            sysHandle= h;
        };

        ~__prot__handle_t(void) {
        };

        friend class smartReference_t<__prot__handle_t>;
    };
public:
    smartReference_t<__prot__handle_t>  __prot__handle;

    io_t(int);
    io_t(const io_t &);
    virtual ~io_t(void);

    virtual int read(void *, int);
    virtual int write(const void *, int);

    virtual io_t & operator=(const io_t &);


    virtual int sysHandle(void) const { return __prot__handle->sysHandle; }

    virtual const char *className(void) const { return "io_t"; };


    friend class smartReference_t<io_t>;
};


typedef smartReference_t<io_t> io_ref;
typedef ql_t<io_ref> IOList_t;

typedef smartReference_t<IOList_t> IOList_ref;


#endif
