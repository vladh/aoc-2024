#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

#define GRID_SIZE 50
#define MAX_N_THREADS 5000

struct Pos {
    i32 x;
    i32 y;
};

struct Thread {
    i32 len;
    struct Pos origin;
    struct Pos pos;
};

void thread_print(struct Thread thread) {
    eprintf("Thread { origin = (%d,%d), pos = (%d,%d), len = %d }\n",
        thread.origin.x, thread.origin.y,
        thread.pos.x, thread.pos.y,
        thread.len);
}

void threads_add(struct Thread threads[MAX_N_THREADS], i32 *n_threads, struct Thread new_thread) {
    thread_print(new_thread);
    threads[*n_threads] = new_thread;
    *n_threads += 1;
}

bool is_in_bounds(struct Pos pos, i32 grid_width, i32 grid_height) {
    return pos.x >= 0 && pos.x < grid_width && pos.y >= 0 && pos.y < grid_height;
}

int main() {
    i32 grid_width = 0;
    i32 grid_height = 0;
    i8 grid[GRID_SIZE][GRID_SIZE] = {0};
    struct Thread threads[MAX_N_THREADS] = {0};
    i32 n_threads = 0;
    struct Thread done_threads[MAX_N_THREADS] = {0};
    i32 n_done_threads = 0;
    bool is_reachable[GRID_SIZE][GRID_SIZE][GRID_SIZE][GRID_SIZE] = {0};
    i32 n_reachable = 0;

    int fd = open("data/10", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;
    i32 x = 0;
    i32 y = 0;

    while (*curr) {
        if (*curr == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            grid[x][y] = *curr - '0';
            if (grid[x][y] == 0) {
                struct Pos pos = (struct Pos) {
                    .x = x,
                    .y = y,
                };
                struct Thread new_thread = (struct Thread) {
                    .len = 0,
                    .origin = pos,
                    .pos = pos,
                };
                threads_add(threads, &n_threads, new_thread);
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    struct Thread new_threads[MAX_N_THREADS] = {0};
    i32 n_new_threads = 0;

    while (n_threads > 0) {
        n_new_threads = 0;
        for (i32 idx_thread = 0; idx_thread < n_threads; idx_thread += 1) {
            struct Thread *thread = &threads[idx_thread];
            i32 n = grid[thread->pos.x][thread->pos.y];
            eprintf("(%d,%d), %d\n", thread->pos.x, thread->pos.y, n);
            for (i32 dy = -1; dy <= 1; dy += 1) {
                for (i32 dx = -1; dx <= 1; dx += 1) {
                    if (abs(dx - dy) != 1) {
                        continue;
                    }
                    struct Pos new_pos = (struct Pos) {
                        .x = thread->pos.x + dx,
                        .y = thread->pos.y + dy,
                    };
                    if (!is_in_bounds(new_pos, grid_width, grid_height)) {
                        continue;
                    }
                    i32 candidate_n = grid[new_pos.x][new_pos.y];
                    if (candidate_n == n + 1) {
                        struct Thread new_thread = *thread;
                        new_thread.pos = new_pos;
                        new_thread.len += 1;
                        if (n + 1 == 9) {
                            threads_add(done_threads, &n_done_threads, new_thread);
                            bool *have_already_reached =
                                &is_reachable[new_thread.origin.x][new_thread.origin.y][new_pos.x][new_pos.y];
                            if (!*have_already_reached) {
                                *have_already_reached = true;
                                n_reachable += 1;
                            }
                        } else {
                            threads_add(new_threads, &n_new_threads, new_thread);
                        }
                    }
                }
            }
        }
        for (i32 idx_new = 0; idx_new < n_new_threads; idx_new += 1) {
            threads[idx_new] = new_threads[idx_new];
        }
        n_threads = n_new_threads;
    }

    eprintf("\n");
    for (i32 i = 0; i < n_done_threads; i += 1) {
        thread_print(done_threads[i]);
    }
    printf("%d done threads\n", n_done_threads);
    printf("%d reachable\n", n_reachable);

    return 0;
}
