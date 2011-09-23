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
#ifndef AVIFILE_AVM_EXCEPT_H
#define AVIFILE_AVM_EXCEPT_H

/********************************************************

	Exception object
	Copyright 2000 Eugene Kuznetsov  (divx@euro.ru)

*********************************************************/

/**
 *
 *   The idea is to use error codes for non-fatal problems
 * ( for example, problem in decompressing one frame is not fatal
 * for video decoder ) and exceptions for problems which leave
 * object in unusable state. GenError is an intermediate case.
 *
 */
class BaseError
{
protected:
    const char* file;
    char* module;
    char* description;
    char* severity;
    int line;
public:
    BaseError();
    /** \internal copy constructor */
    BaseError(const BaseError& f);
    BaseError(const char* severity, const char* module, const char* file,
	      int line, const char* desc);
    virtual ~BaseError();
    BaseError& operator=(const BaseError& f);
    void Print();
    void PrintAll();
    const char* GetModule() const;
    const char* GetDesc() const;
};

class FatalError: public BaseError
{
public:
    FatalError(const char* mod, const char* f, int l, const char* desc,...);
    FatalError(const FatalError& f):BaseError(f){}
};

class GenError: public BaseError
{
public:
    GenError(const char* mod, const char* f, int l, const char* desc,...);
    GenError(const GenError& f):BaseError(f){}
};

#define FATAL(X,...) FatalError(__MODULE__,__FILE__,__LINE__,X)
#define WARNING(X,...) GenError(__ERR_MODULE__,__FILE__,__LINE__,X)

/***

If you want to understand how exactly it will work:

#include <stdio.h>

class Error
{
public:
    Error()
    {
	int pos;
	__asm__ __volatile__
	("push %%esp\n\t"
	"pop %%eax\n\t"
	: "=a"(pos)
	);
	printf("Created Error object, esp=%x, this=%x\n", pos, this);
    }
    Error(const Error& e)
    {
	printf("Copy-created Error object %x from %x\n", this, &e);
    }
    ~Error()
    {
	printf("Destroyed Error object, this=%x\n", this);
    }
};

void main()
{
    try
    {
	throw Error();
    }
    catch(Error& e)
    {
	int pos;
	__asm__ __volatile__
	("push %%esp\n\t"
	"pop %%eax\n\t"
	: "=a"(pos)
	);
	printf("esp %x\n", pos);	
	printf("Caught object %x\n", &e);
    }
}

Created Error object, esp=bffff910, this=bffff964
Copy-created Error object 804a0c8 from bffff964
Destroyed Error object, this=bffff964
esp bffff940
Caught object 804a0c8
Destroyed Error object, this=804a0c8

***/

#endif // AVIFILE_AVM_EXCEPT_H
