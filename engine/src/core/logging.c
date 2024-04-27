#include "logging.h"

// TODO: temporary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static u16 MAX_MSG_LEN = 32768;
static char* OUT_MSG;

bool init_logging() {
    OUT_MSG = malloc(MAX_MSG_LEN);

    // TODO: create log file.
    return true;
}

void shutdown_logging() {
    free(OUT_MSG);

    // TODO: cleanup logging/write queued entries.
}

void log_stdout(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};

    memset(OUT_MSG, 0, MAX_MSG_LEN);

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(OUT_MSG, MAX_MSG_LEN, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[32000];
    sprintf(out_message2, "%s%s\n", level_strings[level], OUT_MSG);

    // TODO: platform-specific output.
    printf("%s", out_message2);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_stdout(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}