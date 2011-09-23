
#ifndef __MSVC_MISS__
#define __MSVC_MISS__

#if _MSC_VER

#ifndef isinf
#define isinf(x) _finite((x))
#endif

#ifndef isnan
#include <float.h>
#define isnan(x) _isnan((x))
#endif

#ifndef strcasecmp
#include <winsock2.h>
#include <windows.h>
#define strcasecmp lstrcmpi
#define strncasecmp _strnicmp
#endif

int snprintf(char *buffer, size_t count, const char *fmt, ...);

double rint( double x);

#if 0
def timeval
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};
#endif

void gettimeofday(struct timeval *time, void *nada);

long long atoll(char *str);

static inline void usleep(int usecs) { Sleep (usecs / 1000 ) ; };

#define strtoll(p, e, b) _strtoi64(p, e, b) 

float cbrtf( float xx );

#endif // _MSC_VER

#endif