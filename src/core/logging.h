#pragma once

#include "def.h"

#define STYLE_COMBO(first, second) first second

extern const char* level_strings[6];

// LOGGING
#define GDF_WARN
#define GDF_INFO
#define GDF_TRACE
#define GDF_DEBUG

#ifdef GDF_RELEASE
    #undef GDF_DEBUG
    #undef GDF_TRACE
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level;

bool GDF_InitLogging();
bool GDF_InitThreadLogging(const char* thread_name);
void GDF_ShutdownLogging();

// #define  __declspec(dllexport)


void logging_flush_buffer();
void log_output(log_level level, const char* message, ...);

// should be available wherever lol unlucky
#define LOG_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef GDF_ERR
    #define LOG_ERR(message, ...) log_output(LOG_LEVEL_ERR, message, ##__VA_ARGS__);
#endif

#ifdef GDF_WARN
    #define LOG_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
    #define LOG_WARN(message, ...)
#endif

#ifdef GDF_INFO
    #define LOG_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
    #define LOG_INFO(message, ...)
#endif

#if defined(GDF_DEBUG)
    #define LOG_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
    #define LOG_DEBUG(message, ...)
#endif

#ifdef GDF_TRACE
    #define LOG_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
    #define LOG_TRACE(message, ...)
#endif