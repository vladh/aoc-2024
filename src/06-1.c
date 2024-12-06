#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

#define GRID_SIZE 200

struct Pos {
    i32 x;
    i32 y;
};

enum Dir { UP, DOWN, LEFT, RIGHT };

bool is_in_bounds(struct Pos pos, i32 grid_width, i32 grid_height) {
    return pos.x > 0 && pos.x < grid_width && pos.y > 0 && pos.y < grid_height;
}

struct Pos get_next_guard_pos(struct Pos guard_pos, enum Dir guard_dir) {
    if (guard_dir == UP) {
        return (struct Pos) { .x = guard_pos.x, .y = guard_pos.y - 1};
    } else if (guard_dir == DOWN) {
        return (struct Pos) { .x = guard_pos.x, .y = guard_pos.y + 1};
    } else if (guard_dir == LEFT) {
        return (struct Pos) { .x = guard_pos.x - 1, .y = guard_pos.y};
    } else if (guard_dir == RIGHT) {
        return (struct Pos) { .x = guard_pos.x + 1, .y = guard_pos.y};
    }
}

enum Dir get_rotated_dir(enum Dir guard_dir) {
    if (guard_dir == UP) {
        return RIGHT;
    } else if (guard_dir == DOWN) {
        return LEFT;
    } else if (guard_dir == LEFT) {
        return UP;
    } else if (guard_dir == RIGHT) {
        return DOWN;
    }
}

void print_grid(
    i32 grid_width,
    i32 grid_height,
    bool visited[GRID_SIZE][GRID_SIZE],
    bool obstacles[GRID_SIZE][GRID_SIZE],
    struct Pos guard_pos,
    enum Dir guard_dir
) {
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (obstacles[x][y]) {
                eprintf("#");
            } else if (x == guard_pos.x && y == guard_pos.y) {
                if (guard_dir == UP) {
                    eprintf("^");
                } else if (guard_dir == DOWN) {
                    eprintf("v");
                } else if (guard_dir == LEFT) {
                    eprintf("<");
                } else if (guard_dir == RIGHT) {
                    eprintf(">");
                }
            } else if (visited[x][y]) {
                eprintf("X");
            } else {
                eprintf(".");
            }
        }
        eprintf("\n");
    }
    eprintf("\n");
}

int main() {
    i32 grid_width;
    i32 grid_height;
    bool visited[GRID_SIZE][GRID_SIZE] = {0};
    bool obstacles[GRID_SIZE][GRID_SIZE] = {0};
    struct Pos guard_pos;
    enum Dir guard_dir = UP;

    int fd = open("data/06", O_RDONLY);
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
            if (*curr == '^') {
                guard_pos.x = x;
                guard_pos.y = y;
            } else if (*curr == '#') {
                obstacles[x][y] = true;
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    i32 pos_count = 0;
    struct Pos next_guard_pos;

    while (true) {
        /* print_grid( */
        /*     grid_width, */
        /*     grid_height, */
        /*     visited, */
        /*     obstacles, */
        /*     guard_pos, */
        /*     guard_dir */
        /* ); */
        if (!visited[guard_pos.x][guard_pos.y]) {
            pos_count += 1;
            visited[guard_pos.x][guard_pos.y] = true;
        }
        struct Pos next_guard_pos = get_next_guard_pos(guard_pos, guard_dir);
        if (!is_in_bounds(next_guard_pos, grid_width, grid_height)) {
            break;
        }
        if (obstacles[next_guard_pos.x][next_guard_pos.y]) {
            guard_dir = get_rotated_dir(guard_dir);
            next_guard_pos = get_next_guard_pos(guard_pos, guard_dir);
        }
        guard_pos = next_guard_pos;
    }

    printf("%d\n", pos_count);

    return 0;
}
