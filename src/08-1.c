#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

#define GRID_SIZE 100
#define MAX_N_ANTENNA_TYPES 256
#define MAX_N_ANTENNAS 256

struct Antenna {
    i32 x;
    i32 y;
};

int main() {
    i32 grid_width;
    i32 grid_height;

    int fd = open("data/08-test", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;
    char map[GRID_SIZE][GRID_SIZE] = {0};
    i32 x = 0;
    i32 y = 0;
    i32 n_antennas[MAX_N_ANTENNA_TYPES] = {0};
    struct Antenna antennas[MAX_N_ANTENNA_TYPES][MAX_N_ANTENNAS] = {0};

    while (*curr) {
        if (*curr == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            if (*curr != '.') {
                map[x][y] = *curr;
                i32 idx_c = (int)*curr;
                antennas[idx_c][n_antennas[idx_c]] = (struct Antenna) { .x = x, .y = y };
                n_antennas[idx_c] += 1;
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            if (map[x][y] == 0) {
                eprintf(".");
            } else {
                eprintf("%c", map[x][y]);
            }
        }
        eprintf("\n");
    }

    eprintf("\n");

    for (i32 idx_type = 0; idx_type < MAX_N_ANTENNA_TYPES; idx_type += 1) {
        if (n_antennas[idx_type] > 0) {
            eprintf("%c: ", idx_type);
        }
        for (i32 idx_antenna = 0; idx_antenna < n_antennas[idx_type]; idx_antenna += 1) {
            struct Antenna antenna = antennas[idx_type][idx_antenna];
            eprintf("(%d,%d) ", antenna.x, antenna.y);
        }
        if (n_antennas[idx_type] > 0) {
            eprintf("\n");
        }
    }

    return 0;
}
