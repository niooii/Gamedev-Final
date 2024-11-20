#include "thread.h"

#ifdef OS_WINDOWS
#include <windows.h>

typedef struct GDF_Mutex_T {
    HANDLE mutex_handle;
    bool locked;
} GDF_Mutex_T;

typedef struct GDF_Thread_T {
    HANDLE thread_handle;
    u32 id;
} GDF_Thread_T;

typedef struct GDF_Semaphore_T {
    HANDLE sm_handle;
} GDF_Semaphore_T; 

GDF_Thread GDF_CreateThread(unsigned long thread_fn(void*), void* args) {
    GDF_Thread thread = GDF_Malloc(sizeof(GDF_Thread_T), GDF_MEMTAG_APPLICATION);
    u32 creation_flags = 0;
    thread->thread_handle = CreateThread(
        NULL,
        0,
        thread_fn,
        args,
        creation_flags,
        &thread->id
    );

    return thread;
}

u32 GDF_GetCurrentThreadId()
{
    return GetCurrentThreadId();
}

void GDF_JoinThread(GDF_Thread thread)
{
    WaitForSingleObject(thread->thread_handle, INFINITE);
}

void GDF_ThreadSleep(u64 ms)
{
    Sleep(ms);
}

GDF_Mutex GDF_CreateMutex()
{
    HANDLE handle = CreateMutex(NULL, FALSE, NULL);
    GDF_Mutex mutex = GDF_Malloc(sizeof(GDF_Mutex_T), GDF_MEMTAG_APPLICATION);
    mutex->locked = false;
    mutex->mutex_handle = handle;

    return mutex;
}

bool GDF_LockMutex(GDF_Mutex mutex)
{
    WaitForSingleObject(mutex->mutex_handle, INFINITE);
    return true;
}

bool GDF_ReleaseMutex(GDF_Mutex mutex)
{
    ReleaseMutex(mutex->mutex_handle);
    return true;
}

GDF_Semaphore GDF_CreateSemaphore()
{
    HANDLE handle = CreateSemaphore(NULL, 0, 1, NULL);
    GDF_Semaphore sm = GDF_Malloc(sizeof(GDF_Semaphore_T), GDF_MEMTAG_APPLICATION);
    sm->sm_handle = handle;

    return sm;
}

bool GDF_WaitSemaphore(GDF_Semaphore semaphore)
{
    return WaitForSingleObject(semaphore->sm_handle, INFINITE) == WAIT_OBJECT_0;
}

bool GDF_SignalSemaphore(GDF_Semaphore semaphore)
{
    return ReleaseSemaphore(semaphore->sm_handle, 1, NULL) != FALSE;
}

#endif