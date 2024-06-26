#include "logging.h"
#include "os/io.h"
// TODO: temporary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static u16 MAX_MSG_LEN = 32768;
static char* OUT_MSG;
static char* PREPENDED_OUT_MSG;
static bool INITIALIZED = false;

const char* level_strings[6] = 
{
    "[FATAL]: ",
    "[ERROR]: ",
    "[WARN]:  ",
    "[INFO]:  ",
    "[DEBUG]: ",
    "[TRACE]: ",
};

bool GDF_InitLogging() 
{
    INITIALIZED = true;
    OUT_MSG = GDF_Malloc(MAX_MSG_LEN, GDF_MEMTAG_STRING);
    PREPENDED_OUT_MSG = GDF_Malloc(MAX_MSG_LEN, GDF_MEMTAG_STRING);

    LOG_INFO("Logging system initialized!");
    
    // TODO: create log file.
    return true;
}

void GDF_ShutdownLogging() 
{
    INITIALIZED = false;
    GDF_Free(OUT_MSG);
    GDF_Free(PREPENDED_OUT_MSG);
    // TODO: cleanup logging/write queued entries.
}

void log_output(log_level level, const char* message, ...) 
{
    if (!INITIALIZED)
    {
        return;
    }
    memset(OUT_MSG, 0, MAX_MSG_LEN);

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(OUT_MSG, MAX_MSG_LEN, message, arg_ptr);
    va_end(arg_ptr);

    sprintf(PREPENDED_OUT_MSG, "%s%s\n", level_strings[level], OUT_MSG);

    GDF_WriteConsole(PREPENDED_OUT_MSG, level);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) 
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}