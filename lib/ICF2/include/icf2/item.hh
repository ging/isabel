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
// $Id: item.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__item_hh__
#define __icf2__item_hh__

#include <icf2/general.h>

#include <stdio.h>
#include <stdarg.h>

class _Dll_ item_t
{
protected:
    u32 itemId;

//  debugLevel { 
//    1   => debugMask = dbg_App_Normal 
//    10  => debugMask = dbg_App_Normal | dbg_App_Verbose
//    20  => debugMask = dbg_App_Normal | dbg_App_Verbose | dbg_App_Paranoic
//    100 => debugMask = dbg_App_Normal | dbg_App_Verbose | dbg_App_Paranoic |
//                       dbg_K_Normal
//    200 => debugMask = dbg_App_Normal | dbg_App_Verbose | dbg_App_Paranoic |
//                       dbg_K_Normal   | dbg_K_Verbose
//    666 => debugMask = dbf_App_Normal | dbg_App_Verbose | dbg_App_Paranoic |
//                       dbg_K_Normal   | dbg_K_Verbose   | dbg_K_Paranoic
// };

    enum debugLevel_e {
        app_normal=1, app_verbose=10, app_paranoic=20,
        k_normal=100, k_verbose=200,  k_paranoic=666
    };

    enum debugMask_e {
        dbg_Show_Always    =      00,     // decimal=0
        dbg_No_Msg         =      01,     // decimal=1

        // Kernel debug levels
        dbg_K_Normal       =      02,     // decimal=2     (bit 2)
        dbg_K_Verbose      =      04,     // decimal=4     (bit 3)
        dbg_K_Paranoic     =     010,     // decimal=8     (bit 4)
        dbg_K_Internal     =     020,     // decimal=16    (bit 5)

        // Module debug levels
        dbg_Lib_Low        =     040,     // decimal=32    (bit 6)
        dbg_Lib_Info       =    0100,     // decimal=64    (bit 7)
        dbg_Lib_Normal     =    0200,     // decimal=128   (bit 8)
        dbg_Lib_Verbose    =    0400,     // decimal=256   (bit 9)
        dbg_Lib_Paranoic   =   01000,     // decimal=512   (bit 10)
        dbg_Lib_Internal   =   02000,     // decimal=1024  (bit 11)

        // Application debug levels
        dbg_App_Low        =   04000,     // decimal=2048  (bit 12)
        dbg_App_Info       =  010000,     // decimal=4096  (bit 13)
        dbg_App_Normal     =  020000,     // decimal=8192  (bit 14)
        dbg_App_Verbose    =  040000,     // decimal=16384 (bit 15)
        dbg_App_Paranoic   = 0100000,     // decimal=32768 (bit 16)
        dbg_App_Internal   = 0200000,     // decimal=65536 (bit 17)

        // To ensure that the underlying
        // integral type for this enum is at least 32 bits.
        // NOT USE IT
        DBG_ENSURE_32_BITS = 0x7FFFFFFF
    };

    static int debugMask;
    static int debugLevel; // deprecated
    static FILE *debugFile;

    struct binding_t {
        const char *name;
        debugMask_e mask;
    };

    static binding_t bindMask[];
    static u32       levelN;
   

public:
    item_t(void)        
    { 
        itemId = registerItem(this);
    };

    item_t(const item_t &) 
    {
        itemId = registerItem(this);
    }//;

    virtual ~item_t(void) 
    {
    };

    item_t &operator =(const item_t &) { return *this; };


    debugMask_e strToMask(char *s);
    void setDebugLevel(int level);

    //
    // debug support
    //
    void _debugMsg(debugMask_e dbg, char const *method, 
                   char const *msg, va_list ap);


#ifdef __DEBUG_ICF
    inline void debugMsg(debugMask_e dbg, char const *method, 
                         char const *msg, ...) 
    {
        va_list ap;
        va_start(ap, msg);

        _debugMsg(dbg, method, msg, ap);

        va_end(ap);
    };

#else
    inline void debugMsg(debugMask_e, char const *, char const *, ...) { ; }
#endif

    virtual const char *className(void) const { return "item_t"; };

#if 0
    class debugSection_t {
    private:
        debugSection_t(const char *
    public:
    }
    virtual debugSection_t &beginDebugSection(const char *f) {
        return ;
    }
#endif


    //
    // some friends
    //
    friend _Dll_ u32 registerItem(const item_t *);
};


#endif

