// simple_tklog.h - A simplified version of tklog.h
// This version avoids complex macro constructs and encoding issues

#include <SDL3/SDL.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// Define log levels if not already defined
#ifndef TK_TRACE
#   define TK_TRACE    1
#   define TK_DEBUG    2
#   define TK_INFO     3
#   define TK_NOTICE   4
#   define TK_WARNING  5
#   define TK_ERROR    6
#   define TK_CRITICAL 7
#   define TK_FATAL    8
#endif

// Default log level is off (0)
#ifndef TK_LOG_LEVEL
#   define TK_LOG_LEVEL 0
#endif

// Default exit level is off (0)
#ifndef TK_LOG_EXIT_LEVEL
#   define TK_LOG_EXIT_LEVEL 0
#endif

// Default log function is to stdout
#ifndef TK_LOG_FN
#   include <stdio.h>
#   ifndef _TKLOG_DEFAULT_OUTPUT_DEFINED
#       define _TKLOG_DEFAULT_OUTPUT_DEFINED
        static inline void _tklog_default_output(const char *msg) { fputs(msg, stdout); }
#   endif
#   define TK_LOG_FN _tklog_default_output
#endif

// Implementation for pathstack if needed
#ifdef TK_ENABLE_SCOPE
#   ifndef _TK_LOG_PATHSTACK_GET_DEFINED
#       define _TK_LOG_PATHSTACK_GET_DEFINED
        const char *_tk_log_pathstack_get(void) {
            return "main_scope";
        }
#   endif
#endif

// Generate a unique suffix for each inclusion
#ifndef TK_LOG_CONFIG
#   define TK_LOG_CONFIG __COUNTER__
#endif

// Create a unique name based on the counter
#define _TK_CONCAT_IMPL(a, b) a##b
#define _TK_CONCAT(a, b) _TK_CONCAT_IMPL(a, b)

// Unique function name for this configuration
#define _TK_LOG_FUNC _TK_CONCAT(_tklog_, TK_LOG_CONFIG)

// Define the function only once per unique configuration
#define _TK_LOG_SEEN_NAME _TK_CONCAT(_TKLOG_SEEN_, TK_LOG_CONFIG)

#ifndef _TK_CONCAT(_TKLOG_SEEN_, TK_LOG_CONFIG)
#define _TK_CONCAT(_TKLOG_SEEN_, TK_LOG_CONFIG) 1

// The logging function implementation
static inline void _TK_LOG_FUNC(
        unsigned level, unsigned line, const char *file, const char *fmt, ...)
{
    if (TK_LOG_LEVEL == 0 || level < TK_LOG_LEVEL)
        return;
    
    static const char *lvl[] = {
        "TRACE   ", "DEBUG   ", "INFO    ", "NOTICE  ",
        "WARNING ", "ERROR   ", "CRITICAL", "FATAL   "
    };
    
    char buf[2048];
    char *p = buf;
    size_t n = 0;
    
#ifdef TK_SHOW_LEVEL
    n = (size_t)snprintf(p, sizeof buf, "%s | ", lvl[level-1]); 
    p += n;
#endif

#ifdef TK_SHOW_TIME
    uint64_t t = SDL_GetTicks();
    n = (size_t)snprintf(p, sizeof buf - (p-buf), "%" PRIu64 " ms | ", t); 
    p += n;
#endif

#ifdef TK_SHOW_THREAD
    n = (size_t)snprintf(p, sizeof buf - (p-buf),
                         "Thread %" PRIu64 " | ",
                         (uint64_t)SDL_GetThreadID());
    p += n;
#endif

#ifdef TK_SHOW_PATH
#   ifdef TK_ENABLE_SCOPE
        const char *scope = _tk_log_pathstack_get();
        n = (size_t)snprintf(p, sizeof buf - (p-buf),
                             "%s -> %s:%u | ",
                             scope ? scope : "?", file, line);
        p += n;
#   else
        n = (size_t)snprintf(p, sizeof buf - (p-buf),
                             "%s:%u | ", file, line); 
        p += n;
#   endif
#endif

    va_list ap; 
    va_start(ap, fmt);
    vsnprintf(p, sizeof buf - (p-buf), fmt, ap);
    va_end(ap);
    
    size_t len = strlen(buf);
    if (len+1 < sizeof buf && buf[len-1] != '\n') {
        buf[len] = '\n'; 
        buf[len+1] = '\0';
    }
    
    TK_LOG_FN(buf);
    
    if (TK_LOG_EXIT_LEVEL && level >= TK_LOG_EXIT_LEVEL) {
        fflush(NULL); 
        _Exit(1);
    }
}

#endif // End of function definition guard

// Define the macro used by clients
#undef TK_LOG
#define TK_LOG(level, fmt, ...) \
    _TK_LOG_FUNC(level, __LINE__, __FILE__, fmt, ##__VA_ARGS__)