#ifndef __THREAD_TASK_H__
#define __THREAD_TASK_H__

#include "defines.h"

#if defined(__BUILD_FOR_WINXP)
#include <windows.h>
#define sleep(x) Sleep(x)
#endif
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
#include <unistd.h>
#include <pthread.h>
#endif

namespace libvnc
{

/**
 * Creates a new thread of control that executes concurrently with
 * the calling thread.
 */
class ThreadTask
{
private:
    bool end;
    int period;
    virtual void DoWork(void) = 0;

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    pthread_t* pTask;    
#elif defined(__BUILD_FOR_WINXP)
    HANDLE pTask;
#endif

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
    static void* thProc(void* param);
#elif defined(__BUILD_FOR_WINXP)
    static DWORD WINAPI thProc(void * param);
#endif

public:

    ThreadTask(int period = -1);
    virtual ~ThreadTask();

    virtual void Run(void);
    virtual void Stop(void);
    virtual void SetPeriod(int period);
    virtual int  GetPeriod(void);
};

}

#endif