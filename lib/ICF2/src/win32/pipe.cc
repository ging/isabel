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
// $Id: pipe.cc 20756 2010-07-05 09:57:09Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2004. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/pipe.hh>


pipeServer_t::pipeServer_t(char * pipeName)
{
    memset(Pipename,0,100);
    strcat(Pipename,"\\\\.\\pipe\\");
    strcat(Pipename,pipeName);
    endSignal = false;
    noThreads = CreateEvent(NULL,FALSE,FALSE,NULL);
    threadCount = 0;
}

void
pipeServer_t::run(void)
{
    // Create Server Thread

    hThread = CreateThread(NULL,           // no security attribute 
                           0,              // default stack size 
                           ServerThread,   // Routine 
                           (void *)this,   // thread parameter 
                           0,              // not suspended 
                           &dwThreadId     // returns thread ID 
                          );
}

int
pipeServer_t::reader(HANDLE pipe)
{
    char string[BUFSIZE];
    DWORD cbBytesRead; 
    BOOL fSuccess; 
    HANDLE hPipe; 
 
    // The thread's parameter is a handle to a pipe instance. 
    hPipe = (HANDLE) pipe; 
       
    while (1) 
    { 
        memset(string,0,BUFSIZE);
        if (endSignal) break;
        // Read client requests from the pipe. 
        WaitForSingleObject(hPipe,INFINITE);
        fSuccess = ReadFile(hPipe,        // handle to pipe 
                            string,       // buffer to receive data 
                            BUFSIZE,      // size of buffer 
                            &cbBytesRead, // number of bytes read 
                            NULL          // not overlapped I/O 
                           );
        if (cbBytesRead > 0)
        {
            char *buff = (char*)malloc(cbBytesRead);
            memset(buff,0,cbBytesRead);
            memcpy(buff,string,cbBytesRead);
            readMethod(buff);
            free(buff);
        }
        else
        {
            //readMethod("\" \" string received\r\n");
        }
    } 
 
    FlushFileBuffers(hPipe); 
    DisconnectNamedPipe(hPipe); 
    CloseHandle(hPipe); 
    threadCount--;
    if (threadCount == 0)
    {
        SetEvent(noThreads);
    }

    return cbBytesRead;
}

int
pipeServer_t::serverReader(void)
{
    char string[BUFSIZE];
    DWORD cbBytesRead; 
    BOOL fSuccess; 
 
    while (1) 
    { 
        if(endSignal) 
        {
            threadCount--;
            if (threadCount == 0)
            {
                SetEvent(noThreads);
            }

            ExitThread(0);
        }

        fConnected = ConnectNamedPipe(hPipe, NULL) ? 
                     TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
 
        if (fConnected) 
        { 
            memset(string,0,BUFSIZE);
            if (endSignal) break;
            // Read client requests from the pipe. 
            WaitForSingleObject(hPipe,INFINITE);
            fSuccess = ReadFile(hPipe,        // handle to pipe 
                                string,       // buffer to receive data 
                                BUFSIZE,      // size of buffer 
                                &cbBytesRead, // number of bytes read 
                                NULL          // not overlapped I/O 
                               );
            if (cbBytesRead > 0)
            {
                char *buff = (char*)malloc(cbBytesRead);
                memset(buff,0,cbBytesRead);
                memcpy(buff,string,cbBytesRead);
                readMethod(buff);
                free(buff);
            }
            else
            {
                //readMethod("\" \" string received\r\n");
            }
        } 
        FlushFileBuffers(hPipe); 
        DisconnectNamedPipe(hPipe); 
    }

    return cbBytesRead;
}

void
pipeServer_t::server(void)
{
    // The main loop creates an instance of the named pipe and 
    // then waits for a client to connect to it. When the client 
    // connects, a thread is created to handle communications 
    // with that client, and the loop is repeated. 

    endSignal = false;
    for (;;) 
    { 
        if (endSignal) 
        {
            threadCount--;
            if (threadCount == 0)
            {
                SetEvent(noThreads);
            }

            ExitThread(0);
        }

        hPipe =
            CreateNamedPipe(Pipename,                 // pipe name 
                            PIPE_ACCESS_DUPLEX,       // read/write access 
                            PIPE_TYPE_MESSAGE |       // message type pipe 
                            PIPE_READMODE_MESSAGE |   // message-read mode 
                            PIPE_WAIT,                // blocking mode 
                            PIPE_UNLIMITED_INSTANCES, // max. instances  
                            (DWORD)BUFSIZE,           // output buffer size 
                            (DWORD)BUFSIZE,           // input buffer size 
                            INFINITE,                 // client time-out 
                            NULL                      // no security attribute 
                           );

        if (hPipe == INVALID_HANDLE_VALUE) 
        {
            ExitThread(0);
        }
 
        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function returns 
        // zero, GetLastError returns ERROR_PIPE_CONNECTED. 
#if 0
        fConnected = ConnectNamedPipe(hPipe, NULL) ? 
                     TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
 
        if (fConnected) 
        { 
            // Create a thread for this client. 
            hThread = CreateThread(NULL,           // no security attribute 
                                   0,              // default stack size 
                                   readerThread,   // Routine
                                   (void *) this,  // thread parameter 
                                   0,              // not suspended 
                                   &dwThreadId     // returns thread ID 
                                  );
        } 
#else
        serverReader();
#endif
    } 

    ExitThread(1); 
}


DWORD WINAPI
pipeServer_t::ServerThread(void *lpvParam) 
{
    pipeServer_t *pipe = static_cast<pipeServer_t *>(lpvParam);
    pipe->threadCount++;
    pipe->server();
    return 0;
} 

 
DWORD WINAPI
pipeServer_t::readerThread(void * lpvParam) 
{ 
    pipeServer_t *pipe = static_cast<pipeServer_t *>(lpvParam);
    pipe->threadCount++;
    pipe->reader(pipe->hPipe);
    return 0;
}

void
pipeServer_t::shutDown(void)
{
    endSignal = true;
    WaitForSingleObject(noThreads, INFINITE);
    delete this;
}


pipeServer_t::~pipeServer_t(void)
{

}
pipeClient_t::pipeClient_t(char *pipeName)
{
   memset(Pipename,0,100);
   strcat(Pipename,"\\\\.\\pipe\\");
   strcat(Pipename,pipeName);
}

void
pipeClient_t::connect(void)
{
    hPipe = NULL;
    
    hPipe = CreateFile(Pipename,          // pipe name 
                       GENERIC_WRITE, 
                       FILE_SHARE_READ | FILE_SHARE_WRITE,              
                       NULL,              // no security attributes
                       OPEN_EXISTING,     // opens existing pipe 
                       0,                 // default attributes 
                       NULL               // no template file 
                      );
   
   dwMode = PIPE_READMODE_MESSAGE; 
   fSuccess = SetNamedPipeHandleState(hPipe,    // pipe handle 
                                      &dwMode,  // new pipe mode 
                                      NULL,     // don't set maximum bytes 
                                      NULL      // don't set maximum time 
                                     );
}

int
pipeClient_t::write(const char *string)
{
    this->connect();
    if (hPipe != INVALID_HANDLE_VALUE) 
    {
        unsigned long written = 0;
        fSuccess = WriteFile(hPipe,               // pipe handle 
                             string,              // message 
                             strlen(string) + 1,  // message length 
                             &written,            // bytes written 
                             NULL                 // not overlapped 
                            );
        FlushFileBuffers(hPipe); 
        CloseHandle(hPipe);
        return written;
    }
    else
    {
        return -1;
    }
}


pipeClient_t::~pipeClient_t(void)
{
}

