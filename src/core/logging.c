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
#include "core/collections/carr.h"
#include "core/os/thread.h"
#include "core/os/sysinfo.h"

#define BUF_SIZE 32768
#define CYCLIC_BUFFER_CAPACITY 8192
static char* OUT_MSG;   
static char* PREPENDED_OUT_MSG;
static bool INITIALIZED = false;
static GDF_HashMap thread_info_map = NULL;
static GDF_CArray entries = NULL;
static GDF_Mutex entries_mutex = NULL;

const char* level_strings[6] = 
{
    "FATAL: ",
    "ERROR: ",
    "WARN:  ",
    "INFO:  ",
    "DEBUG: ",
    "TRACE: ",
};

typedef struct LogEntry {
    GDF_DateTime time;
    char* message;
    char* thread_name;
    log_level level;
} LogEntry;

typedef struct ThreadLoggingInfo {
    const char* thread_name;
} ThreadLoggingInfo;

bool GDF_InitLogging()
{
    entries_mutex = GDF_CreateMutex();
    entries = GDF_CArrayCreate(LogEntry, CYCLIC_BUFFER_CAPACITY);
    thread_info_map = GDF_HashmapCreate(u32, ThreadLoggingInfo, false);
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
    OUT_MSG = GDF_Malloc(BUF_SIZE, GDF_MEMTAG_STRING);
    PREPENDED_OUT_MSG = GDF_Malloc(BUF_SIZE, GDF_MEMTAG_STRING);
    
    ThreadLoggingInfo info = {
        .thread_name = thread_name
    };
    
    u32 thread_id = GDF_GetCurrentThreadId();
    GDF_HashmapInsert(thread_info_map, &thread_id, &info);

    // TODO! create log file.
    INITIALIZED = true;
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
    memset(OUT_MSG, 0, BUF_SIZE);

    // Get mutex lock
    if (!GDF_LockMutex(entries_mutex))
    {
        LOG_FATAL("so mutex lock grabbing failed gg");
    }
    LogEntry* entry = GDF_CArrayWriteNext(entries);
    entry->level = level;
    entry->message = message;
    GDF_GetSystemTime(&entry->time);
    // TODO! this could fail too
    GDF_ReleaseMutex(entries_mutex);

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(OUT_MSG, BUF_SIZE, message, arg_ptr);
    va_end(arg_ptr);
    u32 thread_id = GDF_GetCurrentThreadId();
    ThreadLoggingInfo* info = GDF_HashmapGet(thread_info_map, &thread_id);
    GDF_DateTime datetime;
    GDF_GetSystemTime(&datetime);
    char timebuf[80];
    sprintf(timebuf, "%02u:%02u:%02u.%03u", datetime.hour, datetime.minute, datetime.second, datetime.milli);
    sprintf(PREPENDED_OUT_MSG, "[THREAD \"%s\" | %s] %s %s\n", info->thread_name, timebuf, level_strings[level], OUT_MSG);

    GDF_WriteConsole(PREPENDED_OUT_MSG, level);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) 
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}