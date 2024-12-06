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

bool is_visited_any_dir(bool visited_dir[GRID_SIZE][GRID_SIZE][4], i32 x, i32 y) {
    return visited_dir[x][y][0] ||
        visited_dir[x][y][1] ||
        visited_dir[x][y][2] ||
        visited_dir[x][y][3];
}

void print_grid(
    i32 grid_width,
    i32 grid_height,
    bool visited_dir[GRID_SIZE][GRID_SIZE][4],
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
            } else if (is_visited_any_dir(visited_dir, x, y)) {
                eprintf("X");
            } else {
                eprintf(".");
            }
        }
        eprintf("\n");
    }
    eprintf("\n");
}

bool is_guard_in_bounds(struct Pos guard_pos, i32 grid_width, i32 grid_height) {
    return guard_pos.x >= 0 &&
        guard_pos.x < grid_width &&
        guard_pos.y >= 0 &&
        guard_pos.y < grid_height;
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

struct Pos get_next_guard_pos(struct Pos guard_pos, enum Dir guard_dir) {
    if (guard_dir == UP) {
        return (struct Pos) { .x = guard_pos.x, .y = guard_pos.y - 1 };
    } else if (guard_dir == DOWN) {
        return (struct Pos) { .x = guard_pos.x, .y = guard_pos.y + 1 };
    } else if (guard_dir == LEFT) {
        return (struct Pos) { .x = guard_pos.x - 1, .y = guard_pos.y };
    } else if (guard_dir == RIGHT) {
        return (struct Pos) { .x = guard_pos.x + 1, .y = guard_pos.y };
    }
}

struct Pos get_next_walkable_guard_pos_and_dir(
    struct Pos guard_pos,
    enum Dir *guard_dir,
    bool obstacles[GRID_SIZE][GRID_SIZE]
) {
    struct Pos next_guard_pos = get_next_guard_pos(guard_pos, *guard_dir);
    while (obstacles[next_guard_pos.x][next_guard_pos.y]) {
        *guard_dir = get_rotated_dir(*guard_dir);
        next_guard_pos = get_next_guard_pos(guard_pos, *guard_dir);
    }
    return next_guard_pos;
}

bool is_loop_possible(
    i32 grid_width,
    i32 grid_height,
    bool visited_dir[GRID_SIZE][GRID_SIZE][4],
    bool obstacles[GRID_SIZE][GRID_SIZE],
    struct Pos guard_pos,
    enum Dir guard_dir
) {
    bool visited_dir_local[GRID_SIZE][GRID_SIZE][4] = {0};
    guard_dir = get_rotated_dir(guard_dir);

    while (is_guard_in_bounds(guard_pos, grid_width, grid_height)) {
        struct Pos next_guard_pos = get_next_walkable_guard_pos_and_dir(guard_pos, &guard_dir, obstacles);
        visited_dir_local[guard_pos.x][guard_pos.y][guard_dir] = true;

        if (
            visited_dir[next_guard_pos.x][next_guard_pos.y][guard_dir] ||
            visited_dir_local[next_guard_pos.x][next_guard_pos.y][guard_dir]
        ) {
            return true;
        }

        guard_pos = next_guard_pos;
    }

    return false;
}

int main() {
    i32 grid_width;
    i32 grid_height;
    bool visited_dir[GRID_SIZE][GRID_SIZE][4] = {0};
    bool obstacles[GRID_SIZE][GRID_SIZE] = {0};
    bool found_loop[GRID_SIZE][GRID_SIZE] = {0};
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
                guard_pos = (struct Pos) { .x = x, .y = y };
            } else if (*curr == '#') {
                obstacles[x][y] = true;
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    i32 possible_loop_count = 0;
    struct Pos next_guard_pos;

    while (is_guard_in_bounds(guard_pos, grid_width, grid_height)) {
        /* print_grid( */
        /*     grid_width, */
        /*     grid_height, */
        /*     visited_dir, */
        /*     obstacles, */
        /*     guard_pos, */
        /*     guard_dir */
        /* ); */

        struct Pos next_guard_pos = get_next_walkable_guard_pos_and_dir(guard_pos, &guard_dir, obstacles);
        visited_dir[guard_pos.x][guard_pos.y][guard_dir] = true;

        if (
            !found_loop[next_guard_pos.x][next_guard_pos.y] &&
            !is_visited_any_dir(visited_dir, next_guard_pos.x, next_guard_pos.y)
        ) {
            bool have_loop = is_loop_possible(
                grid_width,
                grid_height,
                visited_dir,
                obstacles,
                guard_pos,
                guard_dir
            );
            if (have_loop) {
                possible_loop_count += 1;
                found_loop[next_guard_pos.x][next_guard_pos.y] = true;
            }
        }

        guard_pos = next_guard_pos;
    }

    printf("%d\n", possible_loop_count);

    return 0;
}
