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
////////////////////////////////////////////////////////////////////////
//
// $Id: measures.cc 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include <icf2/icfTime.hh>
#include <icf2/notify.hh>

#include "measures.hh"

measures_t * measures = new measures_t();

measures_t::measures_t(void)
: fp(NULL),
  filename(NULL)
{
}

measures_t::~measures_t(void)
{
    if (filename)
    {
        free(filename);
    }
    if (fp)
    {
        fclose(fp);
    }
}

void
measures_t::__savePkt(const char *msg, u32 SSRC, u8 PT, u32 len, u32 TS, u16 SQ)
{
    if (fp)
    {
        char line[1024];
        struct timeval sysTimeval;
        gettimeofday(&sysTimeval, NULL); // get time from system
        u32 sysTime = sysTimeval.tv_sec*1000 + sysTimeval.tv_usec/1000;
        sprintf(line,
                "%s::%u::%u::%u::%u::%u::%u\n",
                msg,
                (unsigned int)sysTime,
                (unsigned int)SSRC,
                (unsigned int)PT,
                (unsigned int)len,
                (unsigned int)TS,
                (unsigned int)SQ
               );
        fwrite(line,1,strlen(line),fp);
    }
}

void
measures_t::startMeasures(const char *newFilename)
{
    if (filename)
    {
        free(filename);
    }

    filename = strdup(newFilename);

    if (fp != NULL)
    {   // was saving, maybe in other file
        fclose(fp);
        fp= NULL;
    }

    fp = fopen(filename,"a");
    if (fp == NULL)
    {
        NOTIFY("measures_t::SaveMeasures: could not open file \"%s\"\n",
               filename
              );
    }
}

void
measures_t::stopMeasures(void)
{
    if (fp != NULL)
    {
        fclose(fp);
        fp= NULL;
    }
}

