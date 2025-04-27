#ifndef TK_LOG_H
#define TK_LOG_H

#include <cstdio>
#include <stdio.h>


#if defined(__FILE_NAME__)
    #define __TKLOG_FILE_NAME__  __FILE_NAME__
#else
    #define __TKLOG_FILE_NAME__  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

// Ensure LOG_LEVEL is defined
#ifndef TK_LOG_LEVEL
#define TK_LOG_LEVEL 0 // Default: No logging
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

// what column to show
#ifdef TK_SHOW_LEVEL
    #define TK_F_SHOW_LEVEL     1u << 0
#elif 
    #define TK_F_SHOW_LEVEL     0u
#endif 
#ifdef TK_SHOW_TIME
    #define TK_F_SHOW_TIME      1u << 1
#elif 
    #define TK_F_SHOW_TIME      0u
#endif 
#ifdef TK_SHOW_THREAD
    #define TK_F_SHOW_THREAD    1u << 2
#elif 
    #define TK_F_SHOW_THREAD    0u
#endif 
#ifdef TK_SHOW_PATH
    #define TK_F_SHOW_PATH      1u << 3
#elif 
    #define TK_F_SHOW_PATH      0u
#endif 

#define TK_SHOW_FLAGS (TK_F_SHOW_LEVEL | TK_F_SHOW_TIME | TK_F_SHOW_THREAD | TK_F_SHOW_PATH)

// default log function
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