
#include "msvc_miss.h"

//#include <winsock2.h>

#include <stdio.h>
#include <stdarg.h>
int snprintf(char *buffer, size_t count, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = _vsnprintf(buffer, count-1, fmt, ap);
	if (ret < 0)
		buffer[count-1] = '\0';
	va_end(ap);
	return ret;
}

double rint( double x) 
// Copyright (C) 2001 Tor M. Aamodt, University of Toronto 
// Permisssion to use for all purposes commercial and otherwise granted. 
// THIS MATERIAL IS PROVIDED "AS IS" WITHOUT WARRANTY, OR ANY CONDITION OR 
// OTHER TERM OF ANY KIND INCLUDING, WITHOUT LIMITATION, ANY WARRANTY 
// OF MERCHANTABILITY, SATISFACTORY QUALITY, OR FITNESS FOR A PARTICULAR 
// PURPOSE. 
{ 
    if( x > 0 ) { 
        __int64 xint = (__int64) (x+0.5); 
        if( xint % 2 ) { 
            // then we might have an even number... 
            double diff = x - (double)xint; 
            if( diff == -0.5 ) 
                return (double)(xint-1); 
        } 
        return (double)(xint); 
    } else { 
        __int64 xint = (__int64) (x-0.5); 
        if( xint % 2 ) { 
            // then we might have an even number... 
            double diff = x - (double)xint; 
            if( diff == 0.5 ) 
                return (double)(xint+1); 
        } 
        return (double)(xint); 
    } 
} 

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

void gettimeofday(struct timeval *time, void *nada)
{
	struct _timeb __the_time;
	_ftime(&__the_time);
	time->tv_sec = __the_time.time;
	time->tv_usec = __the_time.millitm*1000; 
}


// ascii to long long
long long atoll(char *str)
{
	long long result = 0;
	int negative=0;
	
	while (*str == ' ' || *str == '\t')
		str++;
	if (*str == '+')
		str++;
	else if (*str == '-') {
		negative = 1;
		str++;
	}
	
	while (*str >= '0' && *str <= '9') {
		result = (result*10) - (*str++ - '0');
	}

	return negative ? result : -result;
}


static float CBRT2 = 1.25992104989487316477;
static float CBRT4 = 1.58740105196819947475;


float frexpf(float x, int *expn)
{
    return (float)frexp(x, expn);
}

float ldexpf(const float x, const int expn)
{
    return (float) ldexp (x, expn);
}

float cbrtf( float xx )
{
    int e, rem, sign;
    float x, z;

    x = xx;
    if( x == 0 )
        return( 0.0 );
    if( x > 0 )
        sign = 1;
    else
	{
        sign = -1;
        x = -x;
	}

    z = x;
    /* extract power of 2, leaving
     * mantissa between 0.5 and 1
     */
    x = frexpf( x, &e );

    /* Approximate cube root of number between .5 and 1,
     * peak relative error = 9.2e-6
     */
    x = (((-0.13466110473359520655053  * x
        + 0.54664601366395524503440 ) * x
        - 0.95438224771509446525043 ) * x
        + 1.1399983354717293273738  ) * x
        + 0.40238979564544752126924;
    
    /* exponent divided by 3 */
    if( e >= 0 )
    {
        rem = e;
        e /= 3;
        rem -= 3*e;
        if( rem == 1 )
            x *= CBRT2;
        else if( rem == 2 )
            x *= CBRT4;
    }
    /* argument less than 1 */
    else
	{
        e = -e;
        rem = e;
        e /= 3;
        rem -= 3*e;
        if( rem == 1 )
            x /= CBRT2;
        else if( rem == 2 )
            x /= CBRT4;
        e = -e;
	}

    /* multiply by power of 2 */
    x = ldexpf( x, e );

    /* Newton iteration */
    x -= ( x - (z/(x*x)) ) * 0.333333333333;

    if( sign < 0 )
        x = -x;

    return(x);
}
