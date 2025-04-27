#ifndef TK_LOG_H
#define TK_LOG_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

// Assume SDL is included elsewhere if SDL functions are used; otherwise, replace with standard C equivalents

#if defined(__FILE_NAME__)
    #define __TKLOG_FILE_NAME__  __FILE_NAME__
#else
    #define __TKLOG_FILE_NAME__  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

// Ensure LOG_LEVEL is defined
#ifndef TK_LOG_LEVEL
    #define TK_LOG_LEVEL 0 // Default: No logging
#endif

#ifndef TK_LOG_EXIT_LEVEL
    #define TK_LOG_EXIT_LEVEL 0 // Default: No exit on logging
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

// Default log function
void _tk_log_default_function(const char* msg) {
    printf(msg);
}

// Setting log function
#ifndef TK_LOG_FN
    #define TK_LOG_FN _tk_log_default_function
#endif

void _tklog(unsigned int level, unsigned int line, const char *file, const char *fmt, ...)
{
    // Only log if TK_LOG_LEVEL is non-zero and level is at least TK_LOG_LEVEL
    if (TK_LOG_LEVEL == 0 || level < TK_LOG_LEVEL) return;

    static const char *level_str[] = {"TRACE   ", "DEBUG   ", "INFO    ", "NOTICE  ", "WARNING ", "ERROR   ", "CRITICAL", "FATAL   "};

    char msgbuf[2048];
    char *p = msgbuf;
    size_t n = 0;

    #ifdef TK_SHOW_LEVEL
        n = snprintf(p, sizeof msgbuf, "%s | ", level_str[level - 1]);
        p += n;
    #endif

    #ifdef TK_SHOW_TIME
        uint64_t t_ms = SDL_GetTicks() - g_start_ms;
        n = snprintf(p, sizeof msgbuf - (p - msgbuf), "%" PRIu64 "ms | ", t_ms);
        p += n;
    #endif

    #ifdef TK_SHOW_THREAD
        SDL_ThreadID tid = SDL_GetCurrentThreadID();
        n = snprintf(p, sizeof msgbuf - (p - msgbuf), "Thread %" PRIu64 " | ", (uint64_t)tid);
        p += n;
    #endif

    #ifdef TK_SHOW_PATH
        #ifdef TK_ENABLE_SCOPE
            const char* path = _tk_log_pathstack_get();
            if (path) {
                n = snprintf(p, sizeof msgbuf - (p - msgbuf), "%s → %s:%d | ", path, file, line);
                p += n;
            } else {
                printf("TKLOG: INTERNAL ERROR: _tk_log_pathstack_get returned NULL\n");
            }
        #else
            n = snprintf(p, sizeof msgbuf - (p - msgbuf), "%s:%d | ", file, line);
            p += n;
        #endif
    #endif

    // User message
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(p, sizeof msgbuf - (p - msgbuf), fmt, ap);
    va_end(ap);

    // Ensure newline
    size_t len = strlen(msgbuf);
    if (len + 1 < sizeof msgbuf && msgbuf[len - 1] != '\n') {
        msgbuf[len]   = '\n';
        msgbuf[len+1] = '\0';
    }

    TK_LOG_FN(msgbuf);

    // Exit if level meets or exceeds TK_LOG_EXIT_LEVEL and it's not zero
    if (TK_LOG_EXIT_LEVEL != 0 && level >= TK_LOG_EXIT_LEVEL) {
        fflush(NULL);
        _Exit(1);
    }
}

// Macro to log
#define TK_LOG(level, fmt, ...) _tklog(level, __LINE__, __TKLOG_FILE_NAME__, fmt, __VA_ARGS__)

#endif // TK_LOG_H