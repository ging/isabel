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
// $Id: smartReference.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__smart_reference_hh__
#define __icf2__smart_reference_hh__

#include <icf2/general.h>

#include <assert.h>
#include <stdio.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <unistd.h>
#endif

template <class T> class _Dll_ smartReference_t
{
    T *__wrappedPointer;


    T*
    __adoptPointer(T *v= NULL) {
        if(v) {
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_mutex_lock(&v->__refCountMutex);
#elif defined(WIN32)
            WaitForSingleObject(v->__refCountMutex,INFINITE);
#else
#error "Please, lock mutex in your O.S."
#endif
#endif
            __wrappedPointer= v;
            __wrappedPointer->__refCount++;
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_mutex_unlock(&v->__refCountMutex);
#elif defined(WIN32)
            ReleaseMutex(v->__refCountMutex);
#else
#error "Please, unlock mutex in your O.S."
#endif
#endif
        } else {
            __wrappedPointer= NULL;
        }

        return v;
    }

    void
    __releasePointer(void) {
        __decref();
        __wrappedPointer= NULL;
    }

    T *__incref(void) const {
        if(__wrappedPointer) {

#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_mutex_lock(&__wrappedPointer->__refCountMutex);
#elif defined(WIN32)
            WaitForSingleObject(__wrappedPointer->__refCountMutex,INFINITE);
#else
#error "Please, lock mutex in your O.S."
#endif
#endif
            __wrappedPointer->__refCount++;
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_mutex_unlock(&__wrappedPointer->__refCountMutex);
#elif defined(WIN32)
            ReleaseMutex(__wrappedPointer->__refCountMutex);
#else
#error "Please, unlock mutex in your O.S."
#endif
#endif
        }

        return __wrappedPointer;
    }

    T *__decref(void) {
        bool mustDelete;
        if(__wrappedPointer) {

#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_mutex_lock(&__wrappedPointer->__refCountMutex);
#elif defined(WIN32)
            WaitForSingleObject(__wrappedPointer->__refCountMutex,INFINITE);
#else
#error "Please, lock mutex in your O.S."
#endif
#endif
            __wrappedPointer->__refCount--;
            mustDelete= (__wrappedPointer->__refCount== 0);

#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
            pthread_mutex_unlock(&__wrappedPointer->__refCountMutex);
#elif defined(WIN32)
            ReleaseMutex(__wrappedPointer->__refCountMutex);
#else
#error "Please, unlock mutex in your O.S."
#endif
#endif
            if(mustDelete) {
                delete __wrappedPointer;
            }

            __wrappedPointer= NULL;
        }

        return __wrappedPointer;
    }

    unsigned __getref(void) const {
        unsigned retVal;

#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        pthread_mutex_lock(&__wrappedPointer->__refCountMutex);
#elif defined(WIN32)
        WaitForSingleObject(__wrappedPointer->__refCountMutex,INFINITE);
#else
#error "Please, lock mutex in your O.S."
#endif
#endif
        retVal= __wrappedPointer->__refCount;
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        pthread_mutex_unlock(&__wrappedPointer->__refCountMutex);
#elif defined(WIN32)
        ReleaseMutex(__wrappedPointer->__refCountMutex);
#else
#error "Please, unlock mutex in your O.S."
#endif
#endif

        return retVal;
    }



public:
    smartReference_t(void) {
        __wrappedPointer= NULL;
    }

    smartReference_t(T *t) {
        __adoptPointer(t);
    }

    smartReference_t(smartReference_t &r) {
        __adoptPointer(r.__wrappedPointer);
    }

    smartReference_t(const smartReference_t &r) {
        __adoptPointer(r.__wrappedPointer);
    }

// --#ifdef __HAS_NESTED_TEMPLATES__
// --//    template<class O>
// --//    smartReference_t(smartReference_t<O> &r) {
// --//        Twrapper= new Twrapper_t (static_cast<T*>(r.Twrapper->__value));
// --//    }
// --
// --    template<class O>
// --    smartReference_t(const smartReference_t<O> &r) {
// --        Twrapper= new Twrapper_t (static_cast<T*>(static_cast<O*>(r)));
// --    }
// --#endif

    smartReference_t & operator = (T *t) {
        if(__wrappedPointer== t)
            return *this;

        __decref();
        __wrappedPointer= t; __incref();

        return *this;
    }

    smartReference_t & operator = (const smartReference_t &r) {
        if(this == &r)
            return *this;
        if(this->__wrappedPointer== r.__wrappedPointer)
            return *this;

        __decref();
        __wrappedPointer= r.__incref();

        return *this;
    }

#if 0
#ifdef __HAS_NESTED_TEMPLATES__
    template<class O>
    smartReference_t & operator = (smartReference_t<O> &r) {
        Twrapper->decref();
        Twrapper->__value= r.Twrapper->incref();

        return &this;
    }

    template<class O>
    smartReference_t & operator = (const smartReference_t<O> &r) {
        Twrapper->decref();
        Twrapper->__value= r.Twrapper->incref();

        return *this;
    }
#endif
#endif

    ~smartReference_t(void) {
        __releasePointer();
    }


    operator T * (void) const {
        return __wrappedPointer;
    }
    T * operator -> (void) const {
        return __wrappedPointer;
    }
    T & operator *  (void) const {
        return *__wrappedPointer;
    }
    T & operator [] (int n) const{
        assert(n== 0);
        return *__wrappedPointer;
    }


    int operator == (const smartReference_t &r) const {
        return 
          (
            int(this->isValid()) && 
            int(r.isValid())     && 
            ((this->__wrappedPointer)== (r.__wrappedPointer))
          ) 
          || 
          (
            !this->isValid() &&
            !r.isValid()
          );
    }
    int operator == (const T *t) const {
        return 
          (
            int(this->isValid()) && 
            int(t)     && 
            ((this->__wrappedPointer)== t)
          ) 
          || 
          (
            !this->isValid() &&
            !t
          );
    }

    int operator != (const smartReference_t &r) const {
        return !(r== *this);
    }
    int operator != (const T *t) const {
        return !(*this== t);
    }

    bool isValid(void) const {
        if(__wrappedPointer!= NULL)
            return true;
        else
            return false;
    }

    unsigned getNumUsers(void) const {
        return __getref();
    }
};


class _Dll_ collectible_t
{
protected:
    u32              __refCount;

#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_mutex_t  __refCountMutex;
#elif defined(WIN32)
    HANDLE  __refCountMutex;
#else
#error "Please, declare mutex in your O.S."
#endif
#endif




public:
    collectible_t(void): __refCount(0) {
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        pthread_mutex_init(&__refCountMutex, NULL);
#elif defined(WIN32)
        __refCountMutex = CreateMutex(NULL, FALSE, NULL);
#else
#error "Please, create mutex in your O.S."
#endif
#endif
    };

    collectible_t(const collectible_t &): __refCount(0) {
#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        pthread_mutex_init(&__refCountMutex, NULL);
#elif defined(WIN32)
        __refCountMutex = CreateMutex(NULL, FALSE, NULL);
#else
#error "Please, create mutex in your O.S."
#endif
#endif
    };

    virtual collectible_t & operator =(collectible_t const &) { return *this; }


    virtual ~collectible_t(void) { 
        assert(__refCount == 0);

#ifdef __THREADED_ICF
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
        pthread_mutex_destroy(&__refCountMutex);
#elif defined(WIN32)
        CloseHandle(__refCountMutex);
#else
#error "Please, destroy mutex in your O.S."
#endif
#endif
    }
};

typedef smartReference_t<collectible_t> collectible_ref;


#endif
