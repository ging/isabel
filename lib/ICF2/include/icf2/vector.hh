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
// $Id: vector.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__vector_hh__
#define __icf2__vector_hh__


#include <icf2/general.h>
#include <icf2/item.hh>
#include <icf2/smartReference.hh>

#include <assert.h>

const int INITIAL_CAPACITY= 50;

//
// This class emulates partially the java.util.Vector
// basically, it is a smart resizeable array
//
// Methods:
// void add(T val): appends element at the end of this Vector
// int capacity(void): returns the current capacity of this Vector
// T elementAt(int index): returns the component at the specified index.
// int size(void): returns the number of components in this Vector
// T remove(int index): removes the element at the specified position and
//                      shifts any subsequent elements to the left
//                      (thus, substracting one from their indices).
// Operators:
// assignment: redefined so as the sentence 'v1= v2', first v1 is emptied,
//             then "copies" all elements from v2 to v1
//
template <class T> class _Dll_ vector_t
: public virtual item_t, public virtual collectible_t
{
private:

    T *__arr;

    int __capacity;
    int __size;

public:

    vector_t(void)
    {
        __size= 0;
        __capacity= INITIAL_CAPACITY;
        __arr= new T[INITIAL_CAPACITY];
    };

    vector_t(const vector_t<T> &other)
    {
        __size= 0;
        __capacity= INITIAL_CAPACITY;
        __arr= new T[INITIAL_CAPACITY];
        while(size())
            remove(0);

        for (int i= 0; i < other.size(); i++)
            add(other.elementAt(i));
    };

    ~vector_t(void)
    {
        delete []__arr;
    };

    vector_t &operator =(const vector_t &orig)
    {
        while(size())
            remove(0);

        for (int i= 0; i < orig.size(); i++)
            add(orig.elementAt(i));

        return *this;
    };

    int capacity(void) const
    {
        return __capacity;
    }

    int size(void) const
    {
        return __size;
    };

    void add(T val)
    {
        assert( (__size <= __capacity) && ("__size is out of range"));
        if (__size == __capacity)
        {
            // allocates more room
            __capacity *= 2;
            T *newArr= new T[__capacity];

            for (int i= 0; i < __size; i++)
            {
                newArr[i]= __arr[i];
            }
            delete []__arr;

            __arr= newArr;
        }

        __arr[__size++] = val;
    };

    T elementAt(int index) const
    {
        assert( (index >= 0) && (index < __size) && ("index out of range"));

        return __arr[index];
    };

    T remove(int index)
    {
        T val= elementAt(index);

        // shift elements
        for (int i= index; i < __size-1; i++)
        {
            __arr[i]= __arr[i+1];
        }

        __size--;

        return val;
    }

    virtual const char *className(void) const { return "<vector_t>"; };

    friend class smartReference_t< vector_t<T> >;
};

#endif
