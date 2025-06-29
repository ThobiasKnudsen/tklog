#include <stdio.h>

// First configuration: Log INFO and above, show level, time, and path
#define TK_LOG_LEVEL TK_INFO
#define TK_SHOW_LEVEL
#define TK_SHOW_TIME
#define TK_SHOW_PATH
#include "tklog.h"


void foo1() {
    TK_LOG(TK_TRACE, "This is a trace message: %d", 123);
    TK_LOG(TK_DEBUG, "This is a debug message: %s", "hello");
    TK_LOG(TK_INFO, "This is an info message");
    TK_LOG(TK_WARNING, "This is a warning");
}

// Clear all configuration macros
#include "tklog_undef.h"

// Second configuration: Log FATAL only, show level, time, and thread
#define TK_LOG_LEVEL TK_FATAL
#define TK_SHOW_LEVEL
#define TK_SHOW_TIME
#define TK_SHOW_THREAD
#include "tklog.h"

void foo2() {
    TK_LOG(TK_TRACE, "This is a trace message: %d", 123);
    TK_LOG(TK_DEBUG, "This is a debug message: %s", "hello");
    TK_LOG(TK_INFO, "This is an info message");
    TK_LOG(TK_WARNING, "This is a warning");
    TK_LOG(TK_ERROR, "This is an error");
    TK_LOG(TK_CRITICAL, "This is a critical");
    TK_LOG(TK_FATAL, "This is a fatal");
}

// Clear all configuration macros
#include "tklog_undef.h"

// Third configuration: Log DEBUG and above, show level, path, and scope
#define TK_LOG_LEVEL TK_DEBUG
#define TK_SHOW_LEVEL
#define TK_SHOW_PATH
#define TK_ENABLE_SCOPE
#include "tklog.h"

void foo3() {
    TK_LOG(TK_TRACE, "This is a trace message: %d", 123);
    TK_LOG(TK_DEBUG, "This is a debug message: %s", "hello");
    TK_LOG(TK_INFO, "This is an info message");
}

int main() {
    printf("Testing foo1 (TK_LOG_LEVEL=TK_INFO, show level, time, path):\n");
    foo1();
    printf("\nTesting foo2 (TK_LOG_LEVEL=TK_FATAL, show level, time, thread):\n");
    foo2();
    printf("\nTesting foo3 (TK_LOG_LEVEL=TK_DEBUG, show level, path, scope):\n");
    foo3();
    return 0;
}