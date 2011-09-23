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
// $Id: strOps.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <icf2/general.h>
#include <icf2/strOps.hh>


// ----------------------------------------------------
// countFormatedArgs:
//      count arguments following the next pattern:
//      (arg1, arg2, arg3)
// ----------------------------------------------------

static int
countFormattedArgs(const char *s)
{
	const char	*p;
	const char    	*p2;
	int   	 	count;

	for(p= s; *p; p++)
	if (*p== '(')
		break;

	if(!*p)
		return -1;

	for(p2= p; *p2; p2++)
		if (*p2== ')')
			break;

	if(!*p2)
		return -1;

	p++;

	if (*p== ')')
		return 1;

	for(count= 1; *p; p++) {
		if ((*p== ',') || (*p== ')'))
			count++;
		if (*p== ')')
			break;
	}

	return count;
}

// ---------------------------------------------
// countArgs:
//      count arguments following the next pattern:
//      arg1 arg2 arg3
// ---------------------------------------------
static int
countArgs(const char *s)
{
        int retVal= 0;

        while(*s) {
            while(*s && isspace(*s)) s++;
            if(*s) {
                retVal++;
                while(*s && !isspace(*s)) s++;
            }
        }

        return retVal;
}


static char *
getFirstParamFormattedStr(const char *s)
{
	int	i;
	char	*result;

	for(i= 0; ; i++)
		if(s[i]== '(')
			break;
	

	result= (char *)malloc(i+1);
	memset(result, 0, i+1);
	strncpy(result, s, i);

	return result;
}


static char *
getParamFormattedStr(const char *s)
{
	int	i;
	char	*result;

	for(i= 0; ; i++)
		if((s[i]== ',') || (s[i]== ')'))
			break;
	

	result= (char *)malloc(i+1);
	memset(result, 0, i+1);
	strncpy(result, s, i);

	return result;
}


static char *
getParamStr(const char *s)
{
        int     i;
        char    *result;
        int     len;

	len = strlen(s);

	for(i= 0;i < len ; i++)
		// mientras sea espacio
		if(!isspace(s[i]))
			break;

	for(;i<len ; i++)
		// mientras no sea espacio
		if(isspace(s[i]))
			break;


	result= new char [i+1];
	memset(result, 0, i+1);
	strncpy(result, s, i);

	return result;
}


char **
buildFormattedArgv(const char *s, int &argc, int trim)
{
	int	i;
//	int	len;
	const char	*p;
	char	**data;

//	len= strlen(s);

	argc= countFormattedArgs(s);
	if(argc< 0)
		return NULL;
	

	data= (char**)malloc(argc * sizeof(char *));

	for(i= 0, p= s; i< argc; i++) {

		if(i==0)
			data[i]= getFirstParamFormattedStr(p);
		else
			data[i]= getParamFormattedStr(p);

		p+= strlen(data[i]) + 1;

        if(trim) {
			int j= 0;
			while (isspace(data[i][j]))
                j++;
			char * tmp = (char*)malloc(strlen(data[i]+j)+1);
            strcpy(tmp, data[i]+j);
			strcpy(data[i], tmp);
			free(tmp);

			while(isspace(data[i][strlen(data[i])-1]))
                data[i][strlen(data[i])-1]= 0;
        }
	}

	return data;
}


template<class T> T& MIN(T a, T b) { return (a<b)?a:b;};

typedef char *pString;


char **
buildArgv(const char *s, int &argc, int trim, int maxSlices)
{
        int     i;
        const char    *p;
        char    **data;

        argc= countArgs(s);
        if(argc<= 0)
                return NULL;

	argc= MIN(argc, maxSlices);

	data= new pString[1+argc];

	for(i= 0, p= s; i< argc; i++) {
		if(i!= (argc-1)) {
			data[i]= getParamStr(p);
	        } else {
			//
			// en el ultimo, nos quedamos con todo lo que haya
			//
			data[i]= new char[strlen(p)+1];
			strcpy(data[i], p);
		}

		p+= strlen(data[i]) + 1;

		if(trim) {
			// Quita los espacios iniciales de los argumentos
			int j= 0;
			while (isspace(data[i][j]))
		                j++;
			strcpy(data[i], data[i]+j);
		}
	}

	data[argc]= NULL;

	return data;
}


void
freeArgv(int argc, char **argv)
{
	int	i;

	if(!argv)
		return;


	for(i= 0; i< argc; i++)
		free(argv[i]);

	free(argv);
}


u32
isInetAddr(char *s)
{
	int	dots= 0;

	for(; *s; s++)
		if(*s== '.') dots++;
		else if (!isdigit(*s)) return 0;

	return dots== 3;
}


u32
str2addr(char *s)
{
	u32	acum= 0;
	u8	c= 0;

	for(; *s; s++)
		if(*s== '.') {
			acum= (acum<< 8) | c;
			c= 0;
		} else
			c= c*10 + (*s - '0');

	acum= (acum<< 8) | c;

	return acum;
}


// ----------------------------------------------------
// concatStr_t::
//   string class to alow concat strings using the same
//   format as xprintf functions.
// ----------------------------------------------------

concatStr_t::concatStr_t(unsigned initSize)
{
    __len  = 0;
    __size = initSize;
    __str  = (char *)malloc(__size);
    memset(__str,0,__size);
}


concatStr_t::concatStr_t(const concatStr_t &other)
{
    __size   = other.getSize();
    __str = (char *)malloc(__size);
    strcpy(__str, other.getString());	
}


concatStr_t::~concatStr_t(void)
{
    free(__str);
}


concatStr_t &
concatStr_t::operator =(const concatStr_t &other)
{
    free(__str);
    __size   = other.getSize();
    __str = (char *)malloc(__size);
    strcpy(__str, other.getString());

    return *this;
}


void 
concatStr_t::concat(const char *fmt, ...)
{
    // El formato no debe ocupar mas de 4096
    char    *scratch= (char *)malloc(4096);
    va_list  va;
	 	
    va_start(va, fmt);
	
    vsprintf(scratch, fmt, va);

    __len+= strlen(scratch);
	
    if(__len>= __size) {
        __str  = (char *)realloc(__str, __size + 4096);
        __size+= 4096;
    }
	
    strcat(__str, scratch);
    
    va_end(va);
    free(scratch);
}


