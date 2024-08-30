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

#endif