#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "intrinsics.h"

#define GRID_SIZE 200

void print_grid(
    bool walls[GRID_SIZE][GRID_SIZE],
    i32 grid_width, i32 grid_height,
    i32 start_x, i32 start_y,
    i32 end_x, i32 end_y
) {
    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (walls[x][y]) {
                eprintf("#");
            } else if (start_x == x && start_y == y) {
                eprintf("S");
            } else if (end_x == x && end_y == y) {
                eprintf("E");
            } else {
                eprintf(".");
            }
        }
        eprintf("\n");
    }
    eprintf("\n");
}

int main() {
    i32 grid_width = 0;
    i32 grid_height = 0;
    bool walls[GRID_SIZE][GRID_SIZE] = {0};

    int f = open("data/16-test1", O_RDONLY);
    if (f == -1) {
        eprintf("Could not open file\n");
        exit(1);
    }
    int len = lseek(f, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, f, 0);
    char *curr = data;
    i32 start_x, start_y, end_x, end_y;
    i32 x = 0;
    i32 y = 0;

    while (*curr) {
        if (*curr == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            if (*curr == '#') {
                walls[x][y] = true;
            } else if (*curr == 'S') {
                start_x = x;
                start_y = y;
            } else if (*curr == 'E') {
                end_x = x;
                end_y = y;
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    print_grid(walls,
        grid_width, grid_height,
        start_x, start_y,
        end_x, end_y);

    return 0;
}
