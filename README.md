# tklog - Lightweight Configurable Logging Library for C

tklog is a single-header/single-source (`.h` + `.c`) logging library for C programs. It provides thread-safe, flexible logging with compile-time configuration for minimal runtime overhead. Features include customizable log prefixes (level, timestamp, thread ID, file/line), multiple log levels, optional call-stack tracing, memory leak detection, and performance timing.

Designed for embedded and performance-critical applications, tklog avoids dynamic allocation in the hot path and supports cross-platform use (Linux, macOS, Windows via MinGW).

## Features

- **Log Levels**: DEBUG, INFO, NOTICE, WARNING, ERROR, CRITICAL, ALERT, EMERGENCY.
- **Customizable Prefixes** (compile-time):
  - Log level (e.g., `[DEBUG]`).
  - Relative timestamp (ms since program start).
  - Thread ID (pthread_self()).
  - File and line number, with optional call-stack tracing (e.g., `a.c:12 → b.c:88`).
- **Output Flexibility**: Default to `stdout` via `printf`; customizable via callback function.
- **Optional Exit-on-Log**: Automatically exit on high-severity logs (e.g., ERROR).
- **Scope Tracing**: Automatic call-stack tracking with `TKLOG_SCOPE` macro.
- **Memory Tracking**: Override `malloc`/`free` etc. to detect leaks; dumps on exit or signals (SIGSEGV, SIGABRT, SIGINT).
- **Performance Timer**: Track execution time for code blocks with `TKLOG_TIMER` macro; reports totals, averages, and call paths.
- **Thread-Safe**: Uses `pthread_mutex` for serialization.
- **Cross-Platform Timing**: High-resolution timers (QueryPerformanceCounter on Windows, CLOCK_MONOTONIC on POSIX).
- **Zero Runtime Config**: All behavior decided at compile-time via macros for optimal performance.

## Requirements

- C99 or later.
- `pthread` (for threading; available on Linux/macOS/Windows via MinGW).
- Standard C library.
- For memory/timer: Additional allocations (minimal).
- External dependency for timer: `verstable.h` (a simple hash table library; not included—implement or replace).

## Installation

1. Copy `tklog.h` and `tklog.c` to your project.
2. Include `tklog.h` in source files where logging is needed.
3. Compile `tklog.c` into your project (e.g., `gcc main.c tklog.c -lpthread -o app`).

No installation or linking required beyond standard libs.

## Configuration

All configuration is compile-time via preprocessor macros. Define them in your build system (e.g., CMake `add_compile_definitions`) or before including `tklog.h`.

### Basic Flags

- `TKLOG_SHOW_LOG_LEVEL`: Include log level in output (default: enabled).
- `TKLOG_SHOW_TIME`: Include relative timestamp (default: enabled).
- `TKLOG_SHOW_THREAD`: Include thread ID (default: enabled).
- `TKLOG_SHOW_PATH`: Include file:line (with call-stack if `TKLOG_SCOPE` enabled; default: enabled).

Override defaults globally with `TKLOG_STATIC_FLAGS` (bitmask):
```
add_compile_definitions(TKLOG_STATIC_FLAGS=(TKLOG_SHOW_LOG_LEVEL|TKLOG_SHOW_TIME))
```

### Per-Level Enables

Enable/disable specific log levels:
- `TKLOG_DEBUG`, `TKLOG_INFO`, `TKLOG_NOTICE`: Log these levels.
- `TKLOG_WARNING`, `TKLOG_ERROR`, `TKLOG_CRITICAL`, `TKLOG_ALERT`, `TKLOG_EMERGENCY`: Log or exit (see below).

Undefined levels expand to `((void)0)` (no-op).

### Exit-on-Log

For high-severity levels, use `TKLOG_EXIT_ON_<LEVEL>` (e.g., `TKLOG_EXIT_ON_ERROR`) to log and `exit(-1)` instead of just logging. Overrides the plain `TKLOG_<LEVEL>`.

### Advanced Options

- `TKLOG_OUTPUT_FN`: Custom output callback (default: `tklog_output_stdio` writing to `stdout`).
- `TKLOG_OUTPUT_USERPTR`: User data for callback (default: `NULL`).
- `TKLOG_MEMORY`: Enable memory tracking (overrides stdlib allocators).
- `TKLOG_MEMORY_PRINT_ON_EXIT`: Dump leaks on `atexit` (requires `TKLOG_MEMORY`).
- `TKLOG_SCOPE`: Enable scope-based call-stack tracing.
- `TKLOG_TIMER`: Enable performance timing (requires `verstable.h` for hash tables).

Example CMake:
```cmake
target_compile_definitions(my_target PRIVATE
    TKLOG_SHOW_LOG_LEVEL
    TKLOG_SHOW_TIME
    TKLOG_MEMORY
    TKLOG_SCOPE
)
```

## Usage

### Basic Logging

Include `tklog.h` and use level-specific macros:
```c
#include "tklog.h"

int main() {
    tklog_info("Program started");
    int x = 42;
    tklog_debug("Value of x: %d", x);
    if (x < 0) {
        tklog_error("Negative value!");
    }
    return 0;
}
```

Output (with defaults):
```
INFO     | 0ms | tid 140735327123456 | main.c:8 | Program started
DEBUG    | 0ms | tid 140735327123456 | main.c:10 | Value of x: 42
```

### Custom Output

Define a callback:
```c
bool my_logger(const char *msg, void *user) {
    FILE *logfile = (FILE *)user;
    return fputs(msg, logfile) != EOF;
}

// Before including tklog.h
#define TKLOG_OUTPUT_FN my_logger
#define TKLOG_OUTPUT_USERPTR logfile
```

### Scope Tracing (TKLOG_SCOPE)

Wrap code blocks to auto-push/pop call path:
```c
tklog_scope({
    tklog_info("Entering function");
    // Nested scopes build path: e.g., "main.c:5 → func.c:10"
    tklog_scope({
        tklog_debug("Deep inside");
    });
});
```

Output:
```
INFO     | 1ms | tid 123 | main.c:5 → func.c:8 | Entering function
DEBUG    | 1ms | tid 123 | main.c:5 → func.c:8 → inner.c:12 | Deep inside
```

### Memory Tracking (TKLOG_MEMORY)

Automatically tracks allocations. Use like normal:
```c
char *str = strdup("hello");  // Tracked
free(str);                    // Untracked → error log + exit
```

On exit/signals, dumps unfreed memory:
```
unfreed memory:
	5ms | tid 123 | address 0x7f8b4000 | 6 bytes | at main.c:10
```

Dumps include timestamp, thread, address, size, and allocation path (with scopes).
Warning: Nested allocs in tklog internals use original functions to avoid recursion.

### Performance Timer (TKLOG_TIMER)

Time code blocks and report aggregates:
```c
tklog_timer_init();  // Once at startup

tklog_timer_start();
sleep(1);
tklog_timer_stop();

tklog_timer_print();  // At end
```

Output:
```
1000ms | 1 calls | 1000.000ms avg | main.c:10
1000ms | 1 calls | 1000.000ms avg |     main.c:10 to main.c:12
```

Tracks per-location totals, counts, averages, and full call paths.
Clear data: `tklog_timer_clear()`.

## Examples

See `examples/` (not included; create simple mains testing each feature).

## Limitations

- Timer requires `verstable.h` (simple string-keyed hash table).
- Memory tracking adds overhead; disable for production.
- Call-stack limited to 128-char paths; grows dynamically.
- No log rotation/file output (use custom callback).
- Windows support via MinGW; test thoroughly.

## License

MIT License (assumed; adjust as needed). See LICENSE for details.

## Contributing

Fork, PR improvements (e.g., more platforms, JSON output).