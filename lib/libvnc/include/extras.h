#ifndef ___EXTRAS_H__
#define ___EXTRAS_H__

#ifdef __BUILD_FOR_WINXP
#include <winsock2.h>
double rint(double x);
int inet_aton(const char *cp_arg, struct in_addr *addr);
#endif

#endif