#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

#define GRID_SIZE 100
#define MAX_N_ANTENNA_TYPES 256

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

    while (*curr) {
        if (*curr == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            if (*curr != '.') {
                map[x][y] = *curr;
                n_antennas[(int)*curr] += 1;
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

    for (i32 i = 0; i < MAX_N_ANTENNA_TYPES; i += 1) {
        if (n_antennas[i] > 0) {
            printf("%c: %d\n", i, n_antennas[i]);
        }
    }

    return 0;
}
