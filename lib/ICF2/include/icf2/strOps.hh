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
// $Id: strOps.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__strops_hh__
#define __icf2__strops_hh__

#include <limits.h>

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/smartReference.hh>



extern char _Dll_ **buildFormattedArgv(const char *s, int &argc, int trim= 1);
extern char _Dll_ **buildArgv(const char *s, int &argc, int trim= 1, int maxSlices= INT_MAX);
extern void _Dll_   freeArgv(int argc, char **argv);

extern u32  _Dll_   isInetAddr(char *s);


class _Dll_ concatStr_t: public virtual item_t, 
                         public virtual collectible_t 
{
private:
    char       *__str;
    unsigned    __size;
    unsigned    __len;
    
public:

    concatStr_t(unsigned initSize= 256);

    concatStr_t(const concatStr_t &other); 

    virtual ~concatStr_t(void);

    concatStr_t &operator =(const concatStr_t &other); 

    char     *getString(void) const { return __str; }
    unsigned  getSize  (void) const { return __size; }
    unsigned  getLen   (void) const { return __len; }
    
    void concat(const char *fmt, ...); 

    virtual const char *className(void) const { return "concatStr_t"; }
    friend class smartReference_t<concatStr_t>;
};

typedef smartReference_t<concatStr_t> concatStr_ref;


#endif
