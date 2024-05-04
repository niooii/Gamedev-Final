#pragma once

#include "def.h"

// COLORS
#define ANSI_RESET_ALL          "\x1b[0m"

#define ANSI_COLOR_BLACK        "\x1b[30m"
#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_YELLOW       "\x1b[33m"
#define ANSI_COLOR_BLUE         "\x1b[34m"
#define ANSI_COLOR_MAGENTA      "\x1b[35m"
#define ANSI_COLOR_CYAN         "\x1b[36m"
#define ANSI_COLOR_WHITE        "\x1b[37m"

#define ANSI_BACKGROUND_BLACK   "\x1b[40m"
#define ANSI_BACKGROUND_RED     "\x1b[41m"
#define ANSI_BACKGROUND_GREEN   "\x1b[42m"
#define ANSI_BACKGROUND_YELLOW  "\x1b[43m"
#define ANSI_BACKGROUND_BLUE    "\x1b[44m"
#define ANSI_BACKGROUND_MAGENTA "\x1b[45m"
#define ANSI_BACKGROUND_CYAN    "\x1b[46m"
#define ANSI_BACKGROUND_WHITE   "\x1b[47m"

#define ANSI_STYLE_BOLD         "\x1b[1m"
#define ANSI_STYLE_ITALIC       "\x1b[3m"
#define ANSI_STYLE_UNDERLINE    "\x1b[4m"

#define STYLE_COMBO(first, second) first second

typedef struct {
    const char* str;
    const char* color_code;
} ll_string_and_color;

extern const ll_string_and_color level_strings[6];

// LOGGING
#define LOG_WARN
#define LOG_INFO
#define LOG_DEBUG
#define LOG_TRACE

#ifdef GDF_RELEASE
    #undef LOG_DEBUG
    #undef LOG_TRACE
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level;

GDFAPI bool init_logging();
void shutdown_logging();

// #define GDFAPI __declspec(dllexport)

GDFAPI void log_stdout(log_level level, const char* message, ...);

// should be available wherever lol unlucky
#define GDF_FATAL(message, ...) log_stdout(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef GDF_ERR
    #define GDF_ERR(message, ...) log_stdout(LOG_LEVEL_ERR, message, ##__VA_ARGS__);
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