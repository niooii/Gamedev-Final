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
#define FLUSH_INTERVAL_MS 50 
#define MAX_THREAD_NAME_LEN 128
// timestamp (23) + thread_name (128) + level_str (7) + message (16384) + newline (1) = 16543
#define MAX_FORMATTED_ENTRY_LEN (MAX_MSG_LEN + MAX_THREAD_NAME_LEN + 48)

typedef struct LogEntry {
    GDF_DateTime time;
    const char* message;
    const char* thread_name;
    log_level level;
} LogEntry;

// perhaps expand... 8mb allocation right here, absolutely diabolical
static char FORMAT_BUFFER[MAX_FORMATTED_ENTRY_LEN * ENTRIES_BUFFER_CAPACITY];
static LogEntry entries[ENTRIES_BUFFER_CAPACITY];

// holy globals
static bool INITIALIZED = false;
// index based, start from 0
static u32 next_free_entry = 0;
static GDF_HashMap ti_map = NULL;
// Sync
static GDF_Mutex ti_mutex = NULL;
static GDF_Mutex entries_mutex = NULL;
static GDF_Mutex flushing_mutex = NULL;
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

typedef struct ColorCombination {
    u8 fg;
    u8 bg;
} ColorCombination;

static const ColorCombination log_level_color_combinations[] = {
    [LOG_LEVEL_FATAL] = {
        .fg = TC_WHITE,
        .bg = BG_RED
    },
    [LOG_LEVEL_ERR] = {
        .fg = TC_RED,
        .bg = TC_RED
    },
    [LOG_LEVEL_DEBUG] = {
        .fg = TC_BLUE,
        .bg = TC_BLUE
    },
    [LOG_LEVEL_INFO] = {
        .fg = TC_GREEN,
        .bg = TC_GREEN
    },
    [LOG_LEVEL_WARN] = {
        .fg = TC_YELLOW,
        .bg = TC_YELLOW
    },
    [LOG_LEVEL_TRACE] = {
        .fg = TC_WHITE,
        .bg = TC_WHITE
    },
};

void logging_flush_buffer()
{
    char* write_ptr = FORMAT_BUFFER;
    if (!GDF_LockMutex(entries_mutex) || !GDF_LockMutex(flushing_mutex))
    {
        GDF_WriteConsole("Failed to flush log buffer, mutex not acquired..\n");
        return;
    }
    for (u32 i = 0; i < next_free_entry; i++)
    {
        const LogEntry* const entry = entries + i;
        ColorCombination cc = log_level_color_combinations[entry->level];
        snprintf(
            write_ptr, 
            MAX_FORMATTED_ENTRY_LEN, 
            "\x1B[%dm\x1B[%dm[%s @ %02d:%02d:%02d.%03d] %s %s\n",
            cc.fg,
            cc.bg,
            entry->thread_name,
            entry->time.hour,
            entry->time.minute,
            entry->time.second,
            entry->time.milli,
            level_strings[entry->level],
            entry->message
        );
        write_ptr = strchr(write_ptr, 0);
    }
    GDF_ReleaseMutex(entries_mutex);
    GDF_WriteConsole(FORMAT_BUFFER);
    GDF_ReleaseMutex(flushing_mutex);
    next_free_entry = 0;
    FORMAT_BUFFER[0] = 0;
}

unsigned long flushing_thread_fn(void*)
{
    GDF_Stopwatch stopwatch = GDF_StopwatchCreate();
    while(1)
    {
        // TODO! create timer abstraction to run functions periodically
        if (GDF_StopwatchElasped(stopwatch) * 1000 > FLUSH_INTERVAL_MS)
        {
            GDF_StopwatchReset(stopwatch);
            // TODO! optimized IO
            logging_flush_buffer();
        }
    }
}

bool GDF_InitLogging()
{
    entries_mutex = GDF_CreateMutex();
    flushing_mutex = GDF_CreateMutex();
    ti_mutex = GDF_CreateMutex();
    ti_map = GDF_HashmapCreate(u32, ThreadLoggingInfo, false);

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
    // TODO! cleanup logging/write queued entries.
    GDF_WriteConsole("\033[0m");
}

void log_output(log_level level, const char* message, ...) 
{
    if (!INITIALIZED)
    {
        return;
    }

    if (!GDF_LockMutex(entries_mutex))
    {
        GDF_WriteConsole("Failed to buffer log call, mutex not acquired...\n");
        return;
    }
    // TODO! BAD BAD BAD
    if (next_free_entry >= ENTRIES_BUFFER_CAPACITY)
    {
        logging_flush_buffer();
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

    // TODO! could be optimized more, for now force flush
    if (level == LOG_LEVEL_ERR || level == LOG_LEVEL_FATAL)
        logging_flush_buffer();
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) 
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}