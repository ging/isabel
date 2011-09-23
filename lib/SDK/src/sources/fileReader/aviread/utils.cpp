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

#include <icf2/general.h>

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <sys/time.h>
#include <unistd.h>
#elif defined(__BUILD_FOR_WINXP)
#endif
#include <time.h>
#include <stdlib.h> // getenv, malloc,...
#include <string.h>  // strcat,...

#include "utils.h"

// mutex for locking parallelism in large memory transfers

uint_t (*localcount)(void);
int64_t (*longcount)(void);

WAVEFORMATEX* avm_get_leWAVEFORMATEX(WAVEFORMATEX* wf) 
{
    wf->wFormatTag	= avm_get_le16(&wf->wFormatTag);
    wf->nChannels	= avm_get_le16(&wf->nChannels);
    wf->nSamplesPerSec	= avm_get_le32(&wf->nSamplesPerSec);
    wf->nAvgBytesPerSec	= avm_get_le32(&wf->nAvgBytesPerSec);
    wf->nBlockAlign	= avm_get_le16(&wf->nBlockAlign);
    wf->wBitsPerSample	= avm_get_le16(&wf->wBitsPerSample);
    wf->cbSize	      	= avm_get_le16(&wf->cbSize);
    return wf;
}

BITMAPINFOHEADER* avm_get_leBITMAPINFOHEADER(BITMAPINFOHEADER* bi) 
{
    bi->biSize		= avm_get_le32(&bi->biSize);
    bi->biWidth		= avm_get_le32(&bi->biWidth);
    bi->biHeight	= avm_get_le32(&bi->biHeight);
    bi->biPlanes	= avm_get_le16(&bi->biPlanes);
    bi->biBitCount	= avm_get_le16(&bi->biBitCount);
    bi->biCompression	= avm_get_le32(&bi->biCompression);
    bi->biSizeImage	= avm_get_le32(&bi->biCompression);
    bi->biXPelsPerMeter	= avm_get_le32(&bi->biXPelsPerMeter);
    bi->biYPelsPerMeter	= avm_get_le32(&bi->biYPelsPerMeter);
    bi->biClrUsed	= avm_get_le32(&bi->biClrUsed);
    bi->biClrImportant	= avm_get_le32(&bi->biClrImportant);
    return bi;
}


/*
 * A thread-safe usec sleep
 * (Note: on solaris, usleep is not thread-safe)
 */
#ifndef WIN32
int avm_usleep(unsigned long delay) 
{
#if   HAVE_NANOSLEEP
    struct timespec tsp;
    tsp.tv_sec  =  delay / 1000000;
    tsp.tv_nsec = (delay % 1000000) * 1000;
    return nanosleep(&tsp, NULL);
#else
    return usleep(delay);
#endif
}
#endif


/*
 * Solaris (maybe other operating systems, too) does not have avm_setenv(),
 * and avm_unsetenv() in libc, provide our own implementation.
 */
int avm_setenv(const char *name, const char *value, int overwrite) 
{
//#if HAVE_SETENV
//    return setenv(name, value, overwrite);
//#else
//    char *env;
//
//    if (getenv(name) != NULL && !overwrite)
//	return 0;
//
//    env = malloc(strlen(name) + strlen(value) + 2);
//    strcpy(env, name);
//    strcat(env, "=");
//    strcat(env, value);
//    return putenv(env);
//#endif
	return 0;
}

void avm_unsetenv(const char *name) 
{
//#if HAVE_UNSETENV
//    unsetenv(name);
//#else
//    extern char **environ;
//    char **ep;
//    int len = strlen(name);
//    for (ep = environ; *ep; ep++)
//    {
//	if (strncmp(*ep, name, len) == 0 && (*ep)[len] == '=')
//	{
//	    while ((ep[0] = ep[1]) != NULL)
//		ep++;
//	    break;
//	}
//    }
//#endif
}
