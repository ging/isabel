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
// $Id: lockSupport.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__lock_support__hh__
#define __icf2__lock_support__hh__

#include <icf2/item.hh>
#include <icf2/smartReference.hh>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <pthread.h>
#include <assert.h>
#elif defined(WIN32)
#include <icf2/conditionalvar.hh> // para usar event_t
#else
#error "Please, include lock support for your O.S."
#endif

class _Dll_ lockedItem_t
{
private:
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_t mutex;
#elif defined(WIN32)
    HANDLE mutex;
#else
#error "Please, declared mutex in your O.S."
#endif

    void __lock(void);
    void __unlock(void);
#endif

protected:
    class _locker_t: public virtual item_t, public virtual collectible_t {
    private:
        lockedItem_t *p;

        _locker_t(lockedItem_t *_p) {
            p=_p;
#ifdef __THREADED_ICF
            p->__lock();
#endif
        };

    public:
        ~_locker_t(void) {
#ifdef __THREADED_ICF
            p->__unlock();
#endif
        };
    
        void nop(void) {};


        friend class lockedItem_t;        
        friend class smartReference_t<_locker_t>;
    };
    typedef smartReference_t<_locker_t> locker_t;

public:

    lockedItem_t(void);
    virtual ~lockedItem_t(void);

    locker_t lock(void);


    friend class lockedItem_t::_locker_t;
};




class _Dll_ rwLockedItem_t
{
private:
#ifdef __THREADED_ICF

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_t __wrLock;
    pthread_mutex_t __rdLock;
    pthread_cond_t  __rdCountCond;
#elif defined(WIN32)
    HANDLE  __wrLock;
    HANDLE  __rdLock;
    Event_t __rdCountCond; // aqui se crea e inicializa Evento
#else
#error "Please, declared locks in your O.S."
#endif

    int             __rdCount;

    void __rd_lock(void);
    void __wr_lock(void);
    void __rd_unlock(void);
    void __wr_unlock(void);
#endif

protected:
    class _locker_t: public virtual item_t, public virtual collectible_t {
    private:
        enum OP_e {
            RD,
            WR
        } o;
        rwLockedItem_t *p;

        _locker_t(rwLockedItem_t *_p, OP_e _o) {
            p= _p;
            o= _o;

#ifdef __THREADED_ICF
            switch(o) {
                case RD:
                    p->__rd_lock();
                    break;
                case WR:
                    p->__wr_lock();
                    break;
                default:
                    assert((o== RD) || (o== WR));
                    break;
            };
#endif
        };

    public:
        ~_locker_t(void) {
#ifdef __THREADED_ICF
            switch(o) {
                case RD:
                    p->__rd_unlock();
                    break;
                case WR:
                    p->__wr_unlock();
                    break;
                default:
                    assert((o== RD) || (o== WR));
                    break;
            };
#endif
        };
    
        void nop(void) {};

        friend class rwLockedItem_t;
        friend class smartReference_t<_locker_t>;
    };
    typedef smartReference_t<_locker_t> locker_t;

public:

    rwLockedItem_t(void);
    virtual ~rwLockedItem_t(void);

    locker_t rdLock(void);
    locker_t wrLock(void);


    friend class rwLockedItem_t::_locker_t;
};


#endif
