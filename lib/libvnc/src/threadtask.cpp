#include "threadtask.h"

using namespace libvnc;

ThreadTask::ThreadTask(int period)
{ 
    pTask = NULL; 
    this->period = period;
}

ThreadTask::~ThreadTask()
{
    Stop();
}

void 
ThreadTask::SetPeriod(int period)
{
    this->period = period;
}

int
ThreadTask::GetPeriod(void)
{
    return period;
}

#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
void* 
ThreadTask::thProc(void* param)
#elif defined(__BUILD_FOR_WINXP)
DWORD WINAPI 
ThreadTask::thProc(void * param)
#endif
{    
    ThreadTask * this_class = (ThreadTask*)param;
    while (!this_class->end)
    {
        this_class->DoWork();
        if (this_class->period > 0)
        {
            sleep(this_class->period);
        }
    }
    return 0;
}

void 
ThreadTask::Run(void)
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
{
    end = false;
    pTask = new pthread_t;
    pthread_create(pTask,NULL,thProc,(void*)this);
}
#elif defined(__BUILD_FOR_WINXP)
{
    end = false;
    DWORD ID;
    pTask = CreateThread(NULL,NULL,thProc,(void*)this,0,&ID);    
}
#endif

void 
ThreadTask::Stop(void)
#if defined(__BUILD_FOR_LINUX) || defined(__BUILD_FOR_DARWIN)
{
    if (!pthread_equal(pthread_self(),*pTask))
    {
        end = true;
        pthread_join(*pTask, 0);
        delete pTask;
        pTask = NULL;
    }else{
        pthread_exit(NULL);
    }
}
#elif defined(__BUILD_FOR_WINXP)
{
    if (GetCurrentThread() != pTask)
    {
        end = true;
  	    WaitForSingleObject(pTask,INFINITE);
	    CloseHandle(pTask);
        pTask = NULL;
    }else{
        ExitThread(0);
    }
}
#endif

