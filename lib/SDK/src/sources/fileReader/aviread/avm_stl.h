/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVIFILE_AVM_STL_H
#define AVIFILE_AVM_STL_H

// some basic types for our C++ usage
// we are replacing overbloated STL
//#include <stdio.h>
#include <assert.h>

#include "avm_default.h"

AVM_BEGIN_NAMESPACE;

/**
 * Simple class for storing char*
 *
 *
 * The main reason for existance of this class is faster compilation.
 * We do not need overcomplicated  std::string class for our purpose.
 * The behaviour of implemented methods should mostly match them 1:1
 */
class string
{
public:
    typedef uint_t size_type;
    static const size_type npos = ~0U;
    string();
    string(char s);
    string(const char* s, uint_t len = 0);
    string(const string& s, uint_t len = 0);
    ~string();
    uint_t size() const { uint_t i = 0; while (str[i]) i++; return i; }
    char operator[](int i) const { return str[i]; }
    char& operator[](int i) { return str[i]; }
    bool operator==(const char* s) const;
    bool operator==(const string& s) const { return operator==(s.str); }
    bool operator!=(const char* s) const { return !operator==(s); }
    bool operator!=(const string& s) const { return !operator==(s); }
    bool operator<(const string& s) const;
    operator const char*() const { return str; }
    const char* c_str() const { return str; }
    string& operator=(const char* s);
    string& operator=(const string& s) { return operator=(s.str); }
    string& operator+=(const char* s);
    string& operator+=(const string& s) { return operator+=(s.str); }
    string operator+(const char* s) const { return string(str) += s; }
    string operator+(const string& s) const { return string(str) += s; }
    string substr(uint_t from = 0, uint_t to = npos) const { return string(str + from, to); };
    size_type find(const string& s, size_type startpos = 0) const;
    size_type find(char c) const;
    size_type rfind(char c) const;
    void insert(uint_t pos, const string& s);
    string& erase(uint_t from = 0, uint_t to = npos);
    int sprintf(const char* fmt, ...); // allocates size
    // it will use just 1024 bytes for non _GNU_SOURCE compilation!!

protected:
    char* str;
};

// without this operator attempt to compare const char* with string will give quite unexpected results because
// of implicit usage of operator const char*() with the right operand
inline bool operator==(const char* s1, const string& s2) { return s2==s1; }


/**
 * Simple queue ring template class
 *
 * usable for ring buffer with given size
 */
template <class Type> class qring
{
public:
    qring<Type>(uint_t rcapacity) : m_uiCapacity(rcapacity), m_uiPos(0), m_uiSize(0)
    {
	m_Type = new Type[m_uiCapacity];
    }
    ~qring() { delete[] m_Type; }
    qring<Type>(const qring<Type>& t) : m_Type(0)
    {
	operator=(t);
    }
    qring<Type>& operator=(const qring<Type>& t)
    {
	if (this != &t)
	{
	    Type* tmp = new Type[t.m_uiCapacity];
	    m_uiCapacity = t.m_uiCapacity;
	    m_uiPos = t.m_uiPos;
	    m_uiSize = t.m_uiSize;
	    for (uint_t i = 0; i < m_uiSize; i++)
	    {
		uint_t p = fpos() + i;
		if (p >= m_uiCapacity)
                    p -= m_uiCapacity;
		tmp[p] = t.m_Type[p];
	    }
	    delete[] m_Type;
	    m_Type = tmp;
	}
        return *this;
    }
    void clear() { m_uiPos = m_uiSize = 0; }
    Type& back() { return m_Type[bpos()]; }
    const Type& back() const { return m_Type[bpos()]; }
    Type& front() { return m_Type[fpos()]; }
    const Type& front() const {	return m_Type[fpos()]; }
    bool empty() const { return (m_uiSize == 0); }
    bool full() const { return (m_uiSize + 1) >= m_uiCapacity; }
    void insert(uint_t where, const Type& m)
    {
	assert(m_uiSize < m_uiCapacity && where <= m_uiSize);
        m_uiPos++;
	if (m_uiPos >= m_uiCapacity)
	    m_uiPos -= m_uiCapacity;
	m_uiSize++;
	int f = m_uiPos;
	int e = fpos() + where;
	if (e >= (int)m_uiCapacity)
            e -= m_uiCapacity;
	for (;;)
	{
	    if (--f < 0)
	    {
		f += m_uiCapacity;
		m_Type[0] = m_Type[f];
	    }
	    else
		m_Type[f + 1] = m_Type[f];
	    if (e == f)
                break;
	}
        m_Type[e] = m;
    }
    uint_t size() const { return m_uiSize; }
    void pop() { m_uiSize--; }
    void pop_front() { pop(); } // alias for vector compatibility
    void push(const Type& m)
    {
	assert(m_uiSize < m_uiCapacity);
	m_Type[m_uiPos++] = m;
	m_uiSize++;
	if (m_uiPos >= m_uiCapacity)
	    m_uiPos -= m_uiCapacity;
    }
    void push_back(const Type& m) { push(m); } // alias for vector compatibility
    uint_t capacity() const { return m_uiCapacity; }
    Type& operator[](int i) { i += fpos(); return m_Type[i >= (int)m_uiCapacity ? i - m_uiCapacity : i]; }
    const Type& operator[](int i) const { return m_Type[i]; }
protected:
    Type* m_Type;
    uint_t m_uiCapacity;
    uint_t m_uiPos;
    uint_t m_uiSize;

    // disabled for now
    qring<Type>() {}

    uint_t bpos() const { return (m_uiPos > 0) ? m_uiPos - 1 : m_uiCapacity - 1; }
    uint_t fpos() const { return (m_uiSize > m_uiPos) ? m_uiPos + m_uiCapacity - m_uiSize: m_uiPos - m_uiSize; }

};

/**
 * Simple vector class
 *
 * Implemented methods behaves like std::vector
 */
template <class Type> class vector
{
public:
    static const uint_t invalid=(uint_t)(~0);
    typedef Type* iterator;
    typedef const Type* const_iterator;
    vector<Type>()
	:m_Type(0), m_uiCapacity(0), m_uiSize(0)
    {
    }

    vector<Type>(int prealloc)
	:m_Type(0), m_uiCapacity(prealloc), m_uiSize(prealloc)
    {
	if (m_uiCapacity > 0 )
	    m_Type = new Type[m_uiCapacity];
	//printf("vector %p   (%d)\n", this, m_uiSize);
    }

    // we will not count references - we have to program with this in mind!
    vector<Type>(const vector<Type>& t) :m_Type(0)
    {
	operator=(t);
    }
    vector<Type>& operator=(const vector<Type>& t)
    {
	//printf("operator=    %p   %d  %d\n", t.m_Type, t.m_uiSize, t.m_uiCapacity);
	if (this != &t)
	    copy(t.m_Type, t.m_uiSize, t.m_uiCapacity);
	return *this;
    }
    ~vector() { delete[] m_Type; }
    Type& operator[](int i) { return m_Type[i]; }
    Type* begin() { return m_Type; }
    Type& front() { return *begin(); }
    Type* end() { return m_Type + m_uiSize; }
    Type& back() { return *(end() - 1); }
    uint_t capacity() const { return m_uiCapacity; }
    void clear()
    {
	if (m_uiCapacity > 4)
	{
	    delete[] m_Type;
	    m_uiCapacity = 4;
	    m_Type = new Type[m_uiCapacity];
	}
	m_uiSize = 0;
    }
    void erase(iterator pos)
    {
	assert(m_uiSize > 0);
	if (m_uiSize > 0)
	{
	    while (pos < end() - 1)
	    {
		pos[0] = pos[1];
		pos++;
	    }
	    pop_back();
	}
    }
    uint_t find(const Type& t) const
    {
	for (uint_t i = 0; i < m_uiSize; i++)
	    if (m_Type[i] == t)
		return i;
	return invalid;
    }
    void pop_back()
    {
	//printf("vector pop_back %d\n", m_uiSize);
        assert(m_uiSize > 0);
	m_uiSize--;
	if ((m_uiCapacity >= 8) && (m_uiSize < m_uiCapacity / 4))
	    copy(m_Type, m_uiSize, m_uiCapacity / 2);
    }
    void pop_front()
    {
        assert(m_uiSize > 0);
	for (uint_t i = 1; i < m_uiSize; i++)
	    m_Type[i - 1] = m_Type[i];
	pop_back();
    }
    void push_back(const Type& m)
    {
	if (m_uiSize + 1 >= m_uiCapacity)
	    copy(m_Type, m_uiSize, m_uiCapacity * 2);
	m_Type[m_uiSize++] = m;
    }
    void remove(const Type& t);
    void reserve(uint_t sz)
    {
	if (sz > m_uiCapacity)
	{
	    m_uiCapacity = sz;
	    copy(m_Type, m_uiSize, m_uiCapacity);
	}
    }
    void resize(uint_t sz)
    {
        m_uiCapacity = sz;
	if (m_uiSize > sz)
	    m_uiSize = sz;
	copy(m_Type, m_uiSize, m_uiCapacity);
	m_uiSize = sz;
    }
    uint_t size() const { return m_uiSize; }
    operator const Type*() const { return m_Type; }
    operator Type*() { return m_Type; }

protected:
    Type* m_Type;
    uint_t m_uiCapacity;
    uint_t m_uiSize;
    void copy(const Type* in, uint_t size, uint_t alloc);
};

template <class Type>
void vector<Type>::remove(const Type& t)
{
    int d = 0;
    iterator from = begin();
    for (iterator it = from; it != end(); it++)
    {
	if (t == *it)
	{
	    d++;
	    continue;
	}
	if (from != it)
	    *from++ = *it;
    }
    while (--d >= 0)
	pop_back();
    //printf("REMOVE VECT %d\n", m_uiSize);
}

template <class Type>
void vector<Type>::copy(const Type* in, uint_t sz, uint_t alloc)
{
    Type* tmp = m_Type;
    m_uiCapacity = (alloc < 4) ? 4 : alloc;
    //printf("COPY VECT %d  %d\n", sz, alloc);
    m_Type = new Type[m_uiCapacity];
    m_uiSize = sz;
    assert(sz <= m_uiCapacity);
    for (uint_t i = 0; i < sz; i++)
	m_Type[i] = in[i];
    delete[] tmp;
}

AVM_END_NAMESPACE;

#endif
