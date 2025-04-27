/*
 * test_1.c  —  Exercises every feature of the compile‑time tklogger.
 *
 * Build example (SDL 3 installed and tklog.c / tklog.h in same dir):
 *   cc -std=c11 -Wall -Wextra -I/path/to/SDL3/include \
 *      test_1.c tklog.c `sdl3-config --libs` -o test_1
 */

/* -------------------------------------------------------------------------
 *  Compile‑time configuration flags
 * ------------------------------------------------------------------------- */
#define TKLOG_STATIC_FLAGS  (TKLOG_SHOW_LOG_LEVEL | TKLOG_SHOW_TIME | TKLOG_SHOW_THREAD | TKLOG_SHOW_PATH)
#define TKLOG_MEMORY
#define TKLOG_SCOPE

/* enable every level we want to test (no exit‑on‑level flags) */
#define TKLOG_DEBUG
#define TKLOG_INFO
#define TKLOG_NOTICE
#define TKLOG_WARNING
#define TKLOG_ERROR
#define TKLOG_CRITICAL
#define TKLOG_ALERT
#define TKLOG_EMERGENCY

#define TKLOG_IMPLEMENTATION

#include "tklog.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void foo() {
    tklog_scope(tklog_debug("helloooo\n"));
}

/* -------------------------------------------------------------------------
 *  Worker thread — demonstrates scope tracing + memory tracking
 * ------------------------------------------------------------------------- */
static int worker(void *userdata)
{
    intptr_t id = (intptr_t)userdata;

    tklog_scope(
        
        tklog_scope(foo());

        char *buf = malloc(128);
        memset(buf, 0, 128);

        tklog_debug("worker %zd: allocated 128 bytes", id);

        buf = realloc(buf, 256);
        tklog_info ("worker %zd: grew buffer to 256 bytes", id);

        free(buf);
        tklog_notice("worker %zd: buffer freed", id);

        /* simulate some activity */
        SDL_Delay(10 + (id * 5));
    );

    tklog_warning("worker %zd finished", id);
    return 0;
}

/* ------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return 1;
    }


    tklog_scope(
        tklog_info("main: creating worker threads");

        /* quick heap play in main thread */
        char *data = malloc(64);
        data = realloc(data, 100);
        free(data);

        /* spawn 4 workers */
        #define N 4
        SDL_Thread *thr[N] = {0};
        for (int i = 0; i < N; ++i)
            thr[i] = SDL_CreateThread(worker, "worker", (void*)(intptr_t)i);

        for (int i = 0; i < N; ++i)
            SDL_WaitThread(thr[i], NULL);

        tklog_critical("all workers joined — continuing test");

        /* dump live allocations (should be empty) */
        tklog_memory_dump();
    );

    tklog_alert("main: finished entire test run");

    SDL_Quit();
    return 0;
}
