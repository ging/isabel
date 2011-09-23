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
// $Id: notify.cc 20756 2010-07-05 09:57:09Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <icf2/general.h>
#include <icf2/notify.hh>
#include <icf2/icfTime.hh>


//
// Log messages
//
static FILE *notifyFile= stdout;

void
setNotifyFile(const char *fnam)
{
    char b[1024];

    if ((notifyFile != NULL) && (notifyFile != stdout))
    {
        fclose(notifyFile);
        notifyFile= stdout;
    }

    if (fnam)
    {
        notifyFile= fopen(fnam, "a");
        if (notifyFile == NULL)
        {
            notifyFile = stdout;
            sprintf(b, "NOTIFY3: setNotifyFile: fopen \"%s\"", fnam);
            perror(b);
        }
    }
    else
    {
        notifyFile = stdout;
    }
}

int
NOTIFY(const char *fmt, ...)
{
	if (!notifyFile)
	{
		return -1;
	}
    va_list ap;
    va_start(ap, fmt);

    time_t now;
    struct tm *lt;

    now = time(NULL);

    if (now != -1)
    {
        lt= localtime(&now);
        fprintf(notifyFile,
                "[%d/%02d/%02d %02d:%02d:%02d] ",
                lt->tm_year+1900,
                lt->tm_mon+1,
                lt->tm_mday,
                lt->tm_hour,
                lt->tm_min,
                lt->tm_sec
               );
    }
    int retVal= vfprintf(notifyFile, fmt, ap);
    fflush(notifyFile);

    va_end(ap);

    return retVal;
}

int
NOTIFY_ND(const char *fmt, ...)
{
	if (!notifyFile)
	{
		return -1;
	}
    va_list ap;
    va_start(ap, fmt);

    int retVal= vfprintf(notifyFile, fmt, ap);
    fflush(notifyFile);

    va_end(ap);

    return retVal;
}


