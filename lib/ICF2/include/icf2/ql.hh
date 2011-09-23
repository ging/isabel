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
// $Id: ql.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__ql_hh__
#define __icf2__ql_hh__

#include <assert.h>

#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/smartReference.hh>


template <class T> class _Dll_ ql_t
: public virtual item_t, public virtual collectible_t
{
private:
    class __queueNode_t {
    public:
        T           val;
        __queueNode_t *next;

        __queueNode_t(const T &v): val(v) { next= NULL; };
    };


    __queueNode_t *__first;
    __queueNode_t *__last;

    unsigned __len;

public:
    class iterator_t {
    private:
        __queueNode_t *__val;

    public:
        iterator_t(__queueNode_t *v= NULL): __val(v) {};
        operator T&(void) { return __val->val;  };
        iterator_t &operator ++(void) { __val= __val->next; return *this; };
        iterator_t &operator ++(int ) { __val= __val->next; return *this; };

        int operator ==(const iterator_t &o) const { return __val== o.__val; };
        int operator !=(const iterator_t &o) const { return __val!= o.__val; };

        friend class ql_t<T>;
    };

    iterator_t begin(void) const { return static_cast<iterator_t>(__first); }
    iterator_t end  (void) const { return static_cast<iterator_t>(NULL) ; }

    ql_t(void) {
        __first= __last= NULL;
        __len= 0;
    };

    ql_t(const ql_t &orig) {
        __first= __last= NULL;
        __len= 0;

        insert(orig);
    }

    ~ql_t(void) {
        __queueNode_t *aux = __first;
        __queueNode_t *aux2= NULL;
 
        while(aux) {
            aux2= aux->next;
            delete aux;
            aux= aux2;
        }
        __first= __last= NULL;
        __len= 0;
    };


    unsigned len(void) const {
        return __len;
    };

    ql_t &operator =(const ql_t &orig) {

        while(len())
            behead();

        insert(orig);

        return *this;
    };



    //
    // insertion methods
    //
    ql_t &insert(T val, bool= true) {
        __queueNode_t *v= new __queueNode_t(val);

        if(!__first)
            __first= v;

        if(__last)
            __last->next= v;

        __last= v;

        __len++;

        return *this;
    };

    ql_t &insert(const ql_t &orig) {
        __queueNode_t *ptr= orig.__first;
        while(ptr) {
            insert(ptr->val);
            ptr=ptr->next;
        }

        return *this;
    };

    ql_t & operator <<(T val) {
        return insert(val, false);
    };

    ql_t &operator <<(const ql_t &orig) {
        return insert(orig);
    };



    //
    // indexacion
    //
    T head(void) const {
        assert(__first && "empty queue");

        return __first->val;
    };


    ql_t &behead(void) {
        assert(__first && "empty queue");

        if(__first) {
            __queueNode_t *aux;
            aux = __first;

            __first= __first->next;
            delete aux;

            if(!__first) __last= NULL;

            __len--;
        }

        return *this;
    };

    ql_t & operator >>(T &val) {
        //val= head();
        T v=head();
        val= v;

        behead();

        return *this;
    };

    
    virtual const char *className(void) const { return "<ql_t>"; };

    friend class smartReference_t< ql_t<T> >;
};


typedef ql_t<item_t> itemList_t;
typedef smartReference_t<itemList_t> itemList_ref;

#endif

