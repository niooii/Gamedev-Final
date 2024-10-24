#include "logging.h"
#include "os/io.h"
// TODO: temporary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include "core/collections/hashmap.h"
#include "core/os/thread.h"
#include "core/os/sysinfo.h"

#define MAX_MSG_LEN 16384
#define ENTRIES_BUFFER_CAPACITY 1024

typedef struct LogEntry {
    GDF_DateTime time;
    char* message;
    char* thread_name;
    log_level level;
} LogEntry;

// holy globals
static char* OUT_MSG;   
static char* PREPENDED_OUT_MSG;
static char* FORMAT_BUF;
static bool INITIALIZED = false;
static LogEntry* entries = NULL;
// index based, start from 0
static u32 next_free_entry = 0;
static GDF_HashMap ti_map = NULL;
// Sync
static GDF_Mutex ti_mutex = NULL;
static GDF_Mutex entries_mutex = NULL;
static GDF_Thread flushing_thread = NULL;

const char* level_strings[6] = 
{
    "FATAL: ",
    "ERROR: ",
    "WARN:  ",
    "INFO:  ",
    "DEBUG: ",
    "TRACE: ",
};

typedef struct ThreadLoggingInfo {
    const char* thread_name;
} ThreadLoggingInfo;

void __flush_log_buffer()
{
    GDF_LockMutex(entries_mutex);
    for (u32 i = 0; i < next_free_entry; i++)
    {
        const LogEntry* const entry = entries + i;
        snprintf(
            FORMAT_BUF, 
            MAX_MSG_LEN, 
            "[%s @ %02d:%02d:%02d.%03d] %s %s\n",
            entry->thread_name,
            entry->time.hour,
            entry->time.minute,
            entry->time.second,
            entry->time.milli,
            level_strings[entry->level],
            entry->message
        );
        GDF_WriteConsole(FORMAT_BUF, entry->level);
    }
    next_free_entry = 0;
    GDF_ReleaseMutex(entries_mutex);
}

unsigned long flushing_thread_fn(void*)
{
    GDF_Stopwatch stopwatch = GDF_StopwatchCreate();
    while(1)
    {
        // TODO! create timer abstraction to run functions periodically
        if (GDF_StopwatchElasped(stopwatch) > 0.05)
        {
            GDF_StopwatchReset(stopwatch);
            // TODO! optimized IO
            __flush_log_buffer();
        }
    }
}

bool GDF_InitLogging()
{
    entries_mutex = GDF_CreateMutex();
    entries = GDF_Malloc(sizeof(LogEntry) * ENTRIES_BUFFER_CAPACITY, GDF_MEMTAG_APPLICATION);
    ti_mutex = GDF_CreateMutex();
    ti_map = GDF_HashmapCreate(u32, ThreadLoggingInfo, false);
    FORMAT_BUF = GDF_Malloc(MAX_MSG_LEN, GDF_MEMTAG_STRING);

    int i = 0;
    for (int i = 0; i < ENTRIES_BUFFER_CAPACITY; i++)
    {
        // max thread name 128 chars long.. no way right
        (entries+i)->thread_name = GDF_Malloc(128, GDF_MEMTAG_STRING);
        (entries+i)->message = GDF_Malloc(MAX_MSG_LEN, GDF_MEMTAG_STRING);
    }

    flushing_thread = GDF_CreateThread(flushing_thread_fn, NULL);

    INITIALIZED = true;

    return true;
}

bool GDF_InitThreadLogging(const char* thread_name) 
{
    /*
     * Redesign:
     * Each thread will contest a mutex lock for a cylic array of LogEntries.
     * Separpate thread will be obtaining the cyclic array lock at regular intervals,
     * and reading everything and printing it, aka flushing the entry buffer.
     */
    
    ThreadLoggingInfo info = {
        .thread_name = thread_name
    };
    
    u32 thread_id = GDF_GetCurrentThreadId();
    
    GDF_LockMutex(ti_mutex);
    GDF_HashmapInsert(ti_map, &thread_id, &info);
    GDF_ReleaseMutex(ti_mutex);
    
    // printf("init thread id: %d, name: %s\n", thread_id, thread_name);
    return true;
}

void GDF_ShutdownLogging() 
{
    INITIALIZED = false;
    GDF_Free(OUT_MSG);
    GDF_Free(PREPENDED_OUT_MSG);
    // TODO! cleanup logging/write queued entries.
}

void log_output(log_level level, const char* message, ...) 
{
    if (!INITIALIZED)
    {
        return;
    }

    // TODO! BAD BAD BAD
    if (next_free_entry >= ENTRIES_BUFFER_CAPACITY)
    {
        __flush_log_buffer();
        next_free_entry = 0;
    }

    u32 thread_id = GDF_GetCurrentThreadId();
    GDF_LockMutex(ti_mutex);
    ThreadLoggingInfo* info = (ThreadLoggingInfo*)GDF_HashmapGet(ti_map, &thread_id);
    if (info == NULL)
    {
        printf("Failed thread id: %d\n", thread_id);
        printf("Message: %s", message);
        GDF_ReleaseMutex(entries_mutex);
        GDF_ReleaseMutex(ti_mutex);
        return;
    }
    
    // Get entries mutex lock
    if (!GDF_LockMutex(entries_mutex))
    {
        LOG_FATAL("so mutex lock grabbing failed gg");
    }
    LogEntry* entry = &entries[next_free_entry++];
    entry->thread_name = info->thread_name;
    GDF_ReleaseMutex(ti_mutex);

    entry->level = level;
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(entry->message, MAX_MSG_LEN, message, arg_ptr);
    va_end(arg_ptr);

    GDF_GetSystemTime(&entry->time);

    // TODO! this could fail too
    GDF_ReleaseMutex(entries_mutex);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) 
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}