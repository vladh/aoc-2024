#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

#define GRID_SIZE 100

enum Tile { EMPTY, WALL, BOX, ROBOT };

bool is_in_bounds(i32 x, i32 y, i32 grid_width, i32 grid_height) {
    return x >= 0 && x < grid_width && y >= 0 && y < grid_height;
}

void print_grid(enum Tile grid[GRID_SIZE][GRID_SIZE], i32 grid_width, i32 grid_height) {
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (grid[x][y] == WALL) {
                eprintf("#");
            } else if (grid[x][y] == BOX) {
                eprintf("O");
            } else if (grid[x][y] == ROBOT) {
                eprintf("@");
            } else {
                eprintf(".");
            }
        }
        eprintf("\n");
    }
    eprintf("\n");
}

void move(
    enum Tile grid[GRID_SIZE][GRID_SIZE],
    i32 grid_width,
    i32 grid_height,
    i32 *x,
    i32 *y,
    i32 dx,
    i32 dy
) {
    if (
        !is_in_bounds(*x, *y, grid_width, grid_height) ||
        grid[*x][*y] == WALL ||
        grid[*x][*y] == EMPTY ||
        !is_in_bounds(*x + dx, *y + dy, grid_width, grid_height) ||
        grid[*x + dx][*y + dy] == WALL
    ) {
        return;
    }
    i32 forward_x = *x + dx;
    i32 forward_y = *y + dy;
    move(grid, grid_width, grid_height,
        &forward_x, &forward_y,
        dx, dy);
    if (grid[*x + dx][*y + dy] == EMPTY) {
        grid[*x + dx][*y + dy] = grid[*x][*y];
        grid[*x][*y] = EMPTY;
        *x = *x + dx;
        *y = *y + dy;
    }
}

int main() {
    i32 grid_width = 0;
    i32 grid_height = 0;
    enum Tile grid[GRID_SIZE][GRID_SIZE] = {0};

    int f = open("data/15", O_RDONLY);
    if (f == -1) {
        eprintf("Could not open file\n");
        exit(1);
    }
    int len = lseek(f, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, f, 0);
    char *curr = data;
    i32 x = 0;
    i32 y = 0;
    i32 x_robot;
    i32 y_robot;

    bool saw_newline = false;
    while (*curr) {
        if (*curr == '\n') {
            if (saw_newline) {
                break;
            }
            y += 1;
            grid_width = x;
            x = 0;
            saw_newline = true;
        } else {
            saw_newline = false;
            if (*curr == '#') {
                grid[x][y] = WALL;
            } else if (*curr == 'O') {
                grid[x][y] = BOX;
            } else if (*curr == '@') {
                grid[x][y] = ROBOT;
                x_robot = x;
                y_robot = y;
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    i32 t = 0;
    while (*curr) {
        i32 dx = 0;
        i32 dy = 0;
        if (*curr == '<') {
            dx = -1;
        } else if (*curr == '>') {
            dx = 1;
        } else if (*curr == '^') {
            dy = -1;
        } else if (*curr == 'v') {
            dy = 1;
        }
        if (dx != 0 || dy != 0) {
            eprintf("t = %d, moving %c:\n", t, *curr);
            /* print_grid(grid, grid_width, grid_height); */
            move(grid, grid_width, grid_height, &x_robot, &y_robot, dx, dy);
            t += 1;
        }
        curr += 1;
    }

    i32 sum = 0;
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (grid[x][y] == BOX) {
                sum += 100 * y + x;
            }
        }
    }

    printf("%d\n", sum);

    return 0;
}
