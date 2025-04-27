#ifndef TKLOG_H
#define TKLOG_H

#include <cstdio>

// Ensure LOG_LEVEL is defined
#ifndef TKLOG_LEVEL
#define TKLOG_LEVEL 0 // Default: No logging
#endif

// Logging levels
#define TK_TRACE    1
#define TK_DEBUG    2
#define TK_INFO     3
#define TK_NOTICE   4
#define TK_WARNING  5
#define TK_ERROR    6
#define TK_CRITICAL 7
#define TK_FATAL    8

#define TK_SHOW_LEVEL   1u << 0
#define TK_SHOW_TIME    1u << 1
#define TK_SHOW_THREAD  1u << 2
#define TK_SHOW_PATH    1u << 3

// default log function
#include <stdio.h>
void _tk_log_default_function(const char* msg) {
    printf(msg);
}

// setting log function
#ifndef TK_LOG_FN
#define TK_LOG_FN _tklog_default_function
#endif

// Macro to log only if the level is active
#define TK_LOG(level, message) do { \
    if (level >= TK_LOG_LEVEL) { \
        printf("[%s] %s\n", #level, message); \
    } \
} while (0)

// Undefine LOG_LEVEL to allow reuse
#undef TKLOG_LEVEL

#endif // LOGGER_H