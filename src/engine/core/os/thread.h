#pragma once
#include "core.h"

typedef struct GDF_Thread_T* GDF_Thread;
typedef struct GDF_Mutex_T* GDF_Mutex;

// Run the thread_fn on a separate thread of execution. This does NOT make a copy of the arguments passed in. 
GDF_Thread GDF_CreateThread(unsigned long thread_fn(void*), void* args);
void GDF_JoinThread(GDF_Thread thread);
void GDF_ThreadSleep(u64 ms);

GDF_Mutex GDF_CreateMutex();
// TODO! Check return value for poisoned mutex, if it returns WAIT_ABANDONED
bool GDF_LockMutex(GDF_Mutex mutex);
bool GDF_ReleaseMutex(GDF_Mutex mutex);
 
