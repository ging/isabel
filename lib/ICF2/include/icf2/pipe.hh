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
// $Id: pipe.hh 20755 2010-07-05 09:56:17Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __icf2__pipe_hh__
#define __icf2__pipe_hh__

#ifdef WIN32

#include <icf2/general.h>

#define BUFSIZE 65535

class _Dll_ pipeServer_t
{
private:
   BOOL   fConnected;
   HANDLE hPipe, hThread;
   char   Pipename[100];
   DWORD  dwThreadId;
   bool   endSignal;
   HANDLE noThreads;
   int    threadCount;

   static DWORD WINAPI ServerThread(void *);
   static DWORD WINAPI readerThread(void *);
   void server(void);
   int reader(HANDLE);
   int serverReader(void);

public:
   void run(void);
   pipeServer_t(char *);
   void shutDown(void);
   virtual ~pipeServer_t(void);
   virtual void readMethod(char *)=0;
};

class _Dll_ pipeClient_t
{

private:

   HANDLE hPipe;
   BOOL fSuccess;
   char Pipename[100];
   DWORD dwMode;

private:

   void connect(void);

public:

   pipeClient_t(char *);
   virtual ~pipeClient_t(void);
   int write(const char *);

};

#endif

#endif
