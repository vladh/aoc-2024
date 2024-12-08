#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

#define GRID_SIZE 100
#define MAX_N_ANTENNA_TYPES 256
#define MAX_N_ANTENNAS 256

struct Pos {
    i32 x;
    i32 y;
};

struct Dist {
    i32 x;
    i32 y;
};

struct Dist pos_dist(struct Pos a, struct Pos b) {
    return (struct Dist) {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

bool dist_is_double(struct Dist a, struct Dist b) {
    return (
        (a.x == b.x * 2 && a.y == b.y * 2) ||
        (b.x == a.x * 2 && b.y == a.y * 2)
    );
}

int main() {
    i32 grid_width;
    i32 grid_height;

    int fd = open("data/08", O_RDONLY);
    int len = lseek(fd, 0, SEEK_END);
    char *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    char *curr = data;
    i32 x = 0;
    i32 y = 0;
    i32 n_antennas[MAX_N_ANTENNA_TYPES] = {0};
    struct Pos antennas[MAX_N_ANTENNA_TYPES][MAX_N_ANTENNAS] = {0};

    while (*curr) {
        if (*curr == '\n') {
            y += 1;
            grid_width = x;
            x = 0;
        } else {
            if (*curr != '.') {
                i32 idx_c = (int)*curr;
                antennas[idx_c][n_antennas[idx_c]] = (struct Pos) { .x = x, .y = y };
                n_antennas[idx_c] += 1;
            }
            x += 1;
        }
        curr += 1;
    }

    grid_height = y;

    i32 n_antinodes = 0;

    for (i32 y = 0; y < grid_height; y += 1) {
        for (i32 x = 0; x < grid_width; x += 1) {
            struct Pos pos = { .x = x, .y = y };
            for (i32 idx_type = 0; idx_type < MAX_N_ANTENNA_TYPES; idx_type += 1) {
                for (i32 idx_antenna1 = 0; idx_antenna1 < n_antennas[idx_type]; idx_antenna1 += 1) {
                    struct Pos antenna1 = antennas[idx_type][idx_antenna1];
                    for (i32 idx_antenna2 = idx_antenna1 + 1; idx_antenna2 < n_antennas[idx_type]; idx_antenna2 += 1) {
                        struct Pos antenna2 = antennas[idx_type][idx_antenna2];
                        struct Dist dist1 = pos_dist(pos, antenna1);
                        struct Dist dist2 = pos_dist(pos, antenna2);
                        if (dist_is_double(dist1, dist2)) {
                            /* eprintf("(%d, %d) with (%d, %d) and (%d, %d)\n", */
                            /*     x, y, */
                            /*     antenna1.x, antenna1.y, */
                            /*     antenna2.x, antenna2.y); */
                            n_antinodes += 1;
                            goto end_pos;
                        }
                    }
                }
            }
end_pos:
            ;
        }
    }

    printf("%d\n", n_antinodes);

    return 0;
}
