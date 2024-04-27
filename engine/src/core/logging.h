#pragma once

#include "def.h"

#define LOG_WARN
#define LOG_INFO
#define LOG_DEBUG
#define LOG_TRACE

#ifdef GDF_RELEASE
#undef LOG_DEBUG_ENABLED
#undef LOG_TRACE_ENABLED
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level;

bool init_logging();
void shutdown_logging();

GDFAPI void log_stdout(log_level level, const char* message, ...);

// should be available wherever lol unlucky
#define GDF_FATAL(message, ...) log_stdout(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef GDF_ERROR
    #define GDF_ERROR(message, ...) log_stdout(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#ifdef LOG_WARN
    #define GDF_WARN(message, ...) log_stdout(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
    #define GDF_WARN(message, ...)
#endif

#ifdef LOG_INFO
    #define GDF_INFO(message, ...) log_stdout(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
    #define GDF_INFO(message, ...)
#endif

#ifdef LOG_DEBUG
    #define GDF_DEBUG(message, ...) log_stdout(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
    #define GDF_DEBUG(message, ...)
#endif

#ifdef LOG_TRACE
    #define GDF_TRACE(message, ...) log_stdout(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
    #define GDF_TRACE(message, ...)
#endif