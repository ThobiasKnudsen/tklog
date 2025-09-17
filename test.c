// test.c - Comprehensive test file for tklog logging library
// Compile with: gcc test.c tklog.c -lpthread -o test
// Run: ./test
// Note: This test enables all optional features for demonstration.
// In production, disable TKLOG_MEMORY and TKLOG_TIMER for performance.
// Assumes verstable.h is available for TKLOG_TIMER; if not, comment out timer tests.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep()
#include <pthread.h> // for threading demo

#include "tklog.h"  // Include after defining macros

// Thread function for testing thread-safety
void *thread_func(void *arg) {
    tklog_info("Hello from thread %ld", (long)pthread_self());
    int x = 100;
    tklog_debug("Thread local var: %d", x);
    return NULL;
}

int main(int argc, char **argv) {

    tklog_timer_init();  // Init timer if enabled

    // Basic logging tests
    tklog_debug("Debug message with int: %d", 42);
    tklog_info("Info: Program started with args: %d", argc);
    tklog_notice("Notice: Everything nominal");
    tklog_warning("Warning: This is a warning");
    tklog_error("Error: Simulated error");
    // tklog_critical("Critical: Would exit if TKLOG_EXIT_ON_CRITICAL defined");
    // tklog_alert("Alert: High priority");
    // tklog_emergency("Emergency: Catastrophic failure");

    // Test exit-on-log (uncomment to test, but it will exit)
    // #define TKLOG_EXIT_ON_ERROR
    // tklog_error("This will log and exit");

    // Scope tracing test
    tklog_info("Entering main scope");
    tklog_scope({
        tklog_info("Nested scope 1");
        tklog_scope({
            tklog_debug("Deep nested scope");
            tklog_notice("Exiting deep");
        });
        tklog_warning("After nested");
    });
    tklog_info("Exited main scope");

    // Memory tracking test
    // This should track allocations
    char *str1 = strdup("Tracked string 1");
    tklog_info("Allocated tracked string: %s", str1);
    free(str1);  // Should untrack

    char *str2 = strdup("Leaked string");  // Intentional leak for test
    tklog_info("Allocated leaked string: %s", str2);
    // Don't free str2; should dump on exit

    // Performance timer test
    tklog_timer_start();
    sleep(1);  // Simulate work
    tklog_timer_stop();
    tklog_info("After 1s sleep");

    tklog_timer_start();
    for (int i = 0; i < 1000; i++) {
        // Simulate loop work
    }
    tklog_timer_stop();
    tklog_info("After tight loop");

    // Thread safety test
    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_func, NULL) != 0) {
        tklog_error("Failed to create thread");
    } else {
        pthread_join(thread, NULL);
        tklog_info("Thread joined");
    }

    // Manual memory dump (if enabled)
    tklog_memory_dump();

    // Print timer aggregates
    tklog_timer_print();

    // Clear timer data
    tklog_timer_clear();

    tklog_info("Test completed successfully");
    return 0;
}