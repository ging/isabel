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
// $Id: dictionary.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__dictionary_hh__
#define __icf2__dictionary_hh__

#include <stdlib.h>
#include <string.h>

#include <icf2/item.hh>
#include <icf2/smartReference.hh>
#include <icf2/ql.hh>


#define DEFAULT_DICTIONARY_SIZE (128)


//
// dictionary_t<K,T> -- genereic dictionary
//
//
// This is template for implementing dictionaries. The template
// parameters are:
//    + K: the type of the key
//    + T: the type of the stored data
//
// Five public methods are supplied:
//
//    + bool insert(K key, T v);
//    + bool replace(K key, T v);
//    + bool remove(K key);
//    + T        lookUp(K key) const;
//    + ql_t<K> *getKeys(void) const;
//    + ql_t<T> *getValues(void) const;
//
// The "insert" method is used to insert a pair (K,T) into the dictionary.
// It will return 'true' if the insertion actually took
// place, 'false' if not. Currently the only reason to fail
// on insertion is that the dictionary already has a pair with
// the same key.
//
// The "replace" method is used to insert a pair (K,T) into the dictionary.
// It will return 'true' if the insertion actually took
// place, 'false' if not. Currently the only reason to fail
// on insertion is that the dictionary already has a pair with
// the same key.
//
// The "remove" method is used to remove a value from the
// dictionary given its key.
//
// The "lookUp" method is used to find a value given its key. Note that
// a copy of the valueis returned not a reference to it...
// check NOTES below.
//
// The "getKeys" and "getValues" methods are provided for browsing the
// dictionary, they return a list of keys/values. The user
// is responsible of deleting them.
//
//
// NOTES:
//
//   + The dictionary assumes that values and keys can be safely
//   copied. For most applications values will be smartReference_t<>
//   objects and keys will be either smartReference_t<> or integral
//   types.
//
//   + Current implementation is under optimal and should be only used
//   for small dictionaries.
//
//
// Details on smartReference_t<> can be checked at <icf2/smartReference.h>
// Details on ql_t<> can be checked at <icf2/ql.h>
//

template <class K, class T> class _Dll_ dictionary_t
: public virtual item_t, public virtual collectible_t
{
private:
    //
    // internal class
    //
    struct __holder_t {
        K        key;
        T        value;
    };

    __holder_t **__data;

    int __size;
    int __numUsed;
    mutable int __foundFree;


    int __lookUp(K key) const {
        dictionary_t *wThis= const_cast<dictionary_t<K,T> *>(this);

        wThis->__foundFree= -1;
        for(int i= 0; i< __numUsed; i++) {
            if(__data[i]) {
                if(_priv_compare_keys(__data[i]->key, key))
                    return i;
            } else
                wThis->__foundFree= i;
        }

        return -1;
    };

    virtual int _priv_compare_keys(K const key1, K const key2) const {
              return (key1==key2);
    };

    virtual int _priv_compare_values(T const value1, T const value2) const {
              return (value1==value2);
    };


public:
    dictionary_t(int size= DEFAULT_DICTIONARY_SIZE) {
        __size= size;
        __data= (__holder_t **)malloc(__size*sizeof(__holder_t*));

        for(int i= 0; i< __size; i++) {
            __data[i]= NULL;
        }

        __numUsed= 0;
        __foundFree= -1;
    };

    dictionary_t(const dictionary_t<K,T> &other) {
        //mismo tamaño que original
        __size= other.__size;

        //inicializamos igual que constructor por defecto
        __data= (__holder_t **)malloc(__size*sizeof(__holder_t*));

        for(int i= 0; i< __size; i++) {
            __data[i]= NULL;
        }

        __numUsed= 0;
        __foundFree= -1;

        //copiar contenido del original
        for(int i= 0; i< other.__numUsed; i++)
        {
            if(other.__data[i])
            {
                insert(other.__data[i]->key,other.__data[i]->value);
            }
        }
    };

    virtual ~dictionary_t(void) {
        for(int i= 0; i< __size; i++)
            if(__data[i]) {
				__holder_t * tmp = __data[i];
                __data[i]= NULL;
                delete tmp;
            }
        free(__data);
        __data= NULL;
    }

    virtual bool insert(K key, T v) {
        debugMsg(dbg_App_Normal, "insertar", "inserto clave");
        if(__lookUp(key)>= 0)
            return false;       // already in dictionary

        if(__foundFree>= 0) {
            __data[__foundFree]= new __holder_t;
            __data[__foundFree]->key= key;
            __data[__foundFree]->value= v;
        } else {
            if(__numUsed== __size) {
                int        nSize= static_cast<int>(__size * 1.618);
                __holder_t **nData= (__holder_t **)malloc(nSize*sizeof(__holder_t*));

                for(int i= 0; i< __size; i++) {
                    nData[i]= __data[i];
                    __data[i]= NULL;
                }
                for(int j= __size; j< nSize; j++) {
                    nData[j]= NULL;
                }

                free(__data);

                __size= nSize;
                __data= nData;
            }

            __data[__numUsed]= new __holder_t;
            __data[__numUsed]->key= key;
            __data[__numUsed]->value= v;

            __numUsed++;
        }

        return true;
    };

    virtual bool replace(K key, T v) {
        int idx= __lookUp(key);
        if(idx< 0) {
            insert(key, v);
            return false;       // already in dictionary
        }

        __data[idx]->value= v;

        return true;
    };

    virtual bool remove(K key) {
        int foundKey= __lookUp(key);

        if(foundKey< 0)
            return false;       // not present in dictionary

        __holder_t* tmp = __data[foundKey];
        __data[foundKey]= NULL;
        delete tmp;

        if(foundKey== __numUsed-1) // last one
            __numUsed--;

        return true;
    };

    virtual T lookUp(K key) const {
        int tgt= __lookUp(key);

        if(tgt< 0)
            return NULL;
        else
            return __data[tgt]->value;
    };

    virtual ql_t<K> *getKeys(void) const {
        ql_t<K> *retVal= new ql_t<K>;

        for(int i= 0; i< __numUsed; i++)
            if(__data[i])
                (*retVal)<< __data[i]->key;

        return retVal;
    }

    virtual ql_t<T> *getValues(void) const {
        ql_t<T> *retVal= new ql_t<T>;

        for(int i= 0; i< __numUsed; i++)
            if(__data[i])
                (*retVal)<< __data[i]->value;

        return retVal;
    }

    virtual const char *className(void) const { return "<dictionary_t>"; }

    friend class smartReference_t< dictionary_t<K, T> >;
};

#endif

