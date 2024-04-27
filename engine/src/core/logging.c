#include "logging.h"

// TODO: temporary
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static u16 MAX_MSG_LEN = 32768;
static char* OUT_MSG;
static char* PREPENDED_OUT_MSG;

bool init_logging() {
    OUT_MSG = malloc(MAX_MSG_LEN);
    PREPENDED_OUT_MSG = malloc(MAX_MSG_LEN);
    
    // TODO: create log file.
    return true;
}

void shutdown_logging() {
    free(OUT_MSG);
    free(PREPENDED_OUT_MSG);

    // TODO: cleanup logging/write queued entries.
}

void log_stdout(log_level level, const char* message, ...) {
    memset(OUT_MSG, 0, MAX_MSG_LEN);

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(OUT_MSG, MAX_MSG_LEN, message, arg_ptr);
    va_end(arg_ptr);

    sprintf(PREPENDED_OUT_MSG, "%s%s%s%s\n", level_strings[level].color_code, level_strings[level].str, OUT_MSG, ANSI_RESET_ALL);

    printf("%s", PREPENDED_OUT_MSG);
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_stdout(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}